/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxDxChBrgFdb.c
*
* DESCRIPTION:
*       FDB tables facility CPSS DxCh implementation.
*
*
* FILE REVISION NUMBER:
*       $Revision: 115 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
/* get the specific signaling for trigger action and AU message from CPU ended*/
#include <cpss/generic/bridge/private/prvCpssGenBrgFdbIsrSig.h>
/* get vid checking */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
/* get Cheetah FDB Hash function */
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgFdbHash.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrg.h>

#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrg.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/log/prvCpssLog.h>

/* #define CPSS_DXCH_FDB_DUMP */

#ifdef CPSS_DXCH_FDB_DUMP
    #define debugPrint(_x)      cpssOsPrintf _x
#else
    #define debugPrint(_x)
#endif

/*
number of messages queues :
1. primary AUQ
2. additional primary AUQ
3. primary FUQ
4. secondary AUQ for first primary AUQ
5. secondary AUQ for additional primary AUQ
*/
#define NUM_OF_MESSAGES_QUEUES_CNS   5

/* by default do not allow the WA to start when AUQ is not full */
static GT_BOOL    prvCpssAuqDeadlockOnQueueNotFullWaEnable = GT_FALSE;

/*******************************************************************************
 * External usage variables
 ******************************************************************************/
#define PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index)                 \
    if(index >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb)   \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, \
            prvCpssLogErrorMsgFdbIndexOutOfRange, devNum, index);

/* Converts packet command to hardware value */
#define PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(_val, _cmd)      \
    switch (_cmd)                                                       \
    {                                                                   \
        case CPSS_MAC_TABLE_FRWRD_E:                                    \
            _val = 0;                                                   \
            break;                                                      \
        case CPSS_MAC_TABLE_MIRROR_TO_CPU_E:                            \
            _val = 1;                                                   \
            break;                                                      \
        case CPSS_MAC_TABLE_CNTL_E:                                     \
            _val = 2;                                                   \
            break;                                                      \
        case CPSS_MAC_TABLE_DROP_E:                                     \
            _val = 3;                                                   \
            break;                                                      \
        case CPSS_MAC_TABLE_SOFT_DROP_E:                                \
            _val = 4;                                                   \
            break;                                                      \
        default:                                                        \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(_cmd)); \
    }

/* Converts  hardware value to packet command */
#define PRV_CPSS_DXCH_FDB_CONVERT_HW_CMD_TO_SW_VAL_MAC(_cmd, _val)      \
    switch (_val)                                                       \
    {                                                                   \
        case 0:                                                         \
            _cmd = CPSS_MAC_TABLE_FRWRD_E;                              \
            break;                                                      \
        case 1:                                                         \
            _cmd = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;                      \
            break;                                                      \
        case 2:                                                         \
            _cmd = CPSS_MAC_TABLE_CNTL_E;                               \
            break;                                                      \
        case 3:                                                         \
            _cmd = CPSS_MAC_TABLE_DROP_E;                               \
            break;                                                      \
        case 4:                                                         \
            _cmd = CPSS_MAC_TABLE_SOFT_DROP_E;                          \
            break;                                                      \
        default:                                                        \
            _cmd = (_val);                                              \
            break;                                                      \
    }

/* number of words in the mac entry */
#define CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS   5

/* maximal index of analyzer */
#define  PRV_CPSS_DXCH_FDB_MIRROR_TO_ANALYZER_MAX_INDEX_CNS 6
/* mask to remove the LSBit from a value .
   needed for dual device Id logic,
for example :
    x = 4
    y = 5

    (x & MASK_DUAL_DEVICE_ID_LSB_CNS) == (y & MASK_DUAL_DEVICE_ID_LSB_CNS)
*/
#define MASK_DUAL_DEVICE_ID_LSB_CNS     0xFFFFFFFE


static GT_STATUS prvCpssDxChBrgFdbLion3FdbIpUcRouteParametersCheck(
    IN  GT_U8                   devNum,
    IN  CPSS_MAC_ENTRY_EXT_STC  *macEntryPtr
);

static GT_STATUS prvCpssDxChBrgFdbLion3FdbIpUcRoutByMessageFormatFieldsSet(
    IN  GT_U8                   devNum,
    IN  CPSS_MAC_ENTRY_EXT_STC  *macEntryPtr,
    OUT GT_U32                  hwDataArr[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]
);

/*******************************************************************************
* prvCpssDxChBrgFdbLion3FdbAuMsgSpecialMuxedFieldsSet
*
* DESCRIPTION:
*       Set Muxed fields from the FDB Au Msg that depend on :
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
*       hwDataArr       - (pointer to) AU message format
*       entryType       - entry type :
*                           0 -- mac entry
*                           1 -- ipmcV4 entry
*                           2 -- ipmcV6 entry
*       specialFieldsPtr  - (pointer to) special muxed fields values
*
* OUTPUTS:
*       hwDataArr       - (pointer to) AU message format
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - on bad parameter error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChBrgFdbLion3FdbAuMsgSpecialMuxedFieldsSet
(
    IN GT_U8                        devNum,
    INOUT GT_U32                    hwDataArr[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS],
    IN GT_U32                       entryType,
    IN PRV_CPSS_DXCH_LION3_FDB_SPECIAL_MUXED_FIELDS_STC *specialFieldsPtr
)
{
    GT_U32      hwValue;             /* hardware value of the field      */
    GT_BOOL     srcIdLengthInFDB;    /* GT_FALSE - The SrcID filed in FDB is 9b
                                        GT_TRUE  - The SrcID field in FDB table is 12b.
                                        SrcID[11:9] are used for extending the user defined bits */
    GT_BOOL     tag1VidFdbEn;        /* GT_FALSE - <Tag1 VID> is not written in the FDB and is not read from the FDB.
                                            <SrcID>[8:6] can be used for src-id filtering and
                                            <SA Security Level> and <DA Security Level> are written/read from the FDB.
                                        GT_TRUE   - <Tag1 VID> is written in the FDB and read from the FDB as described in
                                            Section N:1 Mac Based VLAN .
                                            <SrcID>[8:6], <SA Security Level> and <DA Security Level>
                                            are read as 0 from the FDB entry.*/

    GT_U32      maxSourceId;         /* upper bound for valid sourceId */
    GT_U32      maxUdb;              /* upper bound for valid udb      */

    srcIdLengthInFDB = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.maxLengthSrcIdInFdbEn;
    tag1VidFdbEn     = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.tag1VidFdbEn;

    switch ((BOOL2BIT_MAC(tag1VidFdbEn) << 1) | BOOL2BIT_MAC(srcIdLengthInFDB))
    {
        default:
        case 0: /* tag1VidFdbEn==0, no srcIdLengthInFDB==0*/
            maxSourceId = (1 << 9);
            maxUdb      = (1 << 8);
            break;
        case 1: /* tag1VidFdbEn==0, no srcIdLengthInFDB==1*/
            maxSourceId = (1 << 12);
            maxUdb      = (1 << 5);
            break;
        case 2: /* tag1VidFdbEn==1, no srcIdLengthInFDB==0*/
            maxSourceId = (1 << 6);
            maxUdb      = (1 << 8);
            break;
        case 3: /* tag1VidFdbEn==1, no srcIdLengthInFDB==1*/
            maxSourceId = (1 << 6);
            maxUdb      = (1 << 5);
            break;
    }

    if (specialFieldsPtr->srcId >= maxSourceId)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(specialFieldsPtr->srcId));
    }

    if (specialFieldsPtr->udb >= maxUdb)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(specialFieldsPtr->udb));
    }

    if(entryType == 0) /* valid when MACEntryType = "MAC" */
    {
        hwValue = specialFieldsPtr->srcId & 0x3f;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_5_0_E,
            hwValue);

        if (tag1VidFdbEn == GT_FALSE)
        {
            hwValue = (specialFieldsPtr->srcId >> 6) & 0x07;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_8_6_E,
                hwValue);
        }
    }

    /* NOTE: setting the 8 bits of UDB must come before setting of SOURCE_ID_11_9
        to allow SOURCE_ID_11_9 to override the 3 bits ! */
    hwValue = specialFieldsPtr->udb;
    LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_USER_DEFINED_E,
        hwValue);

    if(entryType == 0) /* valid when MACEntryType = "MAC" */
    {
        if (srcIdLengthInFDB != GT_FALSE)
        {
            hwValue = specialFieldsPtr->srcId >> 9;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_11_9_E,
                hwValue);
        }
    }

    if (tag1VidFdbEn != GT_FALSE)
    {
        hwValue = specialFieldsPtr->origVid1;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_ORIG_VID1_E,
            hwValue);
    }
    else
    {
        if(specialFieldsPtr->daAccessLevel != PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
        {
            hwValue = specialFieldsPtr->daAccessLevel;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_DA_ACCESS_LEVEL_E,
                hwValue);
        }

        if(specialFieldsPtr->saAccessLevel != PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
        {
            hwValue = specialFieldsPtr->saAccessLevel;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_SA_ACCESS_LEVEL_E,
                hwValue);
        }
    }

    return GT_OK;
}

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
)
{
    GT_U32      hwValue;             /* hardware value of the field      */
    GT_BOOL     srcIdLengthInFDB;    /* GT_FALSE - The SrcID filed in FDB is 9b
                                        GT_TRUE  - The SrcID field in FDB table is 12b.
                                        SrcID[11:9] are used for extending the user defined bits */
    GT_BOOL     vid1AssignmentMode;  /* GT_FALSE - <Tag1 VID> is not written in the FDB and is not read from the FDB.
                                            <SrcID>[8:6] can be used for src-id filtering and
                                            <SA Security Level> and <DA Security Level> are written/read from the FDB.
                                        GT_TRUE   - <Tag1 VID> is written in the FDB and read from the FDB as described in
                                            Section N:1 Mac Based VLAN .
                                            <SrcID>[8:6], <SA Security Level> and <DA Security Level>
                                            are read as 0 from the FDB entry.*/

    srcIdLengthInFDB   = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.maxLengthSrcIdInFdbEn;
    vid1AssignmentMode = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.tag1VidFdbEn;

    if(entryType == 0) /* valid when MACEntryType = "MAC" */
    {
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_5_0_E,
            hwValue);
        specialFieldsPtr->srcId = hwValue;

        if (vid1AssignmentMode == GT_FALSE)
        {
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_8_6_E,
                hwValue);
            specialFieldsPtr->srcId |= (hwValue << 6);
        }

        if (srcIdLengthInFDB != GT_FALSE)
        {
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_11_9_E,
                hwValue);
            specialFieldsPtr->srcId |= (hwValue << 9);
        }
    }
    else
    {
        specialFieldsPtr->srcId = PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS;
    }

    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,hwDataArr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_USER_DEFINED_E,
        hwValue);
    specialFieldsPtr->udb = hwValue;
    if (srcIdLengthInFDB != GT_FALSE)
    {
        specialFieldsPtr->udb &= 0x1f;/*only 5 bits for the UDB */
    }

    if (vid1AssignmentMode != GT_FALSE)
    {
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_ORIG_VID1_E,
            hwValue);
        specialFieldsPtr->origVid1 = hwValue;

        specialFieldsPtr->daAccessLevel = PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS;
        specialFieldsPtr->saAccessLevel = PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS;
    }
    else
    {
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_DA_ACCESS_LEVEL_E,
            hwValue);
        specialFieldsPtr->daAccessLevel = hwValue;

        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_SA_ACCESS_LEVEL_E,
            hwValue);
        specialFieldsPtr->saAccessLevel = hwValue;

        specialFieldsPtr->origVid1 = PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS;
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChBrgFdbLion3FdbSpecialMuxedFieldsSet
*
* DESCRIPTION:
*       Set Muxed fields from the (FDB unit) FDB entry that depend on :
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
*       hwDataArr       - (pointer to) FDB entry format
*       entryType       - entry type :
*                           0 -- mac entry
*                           1 -- ipmcV4 entry
*                           2 -- ipmcV6 entry
*       specialFieldsPtr  - (pointer to) special muxed fields values
*
* OUTPUTS:
*       hwDataArr       - (pointer to) FDB entry format
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - on bad parameter error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChBrgFdbLion3FdbSpecialMuxedFieldsSet
(
    IN GT_U8                        devNum,
    INOUT GT_U32                    hwDataArr[CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS],
    IN GT_U32                       entryType,
    IN PRV_CPSS_DXCH_LION3_FDB_SPECIAL_MUXED_FIELDS_STC *specialFieldsPtr
)
{
    GT_U32      hwValue;             /* hardware value of the field      */
    GT_BOOL     srcIdLengthInFDB;    /* GT_TRUE - The SrcID filed in FDB is 12b
                                        GT_FALSE - The SrcID field in FDB table is 9b.
                                        SrcID[11:9] are used for extending the user defined bits */
    GT_BOOL     tag1VidFdbEn;        /* GT_FALSE - <Tag1 VID> is not written in the FDB and is not read from the FDB.
                                                   <SrcID>[8:6] can be used for src-id filtering and
                                                   <SA Security Level> and <DA Security Level> are written/read from the FDB.
                                        GT_TRUE  - <Tag1 VID> is written in the FDB and read from the FDB as described in
                                                   Section N:1 Mac Based VLAN .
                                                   <SrcID>[8:6], <SA Security Level> and <DA Security Level>
                                                   are read as 0 from the FDB entry.*/
    GT_U32      maxSourceId;         /* upper bound for valid sourceId */
    GT_U32      maxUdb;              /* upper bound for valid udb      */

    srcIdLengthInFDB = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.maxLengthSrcIdInFdbEn;
    tag1VidFdbEn     = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.tag1VidFdbEn;

    switch ((BOOL2BIT_MAC(tag1VidFdbEn) << 1) | BOOL2BIT_MAC(srcIdLengthInFDB))
    {
        default:
        case 0: /* tag1VidFdbEn==0, no srcIdLengthInFDB==0*/
            maxSourceId = (1 << 9);
            maxUdb      = (1 << 8);
            break;
        case 1: /* tag1VidFdbEn==0, no srcIdLengthInFDB==1*/
            maxSourceId = (1 << 12);
            maxUdb      = (1 << 5);
            break;
        case 2: /* tag1VidFdbEn==1, no srcIdLengthInFDB==0*/
            maxSourceId = (1 << 6);
            maxUdb      = (1 << 8);
            break;
        case 3: /* tag1VidFdbEn==1, no srcIdLengthInFDB==1*/
            maxSourceId = (1 << 6);
            maxUdb      = (1 << 5);
            break;
    }

    if (specialFieldsPtr->srcId >= maxSourceId)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (specialFieldsPtr->udb >= maxUdb)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(entryType == 0) /* valid when MACEntryType = "MAC" */
    {
        hwValue = (specialFieldsPtr->srcId & 0x3F);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_5_0_E,
            hwValue);

        if(tag1VidFdbEn == GT_FALSE)
        {
            hwValue = (specialFieldsPtr->srcId >> 6) & 0x7 ;
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_8_6_E,
                hwValue);
        }
    }

    /* NOTE: setting the 8 bits of UDB must come before setting of SOURCE_ID_11_9
        to allow SOURCE_ID_11_9 to override the 3 bits ! */
    hwValue = specialFieldsPtr->udb;
    LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
        LION3_FDB_FDB_TABLE_FIELDS_USER_DEFINED_E,
        hwValue);

    if(entryType == 0) /* valid when MACEntryType = "MAC" */
    {
        if (srcIdLengthInFDB != GT_FALSE)
        {
            hwValue = specialFieldsPtr->srcId >> 9;
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_11_9_E,
                hwValue);
        }
    }

    if (tag1VidFdbEn != GT_FALSE)
    {
        if(specialFieldsPtr->origVid1 != PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
        {
            hwValue = specialFieldsPtr->origVid1;
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_ORIG_VID1_E,
                hwValue);
        }
    }
    else
    {
        if(specialFieldsPtr->daAccessLevel != PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
        {
            hwValue = specialFieldsPtr->daAccessLevel;
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_DA_ACCESS_LEVEL_E,
                hwValue);
        }

        if(specialFieldsPtr->saAccessLevel != PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
        {
            hwValue = specialFieldsPtr->saAccessLevel;
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_SA_ACCESS_LEVEL_E,
                hwValue);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChBrgFdbLion3FdbFdbSpecialMuxedFieldsGet
*
* DESCRIPTION:
*       Get Muxed fields from the (FDB unit) FDB entry that depend on :
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
*       hwDataArr       - (pointer to) FDB entry format
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
static GT_STATUS prvCpssDxChBrgFdbLion3FdbFdbSpecialMuxedFieldsGet
(
    IN GT_U8                        devNum,
    IN GT_U32                       hwDataArr[CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS],
    IN GT_U32                       entryType,
    OUT PRV_CPSS_DXCH_LION3_FDB_SPECIAL_MUXED_FIELDS_STC *specialFieldsPtr
)
{
    GT_U32      hwValue;             /* hardware value of the field      */
    GT_BOOL     srcIdLengthInFDB;    /* GT_TRUE - The SrcID filed in FDB is 12b
                                        GT_FALSE - The SrcID field in FDB table is 9b.
                                        SrcID[11:9] are used for extending the user defined bits */
    GT_BOOL     tag1VidFdbEn;        /* GT_FALSE - <Tag1 VID> is not written in the FDB and is not read from the FDB.
                                                   <SrcID>[8:6] can be used for src-id filtering and
                                                   <SA Security Level> and <DA Security Level> are written/read from the FDB.
                                        GT_TRUE  - <Tag1 VID> is written in the FDB and read from the FDB as described in
                                                   Section N:1 Mac Based VLAN .
                                                   <SrcID>[8:6], <SA Security Level> and <DA Security Level>
                                                   are read as 0 from the FDB entry.*/

    srcIdLengthInFDB = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.maxLengthSrcIdInFdbEn;
    tag1VidFdbEn     = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.tag1VidFdbEn;

    if(entryType == 0) /* valid when MACEntryType = "MAC" */
    {
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_5_0_E,
            hwValue);
        specialFieldsPtr->srcId = hwValue;

        if (tag1VidFdbEn == GT_FALSE)
        {
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_8_6_E,
                hwValue);
            specialFieldsPtr->srcId |= (hwValue << 6);
        }

        if (srcIdLengthInFDB != GT_FALSE)
        {
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_11_9_E,
                hwValue);
            specialFieldsPtr->srcId |= (hwValue << 9);
        }
    }
    else
    {
        specialFieldsPtr->srcId = PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS;
    }

    LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
        LION3_FDB_FDB_TABLE_FIELDS_USER_DEFINED_E,
        hwValue);
    specialFieldsPtr->udb = hwValue;
    if (srcIdLengthInFDB != GT_FALSE)
    {
        specialFieldsPtr->udb &= 0x1f;/*only 5 bits for the UDB */
    }

    if (tag1VidFdbEn != GT_FALSE)
    {
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_ORIG_VID1_E,
            hwValue);
        specialFieldsPtr->origVid1 = hwValue;

        specialFieldsPtr->daAccessLevel = PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS;
        specialFieldsPtr->saAccessLevel = PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS;
    }
    else
    {
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_DA_ACCESS_LEVEL_E,
            hwValue);
        specialFieldsPtr->daAccessLevel = hwValue;

        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_SA_ACCESS_LEVEL_E,
            hwValue);
        specialFieldsPtr->saAccessLevel = hwValue;

        specialFieldsPtr->origVid1 = PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS;
    }

    return GT_OK;
}

/*******************************************************************************
* prvDxChFdbToAuConvert
*
* DESCRIPTION:
*       convert the FDB format of CPSS to DxCh Au (address update message)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to key data of the mac entry
*       macEntryPtr     - MAC format of entry
*       updMessageType  - Au message type
*       deleteEntry     - GT_TRUE - the entry should be deleted
*                         GT_FALSE - update entry
*       hwDataArr       - hwDataArr[0] is used as current FDB index for
*                         AU message type CPSS_NA_MOVED_E
*
* OUTPUTS:
*       hwDataArr       - (pointer to) AU message format
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - on bad parameter error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvDxChFdbToAuConvert
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC  *macEntryKeyPtr,
    IN  CPSS_MAC_ENTRY_EXT_STC      *macEntryPtr,
    IN  CPSS_UPD_MSG_TYPE_ENT       updMessageType,
    IN  GT_BOOL                     deleteEntry,
    INOUT GT_U32                    hwDataArr[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]
)
{
    GT_STATUS   rc;
    GT_U32      hwValue;             /* hardware value of the field      */
    GT_U32      hwUpdMessageType;    /* hw value of Update message type */
    GT_U32      bit40MacAddrOrIsIpm=0; /* bit 40 from MAC Address for MAC entry
                                        or entry type is IPM          */
    GT_U32      portTrunk;            /* port number or trunk Id         */
    GT_U32      isTrunk;              /* trunk flag                      */
    GT_U32      devTmp;               /* entry device number             */
    GT_U32      multiple;             /* if "1", forward packet to all
                                         ports in VIDX/VLAN */
    GT_U16      vidx = 0;
    GT_U16      vlanId=0;             /* vlan ID */
    GT_U32      entryType;            /* the only type supported
                                       0 -- mac entry
                                       1 -- ipmcV4 entry
                                       2 -- ipmcV6 entry
                                       3 -- IPv4 UC route */
    GT_U32  macLow16=0;                /* two low bytes of MAC Address */
    GT_U32  macHi32=0;                 /* four high bytes of MAC Address */
    GT_U8   sip[4];                  /* Source IP */
    GT_U8   dip[4];                  /* Destination IP */
    GT_U32  origFdbIndex;            /* Bobcat2; Caelum; Bobcat3 : old FDB index - relevant to CPSS_NA_MOVED_E */
    GT_U32  validBit;
    GT_U32  fdbLookupKeyModeBit = 0; /* FDB Lookup Key Mode: 0 - single tag, 1 - double tag */
    GT_U32  isDevEarch;              /* not 0 if the device devNum is eArch. */
    PRV_CPSS_DXCH_LION3_FDB_SPECIAL_MUXED_FIELDS_STC specialFields;/*special Muxed fields in the AU msg */


    isDevEarch = PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum);

    /* set update address message type */
    CPSS_LOG_INFORMATION_MAC("Update address message type %d", updMessageType);
    switch (updMessageType)
    {
        case CPSS_NA_E:
            hwUpdMessageType = 0;
            break;
        case CPSS_QA_E:
            hwUpdMessageType = 1;
            break;
        case CPSS_HR_E:
            hwUpdMessageType = 7;/*supported in Bobcat2; Caelum; Bobcat3 (Sip5)*/
            break;
        case CPSS_NA_MOVED_E:
            hwUpdMessageType = 0;/* same as NA */
            origFdbIndex = hwDataArr[0]; /* used as input parameter */

            /* reset all words in hwDataArr */
            cpssOsMemSet((char *) &hwDataArr[0], 0, CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS * sizeof(GT_U32));

            /* <MsgType> */
            hwValue = hwUpdMessageType;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_MSG_TYPE_E,
                hwValue);
            /* <NA move entry bank> */
            hwValue = 1;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVE_ENTRY_ENABLE_E,
                hwValue);
            /* <NA move entry bank> */
            hwValue = origFdbIndex;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVE_ENTRY_INDEX_E,
                hwValue);

            return GT_OK;
        default:
            /* not supported yet */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    cpssOsMemSet((char *) &hwDataArr[0], 0,
                 CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS * sizeof(GT_U32));

    /* check entry type */
    switch (macEntryKeyPtr->entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
            if (!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
            {
                 CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(devNum));
            }
            fdbLookupKeyModeBit = 1;
            /* no break! Next case should be CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E */
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
            macLow16 = GT_HW_MAC_LOW16(&(macEntryKeyPtr->key.macVlan.macAddr));
            macHi32 = GT_HW_MAC_HIGH32(&(macEntryKeyPtr->key.macVlan.macAddr));

            /* get bit 40 from MAC Address */
            bit40MacAddrOrIsIpm = (macHi32 >> 24) & 0x1;

            vlanId = macEntryKeyPtr->key.macVlan.vlanId;
            entryType = 0;

            if (vlanId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(vlanId));

            break;
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
            if (!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
            {
                 CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(devNum));
            }
            fdbLookupKeyModeBit = 1;
            /* no break! Next case should be CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E */
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
            cpssOsMemCpy(dip, macEntryKeyPtr->key.ipMcast.dip, 4);
            cpssOsMemCpy(sip, macEntryKeyPtr->key.ipMcast.sip, 4);
            vlanId = macEntryKeyPtr->key.ipMcast.vlanId;

            if (vlanId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(vlanId));
            if (macEntryKeyPtr->entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E ||
                macEntryKeyPtr->entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E)
                entryType = 1;
            else
                entryType = 2;

            /* set flag that entry is IPM */
            bit40MacAddrOrIsIpm = 1;

            /* set variables to avoid compilation warnings */
            macLow16 = 0;
            macHi32 = 0;
            break;
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
        if(isDevEarch)
        {
            if (macEntryKeyPtr->key.ipv4Unicast.vrfId >= BIT_12)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryKeyPtr->key.ipv4Unicast.vrfId));
            }
            if ((macEntryKeyPtr->key.ipv4Unicast.dip[0] >= 224) &&
                (macEntryKeyPtr->key.ipv4Unicast.dip[0] <= 239))
            {
                /* Multicast range */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryKeyPtr->key.ipv4Unicast.dip[0]));
            }

            bit40MacAddrOrIsIpm = 0;

            /* set variables to avoid compilation warnings */
            macLow16 = 0;
            macHi32 = 0;

            entryType = 3;
            break;
        }
        else
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(isDevEarch));
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryKeyPtr->entryType));
    }

    if(isDevEarch)
    {
        if(deleteEntry == GT_TRUE &&
           PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode == CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
        {
            /* NOTE : when working with 'multi hash' the skip bit is ignored !
                and we need to use the 'valid' */
            validBit = 0;
        }
        else
        {
            validBit = 1;
        }

        hwValue = validBit;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_VALID_E,
            hwValue);

        /* <MsgType> */
        hwValue = hwUpdMessageType;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_MSG_TYPE_E,
            hwValue);

        /* FDB Lookup Key Mode */
        if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            /* should be copied to HW format also for "delete" case */
            hwValue = fdbLookupKeyModeBit;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                            LION3_FDB_AU_MSG_TABLE_FIELDS_FDB_LOOKUP_KEY_MODE_E,
                            hwValue);

            if (fdbLookupKeyModeBit)
            {
                hwValue = macEntryKeyPtr->vid1;
                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                                LION3_FDB_AU_MSG_TABLE_FIELDS_ORIG_VID1_E,
                                hwValue);
            }
        }

        if (entryType == 0)
        {
            /* FID */
            hwValue = vlanId;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                                           LION3_FDB_AU_MSG_TABLE_FIELDS_FID_E,
                                           hwValue);

            LION3_FDB_AU_MSG_FIELD_MAC_ADDR_SET_MAC(devNum, hwDataArr,
                &macEntryKeyPtr->key.macVlan.macAddr.arEther[0]);

        }
        else
        {
            if((entryType==1)||(entryType==2))
            {
                 /* FID */
                hwValue = vlanId;
                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                                               LION3_FDB_AU_MSG_TABLE_FIELDS_FID_E,
                                               hwValue);

                /* set IP Addresses */

                /*DIP*/
                hwValue = (dip[0] << 24) |
                          (dip[1] << 16) |
                          (dip[2] <<  8) |
                          (dip[3] <<  0) ;
                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                    LION3_FDB_AU_MSG_TABLE_FIELDS_DIP_E,
                    hwValue);

                /*SIP*/
                hwValue = (sip[0] << 24) |
                          (sip[1] << 16) |
                          (sip[2] <<  8) |
                          (sip[3] <<  0) ;
                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                    LION3_FDB_AU_MSG_TABLE_FIELDS_SIP_E,
                    hwValue);
            }
            else /* entryType==3 */
            {
                /* vrf Id */
                hwValue = macEntryKeyPtr->key.ipv4Unicast.vrfId;
                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                    LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_VRF_ID_E,
                    hwValue);

                /*IPv4 UC DIP*/
                cpssOsMemCpy(dip, macEntryKeyPtr->key.ipv4Unicast.dip, 4);

                hwValue = (dip[0] << 24) |
                          (dip[1] << 16) |
                          (dip[2] <<  8) |
                          (dip[3] <<  0) ;
                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                    LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV4_DIP_E,
                    hwValue);

            }
        }

        /* Entry Type */
        hwValue = entryType;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_FDB_ENTRY_TYPE_E,
            hwValue);

        if(deleteEntry == GT_TRUE)
        {
            /* NOTE: when hashMode == CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
               the skip bit is ignored */

            /* skip bit */
            hwValue = 1;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_SKIP_E,
                hwValue);

            return GT_OK;
        }
    }
    else
    {
        /* Set Word0 */

        /* set message ID field for Cheetah only */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            hwDataArr[0] = 0x2;

        /* message type */
        hwDataArr[0] |= (hwUpdMessageType << 4);

        if (entryType == 0)
        {
            /* set MAC Address */

            /* MacAddr[15..0] */
            hwDataArr[0] |= (macLow16 << 16);  /* Word0 */
            /* MacAddr[47..16] */
            hwDataArr[1] = macHi32;            /* Word1 */

        }
        else
        {
            /* set IP Addresses */

            /* DIP[15..0] */
            hwDataArr[0] |= ((dip[3] | (dip[2] << 8)) << 16);  /* Word0, bits 16-31 */
            /* DIP[16..31] */
            hwDataArr[1] |= (dip[1] | (dip[0] << 8));          /* Word1, bits 0-15 */
            /* SIP[15..0] */
            hwDataArr[1] |= ((sip[3] | (sip[2] << 8)) << 16);  /* Word1, bits 16-31 */
            /* SIP[27..16] */
            hwDataArr[3] |= (sip[1] | ((sip[0] & 0xF) << 8));  /* Word3, bits 0-11 */
            /* SIP[31..28] */
            hwDataArr[3] |= ((sip[0] >> 4) << 27);             /* Word3, bits 27-30 */
        }

        /* VID - 12 bits */
        hwDataArr[2] = (vlanId & 0xFFF);

        /* Entry Type */
        hwDataArr[3] |= (entryType << 19);


        if(deleteEntry == GT_TRUE)
        {
            /* skip bit */
            hwDataArr[2] |= (1 << 12);
            return GT_OK;
        }
    }


    if(updMessageType == CPSS_QA_E ||
       updMessageType == CPSS_HR_E)
    {
        /* those messages not require any more info */
        return GT_OK;
    }

    if(isDevEarch &&
       (macEntryKeyPtr->entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E))
    {
        /* check ipv4 UC fields */
        rc = prvCpssDxChBrgFdbLion3FdbIpUcRouteParametersCheck(devNum,macEntryPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChBrgFdbLion3FdbIpUcRoutByMessageFormatFieldsSet(devNum,
                                                                       macEntryPtr,
                                                                       hwDataArr);
        if(rc != GT_OK)
            return rc;

        /* age bit */
        hwValue = BOOL2BIT_MAC(macEntryPtr->age);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E,
            hwValue);

        return GT_OK;
    }

    /* check interface destination type */
    switch(macEntryPtr->dstInterface.type)
    {
        /* check that the type of the entry is MAC */
        case CPSS_INTERFACE_PORT_E:
            /* check that type of the entry is MAC and address
               is not multicast */
            if (bit40MacAddrOrIsIpm == 0)
            {
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                    macEntryPtr->dstInterface.devPort.hwDevNum,
                    macEntryPtr->dstInterface.devPort.portNum);

                devTmp = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                    macEntryPtr->dstInterface.devPort.hwDevNum,
                    macEntryPtr->dstInterface.devPort.portNum);
                portTrunk = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                    macEntryPtr->dstInterface.devPort.hwDevNum,
                    macEntryPtr->dstInterface.devPort.portNum);
                isTrunk = 0;
                multiple = 0;

            }
            else
            {
                /* For multicast MAC or IPM entries the VIDX or VLAN
                   target must be used but not port */
                CPSS_LOG_INFORMATION_MAC("For multicast MAC or IPM entries the VIDX or VLAN target must be used but not port");
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(bit40MacAddrOrIsIpm));
            }
            break;

        case CPSS_INTERFACE_TRUNK_E:
            /* check that type of the entry is MAC and address
               is not multicast */
            if (bit40MacAddrOrIsIpm == 0)
            {
                portTrunk = macEntryPtr->dstInterface.trunkId;
                if (portTrunk == 0)
                {
                    /* trunk ID should be > 0 */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(portTrunk));
                }
                isTrunk = 1;
                devTmp = macEntryPtr->dstInterface.hwDevNum;
                multiple = 0;
            }
            else
            {
                /* For multicast MAC or IPM entries the VIDX or VLAN
                   target must be used but not trunk*/
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(bit40MacAddrOrIsIpm));
            }

            break;

        case CPSS_INTERFACE_VIDX_E:
        case CPSS_INTERFACE_VID_E:
            devTmp = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
            if(macEntryPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
            {
                vidx = 0xFFF;
            }
            else /*CPSS_INTERFACE_VIDX_E*/
            {
                vidx = macEntryPtr->dstInterface.vidx;
                /* check range */
                if (vidx > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(devNum))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(vidx));
            }
            multiple = 1;

            /* set variables to avoid compilation warnings */
            isTrunk = 0;
            portTrunk = 0;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->dstInterface.type));
    }

    /* check ranges */
    if ((devTmp > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum)) ||
        (macEntryPtr->userDefined > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_FDB_USER_DEFINED_MAC(devNum)) ||
        (macEntryPtr->saQosIndex >= BIT_3) ||
        (macEntryPtr->daQosIndex >= BIT_3) ||
        ((isTrunk==1) && (portTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))) ||
        ((isTrunk==0) && (portTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))))
    {
        CPSS_LOG_INFORMATION_MAC("Check ranges: devTmp[%d] range[%d], macEntryPtr->userDefined[%d] range[%d],"
                                 "macEntryPtr->saQosIndex[%d] macEntryPtr->daQosIndex[%d] range[%d], "
                                 "isTrunk[%d], portTrunk[%d] range[%d]",
                                 devTmp, PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum),
                                 macEntryPtr->userDefined, PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_FDB_USER_DEFINED_MAC(devNum),
                                 macEntryPtr->saQosIndex, macEntryPtr->daQosIndex, BIT_3,
                                                                 isTrunk, portTrunk, (isTrunk==1) ?
                                 PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum) : PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(isDevEarch)
    {
        /* Multiple */
        hwValue = multiple;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_MULTIPLE_E,
            hwValue);

        /* handle muxed fields that depends on global configurations */
        specialFields.srcId = macEntryPtr->sourceID;
        specialFields.udb = macEntryPtr->userDefined;
        specialFields.origVid1  = macEntryPtr->key.vid1;
        specialFields.daAccessLevel = macEntryPtr->daSecurityLevel;
        specialFields.saAccessLevel = macEntryPtr->saSecurityLevel;

        rc = prvCpssDxChBrgFdbLion3FdbAuMsgSpecialMuxedFieldsSet(devNum,hwDataArr,entryType,&specialFields);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* Multiple */
        hwDataArr[2] |= (multiple << 15);
    }

    if ((multiple == 1) ||
        (entryType == 1 || entryType == 2))
    {
        if(isDevEarch)
        {
            /* VIDX*/
            hwValue = vidx;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_VIDX_E,
                hwValue);
        }
        else
        {
            /* VIDX */
            hwDataArr[2] |= ((vidx & 0xFFF) << 17);
        }
    }

    if (entryType == 0)/* MAC entry */
    {
        if (multiple == 0)
        {/* muliple=0 and bit40MacAddr=0 */

            if(isDevEarch)
            {
                /* is trunk */
                hwValue = isTrunk;
                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                    LION3_FDB_AU_MSG_TABLE_FIELDS_IS_TRUNK_E,
                    hwValue);
                if(isTrunk)
                {
                    /* PortNum */
                    hwValue = portTrunk;
                    LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                        LION3_FDB_AU_MSG_TABLE_FIELDS_TRUNK_NUM_E,
                        hwValue);
                }
                else
                {
                    /* TrunkNum */
                    hwValue = portTrunk;
                    LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                        LION3_FDB_AU_MSG_TABLE_FIELDS_EPORT_NUM_E,
                        hwValue);
                }

                /* DevNum */
                hwValue = devTmp;
                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                    LION3_FDB_AU_MSG_TABLE_FIELDS_DEV_ID_E,
                    hwValue);

                /* SA cmd */
                PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(hwValue,
                                                               macEntryPtr->saCommand);
            }
            else
            {
                /* is Trunk bit */
                hwDataArr[2] |= (isTrunk << 17);

                /* PortNum/TrunkNum */
                hwDataArr[2] |= ((portTrunk & 0x7F) << 18);

                /* UserDefined */
                hwDataArr[2] |= ((macEntryPtr->userDefined & 0xF) << 25);
            }

        }

        if(isDevEarch)
        {
            /* DevNum */
            hwValue = devTmp;
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_DEV_ID_E,
                hwValue);

            /* SA cmd */
            PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(hwValue,
                                                           macEntryPtr->saCommand);
            LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_SA_CMD_E,
                hwValue);
        }
        else
        {
            /* Src ID */
            /* check range */
            if (macEntryPtr->sourceID > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_SRC_ID_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->sourceID));
            }
            hwDataArr[3] |= ((macEntryPtr->sourceID & 0x1F) << 2);

            /* DevNum */
            hwDataArr[3] |= ((devTmp & 0x1F) << 7);

            /* SA cmd */
            PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(hwValue,
                                                           macEntryPtr->saCommand);
            hwDataArr[3] |= (hwValue << 24);
        }

    }

    if(isDevEarch)
    {
        /* age bit */
        hwValue = BOOL2BIT_MAC(macEntryPtr->age);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E,
            hwValue);

        /* spUnknown - NA strorm prevent entry */
        hwValue = BOOL2BIT_MAC(macEntryPtr->spUnknown);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_SP_UNKNOWN_E,
            hwValue);

        /* DA Route */
        hwValue = BOOL2BIT_MAC(macEntryPtr->daRoute);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_DA_ROUTE_E,
            hwValue);

        /* SA QoS Profile Index */
        hwValue = macEntryPtr->saQosIndex;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_SA_QOS_PARAM_SET_IDX_E,
            hwValue);

        /* Da QoS Profile Index */
        hwValue = macEntryPtr->daQosIndex;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_DA_QOS_PARAM_SET_IDX_E,
            hwValue);

        /* Static */
        hwValue = BOOL2BIT_MAC(macEntryPtr->isStatic);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_IS_STATIC_E,
            hwValue);

        /* DA cmd */
        PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(hwValue,
                                                       macEntryPtr->daCommand);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_DA_CMD_E,
            hwValue);

        /* DA Lookup Ingress Mirror to Analyzer Enable */
        hwValue = BOOL2BIT_MAC(macEntryPtr->daMirrorToRxAnalyzerPortEn);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_DA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E,
            hwValue);

        /* SA Lookup Ingress Mirror to Analyzer Enable */
        hwValue = BOOL2BIT_MAC(macEntryPtr->saMirrorToRxAnalyzerPortEn);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_SA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E,
            hwValue);

        /* Application Specific CPU Code */
        hwValue = BOOL2BIT_MAC(macEntryPtr->appSpecificCpuCode);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_E,
            hwValue);

    }
    else
    {
        /* age bit */
        hwDataArr[2] |= (BOOL2BIT_MAC(macEntryPtr->age)  << 13);

        /* spUnknown - NA strorm prevent entry */
        hwDataArr[2] |= (BOOL2BIT_MAC(macEntryPtr->spUnknown) << 14);

        /* DA Route */
        hwDataArr[2] |= ((BOOL2BIT_MAC(macEntryPtr->daRoute)) << 30);

        /* SA QoS Profile Index */
        hwDataArr[3] |= (macEntryPtr->saQosIndex << 12);

        /* Da QoS Profile Index */
        hwDataArr[3] |= (macEntryPtr->daQosIndex << 15);

        /* Static */
        hwDataArr[3] |= ((BOOL2BIT_MAC(macEntryPtr->isStatic)) << 18);

        /* DA cmd */
        PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(hwValue,
                                                       macEntryPtr->daCommand);
        hwDataArr[3] |= (hwValue << 21);

        /* Mirror to Analyzer Port */
        hwValue = BOOL2BIT_MAC(macEntryPtr->mirrorToRxAnalyzerPortEn);
        hwDataArr[3] |= (hwValue << 31);

        /* set Cheetah2 fields */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* check ranges */
            if ((macEntryPtr->daSecurityLevel >= BIT_3) ||
                (macEntryPtr->saSecurityLevel >= BIT_3))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            /* MAC DA Access Level */
            hwDataArr[0] |= (macEntryPtr->daSecurityLevel << 1);
            /* MAC SA Access Level */
            hwDataArr[0] |= (macEntryPtr->saSecurityLevel << 12);
            /* Application Specific CPU Code */
            hwDataArr[2] |= ((BOOL2BIT_MAC(macEntryPtr->appSpecificCpuCode)) << 29);
        }
    }

    return GT_OK;
}



/*******************************************************************************
* prvCpssDxChBrgFdbLion3FdbIpUcRouteFieldsGet
*
* DESCRIPTION:
*       convert the FDB format of CPSS to DxCh Mac hw format
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       macEntryType - uc route fdbEntry type
*       hwDataArr    - pointer to uc route entry in ha format
*
* OUTPUTS:
*       macEntryPtr   - (pointer to)  mac entry format
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_STATE - on unpredictable reading from the hw
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChBrgFdbLion3FdbIpUcRouteFieldsGet(
    IN  GT_U8                   devNum,
    IN  GT_U32                  macEntryType,
    IN GT_U32                   hwDataArr[CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS],
    OUT CPSS_MAC_ENTRY_EXT_STC  *macEntryPtr
)
{
    GT_U32      hwValue = 0;             /* hardware value of the field      */

    switch (macEntryType)
    {
    case 3:
    case 5:
        /* macEntryType is CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E or CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ONLY_ENTRY_E*/
        if(macEntryType == 3)
            macEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E;
        if(macEntryType == 5)
            macEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E;
        /* vrf Id */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_VRF_ID_E,
            hwValue)

        if (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E)
        {
            macEntryPtr->key.key.ipv4Unicast.vrfId = hwValue;

            /*IPv4 UC DIP*/
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_IPV4_DIP_E,
                hwValue);

            macEntryPtr->key.key.ipv4Unicast.dip[3] = (GT_U8)(hwValue >> 0);
            macEntryPtr->key.key.ipv4Unicast.dip[2] = (GT_U8)(hwValue >> 8);
            macEntryPtr->key.key.ipv4Unicast.dip[1] = (GT_U8)(hwValue >> 16);
            macEntryPtr->key.key.ipv4Unicast.dip[0] = (GT_U8)(hwValue >> 24);
        }
        if (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E)
        {
            macEntryPtr->key.key.ipv6Unicast.vrfId = hwValue;

            /* scope checking enable */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_IPV6_SCOPE_CHECK_E,
                hwValue);
            macEntryPtr->fdbRoutingInfo.scopeCheckingEnable = BIT2BOOL_MAC(hwValue);
            /* site Id */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_IPV6_DST_SITE_ID_E,
                hwValue);

            switch(hwValue)
            {
            case 0:
                macEntryPtr->fdbRoutingInfo.siteId = CPSS_IP_SITE_ID_INTERNAL_E;
                break;
            case 1:
                macEntryPtr->fdbRoutingInfo.siteId = CPSS_IP_SITE_ID_EXTERNAL_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
        /* ttl/hop decrement enable */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_DEC_TTL_OR_HOP_COUNT_E,
            hwValue);
        macEntryPtr->fdbRoutingInfo.ttlHopLimitDecEnable = BIT2BOOL_MAC(hwValue);

        /* Bypass TTL Options Or Hop Extension */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E,
            hwValue);
        macEntryPtr->fdbRoutingInfo.ttlHopLimDecOptionsExtChkByPass = BIT2BOOL_MAC(hwValue);

        /* ingress mirror to analyzer index */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E,
           hwValue);
        if (hwValue)
        {
            macEntryPtr->fdbRoutingInfo.ingressMirrorToAnalyzerIndex = hwValue - 1;
            macEntryPtr->fdbRoutingInfo.ingressMirror = GT_TRUE;
        }
        else
        {
            macEntryPtr->fdbRoutingInfo.ingressMirror = GT_FALSE;
        }

        /* qos profile mark enable */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E,
           hwValue);
        macEntryPtr->fdbRoutingInfo.qosProfileMarkingEnable = BIT2BOOL_MAC(hwValue);

        /* qos profile index */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_INDEX_E,
           hwValue);
        macEntryPtr->fdbRoutingInfo.qosProfileIndex = hwValue;

        /* qos precedence */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E,
           hwValue);

        switch(hwValue)
        {
        case 0:
            macEntryPtr->fdbRoutingInfo.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
            break;
        case 1:
            macEntryPtr->fdbRoutingInfo.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        /* modify UP */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_MODIFY_UP_E,
           hwValue);

        switch(hwValue)
        {
        case 0:
            macEntryPtr->fdbRoutingInfo.modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 2:
            macEntryPtr->fdbRoutingInfo.modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        case 1:
            macEntryPtr->fdbRoutingInfo.modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        /* modify DSCP */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_MODIFY_DSCP_E,
           hwValue);

        switch(hwValue)
        {
        case 0:
            macEntryPtr->fdbRoutingInfo.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 2:
            macEntryPtr->fdbRoutingInfo.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        case 1:
            macEntryPtr->fdbRoutingInfo.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        /* counter set */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_COUNTER_SET_INDEX_E,
           hwValue);

        switch(hwValue)
        {
        case 0:
             macEntryPtr->fdbRoutingInfo.countSet = CPSS_IP_CNT_SET0_E;
            break;
        case 1:
             macEntryPtr->fdbRoutingInfo.countSet = CPSS_IP_CNT_SET1_E;
            break;
        case 2:
             macEntryPtr->fdbRoutingInfo.countSet = CPSS_IP_CNT_SET2_E;
            break;
        case 3:
             macEntryPtr->fdbRoutingInfo.countSet = CPSS_IP_CNT_SET3_E;
            break;
        case 7:
            macEntryPtr->fdbRoutingInfo.countSet = CPSS_IP_CNT_NO_SET_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }


        /* trap mirror arp bc enbale */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_ARP_BC_TRAP_MIRROR_EN_E,
           hwValue);
        macEntryPtr->fdbRoutingInfo.trapMirrorArpBcEnable = BIT2BOOL_MAC(hwValue);

        /* dip access level */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_DIP_ACCESS_LEVEL_E,
           hwValue);
        macEntryPtr->fdbRoutingInfo.dipAccessLevel =  hwValue;

        /* ICMP redirect enable */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_ICMP_REDIRECT_EXCEP_MIRROR_EN_E,
           hwValue);
        macEntryPtr->fdbRoutingInfo.ICMPRedirectEnable = BIT2BOOL_MAC(hwValue);

        /* mtu profile index */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_MTU_INDEX_E,
           hwValue);
        macEntryPtr->fdbRoutingInfo.mtuProfileIndex = hwValue;

        /* get vlan associated with this entry */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_NEXT_HOP_EVLAN_E,
           hwValue);
        macEntryPtr->fdbRoutingInfo.nextHopVlanId = (GT_U16)hwValue;

        /* use VIDX */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_USE_VIDX_E,
           hwValue);
        if (hwValue == 1)
        {
            /* get vidx value */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
               LION3_FDB_FDB_TABLE_FIELDS_VIDX_E,
               hwValue);

            if(hwValue == 0xFFF)
            {
                macEntryPtr->dstInterface.type = CPSS_INTERFACE_VID_E;
                macEntryPtr->dstInterface.vlanId = macEntryPtr->fdbRoutingInfo.nextHopVlanId;
            }
            else
            {
                macEntryPtr->dstInterface.type = CPSS_INTERFACE_VIDX_E;
                macEntryPtr->dstInterface.vidx = (GT_U16)hwValue;
            }
        }
        else
        {
            /* check if target is trunk */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
               LION3_FDB_FDB_TABLE_FIELDS_IS_TRUNK_E,
               hwValue);
            if (hwValue == 1)
            {
                /* TRUNK */
                macEntryPtr->dstInterface.type = CPSS_INTERFACE_TRUNK_E;
                /* get trunk_ID value */
                LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
                   LION3_FDB_FDB_TABLE_FIELDS_TRUNK_NUM_E,
                   hwValue);
                macEntryPtr->dstInterface.trunkId = (GT_TRUNK_ID)hwValue;
            }
            else
            {
                /* PORT */
                macEntryPtr->dstInterface.type = CPSS_INTERFACE_PORT_E;
                /* get target device */
                LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
                   LION3_FDB_FDB_TABLE_FIELDS_TARGET_DEVICE_E,
                   hwValue);
                macEntryPtr->dstInterface.devPort.hwDevNum = hwValue;
                /* get target port */
                LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
                   LION3_FDB_FDB_TABLE_FIELDS_EPORT_NUM_E,
                   hwValue);
                macEntryPtr->dstInterface.devPort.portNum = hwValue;
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(macEntryPtr->dstInterface));
            }
        }

        /* check if entry is tunnel start */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
           LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_START_E,
           hwValue);
        macEntryPtr->fdbRoutingInfo.isTunnelStart = BIT2BOOL_MAC(hwValue);
        if (macEntryPtr->fdbRoutingInfo.isTunnelStart == GT_TRUE)
        {
            /* get tunnel ptr */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
               LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_PTR_E,
               hwValue);
            macEntryPtr->fdbRoutingInfo.nextHopTunnelPointer = hwValue;
        }
        else
        {
            /* get arp ptr */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
               LION3_FDB_FDB_TABLE_FIELDS_ARP_PTR_E,
               hwValue);

            macEntryPtr->fdbRoutingInfo.nextHopARPPointer = hwValue;
        }

         break;

    case 4:
        {
            GT_U32 wordNum;
            /* macEntryType is CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E*/
            macEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E;
            /* get IPv6 UC DIP */
            for(wordNum = 0; wordNum < 4; wordNum++)
            {
                LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
                   LION3_FDB_FDB_TABLE_FIELDS_IPV6_DIP_0_E + wordNum,
                   hwValue);

                macEntryPtr->key.key.ipv6Unicast.dip[((3 - wordNum) * 4)]     = (GT_U8)(hwValue >> 24);
                macEntryPtr->key.key.ipv6Unicast.dip[((3 - wordNum) * 4) + 1] = (GT_U8)(hwValue >> 16);
                macEntryPtr->key.key.ipv6Unicast.dip[((3 - wordNum) * 4) + 2] = (GT_U8)(hwValue >> 8);
                macEntryPtr->key.key.ipv6Unicast.dip[((3 - wordNum) * 4) + 3] = (GT_U8)(hwValue);
           }
            /* get NH data bank number */
             LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwDataArr,
                   LION3_FDB_FDB_TABLE_FIELDS_NH_DATA_BANK_NUM_E,
                   hwValue);
             macEntryPtr->fdbRoutingInfo.nextHopDataBankNumber = hwValue;
        }

        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }
    return GT_OK;

}


/*******************************************************************************
* prvCpssDxChBrgFdbLion3FdbIpUcRouteFieldsSet
*
* DESCRIPTION:
*       convert the FDB format of CPSS to DxCh Mac hw format
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       macEntryPtr - pointer to extended MAC entry format
*
* OUTPUTS:
*       hwDataArr   - (pointer to) hw mac entry format
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - on bad parameter error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChBrgFdbLion3FdbIpUcRouteFieldsSet(
    IN  GT_U8                   devNum,
    IN  CPSS_MAC_ENTRY_EXT_STC  *macEntryPtr,
    OUT GT_U32                  hwDataArr[CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS]
)
{
    GT_U32      hwValue = 0;             /* hardware value of the field      */
    GT_U8       dip[16] = {0};              /* Destination IP */
    GT_U32      portTrunk;            /* port number or trunk Id         */

    switch (macEntryPtr->key.entryType)
    {
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:

        if (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E)
        {
            /*IPv4 UC DIP*/
            cpssOsMemCpy(dip, macEntryPtr->key.key.ipv4Unicast.dip, 4);

            hwValue = (dip[0] << 24) |
                      (dip[1] << 16) |
                      (dip[2] <<  8) |
                      (dip[3] <<  0) ;
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_IPV4_DIP_E,
                hwValue);

            hwValue = macEntryPtr->key.key.ipv4Unicast.vrfId;

        }
        if (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E)
        {
            hwValue = BOOL2BIT_MAC(macEntryPtr->fdbRoutingInfo.scopeCheckingEnable);
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_IPV6_SCOPE_CHECK_E,
                hwValue);
            switch(macEntryPtr->fdbRoutingInfo.siteId)
            {
            case CPSS_IP_SITE_ID_INTERNAL_E:
                hwValue = 0;
                break;
            case CPSS_IP_SITE_ID_EXTERNAL_E:
                hwValue = 1;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_IPV6_DST_SITE_ID_E,
                hwValue);

            hwValue = macEntryPtr->key.key.ipv6Unicast.vrfId;
        }

        /* vrf Id */
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_VRF_ID_E,
            hwValue);


        /* ttl/hop decrement enable */
        hwValue = BOOL2BIT_MAC(macEntryPtr->fdbRoutingInfo.ttlHopLimitDecEnable);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_DEC_TTL_OR_HOP_COUNT_E,
            hwValue);
        /* Bypass TTL Options Or Hop Extension */
        hwValue = BOOL2BIT_MAC(macEntryPtr->fdbRoutingInfo.ttlHopLimDecOptionsExtChkByPass);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E,
            hwValue);
        /* ingress mirror to analyzer index */
        if (macEntryPtr->fdbRoutingInfo.ingressMirror)
        {
            hwValue = macEntryPtr->fdbRoutingInfo.ingressMirrorToAnalyzerIndex + 1;
        }
        else
        {
            hwValue = 0;
        }
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E,
            hwValue);
        hwValue = BOOL2BIT_MAC(macEntryPtr->fdbRoutingInfo.qosProfileMarkingEnable);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E,
            hwValue);
        hwValue = macEntryPtr->fdbRoutingInfo.qosProfileIndex;
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_INDEX_E,
            hwValue);
        switch(macEntryPtr->fdbRoutingInfo.qosPrecedence)
        {
        case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
            hwValue = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
            hwValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E,
            hwValue);

        switch(macEntryPtr->fdbRoutingInfo.modifyUp)
        {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            hwValue = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            hwValue = 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            hwValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_MODIFY_UP_E,
            hwValue);

        switch(macEntryPtr->fdbRoutingInfo.modifyDscp)
        {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            hwValue = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            hwValue = 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            hwValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_MODIFY_DSCP_E,
            hwValue);

        switch(macEntryPtr->fdbRoutingInfo.countSet)
        {
        case CPSS_IP_CNT_SET0_E:
             hwValue = 0;
            break;
        case CPSS_IP_CNT_SET1_E:
             hwValue = 1;
            break;
        case CPSS_IP_CNT_SET2_E:
             hwValue = 2;
            break;
        case CPSS_IP_CNT_SET3_E:
             hwValue = 3;
            break;
        case CPSS_IP_CNT_NO_SET_E:
            hwValue = 7;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_COUNTER_SET_INDEX_E,
            hwValue);


        hwValue = BOOL2BIT_MAC(macEntryPtr->fdbRoutingInfo.trapMirrorArpBcEnable);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_ARP_BC_TRAP_MIRROR_EN_E,
            hwValue);
        /* dip access level */
        hwValue = macEntryPtr->fdbRoutingInfo.dipAccessLevel;
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_DIP_ACCESS_LEVEL_E,
            hwValue);

        hwValue = BOOL2BIT_MAC(macEntryPtr->fdbRoutingInfo.ICMPRedirectEnable);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_ICMP_REDIRECT_EXCEP_MIRROR_EN_E,
            hwValue);
        /* mtu profile index */
        hwValue = macEntryPtr->fdbRoutingInfo.mtuProfileIndex;
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_MTU_INDEX_E,
            hwValue);

        switch(macEntryPtr->dstInterface.type)
        {
            case CPSS_INTERFACE_VIDX_E:
            case CPSS_INTERFACE_VID_E:
                /* use VIDX */

                hwValue = 1;

                 LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                    LION3_FDB_FDB_TABLE_FIELDS_USE_VIDX_E,
                    hwValue);

                if (macEntryPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
                {
                    hwValue=0xFFF;
                }
                else
                {
                     /* check range */
                     if (macEntryPtr->dstInterface.vidx > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(devNum))
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

                     /* set vidx value */
                     hwValue = macEntryPtr->dstInterface.vidx;
                }

                  LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                     LION3_FDB_FDB_TABLE_FIELDS_VIDX_E,
                     hwValue);

                 break;

            case CPSS_INTERFACE_TRUNK_E:
                hwValue = 0;

                LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_USE_VIDX_E,
                hwValue);

                portTrunk = macEntryPtr->dstInterface.trunkId;
                if (portTrunk == 0)
                {
                    /* trunk ID should be > 0 */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                hwValue = portTrunk;
                LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_TRUNK_NUM_E,
                hwValue);

                hwValue = 1;

                LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_IS_TRUNK_E,
                hwValue);

                break;
            case CPSS_INTERFACE_PORT_E:

                hwValue = 0;

                LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_USE_VIDX_E,
                hwValue);

                LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_IS_TRUNK_E,
                hwValue);

                /* set port */
                hwValue = macEntryPtr->dstInterface.devPort.portNum;
                LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                     LION3_FDB_FDB_TABLE_FIELDS_EPORT_NUM_E,
                     hwValue);

                /* set hw device */
                hwValue = macEntryPtr->dstInterface.devPort.hwDevNum;
                LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                     LION3_FDB_FDB_TABLE_FIELDS_TARGET_DEVICE_E,
                     hwValue);

                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

         /* set vlan associated with this entry */
         hwValue = macEntryPtr->fdbRoutingInfo.nextHopVlanId;
         LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
             LION3_FDB_FDB_TABLE_FIELDS_NEXT_HOP_EVLAN_E,
             hwValue);

         if (macEntryPtr->fdbRoutingInfo.isTunnelStart == GT_TRUE)
         {
             /* set tunnel type */
             hwValue = 1; /* the only so far supported tunneltype is IPv4 */
             LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                 LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_TYPE_E,
                 hwValue);
             /* set tunnel ptr */
             hwValue = macEntryPtr->fdbRoutingInfo.nextHopTunnelPointer;
             LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                 LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_PTR_E,
                 hwValue);
             /* set that the entry is start of tunnel */
             hwValue = 1;
         }
         else
         {
             /* set that the entry is not a tunnel start */
             hwValue = 0;
         }
         LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
             LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_START_E,
             hwValue);

         /* arp ptr*/
         if (macEntryPtr->fdbRoutingInfo.isTunnelStart == GT_FALSE)
         {
             hwValue = macEntryPtr->fdbRoutingInfo.nextHopARPPointer;
             LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                 LION3_FDB_FDB_TABLE_FIELDS_ARP_PTR_E,
                 hwValue);
         }
         break;

    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
        {
            /* set ipv6 dip */
            GT_U32 wordNum;
            cpssOsMemCpy(dip, macEntryPtr->key.key.ipv6Unicast.dip, sizeof(dip));
            /* ipv6 destination address */
            for(wordNum = 0; wordNum < 4; wordNum++)
            {
                hwValue =                 (dip[((3 - wordNum) * 4)]<< 24)      |
                                          (dip[((3 - wordNum) * 4) + 1] << 16) |
                                          (dip[((3 - wordNum) * 4) + 2] << 8)  |
                                          (dip[((3 - wordNum) * 4) + 3]);
                LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                    LION3_FDB_FDB_TABLE_FIELDS_IPV6_DIP_0_E+wordNum,
                    hwValue);
           }
            /* set NH data bank number */
            hwValue = macEntryPtr->fdbRoutingInfo.nextHopDataBankNumber;
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_NH_DATA_BANK_NUM_E,
                hwValue);
        }
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChBrgFdbLion3FdbIpUcRoutByMessageFormatFieldsSet
*
* DESCRIPTION:
*       convert the FDB format of CPSS to DxCh FDB UC Routing message hw format
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       macEntryPtr - pointer to extended MAC entry format
*
* OUTPUTS:
*       hwDataArr   - (pointer to) hw mac entry format
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - on bad parameter error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChBrgFdbLion3FdbIpUcRoutByMessageFormatFieldsSet(
    IN  GT_U8                   devNum,
    IN  CPSS_MAC_ENTRY_EXT_STC  *macEntryPtr,
    OUT GT_U32                  hwDataArr[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]
)
{
    GT_U32      hwValue = 0;          /* hardware value of the field      */
    GT_U8       dip[16] = {0};        /* Destination IP */
    GT_U32      portTrunk;            /* port number or trunk Id         */

    switch (macEntryPtr->key.entryType)
    {
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
        /* vrf Id */
        hwValue = macEntryPtr->key.key.ipv4Unicast.vrfId;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_VRF_ID_E,
            hwValue);

        /*IPv4 UC DIP*/
        cpssOsMemCpy(dip, macEntryPtr->key.key.ipv4Unicast.dip, 4);

        hwValue = (dip[0] << 24) |
                  (dip[1] << 16) |
                  (dip[2] <<  8) |
                  (dip[3] <<  0) ;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV4_DIP_E,
            hwValue);

        /* ttl/hop decrement enable */
        hwValue = BOOL2BIT_MAC(macEntryPtr->fdbRoutingInfo.ttlHopLimitDecEnable);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_DEC_TTL_OR_HOP_COUNT_E,
            hwValue);
        /* Bypass TTL Options Or Hop Extension */
        hwValue = BOOL2BIT_MAC(macEntryPtr->fdbRoutingInfo.ttlHopLimDecOptionsExtChkByPass);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E,
            hwValue);
        /* ingress mirror to analyzer index */
        if (macEntryPtr->fdbRoutingInfo.ingressMirror)
        {
            hwValue = macEntryPtr->fdbRoutingInfo.ingressMirrorToAnalyzerIndex + 1;
        }
        else
        {
            hwValue = 0;
        }
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_INGRESS_MIRROR_TO_ANALYZER_INDEX_E,
            hwValue);
        hwValue = BOOL2BIT_MAC(macEntryPtr->fdbRoutingInfo.qosProfileMarkingEnable);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_MARKING_EN_E,
            hwValue);
        hwValue = macEntryPtr->fdbRoutingInfo.qosProfileIndex;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_INDEX_E,
            hwValue);
        switch(macEntryPtr->fdbRoutingInfo.qosPrecedence)
        {
        case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
            hwValue = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
            hwValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.qosPrecedence));
        }
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_PRECEDENCE_E,
            hwValue);

        switch(macEntryPtr->fdbRoutingInfo.modifyUp)
        {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            hwValue = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            hwValue = 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            hwValue = 1;
            break;
        default:
           CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.modifyUp));
        }

        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MODIFY_UP_E,
            hwValue);

        switch(macEntryPtr->fdbRoutingInfo.modifyDscp)
        {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            hwValue = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            hwValue = 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            hwValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.modifyDscp));
        }
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MODIFY_DSCP_E,
            hwValue);

        switch(macEntryPtr->fdbRoutingInfo.countSet)
        {
        case CPSS_IP_CNT_SET0_E:
             hwValue = 0;
            break;
        case CPSS_IP_CNT_SET1_E:
             hwValue = 1;
            break;
        case CPSS_IP_CNT_SET2_E:
             hwValue = 2;
            break;
        case CPSS_IP_CNT_SET3_E:
             hwValue = 3;
            break;
        case CPSS_IP_CNT_NO_SET_E:
            hwValue = 7;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.countSet));
        }

        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_COUNTER_SET_INDEX_E,
            hwValue);


        hwValue = BOOL2BIT_MAC(macEntryPtr->fdbRoutingInfo.trapMirrorArpBcEnable);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ARP_BC_TRAP_MIRROR_EN_E,
            hwValue);
        /* dip access level */
        hwValue = macEntryPtr->fdbRoutingInfo.dipAccessLevel;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_DIP_ACCESS_LEVEL_E,
            hwValue);

        hwValue = BOOL2BIT_MAC(macEntryPtr->fdbRoutingInfo.ICMPRedirectEnable);
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ICMP_REDIRECT_EXCEP_MIRROR_EN_E,
            hwValue);
        /* mtu profile index */
        hwValue = macEntryPtr->fdbRoutingInfo.mtuProfileIndex;
        LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MTU_INDEX_E,
            hwValue);

        switch(macEntryPtr->dstInterface.type)
        {
            case CPSS_INTERFACE_VIDX_E:
            case CPSS_INTERFACE_VID_E:
                /* use VIDX */
                if (macEntryPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
                {
                    hwValue=0xFFF;
                }
                else
                {
                     /* check range */
                     if (macEntryPtr->dstInterface.vidx > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(devNum))
                     {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->dstInterface.vidx));
                     }

                     /* set vidx value */
                     hwValue = macEntryPtr->dstInterface.vidx;
                }

                 LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                     LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_EVIDX_E,
                     hwValue);

                 hwValue = 1;

                 LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_USE_VIDX_E,
                 hwValue);

                break;

            case CPSS_INTERFACE_TRUNK_E:
                hwValue = 0;

                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_USE_VIDX_E,
                hwValue);

                portTrunk = macEntryPtr->dstInterface.trunkId;
                if (portTrunk == 0)
                {
                    /* trunk ID should be > 0 */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(portTrunk));
                }

                hwValue = portTrunk;
                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TRUNK_NUM_E,
                hwValue);

                hwValue = 1;

                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IS_TRUNK_E,
                hwValue);

                break;
            case CPSS_INTERFACE_PORT_E:

                hwValue = 0;

                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_USE_VIDX_E,
                hwValue);

                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IS_TRUNK_E,
                hwValue);

                /* set port */
                hwValue = macEntryPtr->dstInterface.devPort.portNum;
                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                     LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_EPORT_NUM_E,
                     hwValue);

                /* set hw device */
                hwValue = macEntryPtr->dstInterface.devPort.hwDevNum;
                LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                     LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TARGET_DEVICE_E,
                     hwValue);

                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->dstInterface.type));
        }

         /* set vlan associated with this entry */
         hwValue = macEntryPtr->fdbRoutingInfo.nextHopVlanId;
         LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
             LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_NEXT_HOP_EVLAN_E,
             hwValue);

         if (macEntryPtr->fdbRoutingInfo.isTunnelStart == GT_TRUE)
         {
             /* set tunnel type */
             hwValue = 1; /* the only so far supported tunneltype is IPv4 */
             LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_TYPE_E,
                 hwValue);
             /* set tunnel ptr */
             hwValue = macEntryPtr->fdbRoutingInfo.nextHopTunnelPointer;
             LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_PTR_E,
                 hwValue);
             /* set that the entry is start of tunnel */
             hwValue = 1;
         }
         else
         {
             /* set that the entry is not a tunnel start */
             hwValue = 0;
         }
         LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
             LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_START_E,
             hwValue);

         /* arp ptr*/
         if (macEntryPtr->fdbRoutingInfo.isTunnelStart == GT_FALSE)
         {
             hwValue = macEntryPtr->fdbRoutingInfo.nextHopARPPointer;
             LION3_FDB_AU_MSG_FIELD_SET_MAC(devNum,hwDataArr,
                 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ARP_PTR_E,
                 hwValue);
         }
         break;

    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->key.entryType));
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->key.entryType));
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChBrgFdbLion3FdbIpUcRouteParametersCheck
*
* DESCRIPTION:
*       check uc route parameters for validity
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       macEntryPtr - pointer to extended MAC entry format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - on bad parameter error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChBrgFdbLion3FdbIpUcRouteParametersCheck(
    IN  GT_U8                   devNum,
    IN  CPSS_MAC_ENTRY_EXT_STC  *macEntryPtr
)
{
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E)
    {
        /* check bank number */
        if (macEntryPtr->fdbRoutingInfo.nextHopDataBankNumber >= BIT_4)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.nextHopDataBankNumber));
        }
        return GT_OK;
    }

    if (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E)
    {
        if (macEntryPtr->key.key.ipv4Unicast.vrfId >= BIT_12)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->key.key.ipv4Unicast.vrfId));
        }
        if ((macEntryPtr->key.key.ipv4Unicast.dip[0] >= 224) && (macEntryPtr->key.key.ipv4Unicast.dip[0] <= 239))
        {
            /* Multicast range */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->key.key.ipv4Unicast.dip[0]));
        }
    }

    if (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E)
    {
        if (macEntryPtr->key.key.ipv6Unicast.vrfId >= BIT_12)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->key.key.ipv6Unicast.vrfId));
        }
    }

    if (macEntryPtr->fdbRoutingInfo.dipAccessLevel >= BIT_6)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.dipAccessLevel));
    }
    if (macEntryPtr->fdbRoutingInfo.qosProfileIndex >= BIT_7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.qosProfileIndex));
    }

    switch(macEntryPtr->fdbRoutingInfo.qosPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.qosPrecedence));
    }

    switch(macEntryPtr->fdbRoutingInfo.modifyUp)
    {
    case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.modifyUp));
    }

    switch(macEntryPtr->fdbRoutingInfo.modifyDscp)
    {
    case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.modifyDscp));
    }

    switch(macEntryPtr->fdbRoutingInfo.countSet)
    {
    case CPSS_IP_CNT_SET0_E:
    case CPSS_IP_CNT_SET1_E:
    case CPSS_IP_CNT_SET2_E:
    case CPSS_IP_CNT_SET3_E:
    case CPSS_IP_CNT_NO_SET_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.countSet));
    }
    if (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E)
    {
        switch(macEntryPtr->fdbRoutingInfo.siteId)
        {
        case CPSS_IP_SITE_ID_INTERNAL_E:
        case CPSS_IP_SITE_ID_EXTERNAL_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.siteId));
        }
    }
    if(macEntryPtr->fdbRoutingInfo.nextHopVlanId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.nextHopVlanId));

    if(macEntryPtr->fdbRoutingInfo.mtuProfileIndex > 7)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.mtuProfileIndex));

    if(macEntryPtr->fdbRoutingInfo.isTunnelStart == GT_TRUE)
    {
        if (macEntryPtr->fdbRoutingInfo.nextHopTunnelPointer >= fineTuningPtr->tableSize.tunnelStart)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.nextHopTunnelPointer));
    }
    else/* ARP */
    {
        if (macEntryPtr->fdbRoutingInfo.nextHopARPPointer >= fineTuningPtr->tableSize.routerArp)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_ONE_PARAM_FORMAT_MAC(macEntryPtr->fdbRoutingInfo.nextHopARPPointer));
    }
    if ((macEntryPtr->fdbRoutingInfo.ingressMirror == GT_TRUE) &&
        (macEntryPtr->fdbRoutingInfo.ingressMirrorToAnalyzerIndex > 6))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

   return GT_OK;

}
/*******************************************************************************
* prvDxChBrgFdbBuildMacEntryHwFormat
*
* DESCRIPTION:
*       convert the FDB format of CPSS to DxCh Mac hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       macEntryPtr - pointer to extended MAC entry format
*       skip        - skip bit
*
* OUTPUTS:
*       hwDataArr   - (pointer to) hw mac entry format
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - on bad parameter error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvDxChBrgFdbBuildMacEntryHwFormat(
    IN  GT_U8                   devNum,
    IN  CPSS_MAC_ENTRY_EXT_STC  *macEntryPtr,
    IN  GT_BOOL                 skip,
    OUT GT_U32                  hwDataArr[CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS]
)
{
    GT_STATUS   rc;
    GT_U32      hwValue;             /* hardware value of the field      */
    GT_U32      bit40MacAddrOrIsIpm = 0; /* bit 40 from MAC Address for MAC entry
                                        or entry type is IPM          */
    GT_U32      portTrunk;      /* port number or trunk Id         */
    GT_U32      isTrunk;        /* trunk flag                      */
    GT_U32      devTmp;         /* entry device number             */
    GT_U32      multiple;       /* if "1", forward packet to all
                                   ports in VIDX/VLAN */
    GT_U16      vidx = 0;
    GT_U16      vlanId = 0;      /* vlan ID */
    GT_U32      entryType = 0;  /* the only type supported
                                   0 -- mac entry
                                   1 -- ipmcV4 entry
                                   2 -- ipmcV6 entry */
    GT_U32  macLow16 = 0;         /* two low bytes of MAC Address */
    GT_U32  macHi32 = 0;          /* four high bytes of MAC Address */
    GT_U8   sip[4];              /* Source IP */
    GT_U8   dip[4];              /* Destination IP */
    GT_U32  saCmd;               /* command when the entry matches source
                                    address lookup for MAC SA entry */
    PRV_CPSS_DXCH_LION3_FDB_SPECIAL_MUXED_FIELDS_STC specialFields;/*special Muxed fields in the AU msg */

    /* reset all words in hwDataArr */
    cpssOsMemSet((char *) &hwDataArr[0], 0,
             CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS * sizeof(GT_U32));

    /* check entry type */
    switch (macEntryPtr->key.entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
            macLow16 = GT_HW_MAC_LOW16(&(macEntryPtr->key.key.macVlan.macAddr));
            macHi32 = GT_HW_MAC_HIGH32(&(macEntryPtr->key.key.macVlan.macAddr));

            /* get bit 40 from MAC Address */
            bit40MacAddrOrIsIpm = (macHi32 >> 24) & 0x1;

            vlanId = macEntryPtr->key.key.macVlan.vlanId;
            entryType = 0;
            break;
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
            cpssOsMemCpy(dip, macEntryPtr->key.key.ipMcast.dip, 4);
            cpssOsMemCpy(sip, macEntryPtr->key.key.ipMcast.sip, 4);
            vlanId = macEntryPtr->key.key.ipMcast.vlanId;

            if (macEntryPtr->key.entryType ==
                CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E)
                entryType = 1;
            else
                entryType = 2;

            /* set flag that entry is IPM*/
            bit40MacAddrOrIsIpm = 1;
            break;
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
            /* those types are supported only for e_arch devices */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                if (macEntryPtr->key.entryType ==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E)
                {
                    entryType = 3;
                }
                if (macEntryPtr->key.entryType ==CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E)
                {
                    entryType = 5;
                }
                if (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E)
                {
                    entryType = 4;
                }
                rc = prvCpssDxChBrgFdbLion3FdbIpUcRouteParametersCheck(devNum,macEntryPtr);
                if (rc != GT_OK)
                {
                    return rc;
                }
                break;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check interface destination type */
    switch(macEntryPtr->dstInterface.type)
    {
        case CPSS_INTERFACE_PORT_E:
            /* check that type of the entry is MAC and address
               is not multicast */
            if (bit40MacAddrOrIsIpm == 0)
            {
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                    macEntryPtr->dstInterface.devPort.hwDevNum,
                    macEntryPtr->dstInterface.devPort.portNum);

                portTrunk = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                    macEntryPtr->dstInterface.devPort.hwDevNum,
                    macEntryPtr->dstInterface.devPort.portNum);

                devTmp = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                    macEntryPtr->dstInterface.devPort.hwDevNum,
                    macEntryPtr->dstInterface.devPort.portNum);

                isTrunk = 0;
                multiple = 0;

                if(portTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            else
            {
                /* For multicast MAC or IPM entries the VIDX or VLAN
                   target must be used but not port */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;

        case CPSS_INTERFACE_TRUNK_E:
            /* check that type of the entry is MAC and address
               is not multicast */
            if (bit40MacAddrOrIsIpm == 0)
            {
                portTrunk = macEntryPtr->dstInterface.trunkId;
                if (portTrunk == 0)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                isTrunk = 1;
                devTmp = macEntryPtr->dstInterface.hwDevNum;
                multiple = 0;

                if(portTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            else
            {
                /* For multicast MAC or IPM entries the VIDX or VLAN
                   target must be used but not trunk */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;

        case CPSS_INTERFACE_VIDX_E:
        case CPSS_INTERFACE_VID_E:
            devTmp = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
            if(macEntryPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
            {
                vidx = 0xFFF;
            }
            else /*CPSS_INTERFACE_VIDX_E*/
            {
                vidx = macEntryPtr->dstInterface.vidx;
                /* check range */
                if (vidx > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(devNum))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            multiple = 1;

            /* set variables to avoid compilation warnings */
            portTrunk = 0;
            isTrunk = 0;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check ranges */
    if (((vlanId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum)) ||
        (devTmp > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum)) ||
        (macEntryPtr->userDefined > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_FDB_USER_DEFINED_MAC(devNum)) ||
        (macEntryPtr->saQosIndex >= BIT_3) ||
        (macEntryPtr->daQosIndex >= BIT_3)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }


    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* valid */
        hwValue = 1;
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_VALID_E,
            hwValue);

        /* skip */
        hwValue = BOOL2BIT_MAC(skip);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_SKIP_E,
            hwValue);

        /* age */
        hwValue = BOOL2BIT_MAC(macEntryPtr->age);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_AGE_E,
            hwValue);

        /* entry type */
        hwValue = entryType;
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_FDB_ENTRY_TYPE_E,
            hwValue);

        if ((macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E)||
            (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E)||
            (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E) )
        {
            return prvCpssDxChBrgFdbLion3FdbIpUcRouteFieldsSet(devNum,macEntryPtr,hwDataArr);
        }
        /* FID */
        hwValue = vlanId;
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_FID_E,
            hwValue);

        /* handle muxed fields that depends on global configurations */
        specialFields.srcId = macEntryPtr->sourceID;
        specialFields.udb = macEntryPtr->userDefined;
        specialFields.origVid1  = macEntryPtr->key.vid1;
        specialFields.daAccessLevel = macEntryPtr->daSecurityLevel;
        specialFields.saAccessLevel = macEntryPtr->saSecurityLevel;

        rc = prvCpssDxChBrgFdbLion3FdbSpecialMuxedFieldsSet(devNum,hwDataArr,entryType,&specialFields);
        if(rc != GT_OK)
        {
            return rc;
        }

        if ((multiple == 1) ||
            (entryType == 1 || entryType == 2))
        {
            /* VIDX */
            hwValue = vidx;
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_VIDX_E,
                hwValue);
        }

        if (entryType == 0)
        {
            /* MAC entry */

            /* MacAddr */

            /* MAC entry */
            LION3_FDB_FDB_FIELD_MAC_ADDR_SET_MAC(devNum, hwDataArr,
                &macEntryPtr->key.key.macVlan.macAddr.arEther[0]);

            /* devNum ID */
            hwValue = devTmp;
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_DEV_ID_E,
                hwValue);

            if (multiple == 0)
            {/* bit40MacAddr=0 and entry is PORT or TRUNK */

                /* is Trunk bit */
                hwValue = isTrunk;
                LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                    LION3_FDB_FDB_TABLE_FIELDS_IS_TRUNK_E,
                    hwValue);

                /* PortNum/TrunkNum */
                if(isTrunk)
                {
                    hwValue = portTrunk;
                    LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                        LION3_FDB_FDB_TABLE_FIELDS_TRUNK_NUM_E,
                        hwValue);
                }
                else
                {
                    hwValue = portTrunk;
                    LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                        LION3_FDB_FDB_TABLE_FIELDS_EPORT_NUM_E,
                        hwValue);
                }
            }

            /* multiple */
            hwValue = multiple;
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_MULTIPLE_E,
                hwValue);

            /* SA cmd */
            PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(hwValue,
                                                           macEntryPtr->saCommand);
            LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                LION3_FDB_FDB_TABLE_FIELDS_SA_CMD_E,
                hwValue);
        }
        else
        {
            /* Entry type ipmcV4 or ipmcV6 */
            if (entryType == 1 || entryType == 2)
            {
                /* set IP Addresses */

                /*DIP*/
                hwValue = (dip[0] << 24) |
                          (dip[1] << 16) |
                          (dip[2] <<  8) |
                          (dip[3] <<  0) ;
                LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                    LION3_FDB_FDB_TABLE_FIELDS_DIP_E,
                    hwValue);

                /*SIP*/
                hwValue = (sip[0] << 24) |
                          (sip[1] << 16) |
                          (sip[2] <<  8) |
                          (sip[3] <<  0) ;
                LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
                    LION3_FDB_FDB_TABLE_FIELDS_SIP_E,
                    hwValue);
            }
        }

        /* static */
        hwValue = BIT2BOOL_MAC(macEntryPtr->isStatic);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_IS_STATIC_E,
            hwValue);

        /* DA cmd */
        PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(hwValue,
                                                       macEntryPtr->daCommand);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_DA_CMD_E,
            hwValue);

        /* DA Route */
        hwValue = BIT2BOOL_MAC(macEntryPtr->daRoute);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_DA_ROUTE_E,
            hwValue);

        /* spUnknown - NA strorm prevent entry */
        hwValue = BIT2BOOL_MAC(macEntryPtr->spUnknown);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_SP_UNKNOWN_E,
            hwValue);


        /* SaQosProfileIndex */
        hwValue = macEntryPtr->saQosIndex;
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_SA_QOS_PARAM_SET_IDX_E,
            hwValue);

        /* DaQosProfileIndex */
        hwValue = macEntryPtr->daQosIndex;
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_DA_QOS_PARAM_SET_IDX_E,
            hwValue);

        /* SA Lookup Ingress Mirror to Analyzer Enable */
        hwValue = BIT2BOOL_MAC(macEntryPtr->saMirrorToRxAnalyzerPortEn);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_SA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E,
            hwValue);

        /* DA Lookup Ingress Mirror to Analyzer Enable */
        hwValue = BIT2BOOL_MAC(macEntryPtr->daMirrorToRxAnalyzerPortEn);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_DA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E,
            hwValue);

        /* AppSpecific CPU Code */
        hwValue = BIT2BOOL_MAC(macEntryPtr->appSpecificCpuCode);
        LION3_FDB_FDB_FIELD_SET_MAC(devNum,hwDataArr,
            LION3_FDB_FDB_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_E,
            hwValue);

        return GT_OK;
    }

    /* NON E_ARCH device */

    /* valid bit */
    hwDataArr[0] = 0x1;

    /* skip */
    hwDataArr[0] |= (BOOL2BIT_MAC(skip) << 1);

    /* age */
    hwDataArr[0] |= (BOOL2BIT_MAC(macEntryPtr->age) << 2);

    /* entry type */
    hwDataArr[0] |= (entryType << 3);

    /* VID */
    hwDataArr[0] |= ((vlanId & 0xFFF) << 5);

    if (entryType == 0)
    {
        /* MAC entry */

        /* MacAddr[14..0] */
        hwDataArr[0] |= ((macLow16 & 0x7FFF) << 17); /* Word0, bits 17-31 */

        /* MacAddr[15..46] */
        hwDataArr[1] = (macLow16 >> 15) & 0x1; /* highest bit from mac second byte */
        hwDataArr[1] |= (macHi32 << 1);

        /* MacAddr[47] */
        hwDataArr[2] |= ((macHi32 >> 31) & 0x1); /* the highest bit fro Mac Address */

        /* devNum ID 5 bits */
        hwDataArr[2] |= ((devTmp & 0x1F) << 1);

        /* Src ID */
        /* check range */
        if (macEntryPtr->sourceID > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_SRC_ID_MAC(devNum))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
        hwDataArr[2] |= ((macEntryPtr->sourceID & 0x1f) << 6);

        /* check if bit40MacAddr==0 or entry is VIDX or VID */
        if (multiple == 1)
        {
            /* VIDX - 12 bits*/
            hwDataArr[2] |= ((vidx & 0xFFF) << 13);
        }
        else
        {/* bit40MacAddr=0 and entry is PORT or TRUNK */

            /* is Trunk bit */
            hwDataArr[2] |= (isTrunk << 13);

            /* PortNum/TrunkNum */
            hwDataArr[2] |= ((portTrunk & 0x7f) << 14);

            /* UserDefined 4 bits */
            hwDataArr[2] |= ((macEntryPtr->userDefined & 0xF) << 21);
        }

        /* multiple */
        hwDataArr[2] |= (multiple << 26);

        /* SA cmd[1..0] */
        PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(saCmd,
                                                       macEntryPtr->saCommand);
        hwDataArr[2] |= ((saCmd & 0x3) << 30);

        /* SA cmd[2] */
        hwDataArr[3] |= ((saCmd >> 2) & 0x1);
    }
    else
    {
        /* Entry type ipmcV4 or ipmcV6 */
        if (entryType == 1 || entryType == 2)
        {
            /* IPM entry */

            /* DIP[14..0] */
            /* Word0, bits 17-31 */
            hwDataArr[0] |= ((dip[3] | ((dip[2] & 0x7F) << 8)) << 17);

            /* DIP[31..15] */
            /* Word1, bits 0-16 */
            hwDataArr[1] = ((dip[2] >> 7) | (dip[1] << 1) | (dip[0] << 9));

            /* SIP[14..0] */
            /* Word1, bits 17-31 */
            hwDataArr[1] |= ((sip[3] | ((sip[2] & 0x7F) << 8)) << 17);

            /* SIP[27..15] */
            /* Word2, bits 0-12 */
            hwDataArr[2] |= ((sip[2] >> 7) | (sip[1] << 1) | ((sip[0] & 0xF) << 9));

            /* VIDX 12 bits */
            hwDataArr[2] |= ((vidx & 0xFFF) << 13);

            /* SIP[28] */
            /* Word2, bit 26 */
            hwDataArr[2] |= (((sip[0] >> 4) & 0x1) << 26);

            /* SIP[30..29] */
            /* Word2, bits 30-31 */
            hwDataArr[2] |= (((sip[0] >> 5) & 0x3) << 30);

            /* SIP[31] */
            /* Word0, bit 31 */
            hwDataArr[3] |= ((sip[0] >> 7) & 0x1);
        }
    }

    /* static */
    hwDataArr[2] |= (BIT2BOOL_MAC(macEntryPtr->isStatic) << 25);

    /* DA cmd */
    PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(hwValue,
                                                   macEntryPtr->daCommand);
    hwDataArr[2] |= (hwValue << 27);

    /* DA Route */
    hwDataArr[3] |= (BOOL2BIT_MAC(macEntryPtr->daRoute) << 1);

    /* spUnknown - NA strorm prevent entry */
    hwDataArr[3] |= (BOOL2BIT_MAC(macEntryPtr->spUnknown) << 2);

    /* SaQosProfileIndex */
    hwDataArr[3] |= (macEntryPtr->saQosIndex << 3);

    /* DaQosProfileIndex */
    hwDataArr[3] |= (macEntryPtr->daQosIndex << 6);

    /* MirrorToAnalyzerPort*/
    hwDataArr[3] |= ((BOOL2BIT_MAC(macEntryPtr->mirrorToRxAnalyzerPortEn)) << 9);

    /* set Cheetah2 only fields */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
    {
        if ((macEntryPtr->daSecurityLevel >= BIT_3) ||
            (macEntryPtr->saSecurityLevel >= BIT_3))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        /* AppSpecific CPU Code */
        hwDataArr[3] |= ((BOOL2BIT_MAC(macEntryPtr->appSpecificCpuCode)) << 10);

        /* DA Access Level */
        hwDataArr[3] |= (macEntryPtr->daSecurityLevel << 11);

        /* SA Access Level */
        hwDataArr[3] |= (macEntryPtr->saSecurityLevel << 14);
    }

    return GT_OK;
}


/*******************************************************************************
* prvDxChFdbSendAuMessage
*
* DESCRIPTION:
*       function send Au message to PP
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portGroupId - the portGroupId. to support multi-port-groups device
*       hwDataArr   - (pointer to) AU message format
*                            data to write to register :
*                            hwDataArr[0] -- FDB entry word 0
*                            hwDataArr[1] -- FDB entry word 1
*                            hwDataArr[2] -- FDB entry word 2
*                            hwDataArr[3] -- FDB entry word 3
*                            hwDataArr[4] -- control register
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error  ---
*            A fail can return due to any of the following:
*             1. The message type is NA and the hash-chain has reached its
*                maximum length
*             2. The message type is AA and the FDB entry does not exist
*             3. The message type is QA and the FDB entry does not exist
*
*
*       GT_HW_ERROR - on hardware error
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvDxChFdbSendAuMessage(
    IN  GT_U8           devNum,
    IN  GT_U32          portGroupId,
    IN  GT_U32          hwDataArr[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]
)
{
    GT_STATUS   rc;                     /* return error code */
    GT_U32      addrUpdateControlReg;   /* address of the control reg */
    GT_U32      addrUpdateReg;          /* hw memory address to write */
    GT_U32      numWordsInAuMsg;        /* number of words in the message */

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        numWordsInAuMsg = 6;
        addrUpdateControlReg = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAddrUpdateMsgs.AUMsgFromCPU.AUMsgFromCPUCtrl;
    }
    else
    {
        numWordsInAuMsg = 4;
        /* non-direct table --- this is the address of the control reg */
        addrUpdateControlReg = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            globalRegs.addrUpdateControlReg ;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        addrUpdateReg = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAddrUpdateMsgs.AUMsgFromCPU.AUMsgFromCPUWord[0];
    }
    else
    {
        /* non-direct table --- this is the address of the data reg */
        addrUpdateReg = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.addrUpdate[0];
    }

    /* check that specific/all port groups are ready */
    rc = prvCpssPortGroupBusyWait(devNum,portGroupId,
                addrUpdateControlReg,0,
                GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if (rc != GT_OK)
    {
        return rc;
    }

    /* non-direct table --- write to it as continues memory
       better performance then 4 registers in a roll  */
    rc = prvCpssHwPpPortGroupWriteRam(devNum,portGroupId,
                                addrUpdateReg,
                                numWordsInAuMsg,
                                &hwDataArr[0]);

    if(rc != GT_OK)
    {
        return rc;
    }

    /* Start FDB messaging */
    rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId, addrUpdateControlReg, 1);
    return rc;
}

/*******************************************************************************
* prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet
*
* DESCRIPTION:
*       Set the field mask for 'MAC entry does NOT reside on the local port group'
*       When Enabled - AU/FU messages are not sent to the CPU if the MAC entry
*                      does NOT reside on the local core, i.e.
*                      the entry port[5:4] != LocalPortGroup
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*                      --ONLY for multi-Port Group device
*
* INPUTS:
*       devNum      - device number
*       enable      - mask enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*    GT_OK           - on success
*    GT_HW_ERROR     - on HW error
*    GT_BAD_PARAM    - on bad device number.
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*       none
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;    /* The register's address of l2IngressCtrl reg. */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);

    /* save the state to DB */
    PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E.
            filterEnabled = enable;

    /* when application asked to do the filtering in the SW , in addition to
       the HW support */

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                  PRV_CPSS_DXCH_LION_PREVENT_REDUNDANT_AU_FU_MESSAGES_NOT_SUPPORTED_WA_E))
    {
        return GT_OK;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 26, 1, BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableGet
*
* DESCRIPTION:
*       Get the field mask for 'MAC entry does NOT reside on the local port group'
*       When Enabled - AU/FU messages are not sent to the CPU if the MAC entry
*                      does NOT reside on the local core, i.e.
*                      the entry port[5:4] != LocalPortGroup
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*                      --ONLY for multi-Port Group device
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) mask enable/disable
*
* RETURNS:
*    GT_OK           - on success
*    GT_HW_ERROR     - on HW error
*    GT_BAD_PARAM    - on bad device number.
*    GT_BAD_PTR      - on NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*       none
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS   rc ;        /* return code */
    GT_U32      regAddr; /* Register address */
    GT_U32      hwValue; /* HW value of register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                  PRV_CPSS_DXCH_LION_PREVENT_REDUNDANT_AU_FU_MESSAGES_NOT_SUPPORTED_WA_E))
    {
        /* when we not set the HW we need to get it from the DB */

        /* get info from the DB */
        *enablePtr = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E.
                filterEnabled;

        return GT_OK;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 26, 1, &hwValue);

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}


/*******************************************************************************
* multiPortGroupCheckAndMessageFilterResolve
*
* DESCRIPTION:
*       1. check that the action mode supported for by the FDB mode.
*       2. resolve the state of the AUQ/FUQ due to 'non source port group'
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*                      --ONLY for multi-Port Group device
*
* INPUTS:
*       devNum         - device number
*       daRefreshGiven - is the 'daRefresh' parameter supplied by called
*       daRefresh      - da refresh value
*       fdbUploadGiven - is the 'fdbUpload' parameter supplied by called
*       fdbUpload      - FDB upload value
*       actionModeGiven - is the 'actionMode' parameter supplied by called
*       actionMode      - action mode value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*    GT_OK           - on success
*    GT_HW_ERROR     - on HW error
*    GT_BAD_PARAM    - on bad device number.
*    GT_BAD_STATE    - the setting not allowed in current state of configuration.
*
* COMMENTS:
*       none
*
*******************************************************************************/
static GT_STATUS multiPortGroupCheckAndMessageFilterResolve
(
    IN GT_U8    devNum,
    IN GT_BOOL  daRefreshGiven,
    IN GT_BOOL  daRefresh,
    IN GT_BOOL  fdbUploadGiven,
    IN GT_BOOL  fdbUpload,
    IN GT_BOOL  actionModeGiven,
    IN CPSS_FDB_ACTION_MODE_ENT  actionMode
)
{
    GT_STATUS   rc;     /* return code*/
    GT_BOOL     filterEnable = GT_FALSE;/* filter enable/disable*/
    GT_BOOL     modifyFilter = GT_FALSE;/* does filter need modification */
    GT_BOOL     needToUseFdbUpload = GT_FALSE;/* do we need FdbUpload ? */
    GT_BOOL     needToUseDaRefresh = GT_FALSE;/* do we need DaRefresh ? */

    if(actionModeGiven == GT_FALSE)
    {
        /* read from HW the current state */
        rc = cpssDxChBrgFdbActionModeGet(devNum, &actionMode);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    switch (actionMode)
    {
        case CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E:
            switch(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode)
            {
                case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E:
                case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
                    /*
                    This type of operation is not supported , due to the control
                    learning being used, the risk of AA messages being dropped & the
                    need to avoid dropping MAC entries from portGroups which are not
                    the source of these entries (meaning they would never be refreshed)
                    */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                default:
                    break;
            }

            needToUseDaRefresh = GT_TRUE;
            break;
        case CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E:
            needToUseFdbUpload = GT_TRUE;
            needToUseDaRefresh = GT_TRUE;
            break;
        case CPSS_FDB_ACTION_TRANSPLANTING_E:
            switch(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode)
            {
                case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
                case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E:
                    /* not supported.
                      because triggering this would possibly result with entries which
                      have their new 'source' information properly set - but these entries
                      would not be relocated to the new source portGroups, meaning that
                      packet received from new station location (which was supposed to
                      be transplanted), may still create NA message.
                      Computing new source portGroup is not always possible, especially
                      when station is relocated to trunk or it was previously residing
                      on multiple-portGroups.
                      Application is required to perform Flush operation instead.
                      This would clear previously learnt MAC
                    */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                default:
                    break;
            }
            break;
        default:
            break;
    }/*switch (actionMode)*/

    if(needToUseFdbUpload == GT_TRUE && fdbUploadGiven == GT_FALSE)
    {
        /* read from HW the current state */
        rc = cpssDxChBrgFdbUploadEnableGet(devNum, &fdbUpload);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(needToUseDaRefresh == GT_TRUE && daRefreshGiven == GT_FALSE)
    {
        /* read from HW the current state */
        rc = cpssDxChBrgFdbAgeBitDaRefreshEnableGet(devNum, &daRefresh);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /********************************************/
    /* resolve the state of the messages filter */
    /********************************************/


    switch (actionMode)
    {
        case CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E:
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode ==
               PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E)
            {
                modifyFilter = GT_TRUE;
                filterEnable = daRefresh == GT_TRUE ? GT_FALSE : GT_TRUE;
            }
            break;
        case CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E:
            modifyFilter = GT_TRUE;
            if(fdbUpload == GT_TRUE)
            {
                filterEnable = GT_TRUE;
            }
            else
            {
                filterEnable = daRefresh == GT_TRUE ? GT_FALSE : GT_TRUE;
            }
            break;
        case CPSS_FDB_ACTION_DELETING_E:
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode ==
                PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E)
            {
                modifyFilter = GT_TRUE;
                filterEnable = GT_TRUE;
            }
            break;
        case CPSS_FDB_ACTION_TRANSPLANTING_E:
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode ==
                PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E)
            {
                modifyFilter = GT_TRUE;
                filterEnable = GT_TRUE;
            }
        default:
            break;
    }/*switch (actionMode)*/

    if(modifyFilter == GT_TRUE)
    {
        rc = prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet(devNum,filterEnable);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChBrgFdbPortLearnStatusSet
*
* DESCRIPTION:
*       Enable/disable learning of new source MAC addresses for packets received
*       on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*       status - GT_TRUE for enable  or GT_FALSE otherwise
*       cmd    - how to forward packets with unknown/changed SA,
*                 if status is GT_FALSE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on unknown command
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*       1. Unified FDBs mode and Unified-Linked FDBs mode :
*           Automatic learning is not recommended in this mode, due to the fact
*           that FDBs are supposed to be synced.
*       2. Linked FDBs mode - no limitations
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortLearnStatusSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  status,
    IN CPSS_PORT_LOCK_CMD_ENT   cmd
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    /* invert value,
       1 - Automatic learning is disabled.
       0 - Automatic learning is enable. */
    value = (status == GT_TRUE) ? 0 : 1;


    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <AUTO_LEARN_DIS>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_AUTO_LEARN_DIS_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* set Auto-learning */
        if (status == GT_FALSE)
        {
            /* set command */
            switch (cmd)
            {
                case CPSS_LOCK_FRWRD_E:
                    value = 0;
                    break;
                case CPSS_LOCK_DROP_E:
                    value = 3;
                    break;
                case CPSS_LOCK_TRAP_E:
                    value = 2;
                    break;
                case CPSS_LOCK_MIRROR_E:
                    value = 1;
                    break;
                case CPSS_LOCK_SOFT_DROP_E:
                    value = 4;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
         }
         else
         {
            value = 0;/* not relevant to 'auto learn' */
         }

        /* set the <UNKNOWN_SRC_ADDR_CMD>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_UNKNOWN_SRC_ADDR_CMD_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);

    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        if (status == GT_FALSE)
        {
            /* set command */
            switch (cmd)
            {
                case CPSS_LOCK_FRWRD_E:
                    break;
                case CPSS_LOCK_DROP_E:
                    value |= (3 << 1);
                    break;
                case CPSS_LOCK_TRAP_E:
                    value |= (2 << 1);
                    break;
                case CPSS_LOCK_MIRROR_E:
                     value |= (1 << 1);
                    break;
                case CPSS_LOCK_SOFT_DROP_E:
                     value |= (4 << 1);
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }

        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
        regAddr, 15, 4, value);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbPortLearnStatusSet
*
* DESCRIPTION:
*       Enable/disable learning of new source MAC addresses for packets received
*       on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*       status - GT_TRUE for enable  or GT_FALSE otherwise
*       cmd    - how to forward packets with unknown/changed SA,
*                 if status is GT_FALSE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on unknown command
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*       1. Unified FDBs mode and Unified-Linked FDBs mode :
*           Automatic learning is not recommended in this mode, due to the fact
*           that FDBs are supposed to be synced.
*       2. Linked FDBs mode - no limitations
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortLearnStatusSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  status,
    IN CPSS_PORT_LOCK_CMD_ENT   cmd
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortLearnStatusSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, status, cmd));

    rc = internal_cpssDxChBrgFdbPortLearnStatusSet(devNum, portNum, status, cmd);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, status, cmd));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortLearnStatusGet
*
* DESCRIPTION:
*       Get state of new source MAC addresses learning on packets received
*       on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*
* OUTPUTS:
*       statusPtr - (pointer to) GT_TRUE for enable  or GT_FALSE otherwise
*       cmdPtr    - (pointer to) how to forward packets with unknown/changed SA,
*                   when (*statusPtr) is GT_FALSE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortLearnStatusGet
(
    IN  GT_U8   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT GT_BOOL *statusPtr,
    OUT CPSS_PORT_LOCK_CMD_ENT *cmdPtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);
    CPSS_NULL_PTR_CHECK_MAC(cmdPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* read from Bridge-Ingress-ePort table */

        /* get <AUTO_LEARN_DIS> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_AUTO_LEARN_DIS_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

        if(rc != GT_OK)
            return rc;

        /* invert value,
        1 - Automatic learning is disabled.
        0 - Automatic learning is enable. */
        *statusPtr = (value & 0x01) ? GT_FALSE : GT_TRUE;

        /* get the <UNKNOWN_SRC_ADDR_CMD>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_UNKNOWN_SRC_ADDR_CMD_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);
        if(rc != GT_OK)
            return rc;


        /* translate hw cmd to sw cmd */
        switch (value)
        {
            case 0:
                *cmdPtr = CPSS_LOCK_FRWRD_E;
                break;
            case 1:
                *cmdPtr = CPSS_LOCK_MIRROR_E;
                break;
            case 2:
                *cmdPtr = CPSS_LOCK_TRAP_E;
                break;
            case 3:
                *cmdPtr = CPSS_LOCK_DROP_E;
                break;
            case 4:
                *cmdPtr = CPSS_LOCK_SOFT_DROP_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 15, 4, &value);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* invert value,
           1 - Automatic learning is disabled.
           0 - Automatic learning is enable. */
        *statusPtr = (value & 0x01) ? GT_FALSE : GT_TRUE;

        value = value >> 1;

        /* translate hw cmd to sw cmd */
        switch (value)
        {
            case 0:
                *cmdPtr = CPSS_LOCK_FRWRD_E;
                break;
            case 1:
                *cmdPtr = CPSS_LOCK_MIRROR_E;
                break;
            case 2:
                *cmdPtr = CPSS_LOCK_TRAP_E;
                break;
            case 3:
                *cmdPtr = CPSS_LOCK_DROP_E;
                break;
            case 4:
                *cmdPtr = CPSS_LOCK_SOFT_DROP_E;
                break;
            default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortLearnStatusGet
*
* DESCRIPTION:
*       Get state of new source MAC addresses learning on packets received
*       on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*
* OUTPUTS:
*       statusPtr - (pointer to) GT_TRUE for enable  or GT_FALSE otherwise
*       cmdPtr    - (pointer to) how to forward packets with unknown/changed SA,
*                   when (*statusPtr) is GT_FALSE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortLearnStatusGet
(
    IN  GT_U8   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT GT_BOOL *statusPtr,
    OUT CPSS_PORT_LOCK_CMD_ENT *cmdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortLearnStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, statusPtr, cmdPtr));

    rc = internal_cpssDxChBrgFdbPortLearnStatusGet(devNum, portNum, statusPtr, cmdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, statusPtr, cmdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbNaToCpuPerPortSet
*
* DESCRIPTION:
*       Enable/disable forwarding a new mac address message to CPU --per port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number, CPU port
*       enable  - If GT_TRUE, forward NA message to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbNaToCpuPerPortSet
(
    IN GT_U8    devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    value = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <NA_MSG_TO_CPU_EN>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_NA_MSG_TO_CPU_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);

    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
             regAddr,0, 1, value);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbNaToCpuPerPortSet
*
* DESCRIPTION:
*       Enable/disable forwarding a new mac address message to CPU --per port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number, CPU port
*       enable  - If GT_TRUE, forward NA message to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaToCpuPerPortSet
(
    IN GT_U8    devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbNaToCpuPerPortSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgFdbNaToCpuPerPortSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbNaToCpuPerPortGet
*
* DESCRIPTION:
*       Get Enable/disable forwarding a new mac address message to CPU --
*       per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number, CPU port
*
* OUTPUTS:
*       enablePtr  - If GT_TRUE, NA message is forwarded to CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbNaToCpuPerPortGet
(
    IN  GT_U8    devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      value;      /* value to write into register */
    GT_STATUS   rc;         /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <NA_MSG_TO_CPU_EN>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_NA_MSG_TO_CPU_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);
         if(rc != GT_OK)
            return rc;

    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum, portNum);

        rc = prvCpssHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 0, 1, &value);

        if(rc != GT_OK)
            return rc;
    }
    *enablePtr = BIT2BOOL_MAC(value);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbNaToCpuPerPortGet
*
* DESCRIPTION:
*       Get Enable/disable forwarding a new mac address message to CPU --
*       per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number, CPU port
*
* OUTPUTS:
*       enablePtr  - If GT_TRUE, NA message is forwarded to CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaToCpuPerPortGet
(
    IN  GT_U8    devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbNaToCpuPerPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgFdbNaToCpuPerPortGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbNaStormPreventSet
*
* DESCRIPTION:
*   Enable/Disable New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       enable     - GT_TRUE - enable NA Storm Prevention (SP),
*                    GT_FALSE - disable NA Storm Prevention
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/portNum/auMsgType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for multi-port groups device :
*       Enabling this mechanism in any of the ports, creates a situation where the
*       different portGroups populate different MACs at the same indexes,
*       causing FDBs to become unsynchronized, this leads to the situation that once
*       sending MAC update to different portGroups from CPU, it may succeed on some
*       of the portGroups, while failing on the others (due to SP entries already
*       occupying indexes)... So the only way to bypass these problems, assuming SP
*       is a must, is to add new MACs by exact index - this mode of operation
*       overwrites any SP entry currently there.
*       Application that enables the SP in any of the ports should not use the NA
*       sending mechanism (cpssDxChBrgFdbMacEntrySet / cpssDxChBrgFdbPortGroupMacEntrySet APIs)
*       to add new MAC addresses, especially to multiple portGroups -
*       as this may fail on some of the portGroups.
*       Removing or updating existing (non-SP) entries may still use the NA messages.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbNaStormPreventSet
(
    IN GT_U8                       devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL                     enable
)
{
    GT_U32    portControl;       /* register address */
    GT_STATUS rc = GT_OK;        /* return code */
    GT_U32    data;              /* reg subfield data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    data = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <NA_STORM_PREV_EN>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_NA_STORM_PREV_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        data);
    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        portControl = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            portControl, 19, 1, data);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbNaStormPreventSet
*
* DESCRIPTION:
*   Enable/Disable New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       enable     - GT_TRUE - enable NA Storm Prevention (SP),
*                    GT_FALSE - disable NA Storm Prevention
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/portNum/auMsgType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for multi-port groups device :
*       Enabling this mechanism in any of the ports, creates a situation where the
*       different portGroups populate different MACs at the same indexes,
*       causing FDBs to become unsynchronized, this leads to the situation that once
*       sending MAC update to different portGroups from CPU, it may succeed on some
*       of the portGroups, while failing on the others (due to SP entries already
*       occupying indexes)... So the only way to bypass these problems, assuming SP
*       is a must, is to add new MACs by exact index - this mode of operation
*       overwrites any SP entry currently there.
*       Application that enables the SP in any of the ports should not use the NA
*       sending mechanism (cpssDxChBrgFdbMacEntrySet / cpssDxChBrgFdbPortGroupMacEntrySet APIs)
*       to add new MAC addresses, especially to multiple portGroups -
*       as this may fail on some of the portGroups.
*       Removing or updating existing (non-SP) entries may still use the NA messages.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaStormPreventSet
(
    IN GT_U8                       devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL                     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbNaStormPreventSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgFdbNaStormPreventSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbNaStormPreventGet
*
* DESCRIPTION:
*   Get status of New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr     - pointer to the status of repeated NA CPU messages
*                       GT_TRUE - NA Storm Prevention (SP) is enabled,
*                       GT_FALSE - NA Storm Prevention is disabled
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/portNum/auMsgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbNaStormPreventGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32    portControl;       /* register address */
    GT_STATUS rc = GT_OK;        /* return code */
    GT_U32    data;              /* reg subfield data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <NA_STORM_PREV_EN>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_NA_STORM_PREV_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &data);
    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        portControl = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            portControl, 19, 1, &data);
    }

    if(rc != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbNaStormPreventGet
*
* DESCRIPTION:
*   Get status of New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr     - pointer to the status of repeated NA CPU messages
*                       GT_TRUE - NA Storm Prevention (SP) is enabled,
*                       GT_FALSE - NA Storm Prevention is disabled
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/portNum/auMsgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaStormPreventGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbNaStormPreventGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgFdbNaStormPreventGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortVid1LearningEnableSet
*
* DESCRIPTION:
*   Enable/Disable copying Tag1 VID from packet to FDB entry
*   at automatic learning.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       enable     - GT_TRUE - enable copying Tag1 VID from packet to FDB entry,
*                    GT_FALSE - disable, the Tag1Vid in FDB will be 0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortVid1LearningEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc ;        /* return code         */
    GT_U32    data;       /* TABLE subfield data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    data = BOOL2BIT_MAC(enable);

    /* set the <Enable Learning Of TAG1 VID>*/
    rc = prvCpssDxChWriteTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
        portNum,
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
        LION3_L2I_EPORT_TABLE_FIELDS_EN_LEARN_ORIG_TAG1_VID_E, /* field name */
        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
        data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbPortVid1LearningEnableSet
*
* DESCRIPTION:
*   Enable/Disable copying Tag1 VID from packet to FDB entry
*   at automatic learning.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       enable     - GT_TRUE - enable copying Tag1 VID from packet to FDB entry,
*                    GT_FALSE - disable, the Tag1Vid in FDB will be 0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortVid1LearningEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortVid1LearningEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgFdbPortVid1LearningEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortVid1LearningEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable status of copying Tag1 VID from packet to FDB entry
*   at automatic learning.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE - enable copying Tag1 VID from packet to FDB entry,
*                    GT_FALSE - disable, the Tag1Vid in FDB will be 0
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortVid1LearningEnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
)
{
    GT_STATUS rc ;        /* return code         */
    GT_U32    data;       /* TABLE subfield data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* set the <Enable Learning Of TAG1 VID>*/
    rc = prvCpssDxChReadTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
        portNum,
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
        LION3_L2I_EPORT_TABLE_FIELDS_EN_LEARN_ORIG_TAG1_VID_E, /* field name */
        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
        &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(data);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortVid1LearningEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable status of copying Tag1 VID from packet to FDB entry
*   at automatic learning.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE - enable copying Tag1 VID from packet to FDB entry,
*                    GT_FALSE - disable, the Tag1Vid in FDB will be 0
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortVid1LearningEnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortVid1LearningEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgFdbPortVid1LearningEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbDeviceTableSet
*
* DESCRIPTION:
*       This function sets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*       So for proper work of PP the application must set the relevant bits of
*       all devices in the system prior to inserting FDB entries associated with
*       them
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       devTableBmp - bmp of devices to set (APPLICABLE RANGES: 0..31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbDeviceTableSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  devTableBmp
)
{
    GT_STATUS   rc;             /* return code      */
    GT_U32      hwDevNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    for (hwDevNum= 0; hwDevNum < 32; hwDevNum+=2)
    {
        if (PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(hwDevNum))
        {   /*allow application to set 'devTableBmp = 0xFFFFFFFF'*/
            if (U32_GET_FIELD_MAC(devTableBmp,hwDevNum  ,1) == 0 &&
                U32_GET_FIELD_MAC(devTableBmp,hwDevNum+1,1) == 1)
            {
                /* the dual device Id not allow to set the 'odd' number unless 'even' value is set */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if (U32_GET_FIELD_MAC(devTableBmp,hwDevNum,1) == 1)
            {
                /* make sure for dual device that the 'odd' number is also set */
                U32_SET_FIELD_MAC(devTableBmp,hwDevNum+1,1,1);
            }
        }
    }

    rc = prvCpssHwPpWriteRegister(devNum,
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.deviceTableBaseAddr,
                      devTableBmp);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* save the info to DB --- needed in the AU messages handling
       PART of WA for Erratum
    */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.devTable = devTableBmp;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgFdbDeviceTableSet
*
* DESCRIPTION:
*       This function sets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*       So for proper work of PP the application must set the relevant bits of
*       all devices in the system prior to inserting FDB entries associated with
*       them
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       devTableBmp - bmp of devices to set (APPLICABLE RANGES: 0..31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDeviceTableSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  devTableBmp
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbDeviceTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, devTableBmp));

    rc = internal_cpssDxChBrgFdbDeviceTableSet(devNum, devTableBmp);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, devTableBmp));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbDeviceTableGet
*
* DESCRIPTION:
*       This function gets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       devTableBmpPtr - pointer to bmp of devices to set (APPLICABLE RANGES: 0..31).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbDeviceTableGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *devTableBmpPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      hwData;         /* hw data from register */
    GT_STATUS   rc;             /* return code      */
    GT_U32      hwDevNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(devTableBmpPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.deviceTableBaseAddr;

    rc = prvCpssHwPpReadRegister(devNum, regAddr, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (hwDevNum= 0; hwDevNum < 32; hwDevNum+=2)
    {
        if ((PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(hwDevNum)) && (U32_GET_FIELD_MAC(hwData,hwDevNum,1) == 1))
        {
            U32_SET_FIELD_MAC(hwData,hwDevNum+1,1,0);
        }
    }

    *devTableBmpPtr = hwData;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbDeviceTableGet
*
* DESCRIPTION:
*       This function gets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       devTableBmpPtr - pointer to bmp of devices to set (APPLICABLE RANGES: 0..31).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDeviceTableGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *devTableBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbDeviceTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, devTableBmpPtr));

    rc = internal_cpssDxChBrgFdbDeviceTableGet(devNum, devTableBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, devTableBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortGroupMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryPtr  - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number or portGroupsBmp.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6_MC, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*       for multi-port groups device :
*           1. Application should not use this API to add new MAC addresses once
*              SP feature is enabled in any of the ports, due to the fact it may
*              fail on one or more of the portGroups - in this case need
*              to write by index.
*           2. Updating existing MAC addresses is not affected and
*              can be preformed with no problem
*           3. application should be aware that when configuring MAC addresses only
*              to portGroups which none of them is the real source portGroups,
*              these entries would be "unseen" from AU perspective
*              (HW is filtering any AA, TA, QR etc... messages these entries invokes).
*              And although these entries are "Unseen" there are still used for forwarding
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortGroupMacEntrySet
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_U32 hwData[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]; /* words of Au hw entry */
    GT_STATUS  rc;                                  /* return status        */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(macEntryPtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
    }

    /* build entry hw format */
    rc = prvDxChFdbToAuConvert(devNum,
                               &(macEntryPtr->key),
                               macEntryPtr,
                               CPSS_NA_E,
                               GT_FALSE,
                               &hwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* set associated hwDevNum with the ownDevNum of the hemisphere */
        if(PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(PRV_CPSS_PP_MAC(devNum)->hwDevNum) &&
           (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E ||
            macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E) &&
           (macEntryPtr->dstInterface.type != CPSS_INTERFACE_PORT_E) && /*trunk/vid/vidx*/
           (portGroupId >= (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups / 2)))/* hemisphere 1 */
        {
            /* hw Associated Dev Num bits */
            U32_SET_FIELD_MAC(hwData[3],7,5,
                (PRV_CPSS_HW_DEV_NUM_MAC(devNum) +1) );
        }

        /* write data to Hw */
        rc = prvDxChFdbSendAuMessage(devNum,portGroupId ,&hwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryPtr  - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number or portGroupsBmp.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6_MC, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*       for multi-port groups device :
*           1. Application should not use this API to add new MAC addresses once
*              SP feature is enabled in any of the ports, due to the fact it may
*              fail on one or more of the portGroups - in this case need
*              to write by index.
*           2. Updating existing MAC addresses is not affected and
*              can be preformed with no problem
*           3. application should be aware that when configuring MAC addresses only
*              to portGroups which none of them is the real source portGroups,
*              these entries would be "unseen" from AU perspective
*              (HW is filtering any AA, TA, QR etc... messages these entries invokes).
*              And although these entries are "Unseen" there are still used for forwarding
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntrySet
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortGroupMacEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, macEntryPtr));

    rc = internal_cpssDxChBrgFdbPortGroupMacEntrySet(devNum, portGroupsBmp, macEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, macEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryPtr     - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6_MC, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacEntrySet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    return cpssDxChBrgFdbPortGroupMacEntrySet(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,macEntryPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryPtr     - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6_MC, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/

GT_STATUS cpssDxChBrgFdbMacEntrySet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacEntrySet);

    CPSS_API_LOCK_MAC(0,0);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, macEntryPtr));

    rc = internal_cpssDxChBrgFdbMacEntrySet(devNum, macEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, macEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgFdbPortGroupQaSend
*
* DESCRIPTION:
*       The function Send Query Address (QA) message to the hardware MAC address
*       table.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that QA message was processed by PP.
*       The PP sends Query Response message after QA processing.
*       An application can get QR message by general AU message get API.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryKeyPtr  - pointer to mac entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/vlan or portGroupsBmp
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the QA message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortGroupQaSend
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC  *macEntryKeyPtr
)
{
    GT_STATUS  rc;                                  /* return status        */
    GT_U32 hwData[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]; /* words of Au hw entry */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(macEntryKeyPtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
    }

    /* build entry hw format */
    rc = prvDxChFdbToAuConvert(devNum,
                               macEntryKeyPtr,
                               NULL,/* the entry not needed ... only need the key */
                               CPSS_QA_E,
                               GT_FALSE,
                               &hwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* write data to Hw */
        rc = prvDxChFdbSendAuMessage(devNum,portGroupId,&hwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupQaSend
*
* DESCRIPTION:
*       The function Send Query Address (QA) message to the hardware MAC address
*       table.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that QA message was processed by PP.
*       The PP sends Query Response message after QA processing.
*       An application can get QR message by general AU message get API.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryKeyPtr  - pointer to mac entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/vlan or portGroupsBmp
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the QA message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupQaSend
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC  *macEntryKeyPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortGroupQaSend);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, macEntryKeyPtr));

    rc = internal_cpssDxChBrgFdbPortGroupQaSend(devNum, portGroupsBmp, macEntryKeyPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, macEntryKeyPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbQaSend
*
* DESCRIPTION:
*       The function Send Query Address (QA) message to the hardware MAC address
*       table.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that QA message was processed by PP.
*       The PP sends Query Response message after QA processing.
*       An application can get QR message by general AU message get API.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to mac entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/vlan
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the QA message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbQaSend
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    return cpssDxChBrgFdbPortGroupQaSend(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,macEntryKeyPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbQaSend
*
* DESCRIPTION:
*       The function Send Query Address (QA) message to the hardware MAC address
*       table.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that QA message was processed by PP.
*       The PP sends Query Response message after QA processing.
*       An application can get QR message by general AU message get API.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to mac entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/vlan
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the QA message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQaSend
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbQaSend);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, macEntryKeyPtr));

    rc = internal_cpssDxChBrgFdbQaSend(devNum, macEntryKeyPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, macEntryKeyPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortGroupMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table through Address Update
*       message.(AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number or portGroupsBmp.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortGroupMacEntryDelete
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    GT_U32 hwData[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]; /* words of Au hw entry */
    GT_STATUS  rc;                                  /* return status        */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(macEntryKeyPtr);

    /* [JIRA]:[MT-231] [FE-2293984]
       CPU NA message for deleting an entry does not work for UC route entries */
    if (GT_TRUE==PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_FDB_ROUTE_UC_DELETE_BY_MESSAGE_WA_E))
    {
        if((macEntryKeyPtr->entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E)||
           (macEntryKeyPtr->entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E)||
           (macEntryKeyPtr->entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
    }

    /* build entry hw format */
    rc = prvDxChFdbToAuConvert(devNum,
                               macEntryKeyPtr,
                               NULL,
                               CPSS_NA_E,
                               GT_TRUE,
                               &hwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* write data to Hw */
        rc = prvDxChFdbSendAuMessage(devNum, portGroupId,&hwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table through Address Update
*       message.(AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number or portGroupsBmp.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryDelete
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortGroupMacEntryDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, macEntryKeyPtr));

    rc = internal_cpssDxChBrgFdbPortGroupMacEntryDelete(devNum, portGroupsBmp, macEntryKeyPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, macEntryKeyPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table through Address Update
*       message.(AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacEntryDelete
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    return cpssDxChBrgFdbPortGroupMacEntryDelete(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,macEntryKeyPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table through Address Update
*       message.(AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
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
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryDelete
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacEntryDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, macEntryKeyPtr));

    rc = internal_cpssDxChBrgFdbMacEntryDelete(devNum, macEntryKeyPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, macEntryKeyPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortGroupMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
*
*       for multi-port groups device :
*           1. Unified FDBs mode :
*              API should return GT_BAD_PARAM if value other then CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*              was passed when working in this mode, due to the fact that all
*              FDBs are supposed to be synced.
*              Application should use this API to add new MAC addresses once SP
*              feature is enabled in any of the ports , due to the fact
*              cpssDxChBrgFdbMacEntrySet/cpssDxChBrgFdbPortGroupMacEntrySet API
*              may fail on one or more of the portGroups.
*           2. Unified-Linked FDB mode / Linked FDBs mode :
*              Setting either CPSS_PORT_GROUP_UNAWARE_MODE_CNS or any subset of
*              portGroup can be supported in this mode, still application should
*              note that in Linked FDBs when using multiple portGroups, setting
*              same index would potentially result affecting up to 4 different
*              MAC addresses (up to 2 different MAC addresses in Unified-Linked
*              FDB mode).
*           3. application should be aware that when configuring MAC addresses only
*              to portGroups which none of them is the real source portGroups,
*              these entries would be "unseen" from AU perspective
*              (HW is filtering any AA, TA, QR etc... messages these entries invokes).
*              And although these entries are "Unseen" there are still used for forwarding
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index           - hw mac entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       macEntryPtr     - pointer to MAC entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum,portGroupsBmp,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortGroupMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_U32 hwData[CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS];
                                            /* words of mac hw entry    */
    GT_STATUS  rc;                          /* return code              */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(macEntryPtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
    }

    PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index);

    rc = prvDxChBrgFdbBuildMacEntryHwFormat(devNum,
                                            macEntryPtr,
                                            skip,
                                            hwData);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* set associated hwDevNum with the ownDevNum of the hemisphere */
        if(PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(PRV_CPSS_PP_MAC(devNum)->hwDevNum) &&
           (macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E ||
            macEntryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E) &&
           (macEntryPtr->dstInterface.type != CPSS_INTERFACE_PORT_E) && /*trunk/vid/vidx*/
           (portGroupId >= (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups / 2)))/* hemisphere 1 */
        {
            /* hw Associated Dev Num bits */
            U32_SET_FIELD_MAC(hwData[2],1,5,
                (PRV_CPSS_HW_DEV_NUM_MAC(devNum) +1) );
        }

        /* write entry to specific index format */
        rc = prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,
                                        PRV_CPSS_DXCH_TABLE_FDB_E,
                                        index,
                                        &hwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
*
*       for multi-port groups device :
*           1. Unified FDBs mode :
*              API should return GT_BAD_PARAM if value other then CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*              was passed when working in this mode, due to the fact that all
*              FDBs are supposed to be synced.
*              Application should use this API to add new MAC addresses once SP
*              feature is enabled in any of the ports , due to the fact
*              cpssDxChBrgFdbMacEntrySet/cpssDxChBrgFdbPortGroupMacEntrySet API
*              may fail on one or more of the portGroups.
*           2. Unified-Linked FDB mode / Linked FDBs mode :
*              Setting either CPSS_PORT_GROUP_UNAWARE_MODE_CNS or any subset of
*              portGroup can be supported in this mode, still application should
*              note that in Linked FDBs when using multiple portGroups, setting
*              same index would potentially result affecting up to 4 different
*              MAC addresses (up to 2 different MAC addresses in Unified-Linked
*              FDB mode).
*           3. application should be aware that when configuring MAC addresses only
*              to portGroups which none of them is the real source portGroups,
*              these entries would be "unseen" from AU perspective
*              (HW is filtering any AA, TA, QR etc... messages these entries invokes).
*              And although these entries are "Unseen" there are still used for forwarding
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index           - hw mac entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       macEntryPtr     - pointer to MAC entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum,portGroupsBmp,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortGroupMacEntryWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, index, skip, macEntryPtr));

    rc = internal_cpssDxChBrgFdbPortGroupMacEntryWrite(devNum, portGroupsBmp, index, skip, macEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, index, skip, macEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       index           - hw mac entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       macEntryPtr     - pointer to MAC entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    return cpssDxChBrgFdbPortGroupMacEntryWrite(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        index,skip,macEntryPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       index           - hw mac entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       macEntryPtr     - pointer to MAC entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacEntryWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, skip, macEntryPtr));

    rc = internal_cpssDxChBrgFdbMacEntryWrite(devNum, index, skip, macEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, skip, macEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortGroupMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortGroupMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
)
{
    GT_STATUS  rc;
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_DXCH_TABLE_ENT tableType ;
    GT_U32     fieldWordNum = 0;/* The 1st word contains the Valid and Skip bits */
    GT_U32     fieldOffset  = 0;/* The offset in the word                        */
    GT_U32     fieldLength  = 32;/* The length to be read                        */
    GT_U32     fieldValue;        /* The 1st word content */
    GT_U32     hwValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(skipPtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
    }

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);
    PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index);

    tableType = PRV_CPSS_DXCH_TABLE_FDB_E;

    rc =  prvCpssDxChPortGroupReadTableEntryField(devNum,
                                             portGroupId,
                                             tableType,
                                             index,
                                             fieldWordNum,
                                             fieldOffset,
                                             fieldLength,
                                             &fieldValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* valid bit */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,&fieldValue,
            LION3_FDB_FDB_TABLE_FIELDS_VALID_E,
            hwValue)
        *validPtr = BIT2BOOL_MAC(hwValue);

        /* skip bit */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,&fieldValue,
            LION3_FDB_FDB_TABLE_FIELDS_SKIP_E,
            hwValue)
        *skipPtr = BIT2BOOL_MAC(hwValue);
    }
    else
    {
        /* valid bit */
        *validPtr = BIT2BOOL_MAC(U32_GET_FIELD_MAC(fieldValue,0,1));

        /* skip bit */
        *skipPtr = BIT2BOOL_MAC(U32_GET_FIELD_MAC(fieldValue,1,1));
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortGroupMacEntryStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, index, validPtr, skipPtr));

    rc = internal_cpssDxChBrgFdbPortGroupMacEntryStatusGet(devNum, portGroupsBmp, index, validPtr, skipPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, index, validPtr, skipPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
)
{
    return cpssDxChBrgFdbPortGroupMacEntryStatusGet(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index,validPtr,skipPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacEntryStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, validPtr, skipPtr));

    rc = internal_cpssDxChBrgFdbMacEntryStatusGet(devNum, index, validPtr, skipPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, validPtr, skipPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortGroupMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedHwDevNumPtr = (pointer to) is HW device number associated with the
*                     entry (even for vidx/trunk entries the field is used by
*                     PP for aging/flush/transplant purpose).
*                     Relevant only in case of Mac Address entry.
*       entryPtr    - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortGroupMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_HW_DEV_NUM           *associatedHwDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
)
{
    GT_STATUS rc;                   /* return code  */
    GT_U32    macEntryType;         /* entry type   */
    GT_U16    vlanId;               /* vlan ID      */
    GT_U32    multiple;             /* multiple bit */
    GT_U32    macAddr40bit;         /* the 40th bit from Mac address */
    GT_BOOL   useVidx = GT_FALSE;   /* flag for using vidx */
    GT_U32    hwValue;              /* temporary hw value */
    GT_U32    hwData[CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS];
                                    /* hw data      */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_DXCH_LION3_FDB_SPECIAL_MUXED_FIELDS_STC specialFields;/*special Muxed fields in the AU msg */
    GT_U32    isTrunk;             /* is trunk bit */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(skipPtr);
    CPSS_NULL_PTR_CHECK_MAC(agedPtr);
    CPSS_NULL_PTR_CHECK_MAC(associatedHwDevNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);
    PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index);

    rc = prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,
                                   PRV_CPSS_DXCH_TABLE_FDB_E,
                                   index,
                                   &hwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    cpssOsMemSet(entryPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* valid bit */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_VALID_E,
            hwValue)
        *validPtr = BIT2BOOL_MAC(hwValue);

        /* skip bit */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_SKIP_E,
            hwValue)
        *skipPtr = BIT2BOOL_MAC(hwValue);

        /* age bit */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_AGE_E,
            hwValue)
        *agedPtr = BIT2BOOL_MAC(hwValue);

        entryPtr->age = *agedPtr;

        /* get entry type */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_FDB_ENTRY_TYPE_E,
            hwValue);
        macEntryType = hwValue;

        if ( (macEntryType == 3)||(macEntryType == 4)||(macEntryType == 5) )
        {
           return prvCpssDxChBrgFdbLion3FdbIpUcRouteFieldsGet(devNum,macEntryType,hwData,entryPtr);
        }

        /* get FID */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_FID_E,
            hwValue);
        vlanId = (GT_U16)hwValue;

        rc = prvCpssDxChBrgFdbLion3FdbFdbSpecialMuxedFieldsGet(devNum,hwData,macEntryType,&specialFields);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(specialFields.srcId == PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
        {
            entryPtr->sourceID = 0;
        }
        else
        {
            entryPtr->sourceID = specialFields.srcId;
        }

        entryPtr->userDefined = specialFields.udb;

        if(specialFields.daAccessLevel == PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
        {
            entryPtr->daSecurityLevel = 0;
        }
        else
        {
            entryPtr->daSecurityLevel = specialFields.daAccessLevel;
        }

        if(specialFields.saAccessLevel == PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
        {
            entryPtr->saSecurityLevel = 0;
        }
        else
        {
            entryPtr->saSecurityLevel = specialFields.saAccessLevel;
        }

        if(specialFields.origVid1 == PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
        {
            entryPtr->key.vid1 = 0;
        }
        else
        {
            entryPtr->key.vid1 = specialFields.origVid1;
        }
    }
    else
    {
        /* valid bit */
        *validPtr = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[0],0,1));

        /* skip bit */
        *skipPtr = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[0],1,1));

        /* age bit */
        *agedPtr = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[0],2,1));

        entryPtr->age = *agedPtr;

        /* get entry type */
        macEntryType = U32_GET_FIELD_MAC(hwData[0],3,2);

        /* get vlanId */
        vlanId = (GT_U16)U32_GET_FIELD_MAC(hwData[0],5,12);
    }

    if (macEntryType == 0)
    {
        /* MAC Address entry */

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            LION3_FDB_FDB_FIELD_MAC_ADDR_GET_MAC(devNum,hwData,
                &entryPtr->key.key.macVlan.macAddr.arEther[0]);

            /* get multiple bit */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
                LION3_FDB_FDB_TABLE_FIELDS_MULTIPLE_E,
                hwValue);
            multiple = hwValue;

            /* associated devNum number */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
                LION3_FDB_FDB_TABLE_FIELDS_DEV_ID_E,
                hwValue);
            *associatedHwDevNumPtr = hwValue;

            /* is trunk */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
                LION3_FDB_FDB_TABLE_FIELDS_IS_TRUNK_E,
                hwValue);
            isTrunk = hwValue;
        }
        else
        {
            /* set MAC Address */
            entryPtr->key.key.macVlan.macAddr.arEther[5] =
                            (GT_U8)U32_GET_FIELD_MAC(hwData[0],17,8);

            entryPtr->key.key.macVlan.macAddr.arEther[4] =
                            (GT_U8)(U32_GET_FIELD_MAC(hwData[0],25,7) |
                                    (U32_GET_FIELD_MAC(hwData[1],0,1) << 7));

            entryPtr->key.key.macVlan.macAddr.arEther[3] =
                             (GT_U8)U32_GET_FIELD_MAC(hwData[1],1,8);

            entryPtr->key.key.macVlan.macAddr.arEther[2] =
                             (GT_U8)U32_GET_FIELD_MAC(hwData[1],9,8);

            entryPtr->key.key.macVlan.macAddr.arEther[1] =
                             (GT_U8)U32_GET_FIELD_MAC(hwData[1],17,8);

            entryPtr->key.key.macVlan.macAddr.arEther[0] =
                             (GT_U8)(U32_GET_FIELD_MAC(hwData[1],25,7) |
                                     (U32_GET_FIELD_MAC(hwData[2],0,1) << 7));

            /* get multiple bit */
            multiple = U32_GET_FIELD_MAC(hwData[2],26,1);
            /* associated devNum number */
            *associatedHwDevNumPtr = U32_GET_FIELD_MAC(hwData[2],1,5);
            /* is trunk */
            isTrunk = U32_GET_FIELD_MAC(hwData[2],13,1);
        }

        /* set vid */
        entryPtr->key.key.macVlan.vlanId = vlanId;

        /* get MacAddr[40] bit */
        macAddr40bit = (entryPtr->key.key.macVlan.macAddr.arEther[0] & 0x1);

        /* set interface parameters */
        if((multiple) || (macAddr40bit))
        {
            /* multicast entry */
            useVidx = GT_TRUE;
        }
        else if(isTrunk)/* trunk bit */
        {
            entryPtr->dstInterface.type = CPSS_INTERFACE_TRUNK_E;
            entryPtr->dstInterface.hwDevNum = *associatedHwDevNumPtr;
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
                    LION3_FDB_FDB_TABLE_FIELDS_TRUNK_NUM_E,
                    hwValue);
                entryPtr->dstInterface.trunkId = (GT_TRUNK_ID)hwValue;
            }
            else
            {
                entryPtr->dstInterface.trunkId =
                                (GT_TRUNK_ID)U32_GET_FIELD_MAC(hwData[2],14,7);
            }
        }
        else
        {
            entryPtr->dstInterface.type = CPSS_INTERFACE_PORT_E;
            entryPtr->dstInterface.devPort.hwDevNum = *associatedHwDevNumPtr;

            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
                    LION3_FDB_FDB_TABLE_FIELDS_EPORT_NUM_E,
                    hwValue);
                entryPtr->dstInterface.devPort.portNum = hwValue;
            }
            else
            {
                /* portNum 6 bits[78:83]  */
                entryPtr->dstInterface.devPort.portNum =
                            U32_GET_FIELD_MAC(hwData[2],14,6);
            }

            PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(entryPtr->dstInterface));
            *associatedHwDevNumPtr = entryPtr->dstInterface.devPort.hwDevNum;
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* SA cmd */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
                LION3_FDB_FDB_TABLE_FIELDS_SA_CMD_E,
                hwValue);

            PRV_CPSS_DXCH_FDB_CONVERT_HW_CMD_TO_SW_VAL_MAC(entryPtr->saCommand,
                                                           hwValue);
        }
        else
        {
            /* SA cmd */
            hwValue = ((U32_GET_FIELD_MAC(hwData[2],30,2)) |
                       ((U32_GET_FIELD_MAC(hwData[3],0,1)) << 2));

            PRV_CPSS_DXCH_FDB_CONVERT_HW_CMD_TO_SW_VAL_MAC(entryPtr->saCommand,
                                                           hwValue);
            /* Source ID */
            entryPtr->sourceID = U32_GET_FIELD_MAC(hwData[2],6,5);
        }
    }
    else
    {
        /* IP Multicast entry */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* get DIP Address */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
                LION3_FDB_FDB_TABLE_FIELDS_DIP_E,
                hwValue);

            entryPtr->key.key.ipMcast.dip[3] = (GT_U8)(hwValue >> 0);
            entryPtr->key.key.ipMcast.dip[2] = (GT_U8)(hwValue >> 8);
            entryPtr->key.key.ipMcast.dip[1] = (GT_U8)(hwValue >> 16);
            entryPtr->key.key.ipMcast.dip[0] = (GT_U8)(hwValue >> 24);

            /* get SIP Address */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
                LION3_FDB_FDB_TABLE_FIELDS_SIP_E,
                hwValue);

            entryPtr->key.key.ipMcast.sip[3] = (GT_U8)(hwValue >> 0);
            entryPtr->key.key.ipMcast.sip[2] = (GT_U8)(hwValue >> 8);
            entryPtr->key.key.ipMcast.sip[1] = (GT_U8)(hwValue >> 16);
            entryPtr->key.key.ipMcast.sip[0] = (GT_U8)(hwValue >> 24);
        }
        else
        {
            /* set DIP Address */
            entryPtr->key.key.ipMcast.dip[3] =
                            (GT_U8)U32_GET_FIELD_MAC(hwData[0],17,8);

            entryPtr->key.key.ipMcast.dip[2] =
                            (GT_U8)(U32_GET_FIELD_MAC(hwData[0],25,7) |
                                    (U32_GET_FIELD_MAC(hwData[1],0,1) << 7));

            entryPtr->key.key.ipMcast.dip[1] =
                            (GT_U8)U32_GET_FIELD_MAC(hwData[1],1,8);

            entryPtr->key.key.ipMcast.dip[0] =
                            (GT_U8)U32_GET_FIELD_MAC(hwData[1],9,8);

            /* set SIP Address */
            entryPtr->key.key.ipMcast.sip[3] =
                            (GT_U8)U32_GET_FIELD_MAC(hwData[1],17,8);

            entryPtr->key.key.ipMcast.sip[2] =
                            (GT_U8)(U32_GET_FIELD_MAC(hwData[1],25,7) |
                                    (U32_GET_FIELD_MAC(hwData[2],0,1) << 7));

            entryPtr->key.key.ipMcast.sip[1] =
                            (GT_U8)U32_GET_FIELD_MAC(hwData[2],1,8);

            entryPtr->key.key.ipMcast.sip[0] =
                            (GT_U8)(U32_GET_FIELD_MAC(hwData[2],9,4)         |
                                    (U32_GET_FIELD_MAC(hwData[2],26,1) << 4) |
                                    (U32_GET_FIELD_MAC(hwData[2],30,2) << 5) |
                                    (U32_GET_FIELD_MAC(hwData[3],0,1)  << 7));
        }

        /* set VID */
        entryPtr->key.key.ipMcast.vlanId = vlanId;

        useVidx = GT_TRUE;
    }

    if (useVidx)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* VIDX */
            LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
                LION3_FDB_FDB_TABLE_FIELDS_VIDX_E,
                hwValue);
            entryPtr->dstInterface.vidx = (GT_U16)hwValue;
        }
        else
        {
            entryPtr->dstInterface.vidx = (GT_U16)U32_GET_FIELD_MAC(hwData[2],13,12);
        }

        /* set destination interface type to VID or VIDX */
        if(entryPtr->dstInterface.vidx == 0xFFF)
        {
            entryPtr->dstInterface.type = CPSS_INTERFACE_VID_E;
            /* set VID */
            entryPtr->dstInterface.vlanId = vlanId;
        }
        else
        {
            /* set vidx */
            entryPtr->dstInterface.type = CPSS_INTERFACE_VIDX_E;
        }
    }
    else
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* user defined already got special treatment (of muxed fields) */
        }
        else
        {
            /* user defined */
            entryPtr->userDefined = U32_GET_FIELD_MAC(hwData[2],21,4);
        }
    }


    /* set mac entry type. For eArch it can be additionally modified below */
    switch(macEntryType)
    {
        case 0:
            entryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
            break;
        case 1:
            entryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
            break;
        case 2:
            entryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum) &&
            PRV_CPSS_DXCH_FDB_LOOKUP_KEY_MODE_PER_FID_IS_DOUBLE_MAC(devNum, vlanId))
        {
            /* it can be required to adjust key.EntryType */
            switch(entryPtr->key.entryType)
            {
                case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
                    entryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E;
                    break;
                case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
                    entryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E;
                    break;
                case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
                    entryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E;
                    break;
                default:
                    break;
            }

        }
        /* set DA cmd */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_DA_CMD_E,
            hwValue);
        PRV_CPSS_DXCH_FDB_CONVERT_HW_CMD_TO_SW_VAL_MAC(entryPtr->daCommand,
                                                       hwValue);

        /* set static */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_IS_STATIC_E,
            hwValue);
        entryPtr->isStatic = BIT2BOOL_MAC(hwValue);

        /* DA Route */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_DA_ROUTE_E,
            hwValue);
        entryPtr->daRoute = BIT2BOOL_MAC(hwValue);

        /* spUnknown - NA strorm prevent entry */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_SP_UNKNOWN_E,
            hwValue);
        entryPtr->spUnknown = BIT2BOOL_MAC(hwValue);

        /* SA QoS Profile index */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_SA_QOS_PARAM_SET_IDX_E,
            hwValue);
        entryPtr->saQosIndex = hwValue;

        /* DA QoS Profile index */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_DA_QOS_PARAM_SET_IDX_E,
            hwValue);
        entryPtr->daQosIndex = hwValue;

        /* SA Lookup Ingress Mirror to Analyzer Enable */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_SA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E,
            hwValue);
        entryPtr->saMirrorToRxAnalyzerPortEn = BIT2BOOL_MAC(hwValue);

        /* DA Lookup Ingress Mirror to Analyzer Enable */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_DA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E,
            hwValue);
        entryPtr->daMirrorToRxAnalyzerPortEn = BIT2BOOL_MAC(hwValue);

        /* Application Specific CPU Code */
        LION3_FDB_FDB_FIELD_GET_MAC(devNum,hwData,
            LION3_FDB_FDB_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_E,
            hwValue);
        entryPtr->appSpecificCpuCode = BIT2BOOL_MAC(hwValue);
    }
    else
    {
        /* set DA cmd */
        hwValue = (GT_U8)U32_GET_FIELD_MAC(hwData[2],27,3);
        PRV_CPSS_DXCH_FDB_CONVERT_HW_CMD_TO_SW_VAL_MAC(entryPtr->daCommand,
                                                       hwValue);
        /* set static */
        entryPtr->isStatic = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[2],25,1));

        /* DA Route */
        entryPtr->daRoute = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[3],1,1));

        /* spUnknown - NA strorm prevent entry */
        entryPtr->spUnknown = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[3],2,1));

        /* SA QoS Profile index */
        entryPtr->saQosIndex = U32_GET_FIELD_MAC(hwData[3],3,3);

        /* DA QoS Profile index */
        entryPtr->daQosIndex = U32_GET_FIELD_MAC(hwData[3],6,3);

        /* Mirror to Analyzer Port */
        entryPtr->mirrorToRxAnalyzerPortEn = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[3],9,1));

        /* set Cheetah2 fields */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* Application Specific CPU Code */
            entryPtr->appSpecificCpuCode = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[3],10,1));

            /* DA Access Level */
            entryPtr->daSecurityLevel = U32_GET_FIELD_MAC(hwData[3],11,3);

            /* SA Access Level */
            entryPtr->saSecurityLevel = U32_GET_FIELD_MAC(hwData[3],14,3);
        }
    }

    /* set associated hwDevNum with the ownDevNum of the hemisphere */
    if((entryPtr->key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E) &&
       (entryPtr->dstInterface.type != CPSS_INTERFACE_PORT_E) &&  /*trunk/vid/vidx*/
       PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(*associatedHwDevNumPtr))
    {
        /* set associated hwDevNum without the hemisphere indication */
        (*associatedHwDevNumPtr) &= MASK_DUAL_DEVICE_ID_LSB_CNS;
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedHwDevNumPtr = (pointer to) is HW device number associated with the
*                     entry (even for vidx/trunk entries the field is used by
*                     PP for aging/flush/transplant purpose).
*                     Relevant only in case of Mac Address entry.
*       entryPtr    - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_HW_DEV_NUM           *associatedHwDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortGroupMacEntryRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, index, validPtr, skipPtr, agedPtr, associatedHwDevNumPtr, entryPtr));

    rc = internal_cpssDxChBrgFdbPortGroupMacEntryRead(devNum, portGroupsBmp, index, validPtr, skipPtr, agedPtr, associatedHwDevNumPtr, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, index, validPtr, skipPtr, agedPtr, associatedHwDevNumPtr, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedHwDevNumPtr = (pointer to) is HW device number associated with the
*                     entry (even for vidx/trunk entries the field is used by
*                     PP for aging/flush/transplant purpose).
*                     Relevant only in case of Mac Address entry.
*       entryPtr    - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_HW_DEV_NUM           *associatedHwDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
)
{
    return cpssDxChBrgFdbPortGroupMacEntryRead(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index,validPtr,skipPtr,
        agedPtr,associatedHwDevNumPtr,entryPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedHwDevNumPtr = (pointer to) is HW device number associated with the
*                     entry (even for vidx/trunk entries the field is used by
*                     PP for aging/flush/transplant purpose).
*                     Relevant only in case of Mac Address entry.
*       entryPtr    - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_HW_DEV_NUM           *associatedHwDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacEntryRead);

    CPSS_API_LOCK_MAC(0,0);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, validPtr, skipPtr, agedPtr, associatedHwDevNumPtr, entryPtr));
    rc = internal_cpssDxChBrgFdbMacEntryRead(devNum, index, validPtr, skipPtr, agedPtr, associatedHwDevNumPtr, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, validPtr, skipPtr, agedPtr, associatedHwDevNumPtr, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortGroupMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index       - hw mac entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortGroupMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN GT_U32        index
)
{
    GT_STATUS   rc;
    GT_U32      hwData = 0;/* value to write to first word of the FDB entry */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* write 0 to first word */
        rc = prvCpssDxChPortGroupWriteTableEntryField(devNum,portGroupId,
                                         PRV_CPSS_DXCH_TABLE_FDB_E,
                                         index,
                                         0,
                                         0,
                                         32,
                                         hwData);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index       - hw mac entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN GT_U32        index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortGroupMacEntryInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, index));

    rc = internal_cpssDxChBrgFdbPortGroupMacEntryInvalidate(devNum, portGroupsBmp, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_U32        index
)
{
    return cpssDxChBrgFdbPortGroupMacEntryInvalidate(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index);
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_U32        index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacEntryInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index));

    rc = internal_cpssDxChBrgFdbMacEntryInvalidate(devNum, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChBrgFdbMaxLookupLenSet
*
* DESCRIPTION:
*       Set the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       lookupLen   - The maximal length of MAC table lookup, this must be
*                     value divided by 4 with no left over.
*                     (APPLICABLE RANGES: 4, 8, 12..32).
*                     in 'multi hash' mode the valid value is : 16.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_OUT_OF_RANGE          - lookupLen > 32 or lookupLen < 4
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMaxLookupLenSet
(
    IN GT_U8    devNum,
    IN GT_U32   lookupLen
)
{
    GT_U32      regAddr;    /* The register's address of l2IngressCtrl reg. */
    GT_U32      value;      /* The value to be written to the register.     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(lookupLen & 0x3) /* only divided by 4 with no left over */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode == CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
    {
        if (lookupLen != PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.numOfBanks)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        value = 0;
    }
    else
    {
        value = (lookupLen >> 2) - 1;
    }

    if(lookupLen < 4 || value > 7)/* 3 bits for the field */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 3, value);
}

/*******************************************************************************
* cpssDxChBrgFdbMaxLookupLenSet
*
* DESCRIPTION:
*       Set the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       lookupLen   - The maximal length of MAC table lookup, this must be
*                     value divided by 4 with no left over.
*                     (APPLICABLE RANGES: 4, 8, 12..32).
*                     in 'multi hash' mode the valid value is : 16.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_OUT_OF_RANGE          - lookupLen > 32 or lookupLen < 4
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMaxLookupLenSet
(
    IN GT_U8    devNum,
    IN GT_U32   lookupLen
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMaxLookupLenSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lookupLen));

    rc = internal_cpssDxChBrgFdbMaxLookupLenSet(devNum, lookupLen);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lookupLen));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMaxLookupLenGet
*
* DESCRIPTION:
*       Get the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       lookupLenPtr   - The maximal length of MAC table lookup, this must be
*                        value divided by 4 with no left over.
*                        (APPLICABLE RANGES: 4, 8, 12..32).
*                        in 'multi hash' mode the valid value is : 16.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMaxLookupLenGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *lookupLenPtr
)
{
    GT_U32  regAddr;        /* The register's address of l2IngressCtrl reg. */
    GT_U32  hwValue;        /* The value to be read from the register.     */
    GT_STATUS   rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(lookupLenPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 3, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode == CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
    {
        if(hwValue == 0)
        {
            *lookupLenPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.numOfBanks;
            return GT_OK;
        }

        /* HW should hold value 0 */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }
    else
    {
        *lookupLenPtr = ((hwValue + 1) << 2);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMaxLookupLenGet
*
* DESCRIPTION:
*       Get the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       lookupLenPtr   - The maximal length of MAC table lookup, this must be
*                        value divided by 4 with no left over.
*                        (APPLICABLE RANGES: 4, 8, 12..32).
*                        in 'multi hash' mode the valid value is : 16.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMaxLookupLenGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *lookupLenPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMaxLookupLenGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lookupLenPtr));

    rc = internal_cpssDxChBrgFdbMaxLookupLenGet(devNum, lookupLenPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lookupLenPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMacVlanLookupModeSet
*
* DESCRIPTION:
*       Sets the VLAN Lookup mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad param
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacVlanLookupModeSet
(
    IN GT_U8                devNum,
    IN CPSS_MAC_VL_ENT      mode
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      value;      /* value to write into register */
    GT_STATUS   rc;         /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch(mode)
    {
        case CPSS_IVL_E:
            value = 1;
            break;
        case CPSS_SVL_E:
            value = 0;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 3, 1, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Update FDB hash parameters */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.vlanMode = mode;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacVlanLookupModeSet
*
* DESCRIPTION:
*       Sets the VLAN Lookup mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad param
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacVlanLookupModeSet
(
    IN GT_U8                devNum,
    IN CPSS_MAC_VL_ENT      mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacVlanLookupModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChBrgFdbMacVlanLookupModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMacVlanLookupModeGet
*
* DESCRIPTION:
*       Get VLAN Lookup mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad param
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacVlanLookupModeGet
(
    IN  GT_U8            devNum,
    OUT CPSS_MAC_VL_ENT  *modePtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 hwValue;     /* value to read from register  */
    GT_STATUS rc;       /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 3, 1, &hwValue);

    *modePtr = (hwValue == 1) ? CPSS_IVL_E : CPSS_SVL_E;

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbMacVlanLookupModeGet
*
* DESCRIPTION:
*       Get VLAN Lookup mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad param
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacVlanLookupModeGet
(
    IN  GT_U8            devNum,
    OUT CPSS_MAC_VL_ENT  *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacVlanLookupModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChBrgFdbMacVlanLookupModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbAuMsgRateLimitSet
*
* DESCRIPTION:
*     Set Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       msgRate    - maximal rate of AU messages per second
*       enable     - enable/disable message limit
*                    GT_TRUE - message rate is limited with msgRate
*                    GT_FALSE - there is no limitations, WA to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_OUT_OF_RANGE          - out of range.
*       GT_BAD_PARAM             - on bad parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when DxCh Clock 200 MHz(DxCh2 220 MHz Clock) ,
*          granularity and rate changed linearly depending on clock.
*
*     for multi-port groups device :
*       The rate-limiting is done individually in each one of the port Groups.
*       This means that potentially configuring this value would result a rate
*       which is up to n times bigger then expected (n is number of port groups)
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAuMsgRateLimitSet
(
    IN GT_U8                       devNum,
    IN GT_U32                      msgRate,
    IN GT_BOOL                     enable
)
{
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwValue;          /* value to write to register   */
    GT_U32    rate;             /* value of "rate" subfield     */
    GT_U32    granularity;      /* granularity                  */
    GT_U32    window;           /* window in uSec               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    /* convert specified rate to HW format */
    granularity = 2;

    window      = (10000 * PRV_CPSS_PP_MAC(devNum)->baseCoreClock) / PRV_CPSS_PP_MAC(devNum)->coreClock;

    rate = ((msgRate / granularity) * window) / 1000000;

    /* max rate in a 10ms window is 0xFF */
    if ((rate >= BIT_8) && enable)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* store enable bit and rate together */
    hwValue = (BOOL2BIT_MAC(enable) << 8) | rate;

    return prvCpssHwPpSetRegField(devNum, regAddr, 10, 9, hwValue);
}

/*******************************************************************************
* cpssDxChBrgFdbAuMsgRateLimitSet
*
* DESCRIPTION:
*     Set Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       msgRate    - maximal rate of AU messages per second
*       enable     - enable/disable message limit
*                    GT_TRUE - message rate is limited with msgRate
*                    GT_FALSE - there is no limitations, WA to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_OUT_OF_RANGE          - out of range.
*       GT_BAD_PARAM             - on bad parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when DxCh Clock 200 MHz(DxCh2 220 MHz Clock) ,
*          granularity and rate changed linearly depending on clock.
*
*     for multi-port groups device :
*       The rate-limiting is done individually in each one of the port Groups.
*       This means that potentially configuring this value would result a rate
*       which is up to n times bigger then expected (n is number of port groups)
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAuMsgRateLimitSet
(
    IN GT_U8                       devNum,
    IN GT_U32                      msgRate,
    IN GT_BOOL                     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAuMsgRateLimitSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, msgRate, enable));

    rc = internal_cpssDxChBrgFdbAuMsgRateLimitSet(devNum, msgRate, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, msgRate, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbAuMsgRateLimitGet
*
* DESCRIPTION:
*     Get Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       msgRatePtr - (pointer to) maximal rate of AU messages per second
*       enablePtr  - (pointer to) GT_TRUE - message rate is limited with
*                                           msgRatePtr
*                                 GT_FALSE - there is no limitations, WA to CPU
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_OUT_OF_RANGE          - out of range.
*       GT_BAD_PARAM             - on bad parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when DxCh Clock 200 MHz(DxCh2 220 MHz Clock) ,
*          granularity and rate changed linearly depending on clock.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAuMsgRateLimitGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *msgRatePtr,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwValue;          /* value to read from register  */
    GT_STATUS rc;               /* return code                  */
    GT_U32    rate;             /* value of "rate" subfield     */
    GT_U32    granularity;      /* granularity                  */
    GT_U32    window;           /* window in uSec               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(msgRatePtr);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    granularity = 2;

    window      = (10000 * PRV_CPSS_PP_MAC(devNum)->baseCoreClock) / PRV_CPSS_PP_MAC(devNum)->coreClock;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 10, 9, &hwValue);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    rate = (hwValue & 0xFF);

    /* convert HW value to SW rate */
    *msgRatePtr = ((rate * granularity * 1000000) / window) ;

    *enablePtr = BIT2BOOL_MAC(((hwValue >> 8) & 0x1));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbAuMsgRateLimitGet
*
* DESCRIPTION:
*     Get Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       msgRatePtr - (pointer to) maximal rate of AU messages per second
*       enablePtr  - (pointer to) GT_TRUE - message rate is limited with
*                                           msgRatePtr
*                                 GT_FALSE - there is no limitations, WA to CPU
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_OUT_OF_RANGE          - out of range.
*       GT_BAD_PARAM             - on bad parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when DxCh Clock 200 MHz(DxCh2 220 MHz Clock) ,
*          granularity and rate changed linearly depending on clock.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAuMsgRateLimitGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *msgRatePtr,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAuMsgRateLimitGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, msgRatePtr, enablePtr));

    rc = internal_cpssDxChBrgFdbAuMsgRateLimitGet(devNum, msgRatePtr, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, msgRatePtr, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbNaMsgOnChainTooLongSet
*
* DESCRIPTION:
*       Enable/Disable sending NA messages to the CPU indicating that the device
*       cannot learn a new SA . It has reached its max hop (bucket is full).
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbNaMsgOnChainTooLongSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    data = (enable == GT_TRUE) ? 1 : 0;

    /* Enable/Disable NA msg to CPU if chain too long */
    return prvCpssHwPpSetRegField(devNum, regAddr, 4, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbNaMsgOnChainTooLongSet
*
* DESCRIPTION:
*       Enable/Disable sending NA messages to the CPU indicating that the device
*       cannot learn a new SA . It has reached its max hop (bucket is full).
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgOnChainTooLongSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbNaMsgOnChainTooLongSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbNaMsgOnChainTooLongSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbNaMsgOnChainTooLongGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending NA messages to the CPU
*       indicating that the device cannot learn a new SA. It has reached its
*       max hop (bucket is full).
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the NA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbNaMsgOnChainTooLongGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32    regAddr;           /* register address    */
    GT_U32    data;              /* reg sub field data  */
    GT_STATUS rc;                /* return code         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 4, 1, &data);

    /* Enable/Disable NA msg to CPU if chain too long */
    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbNaMsgOnChainTooLongGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending NA messages to the CPU
*       indicating that the device cannot learn a new SA. It has reached its
*       max hop (bucket is full).
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the NA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgOnChainTooLongGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbNaMsgOnChainTooLongGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbNaMsgOnChainTooLongGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbSpAaMsgToCpuSet
*
* DESCRIPTION:
*       Enabled/Disabled sending Aged Address (AA) messages to the CPU for the
*       Storm Prevention (SP) entries when those entries reach an aged out status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the SP AA message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*           Whenever applicable it's highly recommended disabling these messages,
*           as enabling this would stress the AUQ with excessive messages
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbSpAaMsgToCpuSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    data = (enable == GT_TRUE) ? 1 : 0;

    /* Enable/Disable SP AA msg to CPU */
    return prvCpssHwPpSetRegField(devNum, regAddr, 5, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbSpAaMsgToCpuSet
*
* DESCRIPTION:
*       Enabled/Disabled sending Aged Address (AA) messages to the CPU for the
*       Storm Prevention (SP) entries when those entries reach an aged out status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the SP AA message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*           Whenever applicable it's highly recommended disabling these messages,
*           as enabling this would stress the AUQ with excessive messages
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSpAaMsgToCpuSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbSpAaMsgToCpuSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbSpAaMsgToCpuSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbSpAaMsgToCpuGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending Aged Address (AA) messages
*       to the CPU for the Storm Prevention (SP) entries when those entries reach
*       an aged out status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the SP AA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbSpAaMsgToCpuGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32    regAddr;           /* register address    */
    GT_U32    data;              /* reg sub field data  */
    GT_STATUS rc;                /* return code         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 5, 1, &data);

    /* Enable/Disable status of SP AA msg to CPU */
    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbSpAaMsgToCpuGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending Aged Address (AA) messages
*       to the CPU for the Storm Prevention (SP) entries when those entries reach
*       an aged out status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the SP AA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSpAaMsgToCpuGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbSpAaMsgToCpuGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbSpAaMsgToCpuGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbAAandTAToCpuSet
*
* DESCRIPTION:
*       Enable/Disable the PP to/from sending an AA and TA address
*       update messages to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable the message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or auMsgType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           Whenever applicable it's highly recommended disabling these messages,
*           as enabling this would stress the AUQ with excessive messages
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAAandTAToCpuSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* Disable/Enable TA and AA */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }
    value = (enable == GT_TRUE) ? 1 : 0;

    /* Disable/Enable TA and AA */
    return prvCpssHwPpSetRegField(devNum, regAddr, 19, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbAAandTAToCpuSet
*
* DESCRIPTION:
*       Enable/Disable the PP to/from sending an AA and TA address
*       update messages to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable the message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or auMsgType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           Whenever applicable it's highly recommended disabling these messages,
*           as enabling this would stress the AUQ with excessive messages
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAAandTAToCpuSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAAandTAToCpuSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbAAandTAToCpuSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbAAandTAToCpuGet
*
* DESCRIPTION:
*       Get state of sending an AA and TA address update messages to the CPU
*       as configured to PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - the messages to CPU enabled
*                                  GT_FALSE - the messages to CPU disabled
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or auMsgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAAandTAToCpuGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
{
    GT_STATUS rc;           /* HW write return value*/
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to read from register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* HW support only AA,TA */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    /* Get Disable/Enable TA and AA */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 19, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbAAandTAToCpuGet
*
* DESCRIPTION:
*       Get state of sending an AA and TA address update messages to the CPU
*       as configured to PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - the messages to CPU enabled
*                                  GT_FALSE - the messages to CPU disabled
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or auMsgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAAandTAToCpuGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAAandTAToCpuGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbAAandTAToCpuGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbHashModeSet
*
* DESCRIPTION:
*       Sets the FDB hash function mode.
*       The CRC based hash function provides the best hash index distribution
*       for random addresses and vlans.
*       The XOR based hash function provides optimal hash index distribution
*       for controlled testing scenarios, where sequential addresses and vlans
*       are often used.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - hash function based mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbHashModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_HASH_FUNC_MODE_ENT  mode
)
{
    GT_U32      regAddr;    /* register adress              */
    GT_U32      data;       /* data to write to register    */
    GT_U32      data1 = 0;  /* data to write to register    */
    GT_STATUS   rc;         /* return code                  */
    GT_U32      fieldOffset; /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (mode)
    {
        case CPSS_MAC_HASH_FUNC_XOR_E:
            data = 0x0;
            break;
        case CPSS_MAC_HASH_FUNC_CRC_E:
            data = 0x1;
            break;
        case CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E:
            PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
            /* NOTE: For lookup it is relevant only when < FDBHashMode == CRC>
              But <Multi Hash Enable > is also responsible to ‘Legacy valid,skip’
              logic for the aging daemon --> 'chains' */
            data = 0x1;
            data1 = 0x1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig1;
        fieldOffset = 2;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
        fieldOffset = 21;
    }

    /* Set the FDBHashMode in the FDB Global Configuration Register */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, data);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 3, 1, data1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* Update FDB hash parameters */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode = mode;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbHashModeSet
*
* DESCRIPTION:
*       Sets the FDB hash function mode.
*       The CRC based hash function provides the best hash index distribution
*       for random addresses and vlans.
*       The XOR based hash function provides optimal hash index distribution
*       for controlled testing scenarios, where sequential addresses and vlans
*       are often used.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - hash function based mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_HASH_FUNC_MODE_ENT  mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbHashModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChBrgFdbHashModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbHashModeGet
*
* DESCRIPTION:
*       Gets the FDB hash function mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - hash function based mode:
*                 CPSS_FDB_HASH_FUNC_XOR_E - XOR based hash function mode
*                 CPSS_FDB_HASH_FUNC_CRC_E - CRC based hash function mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbHashModeGet
(
    IN  GT_U8                         devNum,
    OUT CPSS_MAC_HASH_FUNC_MODE_ENT   *modePtr
)
{
    GT_STATUS rc;           /* HW write return value*/
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to read from register */
    GT_U32 fieldOffset;     /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig1;
        fieldOffset = 2;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
        fieldOffset = 21;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (value == 1) ? CPSS_MAC_HASH_FUNC_CRC_E :
                              CPSS_MAC_HASH_FUNC_XOR_E;

    if((value == 1) && /*CPSS_MAC_HASH_FUNC_CRC_E*/
       PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 3, 1, &value);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(value)
        {
            *modePtr = CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E;
        }
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbHashModeGet
*
* DESCRIPTION:
*       Gets the FDB hash function mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - hash function based mode:
*                 CPSS_FDB_HASH_FUNC_XOR_E - XOR based hash function mode
*                 CPSS_FDB_HASH_FUNC_CRC_E - CRC based hash function mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashModeGet
(
    IN  GT_U8                         devNum,
    OUT CPSS_MAC_HASH_FUNC_MODE_ENT   *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbHashModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChBrgFdbHashModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* internal_cpssDxChBrgFdbAgingTimeoutSet
*
* DESCRIPTION:
*       Sets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE :
*       For Bobcat2 the device support aging time with granularity of 1 second
*           in range 1.. max. where max depend on FDB size and 'core clock'.
*           (the max value is very high and at least several thousands seconds !)
*       For DxCh3 the device support aging time with granularity of 16 seconds
*       (16,32..1008) . So value of 226 will be round down to 224 , and value of
*       255 will be round up to 256.
*       For others the device support aging time with
*       granularity of 10 seconds (10,20..630) .
*       So value of 234 will be round down to 230 ,
*       and value of 255 will be round up to 260.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       timeout - aging time in seconds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   For the 270MHz range from 16..1008 (but due to rounding we support values 8..636)
*   HW supports steps 16
*   For the 220MHz range from 9..573 (but due to rounding we support values 5..577)
*   HW supports steps 9 (9.09)
*   For the 200MHz range from 10..630 (but due to rounding we support values 5..634)
*   HW supports steps 10
*   For the 144 MHz range from 14..875 (but due to rounding we support values 7..882)
*   HW supports steps 13.88
*   For the 167 MHz range from 14..886 (but due to rounding we support values 7..893)
*   HW supports steps 14 (14.07)
*   For the 360MHz range from 10..630 (but due to rounding we support values 5..634)
*   HW supports steps 10
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAgingTimeoutSet
(
    IN GT_U8  devNum,
    IN GT_U32 timeout
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      refClock;   /* reference clock for aging unit calculation */
    GT_U32      hwValue;    /* value to write to register  */
    GT_STATUS   rc;         /* return status    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {

        /* check boundary */
        if (timeout == 0 ||
            timeout > PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbAging_maxSec)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }


        hwValue = timeout * PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbAging_oneSecGranularity;

        /* DO NOT modify value in
           PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral

           in bits 5..10 --> initialized during init without need to be changed.

           we will manipulate only the <FDBAgingWindowSize> register to get
           needed value in seconds.
        */
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBAgingWindowSize;

        /* write <aging_end_counter_val> (full register) Register */
        rc = prvCpssHwPpWriteRegister(devNum, regAddr, hwValue);
    }
    else
    {
        refClock = PRV_CPSS_PP_MAC(devNum)->baseCoreClock;
        /* xCat, xCat3 and xCat2 have reference clock different from base one (222 MHz). */
        switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            case CPSS_PP_FAMILY_DXCH_XCAT_E:
            case CPSS_PP_FAMILY_DXCH_XCAT3_E:
                refClock = 235; /* aging_unit = 2350/core_clock [sec] */
                break;
            case CPSS_PP_FAMILY_DXCH_XCAT2_E:
                refClock = 167; /* aging_unit = 1670/core_clock [sec] */
                break;
            default: break;
        }

        /* adjust timeout value according to aging unit.
           aging unit depends on core clock.
           register_value = timeout / aging_unit
           where aging_unit = reference_clock / core_clock  */
        timeout = (timeout * PRV_CPSS_PP_MAC(devNum)->coreClock) / refClock;

        if((PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E) ||
           (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E))
        {
            /* To round the number to nearest (*10) value, add 5 before divide by 10 .
            So value of 234 will be round down to 230 , and value of 255 will be
            round up to 260.
            */
            timeout = (timeout + 5) / 10;
        }
        /* DxCh3 devices */
        else
        {
            /* To round the number to nearest (*16) value, add 8 before divide by 16 .
            So value of 226 will be round down to 224 , and value of 255 will be
            round up to 256.
            */
            timeout = (timeout + 8) / 16;
        }
        /* check boundary */
        if (timeout == 0 || timeout > 63) /* values according to HW spec */
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

        /* write ActionTimer to FDB Action0 Register */
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 5, 6, (timeout));
    }

    return rc;
}

/******************************************************************************
* cpssDxChBrgFdbAgingTimeoutSet
*
* DESCRIPTION:
*       Sets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE :
*       For Bobcat2 the device support aging time with granularity of 1 second
*           in range 1.. max. where max depend on FDB size and 'core clock'.
*           (the max value is very high and at least several thousands seconds !)
*       For DxCh3 the device support aging time with granularity of 16 seconds
*       (16,32..1008) . So value of 226 will be round down to 224 , and value of
*       255 will be round up to 256.
*       For others the device support aging time with
*       granularity of 10 seconds (10,20..630) .
*       So value of 234 will be round down to 230 ,
*       and value of 255 will be round up to 260.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       timeout - aging time in seconds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   For the 270MHz range from 16..1008 (but due to rounding we support values 8..636)
*   HW supports steps 16
*   For the 220MHz range from 9..573 (but due to rounding we support values 5..577)
*   HW supports steps 9 (9.09)
*   For the 200MHz range from 10..630 (but due to rounding we support values 5..634)
*   HW supports steps 10
*   For the 144 MHz range from 14..875 (but due to rounding we support values 7..882)
*   HW supports steps 13.88
*   For the 167 MHz range from 14..886 (but due to rounding we support values 7..893)
*   HW supports steps 14 (14.07)
*   For the 360MHz range from 10..630 (but due to rounding we support values 5..634)
*   HW supports steps 10
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgingTimeoutSet
(
    IN GT_U8  devNum,
    IN GT_U32 timeout
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAgingTimeoutSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, timeout));

    rc = internal_cpssDxChBrgFdbAgingTimeoutSet(devNum, timeout);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, timeout));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* internal_cpssDxChBrgFdbAgingTimeoutGet
*
* DESCRIPTION:
*       Gets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE :
*       For Bobcat2 the device support aging time with granularity of 1 second
*           in range 1.. max. where max depend on FDB size and 'core clock'.
*           (the max value is very high and at least several thousands seconds !)
*       For DxCh3 the device support aging time with granularity of 16 seconds
*       (16,32..1008).
*       For others the device support aging time with
*       granularity of 10 seconds (10,20..630) .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       timeoutPtr - pointer to aging time in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For core clock of 270MHz The value ranges from 16 seconds to 1008
*       seconds in steps of 16 seconds.
*       For core clock of 220MHz The value ranges from 9 seconds to 573
*       seconds in steps of 9 seconds.
*       For core clock of 200 MHZ The value ranges from 10 seconds to 630
*       seconds in steps of 10 seconds.
*       For core clock of 144MHZ the aging timeout ranges from 14 (13.8) to 875
*       seconds in steps of 14 seconds.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAgingTimeoutGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *timeoutPtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_STATUS   rc;         /* return status                */
    GT_U32      hwValue;    /* value to read from register  */
    GT_U32      refClock;   /* reference clock for aging unit calculation */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(timeoutPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBAgingWindowSize;

        /* read <aging_end_counter_val> (full register) Register */
        rc = prvCpssHwPpReadRegister(devNum, regAddr, &hwValue);

        (*timeoutPtr) = hwValue / PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbAging_oneSecGranularity;
    }
    else
    {
        refClock = PRV_CPSS_PP_MAC(devNum)->baseCoreClock;
        /* xCat and xCat2 have reference clock different from base one (222 MHz). */
        switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            case CPSS_PP_FAMILY_DXCH_XCAT_E:
            case CPSS_PP_FAMILY_DXCH_XCAT3_E:
                refClock = 235; /* aging_unit = 2350/core_clock [sec] */
                break;
            case CPSS_PP_FAMILY_DXCH_XCAT2_E:
                refClock = 167; /* aging_unit = 1670/core_clock [sec] */
                break;
            default: break;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

        rc = prvCpssHwPpGetRegField(devNum, regAddr, 5, 6, &hwValue);

        if((PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E) ||
           (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E))
        {
            *timeoutPtr = ((hwValue * 10 * refClock) /
                            PRV_CPSS_PP_MAC(devNum)->coreClock);
        }
        else
        {
            *timeoutPtr = ((hwValue * 16 * refClock) /
                           PRV_CPSS_PP_MAC(devNum)->coreClock);
        }
    }

    return rc;
}

/******************************************************************************
* cpssDxChBrgFdbAgingTimeoutGet
*
* DESCRIPTION:
*       Gets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE :
*       For Bobcat2 the device support aging time with granularity of 1 second
*           in range 1.. max. where max depend on FDB size and 'core clock'.
*           (the max value is very high and at least several thousands seconds !)
*       For DxCh3 the device support aging time with granularity of 16 seconds
*       (16,32..1008).
*       For others the device support aging time with
*       granularity of 10 seconds (10,20..630) .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       timeoutPtr - pointer to aging time in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For core clock of 270MHz The value ranges from 16 seconds to 1008
*       seconds in steps of 16 seconds.
*       For core clock of 220MHz The value ranges from 9 seconds to 573
*       seconds in steps of 9 seconds.
*       For core clock of 200 MHZ The value ranges from 10 seconds to 630
*       seconds in steps of 10 seconds.
*       For core clock of 144MHZ the aging timeout ranges from 14 (13.8) to 875
*       seconds in steps of 14 seconds.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgingTimeoutGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *timeoutPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAgingTimeoutGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, timeoutPtr));

    rc = internal_cpssDxChBrgFdbAgingTimeoutGet(devNum, timeoutPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, timeoutPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbSecureAgingSet
*
* DESCRIPTION:
*      Enable or disable secure aging. This mode is relevant to automatic or
*      triggered aging with removal of aged out FDB entries. This mode is
*      applicable together with secure auto learning mode see
*      cpssDxChBrgFdbSecureAutoLearnSet. In the secure aging mode the device
*      sets aged out unicast FDB entry with <Multiple> =  1 and VIDX = 0xfff but
*      not invalidates entry. This causes packets destined to this FDB entry MAC
*      address to be flooded to the VLAN.  But such packets are known ones.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable secure aging
*                 GT_FALSE - disable secure aging
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for multi-port groups device :
*       1. Unified FDBs mode and Unified-Linked FDBs mode :
*           Since Automatic learning is not recommended in this mode nor secured automatic
*           learning .
*       2. Linked FDBs mode - no limitations
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbSecureAgingSet
(
    IN GT_U8          devNum,
    IN GT_BOOL        enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_U32    fieldOffset;       /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
        fieldOffset = 17;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;
        fieldOffset = 29;
    }

    data = (enable == GT_TRUE) ? 1 : 0;

    /* Enable/Disable secure aging */
    return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, data);

}

/*******************************************************************************
* cpssDxChBrgFdbSecureAgingSet
*
* DESCRIPTION:
*      Enable or disable secure aging. This mode is relevant to automatic or
*      triggered aging with removal of aged out FDB entries. This mode is
*      applicable together with secure auto learning mode see
*      cpssDxChBrgFdbSecureAutoLearnSet. In the secure aging mode the device
*      sets aged out unicast FDB entry with <Multiple> =  1 and VIDX = 0xfff but
*      not invalidates entry. This causes packets destined to this FDB entry MAC
*      address to be flooded to the VLAN.  But such packets are known ones.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable secure aging
*                 GT_FALSE - disable secure aging
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for multi-port groups device :
*       1. Unified FDBs mode and Unified-Linked FDBs mode :
*           Since Automatic learning is not recommended in this mode nor secured automatic
*           learning .
*       2. Linked FDBs mode - no limitations
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAgingSet
(
    IN GT_U8          devNum,
    IN GT_BOOL        enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbSecureAgingSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbSecureAgingSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbSecureAgingGet
*
* DESCRIPTION:
*      Get the status (enabled/disabled) of secure aging.
*      This mode is relevant to automatic or triggered aging with removal of
*      aged out FDB entries. This mode is applicable together with secure auto
*      learning mode see cpssDxChBrgFdbSecureAutoLearnGet. In the secure aging
*      mode  the device sets aged out unicast FDB entry with <Multiple> =  1 and
*      VIDX = 0xfff but not invalidates entry. This causes packets destined to
*      this FDB entry MAC address to be flooded to the VLAN.
*      But such packets are known ones.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable secure aging
*                    GT_FALSE - disable secure aging
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbSecureAgingGet
(
    IN  GT_U8          devNum,
    OUT GT_BOOL        *enablePtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_STATUS   rc;         /* return status                */
    GT_U32      hwValue;    /* value to read from register  */
    GT_U32      fieldOffset; /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
        fieldOffset = 17;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;
        fieldOffset = 29;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &hwValue);

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbSecureAgingGet
*
* DESCRIPTION:
*      Get the status (enabled/disabled) of secure aging.
*      This mode is relevant to automatic or triggered aging with removal of
*      aged out FDB entries. This mode is applicable together with secure auto
*      learning mode see cpssDxChBrgFdbSecureAutoLearnGet. In the secure aging
*      mode  the device sets aged out unicast FDB entry with <Multiple> =  1 and
*      VIDX = 0xfff but not invalidates entry. This causes packets destined to
*      this FDB entry MAC address to be flooded to the VLAN.
*      But such packets are known ones.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable secure aging
*                    GT_FALSE - disable secure aging
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAgingGet
(
    IN  GT_U8          devNum,
    OUT GT_BOOL        *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbSecureAgingGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbSecureAgingGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbSecureAutoLearnSet
*
* DESCRIPTION:
*        Configure secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       mode    - Enable/Disable secure automatic learning.
*                 The command assigned to packet with new SA not found in FDB
*                 when secure automatic learning enable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbSecureAutoLearnSet
(
    IN GT_U8                                devNum,
    IN CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  mode
)
{
    GT_STATUS   rc;
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg subfield data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    /* enable security Automatic Learning */
    data = 1;

    switch(mode)
    {
        case CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E:
            /* disable security Automatic Learning, New source addresses
               received on Port/Vlan configured to Automatic learning are
               learned automatically */
            data = 0;
            break;
        case CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E:
            /* enable security Automatic Learning , Set security automatic learn
             unknown Source command: 0 - trap to CPU */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                   data |= (2 << 1);
            }
            break;
        case CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E:
            /* enable security Automatic Learning , Set security automatic learn
            unknown Source command:1 - hard drop */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                   data |= (3 << 1);
            }
            else
            {
                data |= (1 << 1);
            }
            break;
        case CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E:
            /* enable security Automatic Learning , Set security automatic learn
            unknown Source command: 2 - soft drop */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                   data |= (4 << 1);
            }
            else
            {
                data |= (2 << 1);
            }
            break;
        case CPSS_MAC_SECURE_AUTO_LEARN_UNK_FORWARD_E:
            /* enable security Automatic Learning , Set security automatic learn
            unknown Source command: 0 - forward.
            The variable data is already has this value - do nothing in the case */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_MAC_SECURE_AUTO_LEARN_UNK_MIRROR_TO_CPU_E:
            /* enable security Automatic Learning , Set security automatic learn
            unknown Source command: 1 - Mirror To CPU */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                   data |= (1 << 1);
            }
            else
            {
                 CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
            bridgeEngineConfig.bridgeGlobalConfig0;

        /* Secure Auto Learning Enable */
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 9, 1, (data & 0x1));
        if (rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
            bridgeEngineConfig.bridgeCommandConfig0;

        /* Secure Auto Learning Unknown Source Command */
        return prvCpssHwPpSetRegField(devNum, regAddr, 24, 3, (data >> 1));
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            bridgeRegs.bridgeGlobalConfigRegArray[0];

    return prvCpssHwPpSetRegField(devNum, regAddr, 18, 3, data);

}

/*******************************************************************************
* cpssDxChBrgFdbSecureAutoLearnSet
*
* DESCRIPTION:
*        Configure secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       mode    - Enable/Disable secure automatic learning.
*                 The command assigned to packet with new SA not found in FDB
*                 when secure automatic learning enable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAutoLearnSet
(
    IN GT_U8                                devNum,
    IN CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbSecureAutoLearnSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChBrgFdbSecureAutoLearnSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbSecureAutoLearnGet
*
* DESCRIPTION:
*        Get secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr  - Enabled/Disabled secure automatic learning.
*                  The command assigned to packet with new SA not found in FDB
*                  when secure automatic learning enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbSecureAutoLearnGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  *modePtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_STATUS   rc;         /* return status                */
    GT_U32      hwValue;    /* value to read from register  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
            bridgeEngineConfig.bridgeGlobalConfig0;

        /* Secure Auto Learning Enable */
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 9, 1, &hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (hwValue == 0)
        {
            *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E;
            return GT_OK;
        }

        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
            bridgeEngineConfig.bridgeCommandConfig0;

        /* Secure Auto Learning Unknown Source Command */
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 24, 3, &hwValue);

        switch (hwValue)
        {
            case 0:
                *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_UNK_FORWARD_E;
                /* Security Automatic Learning enabled, Security automatic learn
                 unknown Source command is Forward */
                break;
            case 1:
                *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_UNK_MIRROR_TO_CPU_E;
                /* Security Automatic Learning enabled, Security automatic learn
                 unknown Source command is Mirror to CPU */
                break;
            case 2:
                *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E;
                /* Security Automatic Learning enabled, Security automatic learn
                 unknown Source command is trap to CPU */
                break;
            case 3:
                *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E;
                /* Security Automatic Learning enabled, Security automatic learn
                 unknown Source command is hard drop */
                break;
            case 4:
                *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E;
                /* Security Automatic Learning enabled, Security automatic learn
                 unknown Source command is soft drop */
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        return rc;
    }

    /* Get Bridge Global Configuration Register0 address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.bridgeGlobalConfigRegArray[0];

    /* get <Secure Auto Learn Enable> and
      <Secure Auto Learning Unknown Src Command> */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 18, 3, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (hwValue)
    {
        case 0:
            *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E;
            /* Security Automatic Learning is disabled. New source addresses
               received on Port/Vlan configured to Automatic learning are
               learned automatically */
            break;
        case 1:
            *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E;
            /* Security Automatic Learning enabled, Security automatic learn
             unknown Source command is trap to CPU */
            break;
        case 3:
            *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E;
            /* Security Automatic Learning enabled, Security automatic learn
             unknown Source command is hard drop */
            break;
        case 5:
            *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E;
            /* Security Automatic Learning enabled, Security automatic learn
             unknown Source command is soft drop */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbSecureAutoLearnGet
*
* DESCRIPTION:
*        Get secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr  - Enabled/Disabled secure automatic learning.
*                  The command assigned to packet with new SA not found in FDB
*                  when secure automatic learning enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAutoLearnGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbSecureAutoLearnGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChBrgFdbSecureAutoLearnGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbStaticTransEnable
*
* DESCRIPTION:
*       This routine determines whether the transplanting operate on static
*       entries.
*       When the PP will do the transplanting , it will/won't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it will/won't Flush the FDB static entries
*       (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE transplanting is enabled on static entries,
*                GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbStaticTransEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    value = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 11, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbStaticTransEnable
*
* DESCRIPTION:
*       This routine determines whether the transplanting operate on static
*       entries.
*       When the PP will do the transplanting , it will/won't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it will/won't Flush the FDB static entries
*       (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE transplanting is enabled on static entries,
*                GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticTransEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbStaticTransEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbStaticTransEnable(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbStaticTransEnableGet
*
* DESCRIPTION:
*       Get whether the transplanting enabled to operate on static entries.
*       when the PP do the transplanting , it do/don't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it do/don't Flush the FDB static entries
*       (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE transplanting is enabled on static entries,
*                   GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbStaticTransEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */
    GT_STATUS rc;       /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 11, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbStaticTransEnableGet
*
* DESCRIPTION:
*       Get whether the transplanting enabled to operate on static entries.
*       when the PP do the transplanting , it do/don't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it do/don't Flush the FDB static entries
*       (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE transplanting is enabled on static entries,
*                   GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticTransEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbStaticTransEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbStaticTransEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbStaticDelEnable
*
* DESCRIPTION:
*       This routine determines whether flush delete operates on static entries.
*       When the PP will do the Flush , it will/won't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - FDB deletion mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbStaticDelEnable
(
    IN GT_U8                             devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT    mode
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    CPSS_TBD_BOOKMARK_EARCH
    /* need to add support for 3 values */

    switch (mode)
    {
        case CPSS_DXCH_BRG_FDB_DEL_MODE_DYNAMIC_ONLY_E:
            value=0;
            break;
        case CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E:
            value=1;
            break;
        case CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E:
            value=2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
        return prvCpssHwPpSetRegField(devNum, regAddr, 12, 2, value);
    }
    else
    {
        if (mode==CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
        return prvCpssHwPpSetRegField(devNum, regAddr, 12, 1, value);
    }

}

/*******************************************************************************
* cpssDxChBrgFdbStaticDelEnable
*
* DESCRIPTION:
*       This routine determines whether flush delete operates on static entries.
*       When the PP will do the Flush , it will/won't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - FDB deletion mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticDelEnable
(
    IN GT_U8                             devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT    mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbStaticDelEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChBrgFdbStaticDelEnable(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbStaticDelEnableGet
*
* DESCRIPTION:
*       Get whether flush delete operates on static entries.
*       When the PP do the Flush , it do/don't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - FDB deletion mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbStaticDelEnableGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_BRG_FDB_DEL_MODE_ENT  *modePtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */
    GT_STATUS rc;       /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    CPSS_TBD_BOOKMARK_EARCH
    /* need to add support for 3 values */

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 12, 2, &value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 12, 1, &value);
    }

    switch(value)
    {
        case 0:
            *modePtr=CPSS_DXCH_BRG_FDB_DEL_MODE_DYNAMIC_ONLY_E;
            break;
        case 1:
            *modePtr=CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E;
            break;
        case 2:
            *modePtr=CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* should not happen */
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbStaticDelEnableGet
*
* DESCRIPTION:
*       Get whether flush delete operates on static entries.
*       When the PP do the Flush , it do/don't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - FDB deletion mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticDelEnableGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_BRG_FDB_DEL_MODE_ENT  *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbStaticDelEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChBrgFdbStaticDelEnableGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionsEnableSet
*
* DESCRIPTION:
*       Enables/Disables FDB actions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE -  Actions are enabled
*                 GT_FALSE -  Actions are disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    value = BOOL2BIT_MAC(enable);

    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbActionsEnableSet
*
* DESCRIPTION:
*       Enables/Disables FDB actions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE -  Actions are enabled
*                 GT_FALSE -  Actions are disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionsEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbActionsEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionsEnableGet
*
* DESCRIPTION:
*       Get the status of FDB actions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - actions are enabled
*                    GT_FALSE - actions are disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionsEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */
    GT_STATUS rc;       /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbActionsEnableGet
*
* DESCRIPTION:
*       Get the status of FDB actions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - actions are enabled
*                    GT_FALSE - actions are disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionsEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionsEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbActionsEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbTrigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       actFinishedPtr  - GT_TRUE  - triggered action completed
*                         GT_FALSE - triggered action is not completed yet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           'action finished' only when action finished on all port groups.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbTrigActionStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *actFinishedPtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_STATUS rc;       /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actFinishedPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    /* check that all port groups are ready */
    rc = prvCpssPortGroupBusyWait(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                regAddr,1,
                GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if(rc == GT_BAD_STATE)
    {
        rc = GT_OK;
        /* not all port groups has the bit with value 0 */
        *actFinishedPtr = GT_FALSE;
    }
    else
    {
        *actFinishedPtr = GT_TRUE;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbTrigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       actFinishedPtr  - GT_TRUE  - triggered action completed
*                         GT_FALSE - triggered action is not completed yet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           'action finished' only when action finished on all port groups.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrigActionStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *actFinishedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbTrigActionStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, actFinishedPtr));

    rc = internal_cpssDxChBrgFdbTrigActionStatusGet(devNum, actFinishedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, actFinishedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMacTriggerModeSet
*
* DESCRIPTION:
*       Sets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacTriggerModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ACTION_MODE_ENT     mode
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    switch (mode)
    {
        case CPSS_ACT_AUTO_E:
            value = 0;
            break;
        case CPSS_ACT_TRIG_E:
            value = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 2, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerModeSet
*
* DESCRIPTION:
*       Sets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ACTION_MODE_ENT     mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacTriggerModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChBrgFdbMacTriggerModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMacTriggerModeGet
*
* DESCRIPTION:
*       Gets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacTriggerModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_MAC_ACTION_MODE_ENT     *modePtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */
    GT_STATUS rc;       /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 2, 1, &value);

    *modePtr = (value == 1) ? CPSS_ACT_TRIG_E : CPSS_ACT_AUTO_E;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerModeGet
*
* DESCRIPTION:
*       Gets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_MAC_ACTION_MODE_ENT     *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacTriggerModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChBrgFdbMacTriggerModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbStaticOfNonExistDevRemove
*
* DESCRIPTION:
*       enable the ability to delete static mac entries when the aging daemon in
*       PP encounter with entry registered on non-exist device in the
*       "device table" the PP removes it ---> this flag regard the static
*       entries
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*       deleteStatic -  GT_TRUE - Action will delete static entries as well as
*                                 dynamic
*                       GT_FALSE - only dynamic entries will be deleted.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It's application responsibility to check the status of Aging Trigger by
*       busy wait (use cpssDxChBrgFdbTrigActionStatusGet API), in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbStaticOfNonExistDevRemove
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     deleteStatic
)
{
    GT_U32      regAddr; /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    /* set action 2 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    return prvCpssHwPpSetRegField(devNum, regAddr, 28, 1,
                                   ((deleteStatic == GT_TRUE) ? 1 : 0));
}

/*******************************************************************************
* cpssDxChBrgFdbStaticOfNonExistDevRemove
*
* DESCRIPTION:
*       enable the ability to delete static mac entries when the aging daemon in
*       PP encounter with entry registered on non-exist device in the
*       "device table" the PP removes it ---> this flag regard the static
*       entries
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*       deleteStatic -  GT_TRUE - Action will delete static entries as well as
*                                 dynamic
*                       GT_FALSE - only dynamic entries will be deleted.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It's application responsibility to check the status of Aging Trigger by
*       busy wait (use cpssDxChBrgFdbTrigActionStatusGet API), in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticOfNonExistDevRemove
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     deleteStatic
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbStaticOfNonExistDevRemove);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, deleteStatic));

    rc = internal_cpssDxChBrgFdbStaticOfNonExistDevRemove(devNum, deleteStatic);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, deleteStatic));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
*
* DESCRIPTION:
*       Get whether enabled/disabled the ability to delete static mac entries
*       when the aging daemon in PP encounter with entry registered on non-exist
*       device in the "device table".
*       entries
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       deleteStaticPtr -  GT_TRUE - Action deletes entries if there associated
*                                    device number don't exist (static etries
*                                                                  as well)
*                          GT_FALSE - Action don't delete entries if there
*                                     associated device number don't exist
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *deleteStaticPtr
)
{
    GT_U32      value;      /* hw value from register   */
    GT_U32      regAddr;    /* register address         */
    GT_STATUS   rc;         /* return code              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(deleteStaticPtr);

    /* set action 2 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 28, 1, &value);

    *deleteStaticPtr = BIT2BOOL_MAC(value);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
*
* DESCRIPTION:
*       Get whether enabled/disabled the ability to delete static mac entries
*       when the aging daemon in PP encounter with entry registered on non-exist
*       device in the "device table".
*       entries
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       deleteStaticPtr -  GT_TRUE - Action deletes entries if there associated
*                                    device number don't exist (static etries
*                                                                  as well)
*                          GT_FALSE - Action don't delete entries if there
*                                     associated device number don't exist
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *deleteStaticPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbStaticOfNonExistDevRemoveGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, deleteStaticPtr));

    rc = internal_cpssDxChBrgFdbStaticOfNonExistDevRemoveGet(devNum, deleteStaticPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, deleteStaticPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbDropAuEnableSet
*
* DESCRIPTION:
*       Enable/Disable dropping the Address Update messages when the queue is
*       full .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - Drop the address update messages
*                 GT_FALSE - Do not drop the address update messages
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbDropAuEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      data;       /* data to be written to register   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* get address of Address Update Queue Control register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;

    data = BOOL2BIT_MAC(enable);

    return prvCpssHwPpSetRegField(devNum, regAddr, 31, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbDropAuEnableSet
*
* DESCRIPTION:
*       Enable/Disable dropping the Address Update messages when the queue is
*       full .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - Drop the address update messages
*                 GT_FALSE - Do not drop the address update messages
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDropAuEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbDropAuEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbDropAuEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbDropAuEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of dropping the Address Update messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - The address update messages are dropped
*                    GT_FALSE - The address update messages are not dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbDropAuEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      data;       /* data to be written to register   */
    GT_STATUS   rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* get address of Address Update Queue Control register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 31, 1, &data);

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbDropAuEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of dropping the Address Update messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - The address update messages are dropped
*                    GT_FALSE - The address update messages are not dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDropAuEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbDropAuEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbDropAuEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with trunk,
*       regardless of the device to which they are associated.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*       ageOutAllDevOnTrunkEnable   - GT_TRUE : All entries associated with a
*                                     trunk, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     trunk on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnTrunkEnable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_U32    fieldOffset;       /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
        fieldOffset = 14;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;
        fieldOffset = 16;
    }

    data = (ageOutAllDevOnTrunkEnable == GT_TRUE) ? 1 : 0;

    /* Enable/Disable aging out of all entries associated with trunk */
    return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with trunk,
*       regardless of the device to which they are associated.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*       ageOutAllDevOnTrunkEnable   - GT_TRUE : All entries associated with a
*                                     trunk, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     trunk on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnTrunkEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ageOutAllDevOnTrunkEnable));

    rc = internal_cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable(devNum, ageOutAllDevOnTrunkEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ageOutAllDevOnTrunkEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with trunk, regardless of
*       the device to which they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                      trunk, regardless of the device they
*                                      are associated with are aged out.
*                                      GT_FALSE: Only entries associated with a
*                                      trunk on the local device are aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnTrunkEnablePtr
)
{
    GT_U32      regAddr;    /* register address         */
    GT_U32      data;       /* reg sub field data       */
    GT_STATUS   rc;         /* return code              */
    GT_U32      fieldOffset; /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ageOutAllDevOnTrunkEnablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
        fieldOffset = 14;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;
        fieldOffset = 16;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &data);

    *ageOutAllDevOnTrunkEnablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with trunk, regardless of
*       the device to which they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                      trunk, regardless of the device they
*                                      are associated with are aged out.
*                                      GT_FALSE: Only entries associated with a
*                                      trunk on the local device are aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnTrunkEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ageOutAllDevOnTrunkEnablePtr));

    rc = internal_cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet(devNum, ageOutAllDevOnTrunkEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ageOutAllDevOnTrunkEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with port,
*       regardless of the device to which they are associated.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                       - device number
*       ageOutAllDevOnNonTrunkEnable - GT_TRUE : All entries associated with a
*                                     port, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     port on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnNonTrunkEnable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_U32    fieldOffset;       /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
        fieldOffset = 15;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;
        fieldOffset = 17;
    }

    data = (ageOutAllDevOnNonTrunkEnable == GT_TRUE) ? 1 : 0;

    /* Enable/Disable aging out of all entries associated with trunk */
    return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with port,
*       regardless of the device to which they are associated.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                       - device number
*       ageOutAllDevOnNonTrunkEnable - GT_TRUE : All entries associated with a
*                                     port, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     port on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnNonTrunkEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ageOutAllDevOnNonTrunkEnable));

    rc = internal_cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable(devNum, ageOutAllDevOnNonTrunkEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ageOutAllDevOnNonTrunkEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with port, regardless of
*       the device to which they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnNonTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                         port, regardless of the device they
*                                         are associated with are aged out.
*                                         GT_FALSE: Only entries associated with a
*                                         port on the local device are aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnNonTrunkEnablePtr
)
{
    GT_U32      regAddr;    /* register address         */
    GT_U32      data;       /* reg sub field data       */
    GT_STATUS   rc;         /* return code              */
    GT_U32     fieldOffset; /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ageOutAllDevOnNonTrunkEnablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
        fieldOffset = 15;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;
        fieldOffset = 17;

    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &data);

    *ageOutAllDevOnNonTrunkEnablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with port, regardless of
*       the device to which they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnNonTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                         port, regardless of the device they
*                                         are associated with are aged out.
*                                         GT_FALSE: Only entries associated with a
*                                         port on the local device are aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnNonTrunkEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ageOutAllDevOnNonTrunkEnablePtr));

    rc = internal_cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet(devNum, ageOutAllDevOnNonTrunkEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ageOutAllDevOnNonTrunkEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionTransplantDataSet
*
* DESCRIPTION:
*       Prepares the entry for transplanting (old and new interface parameters).
*       VLAN and VLAN mask for transplanting is set by
*       cpssDxChBrgFdbActionActiveVlanSet. Execution of transplanting is done
*       by cpssDxChBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       oldInterfacePtr - pointer to old Interface parameters
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters
*                         (it may be only port or trunk).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionTransplantDataSet
(
    IN GT_U8                    devNum,
    IN CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    IN CPSS_INTERFACE_INFO_STC  *newInterfacePtr
)
{
    GT_U32      action0RegAddr;    /* register address of Action0 Register  */
    GT_U32      action1RegAddr;    /* register address of Action1 Register  */
    GT_U32      portGroupId;       /*the port group Id - support multi-port-groups device */
    GT_U32      data;              /* reg sub field data       */
    GT_U32      newPortTrunk;
    GT_U32      oldPortTrunk;
    GT_U8       newIsTrunk;
    GT_U8       oldIsTrunk;
    GT_U32      newDevNum = 0;
    GT_U32      oldDevNum = 0;
    GT_U32      portTrunkSize = 13; /* size of PortTrunk fields in registers */
    GT_STATUS   rc;                 /* return code              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(oldInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(newInterfacePtr);

    /* check old interface type and set old interface parameters accordingly */
    switch (oldInterfacePtr->type)
    {
        case CPSS_INTERFACE_PORT_E:
            oldIsTrunk = 0;

            PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(oldInterfacePtr->devPort.portNum,
                                                   oldInterfacePtr->devPort.portNum);
            oldPortTrunk = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                oldInterfacePtr->devPort.hwDevNum,
                oldInterfacePtr->devPort.portNum);
            oldDevNum = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                oldInterfacePtr->devPort.hwDevNum,
                oldInterfacePtr->devPort.portNum);

            /* check range of device number and portTrunk */
            if ((oldDevNum > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum))||
                (oldPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum)))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            break;

        case CPSS_INTERFACE_TRUNK_E:
            oldIsTrunk = 1;
            oldDevNum = oldInterfacePtr->hwDevNum;
            oldPortTrunk = (GT_U32)oldInterfacePtr->trunkId;
            /* check range of portTrunk */
            if (oldPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check new interface type and set new interface parameters accordingly */
    switch (newInterfacePtr->type)
    {
        case CPSS_INTERFACE_PORT_E:
            newIsTrunk = 0;

            PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(newInterfacePtr->devPort.hwDevNum,
                                                            newInterfacePtr->devPort.portNum);
            newPortTrunk = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                newInterfacePtr->devPort.hwDevNum,
                newInterfacePtr->devPort.portNum);
            newDevNum = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                newInterfacePtr->devPort.hwDevNum,
                newInterfacePtr->devPort.portNum);

            /* check range of device number */
            /* check range of device number and portTrunk */
            if ((newDevNum > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum))||
                (newPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum)))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            break;

        case CPSS_INTERFACE_TRUNK_E:
            newIsTrunk = 1;
            newDevNum = newInterfacePtr->hwDevNum;
            newPortTrunk = (GT_U32)newInterfacePtr->trunkId;
            if (newPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        action0RegAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionOldParameters;
        action1RegAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionNewParameters;
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            portTrunkSize = 15;
        }
    }
    else
    {
        /* get Action0 and Action1 registers addresses */
        action0RegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                    bridgeRegs.macTblAction0;
        action1RegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                    bridgeRegs.macTblAction1;
    }

    /* loop on all active port groups in the device */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(PRV_CPSS_PP_MAC(devNum)->hwDevNum) &&
            (portGroupId >= (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups / 2)))/* hemisphere 1 */
        {
            if(newIsTrunk)
            {
                /* the 'new' trunk entries in dual deviceId device are set according to the hemisphere */
                newDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum) + 1;
            }

            if(oldIsTrunk)
            {
                /* the 'old' trunk entries in dual deviceId device were set according to the hemisphere */
                oldDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum) + 1;
            }
        }

        /* set data for Action0 register */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            data = ((oldPortTrunk & BIT_MASK_0_31_MAC(portTrunkSize)) |
                    (oldIsTrunk << portTrunkSize) |
                    ((oldDevNum & 0x3FF) << (portTrunkSize + 1)));

            rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId , action0RegAddr, 0, (portTrunkSize + 11), data);
        }
        else
        {
            data = ((oldPortTrunk & 0x7F) | ((oldDevNum & 0x1F) << 7) | ((newPortTrunk & 0x7F) << 12));

            rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId , action0RegAddr, 13, 19, data);
        }
        if (rc != GT_OK)
        {
            return rc;
        }

        /* set data for Action1 register */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            data = ((newPortTrunk & BIT_MASK_0_31_MAC(portTrunkSize)) |
                    (newIsTrunk << portTrunkSize) |
                    ((newDevNum & 0x3FF) << (portTrunkSize+1)));

            rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId , action1RegAddr, 0, (portTrunkSize + 11), data);
        }
        else
        {
            data = ((newDevNum & 0x1F) | (newIsTrunk << 5) | (oldIsTrunk << 6));

            rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId , action1RegAddr, 0, 7, data);
        }
        if (rc != GT_OK)
        {
            return rc;
        }

        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbActionTransplantDataSet
*
* DESCRIPTION:
*       Prepares the entry for transplanting (old and new interface parameters).
*       VLAN and VLAN mask for transplanting is set by
*       cpssDxChBrgFdbActionActiveVlanSet. Execution of transplanting is done
*       by cpssDxChBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       oldInterfacePtr - pointer to old Interface parameters
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters
*                         (it may be only port or trunk).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionTransplantDataSet
(
    IN GT_U8                    devNum,
    IN CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    IN CPSS_INTERFACE_INFO_STC  *newInterfacePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionTransplantDataSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, oldInterfacePtr, newInterfacePtr));

    rc = internal_cpssDxChBrgFdbActionTransplantDataSet(devNum, oldInterfacePtr, newInterfacePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, oldInterfacePtr, newInterfacePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionTransplantDataGet
*
* DESCRIPTION:
*       Get transplant data: old interface parameters and the new ones.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*
*
* OUTPUTS:
*       oldInterfacePtr - pointer to old Interface parameters.
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters.
*                         (it may be only port or trunk).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionTransplantDataGet
(
    IN  GT_U8                    devNum,
    OUT CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    OUT CPSS_INTERFACE_INFO_STC  *newInterfacePtr
)
{
    GT_U32      action0RegAddr;    /* register address of Action0 Register  */
    GT_U32      action1RegAddr;    /* register address of Action1 Register  */
    GT_U32      data;              /* reg sub field data       */
    GT_U32      newPortTrunk;
    GT_U32      oldPortTrunk;
    GT_U32      newIsTrunk;
    GT_U32      oldIsTrunk;
    GT_U32      newDevNum;
    GT_U32      oldDevNum;
    GT_U32      portTrunkSize = 13; /* size of PortTrunk fields in registers */
    GT_STATUS   rc;                 /* return code              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(oldInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(newInterfacePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        action0RegAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionOldParameters;
        action1RegAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionNewParameters;

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            portTrunkSize = 15;
        }

        /* get data from Action0 register */
        rc = prvCpssHwPpGetRegField(devNum, action0RegAddr, 0, (portTrunkSize + 11), &data);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* parse data from Action0 register */
        oldPortTrunk = (data & BIT_MASK_0_31_MAC(portTrunkSize));
        oldIsTrunk   = ((data >> portTrunkSize) & 0x1);
        oldDevNum    = ((data >> (portTrunkSize + 1)) & 0x3FF);

        /* get data from Action1 register */
        rc = prvCpssHwPpGetRegField(devNum, action1RegAddr, 0, (portTrunkSize + 11), &data);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* parse data from Action1 register */
        newPortTrunk = (data & BIT_MASK_0_31_MAC(portTrunkSize));
        newIsTrunk   = ((data >> portTrunkSize) & 0x1);
        newDevNum    = ((data >> (portTrunkSize + 1)) & 0x3FF);
    }
    else
    {
        /* get Action0 and Action1 registers addresses */
        action0RegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                    bridgeRegs.macTblAction0;
        action1RegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                    bridgeRegs.macTblAction1;

        /* get data from Action0 register */
        rc = prvCpssHwPpGetRegField(devNum, action0RegAddr, 13, 19, &data);

        if (rc != GT_OK)
        {
            return rc;
        }

        /* parse data from Action0 register */
        oldPortTrunk = (data & 0x7F);
        oldDevNum = ((data >> 7) & 0x1F);
        newPortTrunk = ((data >> 12) & 0x7F);

        /* get data from Action1 register */
        rc = prvCpssHwPpGetRegField(devNum, action1RegAddr, 0, 7, &data);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* parse data from Action0 register */
        newDevNum = (data & 0x1F);
        newIsTrunk = ((data >> 5) & 0x1);
        oldIsTrunk = ((data >> 6) & 0x1);
    }

    /* check new interface type */
    if (newIsTrunk)
    {
        /* Trunk */
        newInterfacePtr->hwDevNum = newDevNum;
        newInterfacePtr->trunkId = (GT_TRUNK_ID)newPortTrunk;
        newInterfacePtr->type = CPSS_INTERFACE_TRUNK_E;
    }
    else
    {
        /* Port */
        newInterfacePtr->devPort.hwDevNum = newDevNum;
        newInterfacePtr->devPort.portNum = newPortTrunk;

        PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(newInterfacePtr);

        newInterfacePtr->type = CPSS_INTERFACE_PORT_E;
    }

    /* check old interface type */
    if (oldIsTrunk)
    {
        /* Trunk */
        oldInterfacePtr->hwDevNum = oldDevNum;
        oldInterfacePtr->trunkId = (GT_TRUNK_ID)oldPortTrunk;
        oldInterfacePtr->type = CPSS_INTERFACE_TRUNK_E;
    }
    else
    {
        /* Port */
        oldInterfacePtr->devPort.hwDevNum = oldDevNum;
        oldInterfacePtr->devPort.portNum = oldPortTrunk;

        PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(oldInterfacePtr);

        oldInterfacePtr->type = CPSS_INTERFACE_PORT_E;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbActionTransplantDataGet
*
* DESCRIPTION:
*       Get transplant data: old interface parameters and the new ones.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*
*
* OUTPUTS:
*       oldInterfacePtr - pointer to old Interface parameters.
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters.
*                         (it may be only port or trunk).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionTransplantDataGet
(
    IN  GT_U8                    devNum,
    OUT CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    OUT CPSS_INTERFACE_INFO_STC  *newInterfacePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionTransplantDataGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, oldInterfacePtr, newInterfacePtr));

    rc = internal_cpssDxChBrgFdbActionTransplantDataGet(devNum, oldInterfacePtr, newInterfacePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, oldInterfacePtr, newInterfacePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function returns AU message processing completion and success status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       entryOffsetPtr - (pointer to) to receive the offset from the hash calculated index
*                     NULL value means that caller not need to retrieve this value.
*
* OUTPUTS:
*       completedPtr - pointer to AU message processing completion.
*                      GT_TRUE - AU message was processed by PP.
*                      GT_FALSE - AU message processing is not completed yet by PP.
*       succeededPtr - pointer to a success of AU operation.
*                      GT_TRUE  - the AU action succeeded.
*                      GT_FALSE - the AU action has failed.
*       entryOffsetPtr - (pointer to) the offset from the hash calculated index
*                      of the entry that action performed on.
*                      (similar to parameter in an AU message CPSS_MAC_UPDATE_MSG_EXT_STC::entryOffset)
*                      The parameter ignored when NULL pointer.
*                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      the offset is relevant only when:
*                      1. 'completed = GT_TRUE and succeeded = GT_TRUE'
*                      2. The function that triggered the action is one of :
*                        a. cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbPortGroupMacEntryDelete
*                         the offset relate to the delete entry.
*                        b. cpssDxChBrgFdbMacEntrySet, cpssDxChBrgFdbPortGroupMacEntrySet functions.
*                         the offset relate to the inserted/updated entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DXCH devices the succeed status doesn't relevant for query
*       address messages (message type = CPSS_QA_E) -  FEr#93.
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbFromCpuAuMsgStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *completedPtr,
    OUT GT_BOOL  *succeededPtr,
    INOUT GT_U32 *entryOffsetPtr
)
{
    GT_STATUS   rc;             /* return code                  */
    GT_U32      regAddr;        /* register address             */
    GT_U32      hwData;         /* data to read from register   */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(completedPtr);
    CPSS_NULL_PTR_CHECK_MAC(succeededPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAddrUpdateMsgs.AUMsgFromCPU.AUMsgFromCPUCtrl;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    globalRegs.addrUpdateControlReg;
    }

    /* check if the message action is completed, read Message from CPU
       Management register */
    /* check that all port groups are ready */
    rc = prvCpssPortGroupBusyWait(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                regAddr,0,
                GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if(rc == GT_BAD_STATE)
    {
        /* not all port groups has the bit with value 0 */
        *completedPtr = GT_FALSE;

        return GT_OK;
    }
    else if (rc != GT_OK)
    {
        return rc;
    }

    /* the action is completed */
    *completedPtr = GT_TRUE;
    /* loop on all active port groups */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /* get the succeeded bit */
        rc = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId,regAddr,&hwData);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* check if the action has succeeded */
        if((hwData & BIT_1) == 0)
        {
            /* at least one port group not succeeded */
            *succeededPtr = GT_FALSE;
            return GT_OK;
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if(entryOffsetPtr)
            {
                *entryOffsetPtr = U32_GET_FIELD_MAC(hwData,2,5);
            }
        }

        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    /* all port groups succeeded */
    *succeededPtr = GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function returns AU message processing completion and success status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       entryOffsetPtr - (pointer to) to receive the offset from the hash calculated index
*                     NULL value means that caller not need to retrieve this value.
*
* OUTPUTS:
*       completedPtr - pointer to AU message processing completion.
*                      GT_TRUE - AU message was processed by PP.
*                      GT_FALSE - AU message processing is not completed yet by PP.
*       succeededPtr - pointer to a success of AU operation.
*                      GT_TRUE  - the AU action succeeded.
*                      GT_FALSE - the AU action has failed.
*       entryOffsetPtr - (pointer to) the offset from the hash calculated index
*                      of the entry that action performed on.
*                      (similar to parameter in an AU message CPSS_MAC_UPDATE_MSG_EXT_STC::entryOffset)
*                      The parameter ignored when NULL pointer.
*                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      the offset is relevant only when:
*                      1. 'completed = GT_TRUE and succeeded = GT_TRUE'
*                      2. The function that triggered the action is one of :
*                        a. cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbPortGroupMacEntryDelete
*                         the offset relate to the delete entry.
*                        b. cpssDxChBrgFdbMacEntrySet, cpssDxChBrgFdbPortGroupMacEntrySet functions.
*                         the offset relate to the inserted/updated entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DXCH devices the succeed status doesn't relevant for query
*       address messages (message type = CPSS_QA_E) -  FEr#93.
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbFromCpuAuMsgStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *completedPtr,
    OUT GT_BOOL  *succeededPtr,
    INOUT GT_U32 *entryOffsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbFromCpuAuMsgStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, completedPtr, succeededPtr, entryOffsetPtr));

    rc = internal_cpssDxChBrgFdbFromCpuAuMsgStatusGet(devNum, completedPtr, succeededPtr, entryOffsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, completedPtr, succeededPtr, entryOffsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionActiveVlanSet
*
* DESCRIPTION:
*       Set action active vlan and vlan mask.
*       All actions will be taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan Id
*       vlanMask    - vlan mask filter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionActiveVlanSet
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   vlanMask
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      regAddr;        /* register address             */
    GT_U32      hwData;         /* data to read from register   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* check ranges */
    if ((vlanId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum)) ||
        (vlanMask > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction0;
        hwData = ((vlanId & 0x1FFF) | ((vlanMask & 0x1FFF) << 16));

        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 28, hwData);

        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction1;

    hwData = ((vlanId & 0xFFF) | ((vlanMask & 0xFFF) << 12));

    /* set Active Vlan and Active Vlan Mask in FDB Action1 register */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 8, 24, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveVlanSet
*
* DESCRIPTION:
*       Set action active vlan and vlan mask.
*       All actions will be taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan Id
*       vlanMask    - vlan mask filter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveVlanSet
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   vlanMask
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionActiveVlanSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, vlanMask));

    rc = internal_cpssDxChBrgFdbActionActiveVlanSet(devNum, vlanId, vlanMask);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, vlanMask));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionActiveVlanGet
*
* DESCRIPTION:
*       Get action active vlan and vlan mask.
*       All actions are taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       vlanIdPtr      - pointer to Vlan Id
*       vlanMaskPtr    - pointer to vlan mask filter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionActiveVlanGet
(
    IN  GT_U8    devNum,
    OUT GT_U16   *vlanIdPtr,
    OUT GT_U16   *vlanMaskPtr
)
{
    GT_U32      regAddr;        /* register address             */
    GT_U32      hwData;         /* data to read from register   */
    GT_STATUS   rc;             /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(vlanIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(vlanMaskPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction0;

        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 28, &hwData);
        if (rc != GT_OK)
        {
            return rc;
        }

        *vlanIdPtr = (GT_U16)(hwData & 0x1FFF);
        *vlanMaskPtr = (GT_U16)((hwData >> 16) & 0x1FFF);

        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction1;

    /* get Active Vlan and Vlan Mask from FDB Action1 Register */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 8, 24, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *vlanIdPtr = (GT_U16)(hwData & 0xFFF);
    *vlanMaskPtr = (GT_U16)((hwData >> 12) & 0xFFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveVlanGet
*
* DESCRIPTION:
*       Get action active vlan and vlan mask.
*       All actions are taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       vlanIdPtr      - pointer to Vlan Id
*       vlanMaskPtr    - pointer to vlan mask filter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveVlanGet
(
    IN  GT_U8    devNum,
    OUT GT_U16   *vlanIdPtr,
    OUT GT_U16   *vlanMaskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionActiveVlanGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanIdPtr, vlanMaskPtr));

    rc = internal_cpssDxChBrgFdbActionActiveVlanGet(devNum, vlanIdPtr, vlanMaskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanIdPtr, vlanMaskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionActiveDevSet
*
* DESCRIPTION:
*       Set the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       actDev      - Action active device (APPLICABLE RANGES: 0..31)
*       actDevMask  - Action active mask (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*                                  in 'Dual deviceId system' , if the Pattern/mask
*                                  of the LSBit of the actDev,actDevMask are 'exact match = 1'
*                                  because all devices in this system with 'even numbers'
*       GT_OUT_OF_RANGE          - actDev > 31 or actDevMask > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*       For 'Dual deviceId system':
*           the LSBit on the actDev,acdDevMask are ignored and actually taken from
*           MSBit of actTrunkPort and actTrunkPortMask from cpssDxChBrgFdbActionActiveInterfaceSet
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionActiveDevSet
(
    IN GT_U8    devNum,
    IN GT_U32   actDev,
    IN GT_U32   actDevMask
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    regAddr;           /* register address */
    GT_U32    offset;            /* field offset for eArch devices */
    GT_U32    data;              /* reg sub field data */
    GT_U32      numOfBits;       /* number of bits to use */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if(actDevMask   > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum) ||
       actDev       > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;
    }

    if(sysGenGlobalInfo.supportDualDeviceId == GT_TRUE)
    {
        if((1 == U32_GET_FIELD_MAC(actDev,0,1)) &&
           (1 == U32_GET_FIELD_MAC(actDevMask,0,1)))
        {
            /* the Pattern/mask of the LSB of the actDev,actDevMask can't be 'exact match = 1'
                because all devices in this system with 'even numbers'.
            */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }


        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
            {
                offset = 16;
            }
            else
            {
                offset = 14;
            }

            /* skip LSBit - because this bit is managed only from cpssDxChBrgFdbActionActiveInterfaceSet(...) */
            offset += 1;
            numOfBits = 10 -1;

            rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, numOfBits, (actDev & 0x3FF)>>1);
            if(rc != GT_OK)
                return rc;

            regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction2;

            /* skip LSBit - because this bit is managed only from cpssDxChBrgFdbActionActiveInterfaceSet(...) */
            rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, numOfBits, (actDevMask & 0x3FF)>>1);
            if(rc != GT_OK)
                return rc;

        }
        else
        {
            /* read the register */
            rc = prvCpssHwPpReadRegister(devNum, regAddr, &data);
            if(rc != GT_OK)
                return rc;

            /* skip LSBit - because this bit is managed only from cpssDxChBrgFdbActionActiveInterfaceSet(...) */
            U32_SET_FIELD_MASKED_MAC(data,19,4, (actDev >> 1));
            U32_SET_FIELD_MASKED_MAC(data,24,4, (actDevMask >> 1));

            /* write the register */
            rc = prvCpssHwPpWriteRegister(devNum, regAddr, data);
            if(rc != GT_OK)
                return rc;
        }
    }
    else
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
            {
                offset = 16;
            }
            else
            {
                offset = 14;
            }

            rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 10, (actDev & 0x3FF));
            if(rc != GT_OK)
                return rc;

            regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction2;

            rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 10, (actDevMask & 0x3FF));
            if(rc != GT_OK)
                return rc;

        }
        else
        {
            data = ((actDevMask & 0x1F) << 5) | (actDev & 0x1F);

            /* Set the Active device number and active device number mask */
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 18, 10, data);
            if(rc != GT_OK)
                return rc;
        }
    }

    /* save the configured info (when no error).
       currently this DB needed for 'Dual deviceId system' */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNum = actDev;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNumMask = actDevMask;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveDevSet
*
* DESCRIPTION:
*       Set the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       actDev      - Action active device (APPLICABLE RANGES: 0..31)
*       actDevMask  - Action active mask (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*                                  in 'Dual deviceId system' , if the Pattern/mask
*                                  of the LSBit of the actDev,actDevMask are 'exact match = 1'
*                                  because all devices in this system with 'even numbers'
*       GT_OUT_OF_RANGE          - actDev > 31 or actDevMask > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*       For 'Dual deviceId system':
*           the LSBit on the actDev,acdDevMask are ignored and actually taken from
*           MSBit of actTrunkPort and actTrunkPortMask from cpssDxChBrgFdbActionActiveInterfaceSet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveDevSet
(
    IN GT_U8    devNum,
    IN GT_U32   actDev,
    IN GT_U32   actDevMask
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionActiveDevSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, actDev, actDevMask));

    rc = internal_cpssDxChBrgFdbActionActiveDevSet(devNum, actDev, actDevMask);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, actDev, actDevMask));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionActiveDevGet
*
* DESCRIPTION:
*       Get the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       actDevPtr      - pointer to action active device (APPLICABLE RANGES: 0..31)
*       actDevMaskPtr  - pointer to action active mask (APPLICABLE RANGES: 0..31)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - actDev > 31 or actDevMask > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionActiveDevGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *actDevPtr,
    OUT GT_U32   *actDevMaskPtr
)
{
    GT_U32    regAddr;          /* register address     */
    GT_U32    data;             /* reg sub field data   */
    GT_U32    offset;           /* field offset for eArch devices */
    GT_STATUS rc;               /* return code          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actDevPtr);
    CPSS_NULL_PTR_CHECK_MAC(actDevMaskPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction1;
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            offset = 16;
        }
        else
        {
            offset = 14;
        }

        rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 10, &data);
        if(rc != GT_OK)
            return rc;

        *actDevPtr = data;

        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction2;

        rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 10, &data);
        if(rc != GT_OK)
            return rc;

        *actDevMaskPtr = data;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

        /* get Active Dev and Dev Mask from FDB Action1 Register */
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 18, 10, &data);
        if (rc != GT_OK)
        {
            return rc;
        }

        *actDevPtr = (data & 0x1F);
        *actDevMaskPtr = ((data >> 5) & 0x1F);
    }

    if(sysGenGlobalInfo.supportDualDeviceId == GT_TRUE)
    {
        /* retrieve bit 0 info from the DB because we can't retrieve bit0 from the HW */
        U32_SET_FIELD_MASKED_MAC((*actDevPtr),0,1,PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNum);
        U32_SET_FIELD_MASKED_MAC((*actDevMaskPtr),0,1,PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNumMask);
    }

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveDevGet
*
* DESCRIPTION:
*       Get the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       actDevPtr      - pointer to action active device (APPLICABLE RANGES: 0..31)
*       actDevMaskPtr  - pointer to action active mask (APPLICABLE RANGES: 0..31)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - actDev > 31 or actDevMask > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveDevGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *actDevPtr,
    OUT GT_U32   *actDevMaskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionActiveDevGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, actDevPtr, actDevMaskPtr));

    rc = internal_cpssDxChBrgFdbActionActiveDevGet(devNum, actDevPtr, actDevMaskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, actDevPtr, actDevMaskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionActiveInterfaceSet
*
* DESCRIPTION:
*       Set the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of : Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*       A FDB entry will be treated by the FDB action only if the following
*       cases are true:
*       1. The interface is trunk and "active trunk mask" is set and
*       "associated trunkId" masked by the "active trunk/port mask" equal to
*       "Active trunkId".
*       2. The interface is port and "associated portNumber" masked by the
*       "active trunk/port mask" equal to "Active portNumber".
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       actIsTrunk       - determines if the interface is port or trunk
*       actIsTrunkMask   - action active trunk mask.
*       actTrunkPort     - action active interface (port/trunk)
*       actTrunkPortMask - action active port/trunk mask.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To enable Trunk-ID filter set actIsTrunk and actIsTrunkMask to 0x1,
*       set actTrunkPort to trunkID and actTrunkPortMask to 0x7F. To disable
*       Trunk-ID filter set all those parameters to 0x0.
*       To enable Port/device filter set actIsTrunk to 0x0, actTrunkPort to
*       portNum, actTrunkPortMask to 0x7F, actDev to device number and
*       actDevMask to 0x1F (all ones) by cpssDxChBrgFdbActionActiveDevSet.
*       To disable port/device filter set all those parameters to 0x0.
*
*       There is no check done of current status of action's register
*
************************************************
*       For 'Dual deviceId system':
*           the MSBit of the actTrunkPort and actTrunkPortMask are actually used as
*           LSBit of actDev and actDevMask of cpssDxChBrgFdbActionActiveDevSet
*           - configuration limitation :
*               not support next configuration :
*               ((actIsTrunkMask == 0 ) && //care don't if trunk/port
*                (actTrunkPortMask[MSBit] == 1)) // exact match of MSBit of trunk/port
*               reason for the limitation : trunk can come from 2 hemispheres
*
*           more detailed behavior:
*            A. If <isTrunk> is exact match = ‘port’
*                Then Cpss do :
*                < hwDevNum >[0] .Pattern = <port/trunk>[6].Pattern
*                < hwDevNum >[0] .Mask = <port/trunk>[6].Mask
*                <port/trunk>[6]. Pattern = 0
*                <port/trunk>[6]. Mask = no change
*            B. Else If <isTrunk> is exact match = ‘trunk’
*                Then Cpss do :
*                < hwDevNum >[0] .Mask = ‘don’t care’
*           C. Else // <isTrunk> is ‘don’t care’
*               1) if <port/trunk>[6] is ‘don’t care’
*                   Then Cpss do :
*                   < hwDevNum >[0] .Mask = ‘don’t care’
*               2) else //exact match – 0 or 1
*                   return bad state //because trunk can come from 2 hemispheres
************************************************
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionActiveInterfaceSet
(
    IN GT_U8   devNum,
    IN GT_U32  actIsTrunk,
    IN GT_U32  actIsTrunkMask,
    IN GT_U32  actTrunkPort,
    IN GT_U32  actTrunkPortMask
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */
    GT_U32    maxValue;             /* max value supported for the action */
    GT_U32    actDevLsb;            /* actDev LSBit */
    GT_U32    actDevMaskLsb;        /* actDev mask LSBit */
    GT_U32    portTrunkSize;        /* size of PortTrunk fields in registers */
    GT_U32    new_actTrunkPort;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* check ranges according to maximal range */
    maxValue = PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum) > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum) ?
               PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum) : PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum) ;

    if ((actTrunkPort > maxValue) || (actTrunkPortMask > maxValue))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction1;
    }
    else
    {
        /* Get address of FDB Action2 register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;
    }

    if(sysGenGlobalInfo.supportDualDeviceId == GT_TRUE)
    {
        new_actTrunkPort      = actTrunkPort;

            /*
        *       A. If <isTrunk> is exact match = ‘port’
        *           Then Cpss do :
        *           < hwDevNum >[0] .Pattern = <port/trunk>[6].Pattern
        *           < hwDevNum >[0] .Mask = <port/trunk>[6].Mask
        *           <port/trunk>[6]. Pattern = 0
        *           <port/trunk>[6]. Mask = no change
            */
        if(actIsTrunkMask == 1 && actIsTrunk == 0)
        {
            actDevLsb     = U32_GET_FIELD_MAC(new_actTrunkPort,6,1);
            actDevMaskLsb = U32_GET_FIELD_MAC(actTrunkPortMask,6,1);
            U32_SET_FIELD_MASKED_MAC(new_actTrunkPort,6,1,0);
        }
            /*
        *       B. Else If <isTrunk> is exact match = ‘trunk’
        *           Then Cpss do :
        *           < hwDevNum >[0] .Mask = ‘don’t care’
            */
        else if(actIsTrunkMask == 1 && actIsTrunk == 1)
        {
            actDevLsb     = 0;
            actDevMaskLsb = 0;
        }
            /*
        *      C. Else // <isTrunk> is ‘don’t care’
        *          1) if <port/trunk>[6] is ‘don’t care’
        *              Then Cpss do :
        *              < hwDevNum >[0] .Mask = ‘don’t care’
        *          2) else //exact match – 0 or 1
        *              return bad state //because trunk can come from 2 hemispheres
            */
        else
        {
            if(U32_GET_FIELD_MAC(actTrunkPortMask,6,1) == 1)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            actDevLsb     = 0;
            actDevMaskLsb = 0;
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
            {
                portTrunkSize = 15;
            }
            else
            {
                portTrunkSize = 13;
            }

            data = ((new_actTrunkPort & 0x1FFF) | ((actIsTrunk & 0x1) << portTrunkSize));
            data |= actDevLsb << (portTrunkSize + 1);/* the LSB of actDev */

            rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, (portTrunkSize + 1 + 1), data);
            if(rc != GT_OK)
                return rc;

            regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction2;

            data = ((actTrunkPortMask & 0x1FFF) | ((actIsTrunkMask & 0x1) << portTrunkSize));
            data |= actDevMaskLsb << (portTrunkSize + 1); /* the LSB of actDev */

            rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, (portTrunkSize + 1 + 1), data);
            if(rc != GT_OK)
                return rc;
        }
        else
        {
            /* read the register */
            rc = prvCpssHwPpReadRegister(devNum, regAddr, &data);
            if(rc != GT_OK)
                return rc;

            U32_SET_FIELD_MASKED_MAC(data,0,7,  new_actTrunkPort);
            U32_SET_FIELD_MASKED_MAC(data,7,7,  actTrunkPortMask);
            U32_SET_FIELD_MASKED_MAC(data,14,1, actIsTrunk);
            U32_SET_FIELD_MASKED_MAC(data,15,1, actIsTrunkMask);
            U32_SET_FIELD_MASKED_MAC(data,18,1, actDevLsb);
            U32_SET_FIELD_MASKED_MAC(data,23,1, actDevMaskLsb);

            /* write the register */
            rc = prvCpssHwPpWriteRegister(devNum, regAddr, data);
            if(rc != GT_OK)
                return rc;
        }
    }
    else
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
            {
                portTrunkSize = 15;
            }
            else
            {
                portTrunkSize = 13;
            }

            data = ((actTrunkPort & 0x1FFF) | ((actIsTrunk & 0x1) << portTrunkSize));

            rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, (portTrunkSize + 1), data);
            if(rc != GT_OK)
                return rc;

            regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction2;

            data = ((actTrunkPortMask & 0x1FFF) | ((actIsTrunkMask & 0x1) << portTrunkSize));

            rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, (portTrunkSize + 1), data);
            if(rc != GT_OK)
                return rc;
        }
        else
        {
            /* set data */
            data = ((actTrunkPort & 0x7F) | ((actTrunkPortMask & 0x7F) << 7) |
                    ((actIsTrunk & 0x1) << 14) | ((actIsTrunkMask & 0x1) << 15));

            rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 16, data);
            if(rc != GT_OK)
                return rc;
        }
    }

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveInterfaceSet
*
* DESCRIPTION:
*       Set the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of : Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*       A FDB entry will be treated by the FDB action only if the following
*       cases are true:
*       1. The interface is trunk and "active trunk mask" is set and
*       "associated trunkId" masked by the "active trunk/port mask" equal to
*       "Active trunkId".
*       2. The interface is port and "associated portNumber" masked by the
*       "active trunk/port mask" equal to "Active portNumber".
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       actIsTrunk       - determines if the interface is port or trunk
*       actIsTrunkMask   - action active trunk mask.
*       actTrunkPort     - action active interface (port/trunk)
*       actTrunkPortMask - action active port/trunk mask.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To enable Trunk-ID filter set actIsTrunk and actIsTrunkMask to 0x1,
*       set actTrunkPort to trunkID and actTrunkPortMask to 0x7F. To disable
*       Trunk-ID filter set all those parameters to 0x0.
*       To enable Port/device filter set actIsTrunk to 0x0, actTrunkPort to
*       portNum, actTrunkPortMask to 0x7F, actDev to device number and
*       actDevMask to 0x1F (all ones) by cpssDxChBrgFdbActionActiveDevSet.
*       To disable port/device filter set all those parameters to 0x0.
*
*       There is no check done of current status of action's register
*
************************************************
*       For 'Dual deviceId system':
*           the MSBit of the actTrunkPort and actTrunkPortMask are actually used as
*           LSBit of actDev and actDevMask of cpssDxChBrgFdbActionActiveDevSet
*           - configuration limitation :
*               not support next configuration :
*               ((actIsTrunkMask == 0 ) && //care don't if trunk/port
*                (actTrunkPortMask[MSBit] == 1)) // exact match of MSBit of trunk/port
*               reason for the limitation : trunk can come from 2 hemispheres
*
*           more detailed behavior:
*            A. If <isTrunk> is exact match = ‘port’
*                Then Cpss do :
*                < hwDevNum >[0] .Pattern = <port/trunk>[6].Pattern
*                < hwDevNum >[0] .Mask = <port/trunk>[6].Mask
*                <port/trunk>[6]. Pattern = 0
*                <port/trunk>[6]. Mask = no change
*            B. Else If <isTrunk> is exact match = ‘trunk’
*                Then Cpss do :
*                < hwDevNum >[0] .Mask = ‘don’t care’
*           C. Else // <isTrunk> is ‘don’t care’
*               1) if <port/trunk>[6] is ‘don’t care’
*                   Then Cpss do :
*                   < hwDevNum >[0] .Mask = ‘don’t care’
*               2) else //exact match – 0 or 1
*                   return bad state //because trunk can come from 2 hemispheres
************************************************
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveInterfaceSet
(
    IN GT_U8   devNum,
    IN GT_U32  actIsTrunk,
    IN GT_U32  actIsTrunkMask,
    IN GT_U32  actTrunkPort,
    IN GT_U32  actTrunkPortMask
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionActiveInterfaceSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, actIsTrunk, actIsTrunkMask, actTrunkPort, actTrunkPortMask));

    rc = internal_cpssDxChBrgFdbActionActiveInterfaceSet(devNum, actIsTrunk, actIsTrunkMask, actTrunkPort, actTrunkPortMask);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, actIsTrunk, actIsTrunkMask, actTrunkPort, actTrunkPortMask));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionActiveInterfaceGet
*
* DESCRIPTION:
*       Get the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of: Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       actIsTrunkPtr       - determines if the interface is port or trunk
*       actIsTrunkMaskPtr   - action active trunk mask.
*       actTrunkPortPtr     - action active interface (port/trunk)
*       actTrunkPortMaskPtr - action active port/trunk mask.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionActiveInterfaceGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *actIsTrunkPtr,
    OUT GT_U32  *actIsTrunkMaskPtr,
    OUT GT_U32  *actTrunkPortPtr,
    OUT GT_U32  *actTrunkPortMaskPtr
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */
    GT_U32    portTrunkSize;        /* size of PortTrunk fields in registers */
    GT_STATUS rc;                   /* return code                        */
    GT_U32    lsbActDev , lsbActDevMask ; /* lsb of actDev and of actDevMask */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actIsTrunkPtr);
    CPSS_NULL_PTR_CHECK_MAC(actIsTrunkMaskPtr);
    CPSS_NULL_PTR_CHECK_MAC(actTrunkPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(actTrunkPortMaskPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction1;
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            portTrunkSize = 15;
        }
        else
        {
            portTrunkSize = 13;
        }

        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, (portTrunkSize + 1 + 1), &data);
        if(rc != GT_OK)
            return rc;

        *actTrunkPortPtr = (data & 0x1FFF);
        *actIsTrunkPtr   = ((data >> portTrunkSize) & 0x1);
        lsbActDev        = ((data >> (portTrunkSize + 1)) & 0x1);

        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction2;

        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, (portTrunkSize + 1 + 1), &data);
        if(rc != GT_OK)
            return rc;

        *actTrunkPortMaskPtr = (data & 0x1FFF);
        *actIsTrunkMaskPtr   = ((data >> portTrunkSize) & 0x1);
        lsbActDevMask        = ((data >> (portTrunkSize + 1)) & 0x1);
    }
    else
    {
        /* Get address of FDB Action2 register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

        rc = prvCpssHwPpReadRegister(devNum, regAddr,&data);
        if (rc != GT_OK)
        {
            return rc;
        }

        *actTrunkPortPtr = (data & 0x7F);
        *actTrunkPortMaskPtr = ((data >> 7) & 0x7F);
        *actIsTrunkPtr = ((data >> 14) & 0x1);
        *actIsTrunkMaskPtr = ((data >> 15) & 0x1);
        lsbActDev     = ((data >> 18) & 0x1);
        lsbActDevMask = ((data >> 23) & 0x1);
    }

    if(sysGenGlobalInfo.supportDualDeviceId == GT_TRUE)
    {
        if(*actIsTrunkMaskPtr == 1 && *actIsTrunkPtr == 0)
        {
            /*<isTrunk> is exact match = ‘port’*/
            U32_SET_FIELD_MASKED_MAC(*actTrunkPortPtr , 6 , 1 , lsbActDev);
            U32_SET_FIELD_MASKED_MAC(*actTrunkPortMaskPtr , 6 , 1 , lsbActDevMask);
        }
        else if(*actIsTrunkMaskPtr == 1 && *actIsTrunkPtr == 1)
        {
            /*<isTrunk> is exact match = ‘trunk’*/

            /* no manipulation needed */
        }
        else
        {
            /*<isTrunk> is ‘don’t care’*/

            /* no manipulation needed */
        }
    }




    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveInterfaceGet
*
* DESCRIPTION:
*       Get the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of: Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       actIsTrunkPtr       - determines if the interface is port or trunk
*       actIsTrunkMaskPtr   - action active trunk mask.
*       actTrunkPortPtr     - action active interface (port/trunk)
*       actTrunkPortMaskPtr - action active port/trunk mask.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveInterfaceGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *actIsTrunkPtr,
    OUT GT_U32  *actIsTrunkMaskPtr,
    OUT GT_U32  *actTrunkPortPtr,
    OUT GT_U32  *actTrunkPortMaskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionActiveInterfaceGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, actIsTrunkPtr, actIsTrunkMaskPtr, actTrunkPortPtr, actTrunkPortMaskPtr));

    rc = internal_cpssDxChBrgFdbActionActiveInterfaceGet(devNum, actIsTrunkPtr, actIsTrunkMaskPtr, actTrunkPortPtr, actTrunkPortMaskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, actIsTrunkPtr, actIsTrunkMaskPtr, actTrunkPortPtr, actTrunkPortMaskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbIpmcAddrDelEnableSet
*
* DESCRIPTION:
*       Function enable/disable the address aging/deleting IPMC FDB entries.
*       when disabled : the IPMC FDB are not subject to aging/delete operations
*                       by the aging daemon.
*       when enabled : the IPMC FDB are subject to aging/delete operations
*                       by the aging daemon.
*                       aging is still subject to only 'non-static' entries.
*
*       NOTE: no other filter (vlan/dev...) is relevant for those entries.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable/disable the address aging/deleting IPMC FDB entries.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbIpmcAddrDelEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* <IPMCAddrDelMode> */
    rc = prvCpssHwPpSetRegField(devNum,
        PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig1,
        5, 1,
        1 - BOOL2BIT_MAC(enable)); /* NOTE: 'enable' of aging/delete is value 0 in the register */

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbIpmcAddrDelEnableSet
*
* DESCRIPTION:
*       Function enable/disable the address aging/deleting IPMC FDB entries.
*       when disabled : the IPMC FDB are not subject to aging/delete operations
*                       by the aging daemon.
*       when enabled : the IPMC FDB are subject to aging/delete operations
*                       by the aging daemon.
*                       aging is still subject to only 'non-static' entries.
*
*       NOTE: no other filter (vlan/dev...) is relevant for those entries.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable/disable the address aging/deleting IPMC FDB entries.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbIpmcAddrDelEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbIpmcAddrDelEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbIpmcAddrDelEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbIpmcAddrDelEnableGet
*
* DESCRIPTION:
*       Function gets enabled/disabled of the address aging/deleting IPMC FDB entries.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable the address aging/deleting IPMC FDB entries.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbIpmcAddrDelEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    GT_U32  value;/* value read from the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);



    /* <IPMCAddrDelMode> */
    rc = prvCpssHwPpGetRegField(devNum,
        PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig1,
        5, 1, &value);

    *enablePtr = BIT2BOOL_MAC((1 - value));/* NOTE: 'enable' of aging/delete is value 0 in the register */

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbIpmcAddrDelEnableGet
*
* DESCRIPTION:
*       Function gets enabled/disabled of the address aging/deleting IPMC FDB entries.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable the address aging/deleting IPMC FDB entries.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbIpmcAddrDelEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbIpmcAddrDelEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbIpmcAddrDelEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbUploadEnableSet
*
* DESCRIPTION:
*       Enable/Disable reading FDB entries via AU messages to the CPU.
*       The API only configures mode of triggered action.
*       To execute upload use the cpssDxChBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE - enable FDB Upload
*                  GT_FALSE - disable FDB upload
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbUploadEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */
    GT_U32    portGroupId;          /* the port group Id - support multi-port-groups device */
    GT_U32    fieldOffset;          /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    /* the FDB upload occurs on all port groups */
    portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
        fieldOffset = 18;
    }
    else
    {
        /* Get address of FDB Action2 register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;
        fieldOffset = 30;
    }

    data = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* resolve the 'Messages filter' state */
        rc = multiPortGroupCheckAndMessageFilterResolve(devNum,
            GT_FALSE,/* DA refresh NOT given */
            GT_FALSE,/* DA refresh value -- not relevant due to "DA refresh NOT given" */
            GT_TRUE,/* FDB upload given */
            enable,/* FDB upload value */
            GT_FALSE, /* action mode NOT given */
            0);      /* action mode value -- not relevant due to "action mode NOT given" */
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return prvCpssHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, fieldOffset, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbUploadEnableSet
*
* DESCRIPTION:
*       Enable/Disable reading FDB entries via AU messages to the CPU.
*       The API only configures mode of triggered action.
*       To execute upload use the cpssDxChBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE - enable FDB Upload
*                  GT_FALSE - disable FDB upload
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUploadEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbUploadEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbUploadEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbUploadEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of uploading FDB entries via AU messages to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - FDB Upload is enabled
*                     GT_FALSE - FDB upload is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbUploadEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */
    GT_STATUS rc;                   /* return code                        */
    GT_U32    fieldOffset;          /* The number of bits to be written to register */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
        fieldOffset = 18;
    }
    else
    {
        /* Get address of FDB Action2 register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;
        fieldOffset = 30;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &data);

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbUploadEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of uploading FDB entries via AU messages to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - FDB Upload is enabled
*                     GT_FALSE - FDB upload is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUploadEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbUploadEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbUploadEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* fdbTrigAction
*
* DESCRIPTION:
*       trigger FDB action.
*       as part of WA for multi-port groups device , the trigger need to be done
*       only on specific port groups.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       caller must set bit 1 to value 1 (triggering the action)
*
*******************************************************************************/
static GT_STATUS fdbTrigAction
(
    IN GT_U8 devNum,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 fieldData
)
{
    GT_STATUS rc;                   /* return code                        */
    GT_U32    regAddr;              /* register address                   */
    GT_U32    portGroupId;          /* the port group Id - support multi-port-groups device */
    GT_BOOL   multiPortGroupWaNeeded = GT_FALSE;/* indication that WA needed for multi port group device */

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        /* Get address of FDB Action0 register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* loop on all port group */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needWaWaitForReTriggering == GT_TRUE)
            {
                /* at least one port group started the WA , and needs the 're-triggering' */
                multiPortGroupWaNeeded = GT_TRUE;
                break;
            }

            /* check if need to break loop on device that supports single FDB unit instance */
            PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    }

    if(multiPortGroupWaNeeded == GT_FALSE)
    {
        /* non-multi port groups device , or this multi-port groups device not
           doing 're-trigger' of the action as part of 'WA for AUQ stuck'*/


        /* trigger the action on the device (for multi-port group device --> all port groups) */
        return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, fieldData);
    }

    /*************************************************************/
    /* we deal with multi-port groups device that not all port   */
    /* groups need to re-trigger the action                      */
    /*************************************************************/

    /* loop on all port group */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /* check if current port group need to be skipped */
        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needWaWaitForReTriggering == GT_FALSE)
        {
            /* skip this port group -- the action not need re-trigger */

            /* check if need to break loop on device that supports single FDB unit instance */
            PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
            continue;
        }

        rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, fieldOffset, fieldLength, fieldData);
        if(rc != GT_OK)
        {
            return rc;
        }


        debugPrint(("fdbTrigAction : trigger on portGroupId[%d] \n",portGroupId));

        /* clear the flag , it can be set again only from function cpssDxChBrgFdbTriggerAuQueueWa(...) */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needWaWaitForReTriggering = GT_FALSE;

        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* trigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - the previous action not done
*
* COMMENTS:
*       for multi-port groups device :
*           'action finished' only when action finished on all port groups.
*
*******************************************************************************/
static GT_STATUS trigActionStatusGet
(
    IN  GT_U8    devNum
)
{
    GT_STATUS rc;   /* return code                  */
    GT_PORT_GROUPS_BMP  actFinishedPortGroupsBmp; /*bitmap of port groups on which 'trigger action' processing completed*/
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_BOOL startTrigger = GT_FALSE;

    /* replace call to cpssDxChBrgFdbTrigActionStatusGet ... to support WA
       for AUQ full issue ... see related API cpssDxChBrgFdbTriggerAuQueueWa
       check which of the port groups not finished the processing of the action */
    rc = cpssDxChBrgFdbPortGroupTrigActionStatusGet(devNum,
                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                &actFinishedPortGroupsBmp);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check that the previous action has completed */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(actFinishedPortGroupsBmp & (1<<(portGroupId)))
        {
            /* the port group is ready -- clear the flag of <notReady> */
            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].notReady = GT_FALSE;

            /* there is at least one port group that need the triggering */
            startTrigger = GT_TRUE;

            /* check if need to break loop on device that supports single FDB unit instance */
            PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);

            continue;
        }

        /* there maybe some port groups that the previous action not done, But
           we need to ignore the port groups that are skipped as part of the WA
           that may have started ---> so check <notReady> */
        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].notReady == GT_TRUE)
        {
            /* the WA started for the device but this port group was not ready so it was skipped */
            debugPrint(("trigActionStatusGet : 'not ready' portGroupId[%d] \n",portGroupId));
        }
        else
        {
            /*this port group just not finished the previous action*/
            debugPrint((" GT_BAD_STATE : file[%s] line[%d] \n",__FILE__,__LINE__));
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    if(startTrigger == GT_FALSE)
    {
        debugPrint((" GT_BAD_STATE : file[%s] line[%d] \n",__FILE__,__LINE__));
        /* there are NO port groups that need the triggering */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChFullQueueRewind
*
* DESCRIPTION:
*   Rewind Full FU or AU queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupId     - The port group id. relevant only to 'multi-port-groups'
*                         devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       isFuQueue       - is FDB/CNC Upload Queue ?
*                         GT_TRUE  - FDB/CNC Upload Queue
*                         GT_FALSE - FDB AU Messages Queue
*       appDescCtrlPtr  - (pointer to Descriptor Control Structure passed by application)
*                         if NULL specified used primary queues descriptors
*                         This parameter is optional and allows to application
*                         to specify the secondary queues.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChFullQueueRewind
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  GT_BOOL                     isFuQueue,
    IN  PRV_CPSS_AU_DESC_CTRL_STC   *appDescCtrlPtr
)
{
    GT_STATUS                   rc;                     /* return code                       */
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlPtr;           /* pointer to the descriptors DB     */
    GT_UINTPTR                  phyAddr;                /* phy address   of the device       */
    GT_UINTPTR                  cpyAddr;                /* CPU address                       */
    GT_U32                      memoBaseRegAddr;        /* memory base register address      */


    if (appDescCtrlPtr == NULL)
    {
        if (isFuQueue == GT_FALSE)
        {
            /*FDB AU Messages Queue*/
            descCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]);
        }
        else
        {
            /*FDB/CNC Upload Queue*/
            descCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
        }
    }
    else
    {
        /* Descriptor Control Passed by caller */
        descCtrlPtr = appDescCtrlPtr;
    }

    cpyAddr = descCtrlPtr->blockAddr;
    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
    {
        if (isFuQueue == GT_FALSE)
        {
            /*FDB NA Messages Queue*/
            memoBaseRegAddr  =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;
        }
        else
        {
            /*FDB/CNC Upload Queue*/
            memoBaseRegAddr  =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQBaseAddr;
        }
    }
    else
    {
        if (isFuQueue == GT_FALSE)
        {
            /*FDB NA Messages Queue*/
            memoBaseRegAddr  =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;
        }
        else
        {
            /*FDB/CNC Upload Queue*/
            memoBaseRegAddr  =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQBaseAddr;
        }
    }

    /* FU descriptors queue physical address seen from PP */
    rc = cpssOsVirt2Phy(cpyAddr, /*OUT*/&phyAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if __WORDSIZE == 64           /* phyAddr must fit in 32 bit */
        if (0 != (phyAddr & 0xffffffff00000000L))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    #endif
    /* The write triggers continuation of upload */
    /* to the queue memory.                      */
    rc = prvCpssHwPpPortGroupWriteRegister(
        devNum, portGroupId, memoBaseRegAddr, (GT_U32)phyAddr);

    return rc;
}

/*******************************************************************************
* prvCpssDxChFuqOwnerSet
*
* DESCRIPTION:
*   Set FUQ used for CNC / FDB upload.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       isCnc           - is CNC need to be used ?
*                         GT_TRUE - CNC need to use the FUQ
*                         GT_FALSE - FDB upload need to use the FUQ
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChFuqOwnerSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  GT_BOOL                     isCnc
)
{
    GT_STATUS                   rc;                     /* return code                         */
    GT_U32                      cncFuHwValue;           /* value of field                      */
    GT_U32                      mgGlobalControlRegAddr; /* register address                    */
    GT_U32                      mgGlobalControlRegData; /* register data                       */
    GT_U32                      regNum;                 /* number of registers used for reset  */
    GT_U32                      busyWaitRegNum;         /* number of register for buzy wait    */
    GT_U32                      busyWaitBitNum;         /* number of bit for buzy wait         */
    GT_U32                      regAddr[7];             /* register addresses used for reset   */
    GT_U32                      regData[7];             /* register data used for reset        */
    GT_U32                      i;                      /* loop index                          */
    GT_UINTPTR                  phyAddr;                /* phy address                         */
    GT_U32                      memoBaseRegAddr;        /* memory base register address        */
    GT_U32                      memoSizeRegAddr;        /* memory size register address        */
    GT_U32                      memoResetRegAddr;       /* memory size register address        */
    GT_U32                      memoSizeHwVal;          /* memory size HW value                */
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlPtr;           /* pointer to the descriptors DB
                                                         of the device                         */

    if (portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
            devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, portGroupId);
        if (portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
        {
            /* for not multi port group devices */
            portGroupId = 0;
        }
    }

    cncFuHwValue = (isCnc == GT_TRUE) ? 0 : 1;

    if (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* logic similar to SIP5 ... but both CNC and FU messages are with the
           same size ... so no modification needed.
           proper init value was set by hwPpDxChXcat3RegsDefault (set bit 17,18 with value 0)
        */
        return GT_OK;
    }
    else
    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
    {
        if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))/*xcat2,lion2*/
        {
            /* Need not set SelCNCOrFU for xCat2 and above.
               The default value is OK and is not changed by CNC. */
            return GT_OK;
        }
        else/*lion and below */
        {
            /* set CNC/FDB to be the Queue owner */
            mgGlobalControlRegAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
            return prvCpssHwPpPortGroupSetRegField(
                devNum, portGroupId, mgGlobalControlRegAddr, 14 /*startBit*/, 1, cncFuHwValue);
        }
    }

    /* Bobcat2 and above */

    /* Messages inside FU buffer are CNC or FU (never both).
         0x0 = size_of_2; size_of_2; message size of 2*64 (CNC).
         0x1 = size_of_3; size_of_3; message size of 3*64 (FU).
    */
    mgGlobalControlRegAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auqConfig_generalControl;

    rc = prvCpssHwPpPortGroupReadRegister(
        devNum, portGroupId, mgGlobalControlRegAddr, &mgGlobalControlRegData);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (((mgGlobalControlRegData >> 18) & 1) == cncFuHwValue)
    {
        /* already configured as required */
        return GT_OK;
    }

    /* FU Queue full reset must be done */

    /* fix mgGlobalControlRegData */
    U32_SET_FIELD_MAC(
        mgGlobalControlRegData, 18/*offset*/, 1/*length*/, cncFuHwValue);

    descCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
    /* reset SW state */
    descCtrlPtr->currDescIdx       = 0;
    descCtrlPtr->unreadCncCounters = 0;

    /* FU descriptors queue physical address seen from PP */
    rc = cpssOsVirt2Phy(descCtrlPtr->blockAddr, /*OUT*/&phyAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if __WORDSIZE == 64           /* phyAddr must fit in 32 bit */
        if (0 != (phyAddr & 0xffffffff00000000L))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    #endif

    memoBaseRegAddr  =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQBaseAddr;
    memoSizeRegAddr  =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
    memoResetRegAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auqConfig_hostConfig;
    /* size specified in 8-words units as needed for FDB (aligned 192-bit FU message)    */
    /* CNC message is 64 bits, the size for the same memory must be specified twice more */
    memoSizeHwVal    = ((1 << 31) | (descCtrlPtr->blockSize * ((isCnc == GT_FALSE) ? 1 : 2)));
    regNum = 0;
    /* set CNC/FDB message size */
    regAddr[regNum]      = mgGlobalControlRegAddr;
    regData[regNum]      = mgGlobalControlRegData;
    regNum ++;
    /* disable all queues (AU and FU) */
    regAddr[regNum]      = memoResetRegAddr;
    regData[regNum]      = 1;
    regNum ++;
    /* reset queue (being disable) and wait reset bit self clearing */
    regAddr[regNum]      = memoResetRegAddr;
    regData[regNum]      = 5;
    busyWaitRegNum       = regNum;
    busyWaitBitNum       = 2; /*self clear FU Queue reset bit */
    regNum ++;
    /* set size */
    regAddr[regNum]      = memoSizeRegAddr;
    regData[regNum]      = memoSizeHwVal;
    regNum ++;
    /* set base */
    regAddr[regNum]      = memoBaseRegAddr;
    regData[regNum]      = (GT_U32)phyAddr;
    regNum ++;
    /* enable all queues (AU and FU) */
    regAddr[regNum]      = memoResetRegAddr;
    regData[regNum]      = 0;
    regNum ++;

#ifdef GM_USED
    busyWaitRegNum       = 0xFFFFFFFF; /* GM should never wait */
#endif /*GM_USED*/

    for (i = 0; (i < regNum); i++)
    {
        rc = prvCpssHwPpPortGroupWriteRegister(
            devNum, portGroupId, regAddr[i], regData[i]);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (i == busyWaitRegNum)
        {
            rc = prvCpssPortGroupBusyWait(
                devNum, portGroupId, regAddr[busyWaitRegNum], busyWaitBitNum,
                GT_FALSE /*onlyCheck*/);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChBrgFdbTrigActionStart
*
* DESCRIPTION:
*       Enable actions, sets action type, action mode to CPSS_ACT_TRIG_E and
*       starts triggered action by setting Aging Trigger.
*       This API may be used to start one of triggered actions: Aging, Deleting,
*       Transplanting and FDB Upload.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE    - when previous FDB triggered action is not completed yet
*                         or CNC block upload not finished (or not all of it's
*                         results retrieved from the common used FU and CNC
*                         upload queue)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Deleting and Transplanting actions can be done only by CPU triggered
*       FDB action that activated by the function.
*       See below sequence of CPSS API calls for Deleting and
*       Transplanting actions.
*
*       Before making Delete or Transplant sequence the application need to
*       disable Actions to avoid wrong automatic aging.
*
*       It is Application responsibility to get and store all parameters
*       that are changed during those actions.
*       1. AA message to CPU status by cpssDxChBrgFdbAAandTAToCpuGet.
*       2. Action Active configuration by
*          cpssDxChBrgFdbActionActiveInterfaceGet,
*          cpssDxChBrgFdbActionActiveDevGet,
*          cpssDxChBrgFdbActionActiveVlanGet.
*       3. Action mode and trigger mode by cpssDxChBrgFdbActionModeGet
*          cpssDxChBrgFdbActionTriggerModeGet.
*
*       The AA and TA messages may be disabled before the FDB action
*       4. Disable AA and TA messages to CPU by cpssDxChBrgFdbAAandTAToCpuSet.
*
*       5. Set Active configuration by: cpssDxChBrgFdbActionActiveInterfaceSet,
*       cpssDxChBrgFdbActionActiveVlanSet and cpssDxChBrgFdbActionActiveDevSet.
*
*       6. Start triggered action by cpssDxChBrgFdbTrigActionStart
*
*       7. Wait that triggered action is completed by:
*           -  Busy-wait poling of status - cpssDxChBrgFdbTrigActionStatusGet
*           -  Wait of event CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E.
*              This wait may be done in context of dedicated task to restore
*              Active configuration and AA messages configuration.
*
*   for multi-port groups device :
*           the trigger is done on ALL port groups regardless to operation mode
*           (Trigger aging/transplant/delete/upload).
*           see also description of function cpssDxChBrgFdbActionModeSet about
*           'multi-port groups device'.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbTrigActionStart
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
)
{
    GT_STATUS rc;                   /* return code                        */
    GT_U32    data;                 /* reg sub field data                 */
    GT_BOOL   uploadEnable;         /* FU upload Enable                   */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* check if previous trigger action finished */
    rc = trigActionStatusGet(devNum);
    if (rc != GT_OK)
    {
        /*GT_BAD_STATE             - the previous action not done*/
        return rc;
    }

    if ((PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))/* devices with CNC support */
        && (mode == CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E))
    {
        rc = cpssDxChBrgFdbUploadEnableGet(devNum, &uploadEnable);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(uploadEnable == GT_TRUE)
        {
            /* loop on all port groups to check that all CNC upload already read */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
            {
            /* there are CNC upload messages in the queue */
                if (PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId].unreadCncCounters != 0)
                {
                    debugPrint((" GT_BAD_STATE : file[%s] line[%d] \n",__FILE__,__LINE__));
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                /* check if need to break loop on device that supports single FDB unit instance */
                PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

            /* set indication about FUQ used by FDB and not the CNC */
            rc = prvCpssDxChFuqOwnerSet(devNum , CPSS_PORT_GROUP_UNAWARE_MODE_CNS , GT_FALSE/* FDB , not CNC */);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* let the 'action mode set' to set bits 4,5 and to extra logic the needed */
    rc  = cpssDxChBrgFdbActionModeSet(devNum,mode);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* enable actions, set triggerMode = TRIGGER and set AgingTriger bit */
    data = 0x7;

    return fdbTrigAction(devNum,
                               0,/*start bit 0 */
                               3,/* 3 bits */
                               data);/* value of the 3 bits */
}

/*******************************************************************************
* cpssDxChBrgFdbTrigActionStart
*
* DESCRIPTION:
*       Enable actions, sets action type, action mode to CPSS_ACT_TRIG_E and
*       starts triggered action by setting Aging Trigger.
*       This API may be used to start one of triggered actions: Aging, Deleting,
*       Transplanting and FDB Upload.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE    - when previous FDB triggered action is not completed yet
*                         or CNC block upload not finished (or not all of it's
*                         results retrieved from the common used FU and CNC
*                         upload queue)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Deleting and Transplanting actions can be done only by CPU triggered
*       FDB action that activated by the function.
*       See below sequence of CPSS API calls for Deleting and
*       Transplanting actions.
*
*       Before making Delete or Transplant sequence the application need to
*       disable Actions to avoid wrong automatic aging.
*
*       It is Application responsibility to get and store all parameters
*       that are changed during those actions.
*       1. AA message to CPU status by cpssDxChBrgFdbAAandTAToCpuGet.
*       2. Action Active configuration by
*          cpssDxChBrgFdbActionActiveInterfaceGet,
*          cpssDxChBrgFdbActionActiveDevGet,
*          cpssDxChBrgFdbActionActiveVlanGet.
*       3. Action mode and trigger mode by cpssDxChBrgFdbActionModeGet
*          cpssDxChBrgFdbActionTriggerModeGet.
*
*       The AA and TA messages may be disabled before the FDB action
*       4. Disable AA and TA messages to CPU by cpssDxChBrgFdbAAandTAToCpuSet.
*
*       5. Set Active configuration by: cpssDxChBrgFdbActionActiveInterfaceSet,
*       cpssDxChBrgFdbActionActiveVlanSet and cpssDxChBrgFdbActionActiveDevSet.
*
*       6. Start triggered action by cpssDxChBrgFdbTrigActionStart
*
*       7. Wait that triggered action is completed by:
*           -  Busy-wait poling of status - cpssDxChBrgFdbTrigActionStatusGet
*           -  Wait of event CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E.
*              This wait may be done in context of dedicated task to restore
*              Active configuration and AA messages configuration.
*
*   for multi-port groups device :
*           the trigger is done on ALL port groups regardless to operation mode
*           (Trigger aging/transplant/delete/upload).
*           see also description of function cpssDxChBrgFdbActionModeSet about
*           'multi-port groups device'.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrigActionStart
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbTrigActionStart);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChBrgFdbTrigActionStart(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionModeSet
*
* DESCRIPTION:
*       Sets FDB action mode without setting Action Trigger
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - FDB action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*       set configuration to all portGroups with same value.
*       1. Age with delete (may be Triggered or Auto)
*           a. Unified FDBs mode and Unified-Linked FDB mode :
*              This type of operation is not supported , due to the control
*              learning being used, the risk of AA messages being dropped & the
*              need to avoid dropping MAC entries from portGroups which are not
*              the source of these entries (meaning they would never be refreshed)
*              API returns GT_BAD_STATE if trying to use this action mode.
*           b. Linked FDBs mode :
*              Application should use this type of operation only in Automatic mode
*              (not in Controlled mode) + Static FDB entries .
*              In this mode automatic learnt addresses would be silently removed
*              by PP while application addresses would be removed only by specific
*              application API call.
*
*       2. Age without delete (may be Triggered or Auto)
*           The only type of Aging that can be used with Unified FDBs mode and
*           Unified-Linked FDB mode (in Linked FDBs mode it should be used when
*           using controlled mode), AA would be sent by PP to Application that
*           would remove entry from relevant portGroups.
*
*       3. Transplant (only Triggered)
*           a. Unified-Linked FDB mode and Linked FDB mode :
*               This action mode is not supported - API returns GT_BAD_STATE
*               because triggering this would possibly result with entries which
*               have their new 'source' information properly set - but these entries
*               would not be relocated to the new source portGroups, meaning that
*               packet received from new station location (which was supposed to
*               be transplanted), may still create NA message.
*               Computing new source portGroup is not always possible, especially
*               when station is relocated to trunk or it was previously residing
*               on multiple-portGroups.
*               Application is required to perform Flush operation instead.
*               This would clear previously learnt MAC
*
*       4. Delete (only Triggered)
*           supported in all FDB modes
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
)
{
    GT_STATUS   rc;
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        /* Get address of FDB Action0 register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    /* check mode */
    switch (mode)
    {
        case CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E:
            data = 0;
            break;
        case CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E:
            data = 1;
            break;
        case CPSS_FDB_ACTION_DELETING_E:
            data = 2;
            break;
        case CPSS_FDB_ACTION_TRANSPLANTING_E:
            data = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* check FBD state with limitations on action mode */
        /* resolve the 'Messages filter' state */
        rc = multiPortGroupCheckAndMessageFilterResolve(devNum,
            GT_FALSE,/* DA refresh NOT given */
            GT_FALSE,/* DA refresh value -- not relevant due to "DA refresh NOT given" */
            GT_FALSE,/* FDB upload NOT given */
            GT_FALSE,/* FDB upload value -- not relevant due to "FDB upload NOT given" */
            GT_TRUE, /* action mode given */
            mode);   /* action mode value */
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 3, 2, data);
}

/*******************************************************************************
* cpssDxChBrgFdbActionModeSet
*
* DESCRIPTION:
*       Sets FDB action mode without setting Action Trigger
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - FDB action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*       set configuration to all portGroups with same value.
*       1. Age with delete (may be Triggered or Auto)
*           a. Unified FDBs mode and Unified-Linked FDB mode :
*              This type of operation is not supported , due to the control
*              learning being used, the risk of AA messages being dropped & the
*              need to avoid dropping MAC entries from portGroups which are not
*              the source of these entries (meaning they would never be refreshed)
*              API returns GT_BAD_STATE if trying to use this action mode.
*           b. Linked FDBs mode :
*              Application should use this type of operation only in Automatic mode
*              (not in Controlled mode) + Static FDB entries .
*              In this mode automatic learnt addresses would be silently removed
*              by PP while application addresses would be removed only by specific
*              application API call.
*
*       2. Age without delete (may be Triggered or Auto)
*           The only type of Aging that can be used with Unified FDBs mode and
*           Unified-Linked FDB mode (in Linked FDBs mode it should be used when
*           using controlled mode), AA would be sent by PP to Application that
*           would remove entry from relevant portGroups.
*
*       3. Transplant (only Triggered)
*           a. Unified-Linked FDB mode and Linked FDB mode :
*               This action mode is not supported - API returns GT_BAD_STATE
*               because triggering this would possibly result with entries which
*               have their new 'source' information properly set - but these entries
*               would not be relocated to the new source portGroups, meaning that
*               packet received from new station location (which was supposed to
*               be transplanted), may still create NA message.
*               Computing new source portGroup is not always possible, especially
*               when station is relocated to trunk or it was previously residing
*               on multiple-portGroups.
*               Application is required to perform Flush operation instead.
*               This would clear previously learnt MAC
*
*       4. Delete (only Triggered)
*           supported in all FDB modes
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChBrgFdbActionModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionModeGet
*
* DESCRIPTION:
*       Gets FDB action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr    - FDB action mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_FDB_ACTION_MODE_ENT     *modePtr
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */
    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        /* Get address of FDB Action0 register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 3, 2, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* check mode */
    switch (data)
    {
        case 0:
            *modePtr = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
            break;
        case 1:
            *modePtr = CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;
            break;
        case 2:
            *modePtr = CPSS_FDB_ACTION_DELETING_E;
            break;
        case 3:
            *modePtr = CPSS_FDB_ACTION_TRANSPLANTING_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbActionModeGet
*
* DESCRIPTION:
*       Gets FDB action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr    - FDB action mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_FDB_ACTION_MODE_ENT     *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChBrgFdbActionModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMacTriggerToggle
*
* DESCRIPTION:
*       Toggle Aging Trigger and cause the device to scan its MAC address table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - the previous action not done
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*       the trigger is done on ALL port groups regardless to operation mode
*       (Trigger aging/transplant/delete/upload).
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacTriggerToggle
(
    IN GT_U8  devNum
)
{
    GT_STATUS   rc;/*return code*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* check if previous trigger action finished */
    rc = trigActionStatusGet(devNum);
    if (rc != GT_OK)
    {
        /*GT_BAD_STATE             - the previous action not done*/
        return rc;
    }

    /* set self-clear trigger bit */
    return fdbTrigAction(devNum,
                               1,/*start bit 1 */
                               1,/* 1 bit */
                               1);/* value of the bit */
}

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerToggle
*
* DESCRIPTION:
*       Toggle Aging Trigger and cause the device to scan its MAC address table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - the previous action not done
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*       the trigger is done on ALL port groups regardless to operation mode
*       (Trigger aging/transplant/delete/upload).
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerToggle
(
    IN GT_U8  devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacTriggerToggle);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChBrgFdbMacTriggerToggle(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbTrunkAgingModeSet
*
* DESCRIPTION:
*       Sets bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       trunkAgingMode - FDB aging mode for trunk entries.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or aging mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbTrunkAgingModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT  trunkAgingMode
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

     switch (trunkAgingMode)
    {
        case CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_REGULAR_E:
            value = 0;
            break;
        case CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_FORCE_AGE_WITHOUT_REMOVAL_E:
            value = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 8, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbTrunkAgingModeSet
*
* DESCRIPTION:
*       Sets bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       trunkAgingMode - FDB aging mode for trunk entries.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or aging mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrunkAgingModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT  trunkAgingMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbTrunkAgingModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, trunkAgingMode));

    rc = internal_cpssDxChBrgFdbTrunkAgingModeSet(devNum, trunkAgingMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, trunkAgingMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbTrunkAgingModeGet
*
* DESCRIPTION:
*       Get bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       trunkAgingModePtr - (pointer to) FDB aging mode for trunk entries.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or aging mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbTrunkAgingModeGet
(
    IN GT_U8                        devNum,
    OUT CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT *trunkAgingModePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      value;      /* value to read from register */
    GT_STATUS   rc;         /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(trunkAgingModePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 8, 1, &value);

    *trunkAgingModePtr = (value == 0)
            ? CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_REGULAR_E
            : CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_FORCE_AGE_WITHOUT_REMOVAL_E;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbTrunkAgingModeGet
*
* DESCRIPTION:
*       Get bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       trunkAgingModePtr - (pointer to) FDB aging mode for trunk entries.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or aging mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrunkAgingModeGet
(
    IN GT_U8                        devNum,
    OUT CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT *trunkAgingModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbTrunkAgingModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, trunkAgingModePtr));

    rc = internal_cpssDxChBrgFdbTrunkAgingModeGet(devNum, trunkAgingModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, trunkAgingModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChBrgFdbSizeSet
*
* DESCRIPTION:
*       function to set the FDB size.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number.
*       fdbSize        - the FDB size to set.
*                       (each device with it's own relevant values)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad device or FDB table size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbSizeSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  fdbSize
)
{
    GT_STATUS rc; /* returned status */
    GT_U32    hwValue = 0; /* hardware value */
    GT_U32    fieldLength = 0; /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    /* Set the FDB size configuration */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* FDB size configuration for DxCh3 and Lion */
        switch(fdbSize)
        {
            case _16KB: hwValue = 1; break;
            case _32KB: hwValue = 0; break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(fdbSize));
        }
        fieldLength = 1;
    }
    else if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
             (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
             (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        /* FDB size configuration for XCAT, XCAT2, XCAT3 */
        switch(fdbSize)
        {
            case _4KB: hwValue = 3; break;
            case _8KB: hwValue = 2; break;
            case _16KB: hwValue = 1; break;
            case _32KB: hwValue = 0; break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(fdbSize));
        }
        fieldLength = 2;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        /* FDB size configuration for Lion2 */
        switch(fdbSize)
        {
            case _4KB: hwValue = 0; break;
            case _8KB: hwValue = 1; break;
            case _16KB: hwValue = 2; break;
            case _32KB: hwValue = 3; break;
            case _64KB: hwValue = 4; break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(fdbSize));
        }
        fieldLength = 3;
    }
    else /* PRV_CPSS_SIP_5_CHECK_MAC(devNum) */
    {
        /* FDB size configuration for Bobcat2; Caelum; Bobcat3 */
        switch(fdbSize)
        {
            case _4KB: hwValue = 0; break;
            case _8KB: hwValue = 1; break;
            case _16KB: hwValue = 2; break;
            case _32KB: hwValue = 3; break;
            case _64KB: hwValue = 4; break;
            case _128KB: hwValue = 5; break;
            case _256KB: hwValue = 6; break;
            case _512KB: hwValue = 7; break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_ONE_PARAM_FORMAT_MAC(fdbSize));
        }
        fieldLength = 3;
    }

    if (fieldLength != 0)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* set The size of the FDB Table */
            rc = prvCpssHwPpSetRegField(devNum,
                                           PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig1,
                                           11,
                                           fieldLength,
                                           hwValue);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            /* set The size of the FDB Table */
            rc = prvCpssDrvHwPpSetRegField(devNum,
                                           PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg,
                                           24,
                                          (fieldLength > 2) ? 2 : fieldLength,
                                           hwValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            if (fieldLength > 2)
            {
                rc = prvCpssDrvHwPpSetRegField(devNum,
                                               PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg,
                                               30, 1,
                                               hwValue >> 2);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

    /* update the FDB size in the DB */
    PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb = fdbSize;

    CPSS_LOG_INFORMATION_MAC("updated FDB size to [%d] entries", fdbSize);

    /* Init FDB hash parameters  */
    switch(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb)
    {
        case _8K:
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.size =
                    CPSS_DXCH_BRG_FDB_TBL_SIZE_8K_E;
            break;
        case _16K:
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.size =
                    CPSS_DXCH_BRG_FDB_TBL_SIZE_16K_E;
            break;
        case _32K:
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.size =
                    CPSS_DXCH_BRG_FDB_TBL_SIZE_32K_E;
            break;
        case _64K:
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.size =
                    CPSS_DXCH_BRG_FDB_TBL_SIZE_64K_E;
            break;
        case _128K:
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.size =
                    CPSS_DXCH_BRG_FDB_TBL_SIZE_128K_E;
            break;
        case _256K:
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.size =
                    CPSS_DXCH_BRG_FDB_TBL_SIZE_256K_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_ONE_PARAM_FORMAT_MAC(fdbSize));
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbInit
*
* DESCRIPTION:
*       Init FDB system facility for a device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - on not supported FDB table size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbInit
(
    IN  GT_U8   devNum
)
{
    GT_STATUS   rc;
    GT_U32      actionTimer ;/* action timer that considers the FDB size to get best time values */
    GT_U32      fdbSizeGranularityFactor;/* time granularity factor that considers the FDB size */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        rc = cpssDxChBrgFdb16BitFidHashEnableSet(devNum,GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* before power 2 is:
            256k->1,128k->2, 64->4, 32k->8, 16k->16,  8k->32
           after power 2 is:
            256k->1,128k->4,64->16,32k->64,16k->256,8k->1024
        */
        fdbSizeGranularityFactor = (_256K / PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb);
        fdbSizeGranularityFactor *= fdbSizeGranularityFactor;

        if(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb <= _32K )
        {
            actionTimer = 63;
        }
        else
        {
            /* 1,4,16 for FDBs : 256k,128k,64k */
            actionTimer = fdbSizeGranularityFactor;
        }

        /* calculate 1 sec granularity ... consider the value that was written to the <actionTimer>
            that is the actionTimer */
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbAging_oneSecGranularity =
                (PRV_CPSS_PP_MAC(devNum)->coreClock * 1000000/*MHz*/) /
                 (_512K * actionTimer);
        }
        else
        {
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbAging_oneSecGranularity =
                (PRV_CPSS_PP_MAC(devNum)->coreClock * 1000000/*MHz*/) /
                 ((_256K / fdbSizeGranularityFactor) * actionTimer);
        }

        if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbAging_oneSecGranularity == 0)
        {
            /*wrong calculations*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbAging_maxSec =
            (0xFFFFFFFF/*max value in the register*/ /
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbAging_oneSecGranularity);

        /* write actionTimer to FDB Action0 Register --> to <actionTimer> time units

           the functions : cpssDxChBrgFdbAgingTimeoutSet ,
                           cpssDxChBrgFdbAgingTimeoutGet

           considers this value.
        */
        rc = prvCpssHwPpSetRegField(devNum,
            PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral
            , 5, 6, actionTimer);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* we must set default of 300 seconds that initialization should be with.
           this will synch between <actionTimer> and <aging_end_counter_val> */
        rc = cpssDxChBrgFdbAgingTimeoutSet(devNum,300);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.fid16BitHashEn = GT_FALSE;
    }

    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.crcHashUpperBitsMode =
        CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ALL_ZERO_E;

    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.vlanMode =
                                                        CPSS_SVL_E;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode =
                                            CPSS_MAC_HASH_FUNC_CRC_E;

    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode =
        PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_NON_MULTI_PORT_GROUP_DEVICE_E;

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /****************************/
        /* multi port groups device */
        /****************************/

        /* initialize the mask state --> enabled */
        rc = prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet(devNum,GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode =
            PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.numOfBanks = 16;
    }
    else
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.numOfBanks = 4;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbInit
*
* DESCRIPTION:
*       Init FDB system facility for a device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - on not supported FDB table size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbInit
(
    IN  GT_U8   devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChBrgFdbInit(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbAgeBitDaRefreshEnableSet
*
* DESCRIPTION:
*       Enables/disables destination address-based aging. When this bit is
*       set, the aging process is done both on the source and the destination
*       address (i.e. the age bit will be refresh when MAC DA hit occurs, as
*       well as MAC SA hit).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable refreshing
*                 GT_FALSE - disable refreshing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*       for multi-port groups device:
*           Once enabled, MAC address is eligibly to be aged-out if no packets were
*           received from it or sent to it from any other location during aging period.
*           In order to properly compute this application is required to keep a
*           'score board' for every regular entry (same as needed for Trunk entries),
*           each bit representing age bit from each underlying portGroup which this
*           entry is written to. Entry is aged-out if all relevant portGroups have
*           sent AA, indicating entry was aged-out (once DA refresh is disabled -
*           only the source portGroup indication interests us for aging calculations).
*           Note:
*           that it's highly recommended not to enable this feature, as this
*           feature enables transmissions of many AA messages to the CPU
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAgeBitDaRefreshEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS   rc;     /* return code*/
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* resolve the 'Messages filter' state */
        rc = multiPortGroupCheckAndMessageFilterResolve(devNum,
            GT_TRUE,/* DA refresh given */
            enable,/* DA refresh value */
            GT_FALSE,/* FDB upload NOT given */
            GT_FALSE,/* FDB upload value --  not relevant due to "FDB upload NOT given"*/
            GT_FALSE, /* action mode NOT given */
            0);      /* action mode value -- not relevant due to "action mode NOT given" */
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).
            FDBCore.FDBGlobalConfig.FDBGlobalConfig1;

        value = (enable == GT_FALSE) ?
                0 :/* the 2 bits with value 0 */
                3; /* the 2 bits with value 1 */


        /* set 2 different fields: DARefreshMcEn and DARefreshUCEn  */
        return prvCpssHwPpSetRegField(devNum, regAddr, 6, 2, value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[2];

        value = BOOL2BIT_MAC(enable);

        return prvCpssHwPpSetRegField(devNum, regAddr, 11, 1, value);
    }
}

/*******************************************************************************
* cpssDxChBrgFdbAgeBitDaRefreshEnableSet
*
* DESCRIPTION:
*       Enables/disables destination address-based aging. When this bit is
*       set, the aging process is done both on the source and the destination
*       address (i.e. the age bit will be refresh when MAC DA hit occurs, as
*       well as MAC SA hit).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable refreshing
*                 GT_FALSE - disable refreshing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*       for multi-port groups device:
*           Once enabled, MAC address is eligibly to be aged-out if no packets were
*           received from it or sent to it from any other location during aging period.
*           In order to properly compute this application is required to keep a
*           'score board' for every regular entry (same as needed for Trunk entries),
*           each bit representing age bit from each underlying portGroup which this
*           entry is written to. Entry is aged-out if all relevant portGroups have
*           sent AA, indicating entry was aged-out (once DA refresh is disabled -
*           only the source portGroup indication interests us for aging calculations).
*           Note:
*           that it's highly recommended not to enable this feature, as this
*           feature enables transmissions of many AA messages to the CPU
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeBitDaRefreshEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAgeBitDaRefreshEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbAgeBitDaRefreshEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbAgeBitDaRefreshEnableGet
*
* DESCRIPTION:
*       Gets status (enabled/disabled) of destination address-based aging bit.
*       When this bit is set, the aging process is done both on the source and
*       the destination address (i.e. the age bit will be refresh when MAC DA
*       hit occurs, as well as MAC SA hit).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) status DA refreshing of aged bit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAgeBitDaRefreshEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to read from register */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).
            FDBCore.FDBGlobalConfig.FDBGlobalConfig1;

        /* get 2 different fields: DARefreshMcEn and DARefreshUCEn  */
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 6, 2, &value);
        if (rc != GT_OK)
        {
            return rc;
        }

        switch(value)
        {
            case 0:
                /* the 2 bits are 'disabled' */
                *enablePtr = GT_FALSE;
                break;
            case 3:
                /* the 2 bits are 'enabled' */
                *enablePtr = GT_TRUE;
                break;
            default:/* 1 or 2 */
                /* the 2 bits are not with the same value */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[2];

        rc = prvCpssHwPpGetRegField(devNum, regAddr, 11, 1, &value);
        if (rc != GT_OK)
        {
            return rc;
        }

        *enablePtr = BIT2BOOL_MAC(value);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbAgeBitDaRefreshEnableGet
*
* DESCRIPTION:
*       Gets status (enabled/disabled) of destination address-based aging bit.
*       When this bit is set, the aging process is done both on the source and
*       the destination address (i.e. the age bit will be refresh when MAC DA
*       hit occurs, as well as MAC SA hit).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) status DA refreshing of aged bit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeBitDaRefreshEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAgeBitDaRefreshEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbAgeBitDaRefreshEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbRoutedLearningEnableSet
*
* DESCRIPTION:
*       Enable or disable learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet’s MAC SA to the router’s MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers’ Source Addresses.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbRoutedLearningEnableSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write to register */
    GT_U32 offset;      /* offset in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
            bridgeEngineConfig.bridgeGlobalConfig0;
        offset = 1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bridgeRegs.bridgeGlobalConfigRegArray[0];
        offset = 3;
    }

    /* HW: 0 - enabled, 1 - disabled */
    value = (enable == GT_FALSE) ? 1 : 0 ;


    return prvCpssHwPpSetRegField(devNum, regAddr, offset, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbRoutedLearningEnableSet
*
* DESCRIPTION:
*       Enable or disable learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet’s MAC SA to the router’s MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers’ Source Addresses.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutedLearningEnableSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbRoutedLearningEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbRoutedLearningEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbRoutedLearningEnableGet
*
* DESCRIPTION:
*       Get Enable or Disable state of learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet’s MAC SA to the router’s MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers’ Source Addresses.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbRoutedLearningEnableGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to read from register */
    GT_STATUS rc;       /* returned status */
    GT_U32 offset;      /* offset in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
            bridgeEngineConfig.bridgeGlobalConfig0;
        offset = 1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bridgeRegs.bridgeGlobalConfigRegArray[0];
        offset = 3;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* HW: 0 - enabled, 1 - disabled */
    *enablePtr = (value == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutedLearningEnableGet
*
* DESCRIPTION:
*       Get Enable or Disable state of learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet’s MAC SA to the router’s MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers’ Source Addresses.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutedLearningEnableGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbRoutedLearningEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbRoutedLearningEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* fdbTriggerAuQueueWaCheckAndClear
*
* DESCRIPTION:
*       check if for current port group the WA needed , and clear the trigger
*       action bit for the needing port group :
*       Trigger the Address Update (AU) queue workaround for case when learning
*       from CPU is stopped due to full Address Update (AU) fifo queue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*       portGroupId - the portGroupId. to support multi-port-groups device
*
* OUTPUTS:
*       waNeededPtr - (pointer to) does the WA needed for this port group
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or  portGroupId
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS fdbTriggerAuQueueWaCheckAndClear
(
    IN   GT_U8 devNum,
    IN   GT_U32  portGroupId,
    OUT  GT_BOOL *waNeededPtr
)
{
    GT_U32                      regAddr;       /* register address */
    GT_STATUS                   rc;
    GT_U32                      regValue;

    *waNeededPtr = GT_FALSE;

    PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activated = GT_FALSE;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        /* get the address of FDB Scanning0 register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    /* check if this port group requires the WA */
    rc = prvCpssHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 1, 1,&regValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(regValue == 0)
    {
        /* the action is not 'stuck' on this port group */
        return GT_OK;
    }

    /* clear the trigger bit --> allow the Application to trigger that action
       again */
    rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, 1, 1, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    *waNeededPtr = GT_TRUE;

    /* NOTE: the flag of needWaWaitForReTriggering is cleared only from fdbTrigAction */
    PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needWaWaitForReTriggering = GT_TRUE;

    PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activated = GT_TRUE;

    return GT_OK;
}


/*******************************************************************************
* fdbTriggerAuQueueWaCheck
*
* DESCRIPTION:
*       check if the device can start the WA .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       waNeededPortGroupsBmpPtr - (pointer to) bmp of port groups that need the WA.
*
*       waNotReadyPortGroupsBmpPtr - (pointer to) bmp of port groups that action
*                                   not finished but the AUQ not full so those port
*                                   groups NOT ready for the WA !
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS fdbTriggerAuQueueWaCheck
(
    IN   GT_U8 devNum,
    OUT  GT_PORT_GROUPS_BMP  *waNeededPortGroupsBmpPtr,
    OUT  GT_PORT_GROUPS_BMP  *waNotReadyPortGroupsBmpPtr

)
{
    GT_STATUS           rc;
    GT_PORT_GROUPS_BMP  actFinishedPortGroupsBmp; /*bitmap of port groups on which 'trigger action' processing completed*/
    GT_PORT_GROUPS_BMP  isFullPortGroupsBmp; /*bitmap of port groups on which 'the queue is full'*/
    GT_PORT_GROUPS_BMP  actNotFinishedPortGroupsBmp; /*bitmap of port groups on which 'trigger action' processing NOT completed*/

    *waNotReadyPortGroupsBmpPtr = 0;
    /* when the AUQ is not full and pp not finished process the action , it may
       still be due to 'task priority' issues inside the PP (heavy traffic processing) */

    /* check which of the port groups not finished the processing of the action */
    rc = cpssDxChBrgFdbPortGroupTrigActionStatusGet(devNum,
                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                &actFinishedPortGroupsBmp);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* get the port groups that not finished the trigger action */
    actNotFinishedPortGroupsBmp = (~actFinishedPortGroupsBmp) &
        PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp;

    *waNeededPortGroupsBmpPtr = actNotFinishedPortGroupsBmp;

    if(prvCpssAuqDeadlockOnQueueNotFullWaEnable == GT_TRUE)
    {
        /* for debug purposes only ! */
        return GT_OK;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_CHEETAH3_E)
    {
        /* this device (ch1/ch2) not holding 'auq full indication' ,
           so we not check it. */
        isFullPortGroupsBmp = 0xFFFFFFFF;
    }
    else
    {
        /* check that the AUQ is FULL (part of CQ#124401) */
        /* check which of the port groups has 'queue full' */
        rc = cpssDxChBrgFdbPortGroupQueueFullGet(devNum,
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                    CPSS_DXCH_FDB_QUEUE_TYPE_AU_E,
                    &isFullPortGroupsBmp);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* WA needed only on port groups which reached 'AUQ full' (and not finished the trigger action) */
    *waNeededPortGroupsBmpPtr &= isFullPortGroupsBmp;

    /* from port groups on which 'trigger action' processing NOT completed remove those with AUQ full.
       which leave us with :
       port groups that action not finished but the AUQ not full so those port groups NOT ready for the WA !
    */
    *waNotReadyPortGroupsBmpPtr = actNotFinishedPortGroupsBmp & ~(isFullPortGroupsBmp);

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChBrgFdbTriggerAuQueueWa
*
* DESCRIPTION:
*       Trigger the Address Update (AU) queue workaround for case when learning
*       from CPU is stopped due to full Address Update (AU) fifo queue
*       (FEr#3119)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_STATE             - the secondary AUQ not reached the end of
*                                  queue , meaning no need to set the secondary
*                                  base address again.
*                                  this protect the SW from losing unread messages
*                                  in the secondary queue (otherwise the PP may
*                                  override them with new messages).
*                                  caller may use function
*                                  cpssDxChBrgFdbSecondaryAuMsgBlockGet(...)
*                                  to get messages from the secondary queue.
*       GT_NOT_SUPPORTED         - the device not need / not support the WA
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbTriggerAuQueueWa
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlWaPtr;  /* pointer to the WA descriptors DB
                                                      of the device */
    GT_UINTPTR                  phyAddr;       /* phy address */
    GT_U32                      regAddr;       /* register address */
    GT_STATUS                   rc;
    GT_U32                      regValue;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_BOOL waStarted = GT_FALSE;/* was the WA started */
    GT_BOOL waNeeded;/* do we need WA on the current port group */
    GT_PORT_GROUPS_BMP  waNeededPortGroupsBmp;/*bmp of port groups that need the WA*/
    GT_PORT_GROUPS_BMP  waNotReadyPortGroupsBmp;/*bmp of port groups that action not finished but the AUQ not full
                            so those port groups NOT ready for the WA ! */
    GT_BOOL                     useDoubleAuq;  /* support configuration of two AUQ memory regions */
    GT_U32                      activeSecondaryAuqIndex; /* index of active secondary AUQ */


    /* do sanity check on parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    useDoubleAuq = PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.useDoubleAuq;

    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
         PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E) != GT_TRUE)
    {
        debugPrint((" cpssDxChBrgFdbTriggerAuQueueWa : GT_NOT_SUPPORTED \n"));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* get bmp of port groups which need the WA , and those that WA not ready for them */
    rc = fdbTriggerAuQueueWaCheck(devNum,&waNeededPortGroupsBmp,&waNotReadyPortGroupsBmp);
    if(rc != GT_OK)
    {
        debugPrint((" fdbTriggerAuQueueWaCheck : rc[%d] \n",rc));
        return rc;
    }

    if(waNeededPortGroupsBmp == 0)
    {
        debugPrint((" cpssDxChBrgFdbTriggerAuQueueWa : no reason to start the WA  : GT_BAD_STATE \n"));

        /* no reason to start the WA */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }


    if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) &&
        PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                   enabled == GT_FALSE)
    {
        /* check if we already in the process of releasing the dead lock in the PP */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)
        {
            if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needToCheckSecondaryAuQueue == GT_TRUE)
            {
                /* application must first remove all messages from the secondary AUQ
                   before calling this function again

                   use function cpssDxChBrgFdbSecondaryAuMsgBlockGet(...) to get all
                   current messages in the queue.

                   use function cpssDxChBrgFdbQueueFullGet(...) to check that the PP
                   is stuck and need to 'release' it

                   */

                if(useDoubleAuq == GT_TRUE)
                {
                    if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState == PRV_CPSS_AUQ_STATE_ALL_FULL_E)
                    {
                        /* All secondary AUQ busy */
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                    }

                    if (PRV_CPSS_AUQ_INDEX_MAC(devNum, portGroupId) == 1)
                    {
                        if((PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activeSecondaryAuqIndex == 0 &&
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState == PRV_CPSS_AUQ_STATE_ONE_EMPTY_E))
                        {
                            /* Current AUQ is busy */
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                        }
                    }
                    else
                    {
                        if (PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activeSecondaryAuqIndex == 1 &&
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState == PRV_CPSS_AUQ_STATE_ONE_EMPTY_E)
                        {
                            /* Current AUQ is busy */
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                        }
                    }
                    /* check if need to break loop on device that supports single FDB unit instance */
                    PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
                    continue;
                }

                debugPrint((" cpssDxChBrgFdbTriggerAuQueueWa : portGroupId[%d] GT_BAD_STATE \n",portGroupId));
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
            /* check if need to break loop on device that supports single FDB unit instance */
            PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)

        /* check if we already in the process of releasing the dead lock in the PP */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)
        {
            /* check if wa needed for this port group */
            rc = fdbTriggerAuQueueWaCheckAndClear(devNum,portGroupId,&waNeeded);
            if(rc != GT_OK)
            {
                debugPrint((" fdbTriggerAuQueueWaCheckAndClear :  portGroupId[%d] , rc[%d] \n",portGroupId,rc));
                return rc;
            }

            if(waNeeded == GT_FALSE)
            {

                debugPrint((" fdbTriggerAuQueueWaCheckAndClear : WARNING should not happen : portGroupId[%d] not need WA,\n",portGroupId));

                /* the action is not 'stuck' on this port group */

                /* check if need to break loop on device that supports single FDB unit instance */
                PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);

                continue;
            }

            waStarted = GT_TRUE;

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;

            descCtrlWaPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]);

            if(useDoubleAuq == GT_TRUE)
            {
                if(PRV_CPSS_AUQ_INDEX_MAC(devNum, portGroupId) == 1)
                {
                    /* Set pointer to secondary AUQ0 descriptor */
                    descCtrlWaPtr =
                        &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]);
                    /* Set temporary index of active secondary AUQ */
                    activeSecondaryAuqIndex = 0;
                }
                else
                {
                    /* Set pointer to secondary AUQ1 descriptor */
                    descCtrlWaPtr =
                        &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl[portGroupId]);
                    /* Set temporary index of active secondary AUQ */
                    activeSecondaryAuqIndex = 1;
                }

                switch(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState)
                {
                    case PRV_CPSS_AUQ_STATE_ALL_EMPTY_E:
                        /* Assign index of the secondary AUQ for the first time */
                        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activeSecondaryAuqIndex = activeSecondaryAuqIndex;
                        /* Change secondary AUQ current state */
                        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState = PRV_CPSS_AUQ_STATE_ONE_EMPTY_E;
                        break;
                    case PRV_CPSS_AUQ_STATE_ONE_EMPTY_E:
                        /* Change secondary AUQ current state */
                        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState = PRV_CPSS_AUQ_STATE_ALL_FULL_E;
                        break;
                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
            }

            /* get a the value of the AUQ size */
            rc = prvCpssHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 0, 29,
                        &regValue);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(regValue != descCtrlWaPtr->blockSize)
            {
                /* set a new value to the AUQ size , to be with the size of the WA AUQ */
                rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, 0, 29,
                            descCtrlWaPtr->blockSize);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;

            /* set the HW with 'secondary' AUQ base address , that will allow the PP to
               break the  deadlock that was done with the 'primary' AUQ */
            rc = cpssOsVirt2Phy(descCtrlWaPtr->blockAddr,/*OUT*/&phyAddr);
            if (rc != GT_OK)
            {
                return rc;
            }

            #if __WORDSIZE == 64     /* phyAddr must fit in 32 bit */
                if (0 != (phyAddr & 0xffffffff00000000L))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                }
            #endif

            rc =  prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId , regAddr,(GT_U32)phyAddr);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* state that the mechanism should query the 'secondary' AUQ the
               when finish getting messages from primary AUQ */
            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needToCheckSecondaryAuQueue = GT_TRUE;

            /* check if need to break loop on device that supports single FDB unit instance */
            PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)

    }
    else
    {
        /* working without the SDMA AUQ (only with the 'on Chip Fifo') */

        /* take care of all port groups */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)
        {
            /* check if wa needed for this port group */
            rc = fdbTriggerAuQueueWaCheckAndClear(devNum,portGroupId,&waNeeded);
            if(rc != GT_OK)
            {
                debugPrint((" fdbTriggerAuQueueWaCheckAndClear :  portGroupId[%d] , rc[%d] \n",portGroupId,rc));
                return rc;
            }

            if(waNeeded == GT_FALSE)
            {
                debugPrint((" fdbTriggerAuQueueWaCheckAndClear : WARNING should not happen : portGroupId[%d] not need WA,\n",portGroupId));
                /* the action is not 'stuck' on this port group */

                /* check if need to break loop on device that supports single FDB unit instance */
                PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
                continue;
            }

            waStarted = GT_TRUE;
            /* check if need to break loop on device that supports single FDB unit instance */
            PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)

    }

    if(waStarted == GT_FALSE)
    {
        debugPrint((" cpssDxChBrgFdbTriggerAuQueueWa : WARNING should not happen  : GT_BAD_STATE \n"));

        /* no reason to start the WA */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* set/clear the flag of <notReady> */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(0 == (waNotReadyPortGroupsBmp & (1<<(portGroupId))))
        {
            /* clear the flag on this port group */
            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].notReady = GT_FALSE;
        }
        else
        {
            /* the WA started for the device but this port group was not ready so it was skipped */
            debugPrint(("cpssDxChBrgFdbTriggerAuQueueWa : 'not ready' portGroupId[%d] \n",portGroupId));
            /* set the flag on this port group */
            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].notReady = GT_TRUE;
        }

        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)



    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbTriggerAuQueueWa
*
* DESCRIPTION:
*       Trigger the Address Update (AU) queue workaround for case when learning
*       from CPU is stopped due to full Address Update (AU) fifo queue
*       (FEr#3119)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_STATE             - the secondary AUQ not reached the end of
*                                  queue , meaning no need to set the secondary
*                                  base address again.
*                                  this protect the SW from losing unread messages
*                                  in the secondary queue (otherwise the PP may
*                                  override them with new messages).
*                                  caller may use function
*                                  cpssDxChBrgFdbSecondaryAuMsgBlockGet(...)
*                                  to get messages from the secondary queue.
*       GT_NOT_SUPPORTED         - the device not need / not support the WA
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTriggerAuQueueWa
(
    IN  GT_U8 devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbTriggerAuQueueWa);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChBrgFdbTriggerAuQueueWa(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbQueueFullGet
*
* DESCRIPTION:
*       The function returns the state that the AUQ/FUQ - is full/not full
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum     - the device number
*       queueType - queue type AU/FU queue
*                   NOTE: when using shared queue for AU and FU messages, this
*                         parameter is ignored (and the AUQ is queried)
*                         meaning that this parameter always ignored for DxCh1
*                         devices.
*
*
* OUTPUTS:
*       isFullPtr - (pointer to) is the relevant queue full
*                   GT_TRUE  - the queue is full
*                   GT_FALSE - the queue is not full
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum , queueType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           'queue is full' when at least one of the port groups has queue full.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbQueueFullGet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_BOOL *isFullPtr
)
{
    GT_U32                      regAddr;       /* register address */
    GT_STATUS                   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E );
    CPSS_NULL_PTR_CHECK_MAC(isFullPtr);

    if((queueType == CPSS_DXCH_FDB_QUEUE_TYPE_AU_E) ||
       PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.fuqUseSeparate == GT_FALSE)
    {
        /*use Address Update Queue Control register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;
    }
    else if(queueType == CPSS_DXCH_FDB_QUEUE_TYPE_FU_E)
    {
        /*use FU Update Queue Control register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check that at least one port group is full */
    rc = prvCpssPortGroupBusyWait(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                regAddr,30,
                GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if(rc == GT_BAD_STATE)
    {
        rc = GT_OK;
        /* at least one of the port groups has AUQ full */
        *isFullPtr = GT_TRUE;
    }
    else
    {
        /* All port groups not have AUQ full */
        *isFullPtr = GT_FALSE;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbQueueFullGet
*
* DESCRIPTION:
*       The function returns the state that the AUQ/FUQ - is full/not full
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum     - the device number
*       queueType - queue type AU/FU queue
*                   NOTE: when using shared queue for AU and FU messages, this
*                         parameter is ignored (and the AUQ is queried)
*                         meaning that this parameter always ignored for DxCh1
*                         devices.
*
*
* OUTPUTS:
*       isFullPtr - (pointer to) is the relevant queue full
*                   GT_TRUE  - the queue is full
*                   GT_FALSE - the queue is not full
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum , queueType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           'queue is full' when at least one of the port groups has queue full.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQueueFullGet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_BOOL *isFullPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbQueueFullGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueType, isFullPtr));

    rc = internal_cpssDxChBrgFdbQueueFullGet(devNum, queueType, isFullPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueType, isFullPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgFdbQueueRewindStatusGet
*
* DESCRIPTION:
*       function check if the specific AUQ was 'rewind' since the last time
*       this function was called for that AUQ
*       this information allow the application to know when to finish processing
*       of the WA relate to cpssDxChBrgFdbTriggerAuQueueWa(...)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       rewindPtr       - (pointer to) was the AUQ rewind since last call
*                           GT_TRUE - AUQ was rewind since last call
*                           GT_FALSE - AUQ wasn't rewind since last call
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbQueueRewindStatusGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *rewindPtr
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  numPortGroupsForWa = 0;/* number of port groups that started the WA */
    GT_U32  numPortGroupsRewind = 0;/* number of port groups that rewind from the start of the WA */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(rewindPtr);

    *rewindPtr = GT_FALSE;

    if((0 == PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) ||
        PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                   enabled == GT_TRUE)
    {
        /* working without the SDMA AUQ (only with the 'on Chip Fifo') */

        /* there is no meaning for 'rewind' , so we return 'GT_OK' with 'rewind = GT_FALSE'*/

        return GT_OK;
    }


    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activated == GT_FALSE)
        {
            /* this port group was not activated for the WA */

            /* the rewind flag is not relevant for this port group */
            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].auqRewind = GT_FALSE;

            /* check if need to break loop on device that supports single FDB unit instance */
            PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
            continue;
        }

        numPortGroupsForWa ++;

        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].auqRewind == GT_FALSE)
        {
            /* not rewind */

            /* check if need to break loop on device that supports single FDB unit instance */
            PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
            continue;
        }

        /* this port group not need to be counted for next time , because already rewind */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activated = GT_FALSE;
        /* this port group already rewind and counted */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].auqRewind = GT_FALSE;

        /* this port group was rewind */
        numPortGroupsRewind ++;

        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    if(numPortGroupsForWa == 0)
    {
        return GT_OK;
    }

    if(numPortGroupsForWa == numPortGroupsRewind)
    {
        /* all the relevant port groups rewind */
        *rewindPtr = GT_TRUE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbQueueRewindStatusGet
*
* DESCRIPTION:
*       function check if the specific AUQ was 'rewind' since the last time
*       this function was called for that AUQ
*       this information allow the application to know when to finish processing
*       of the WA relate to cpssDxChBrgFdbTriggerAuQueueWa(...)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       rewindPtr       - (pointer to) was the AUQ rewind since last call
*                           GT_TRUE - AUQ was rewind since last call
*                           GT_FALSE - AUQ wasn't rewind since last call
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQueueRewindStatusGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *rewindPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbQueueRewindStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, rewindPtr));

    rc = internal_cpssDxChBrgFdbQueueRewindStatusGet(devNum, rewindPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, rewindPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* cpssDxChPortGroupBrgFdbMessagesQueueManagerInfoGet
*
* DESCRIPTION:
*       debug tool - print info that the CPSS hold about the state of the :
*           primary AUQ,FUQ,secondary AU queue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portGroupId - the port group Id
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       relate to CQ#109715
*
*******************************************************************************/
static GT_STATUS cpssDxChPortGroupBrgFdbMessagesQueueManagerInfoGet
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId
)
{    PRV_CPSS_AU_DESC_CTRL_STC *managerArray[NUM_OF_MESSAGES_QUEUES_CNS] = {0};
    char*                     namesArray[NUM_OF_MESSAGES_QUEUES_CNS] = {0};
    GT_U32                    managerArrayNum;
    GT_U32  ii;
    PRV_CPSS_DXCH_MODULE_CONFIG_STC * moduleCfgPtr =
        &(PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg); /* pointer to the module configure of the PP's database*/

    *managerArray = *managerArray; /* warning fix */
    *namesArray   = *namesArray;   /* warning fix */

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        debugPrint((" current portGroupId is : [%d]  \n", portGroupId));
    }


    managerArrayNum = 0;
    namesArray[managerArrayNum] = "auq_prim";
    managerArray[managerArrayNum] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]);
    managerArrayNum++;

    if(moduleCfgPtr->useDoubleAuq)
    {
        namesArray[managerArrayNum] = "auq1_prim";
        managerArray[managerArrayNum] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl[portGroupId]);
        managerArrayNum++;
    }

    if(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId].blockSize)
    {
        namesArray[managerArrayNum] = "__fuq___";
        managerArray[managerArrayNum] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
        managerArrayNum++;
    }

    if(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId].blockSize)
    {
        namesArray[managerArrayNum] = "auq_secondary";
        managerArray[managerArrayNum] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]);
        managerArrayNum++;
    }

    if(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl[portGroupId].blockSize)
    {
        namesArray[managerArrayNum] = "auq1_secondary";
        managerArray[managerArrayNum] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl[portGroupId]);
        managerArrayNum++;
    }

    debugPrint(("queue   blockAddr  blockSize   currDescIdx unreadCncCounters  \n"));
    debugPrint(("============================================================= \n"));

    for(ii = 0 ; ii < managerArrayNum ; ii++)
    {
        debugPrint(("%s    0x%8.8x  %d \t %d  \t %d \n",
                    namesArray[ii],
                    managerArray[ii]->blockAddr,
                    managerArray[ii]->blockSize,
                    managerArray[ii]->currDescIdx,
                    managerArray[ii]->unreadCncCounters
                    ));
    }

    debugPrint(("activeAuqIndex = [%d],"
             "primaryState = [%d],"
             "needToCheckSecondaryAuQueue = [%d],"
             "auqRewind = [%d],"
             "activated = [%d],"
             "needWaWaitForReTriggering = [%d],"
             "notReady = [%d],"
             "activeSecondaryAuqIndex = [%d],"
             "secondaryState = [%d],"
             "\n"
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.activeAuqIndex[portGroupId]
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needToCheckSecondaryAuQueue
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].auqRewind
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activated
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needWaWaitForReTriggering
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].notReady
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activeSecondaryAuqIndex
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState
            ));

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMessagesQueueManagerInfoGet
*
* DESCRIPTION:
*       debug tool - print info that the CPSS hold about the state of the :
*           primary AUQ,FUQ,secondary AU queue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMessagesQueueManagerInfoGet
(
    IN  GT_U8   devNum
)
{
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        rc = cpssDxChPortGroupBrgFdbMessagesQueueManagerInfoGet(devNum,portGroupId);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMessagesQueueManagerInfoGet
*
* DESCRIPTION:
*       debug tool - print info that the CPSS hold about the state of the :
*           primary AUQ,FUQ,secondary AU queue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMessagesQueueManagerInfoGet
(
    IN  GT_U8   devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMessagesQueueManagerInfoGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChBrgFdbMessagesQueueManagerInfoGet(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortGroupMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index           - hw mac entry index
*       age            - Age flag that is used for the two-step Aging process.
*                        GT_FALSE - The entry will be aged out in the next pass.
*                        GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortGroupMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN GT_U32       index,
    IN GT_BOOL      age
)
{
    GT_STATUS   rc;
    GT_U32      fieldValue;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
    }

    fieldValue = BOOL2BIT_MAC(age);

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = prvCpssDxChPortGroupWriteTableEntryField(devNum,
                                                 portGroupId,
                                                 PRV_CPSS_DXCH_TABLE_FDB_E,
                                                 index,
                                                 PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                                 LION3_FDB_FDB_TABLE_FIELDS_AGE_E, /* field name */
                                                 PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                                 fieldValue);
        }
        else
        {
            rc = prvCpssDxChPortGroupWriteTableEntryField(devNum,
                                                 portGroupId,
                                                 PRV_CPSS_DXCH_TABLE_FDB_E,
                                                 index,
                                                 0, /* word */
                                                 2, /* field offset */
                                                 1, /* field length */
                                                 fieldValue);
        }

        if(rc != GT_OK)
        {
            return rc;
        }
        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index           - hw mac entry index
*       age            - Age flag that is used for the two-step Aging process.
*                        GT_FALSE - The entry will be aged out in the next pass.
*                        GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN GT_U32       index,
    IN GT_BOOL      age
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortGroupMacEntryAgeBitSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, index, age));

    rc = internal_cpssDxChBrgFdbPortGroupMacEntryAgeBitSet(devNum, portGroupsBmp, index, age);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, index, age));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* portGroupBitGet
*
* DESCRIPTION:
*       Read register from needed port groups , and return value of needed bit
*       from those port groups (in bitmap format)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       regAddr     - register address
*       bitIndex    - bit index to read from the register.
*
* OUTPUTS:
*       bitValuePortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      bit is set.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           bit is set , otherwise bit is not set
*                      for multi-port groups device :
*                           when bit is set (1) - the bit is set (in the
*                                            corresponding port group).
*                           when bit is not set (0) - the bit is not set. (in the corresponding
*                                            port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS portGroupBitGet
(
    IN  GT_U8    devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32  regAddr,
    IN  GT_U32  bitIndex,
    OUT GT_PORT_GROUPS_BMP  *bitValuePortGroupsBmpPtr
)
{
    GT_STATUS rc;   /* return code                  */
    GT_U32  hwData;/* hw data - single bit value - 0 or 1 */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(bitValuePortGroupsBmpPtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
    }

    *bitValuePortGroupsBmpPtr = 0;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* get the bit value on this port group */
        rc = prvCpssHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, bitIndex, 1, &hwData);
        if (rc != GT_OK)
        {
            return rc;
        }

        *bitValuePortGroupsBmpPtr |= (hwData) << portGroupId;
        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    if(portGroupsBmp == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        /* in unaware mode convert bmps of 'all active port groups' to the
           'unaware value' CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance == GT_TRUE &&
           (*bitValuePortGroupsBmpPtr) )
        {
            /* single port group checked and is not 0 */
            *bitValuePortGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
        else
        if((*bitValuePortGroupsBmpPtr) ==
            PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp)
        {
            /* all port groups have the bit set to 1 */
            *bitValuePortGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortGroupTrigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       actFinishedPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      'trigger action' processing completion.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation completed , otherwise not completed
*                      for multi-port groups device :
*                           when bit is set (1) - 'trigger action' was completed by PP (in the
*                                            corresponding port group).
*                           when bit is not set (0) - 'trigger action' is not
*                                            completed yet by PP. (in the corresponding
*                                            port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortGroupTrigActionStatusGet
(
    IN  GT_U8    devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_PORT_GROUPS_BMP  *actFinishedPortGroupsBmpPtr
)
{
    GT_STATUS rc;   /* return code                  */
    GT_U32 regAddr;     /* register address             */
    GT_PORT_GROUPS_BMP bitValuePortGroupsBmp;/*bitmap of port groups on which bit is set.*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(actFinishedPortGroupsBmpPtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBActionGeneral;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;
    }

    /* get value of bit 1 in the register from all port groups */
    rc = portGroupBitGet(devNum,portGroupsBmp,regAddr,1,&bitValuePortGroupsBmp);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* the 'trigger action done' is the opposite value to the bit */
    *actFinishedPortGroupsBmpPtr = ~bitValuePortGroupsBmp;
    /* return only relevant bits */
    *actFinishedPortGroupsBmpPtr &= portGroupsBmp;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupTrigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       actFinishedPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      'trigger action' processing completion.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation completed , otherwise not completed
*                      for multi-port groups device :
*                           when bit is set (1) - 'trigger action' was completed by PP (in the
*                                            corresponding port group).
*                           when bit is not set (0) - 'trigger action' is not
*                                            completed yet by PP. (in the corresponding
*                                            port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupTrigActionStatusGet
(
    IN  GT_U8    devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_PORT_GROUPS_BMP  *actFinishedPortGroupsBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortGroupTrigActionStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, actFinishedPortGroupsBmpPtr));

    rc = internal_cpssDxChBrgFdbPortGroupTrigActionStatusGet(devNum, portGroupsBmp, actFinishedPortGroupsBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, actFinishedPortGroupsBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbPortGroupQueueFullGet
*
* DESCRIPTION:
*       The function returns the state that the AUQ/FUQ - is full/not full
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum     - the device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       queueType - queue type AU/FU queue
*                   NOTE: when using shared queue for AU and FU messages, this
*                         parameter is ignored (and the AUQ is queried)
*                         meaning that this parameter always ignored for DxCh1
*                         devices.
*
*
* OUTPUTS:
*       isFullPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      'the queue is full'.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           'queue is full' , otherwise 'queue is NOT full'
*                      for multi-port groups device :
*                           when bit is set (1) - 'queue is full' (in the
*                                            corresponding port group).
*                           when bit is not set (0) - 'queue is NOT full'. (in the corresponding
*                                            port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp or queueType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortGroupQueueFullGet
(
    IN GT_U8    devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_PORT_GROUPS_BMP  *isFullPortGroupsBmpPtr
)
{
    GT_STATUS rc;   /* return code                  */
    GT_U32 regAddr;     /* register address             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E );
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(isFullPortGroupsBmpPtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
    }

    if((queueType == CPSS_DXCH_FDB_QUEUE_TYPE_AU_E) ||
       PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.fuqUseSeparate == GT_FALSE)
    {
        /*use Address Update Queue Control register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;
    }
    else if(queueType == CPSS_DXCH_FDB_QUEUE_TYPE_FU_E)
    {
        /*use FU Update Queue Control register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get value of bit 30 in the register from all port groups */
    rc = portGroupBitGet(devNum,portGroupsBmp,regAddr,30,isFullPortGroupsBmpPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupQueueFullGet
*
* DESCRIPTION:
*       The function returns the state that the AUQ/FUQ - is full/not full
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum     - the device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       queueType - queue type AU/FU queue
*                   NOTE: when using shared queue for AU and FU messages, this
*                         parameter is ignored (and the AUQ is queried)
*                         meaning that this parameter always ignored for DxCh1
*                         devices.
*
*
* OUTPUTS:
*       isFullPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      'the queue is full'.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           'queue is full' , otherwise 'queue is NOT full'
*                      for multi-port groups device :
*                           when bit is set (1) - 'queue is full' (in the
*                                            corresponding port group).
*                           when bit is not set (0) - 'queue is NOT full'. (in the corresponding
*                                            port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp or queueType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupQueueFullGet
(
    IN GT_U8    devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_PORT_GROUPS_BMP  *isFullPortGroupsBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortGroupQueueFullGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, queueType, isFullPortGroupsBmpPtr));

    rc = internal_cpssDxChBrgFdbPortGroupQueueFullGet(devNum, portGroupsBmp, queueType, isFullPortGroupsBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, queueType, isFullPortGroupsBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgFdbMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       index           - hw mac entry index
*       age            - Age flag that is used for the two-step Aging process.
*                        GT_FALSE - The entry will be aged out in the next pass.
*                        GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_BOOL      age
)
{
    return cpssDxChBrgFdbPortGroupMacEntryAgeBitSet(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index,age);
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       index           - hw mac entry index
*       age            - Age flag that is used for the two-step Aging process.
*                        GT_FALSE - The entry will be aged out in the next pass.
*                        GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_BOOL      age
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacEntryAgeBitSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, age));

    rc = internal_cpssDxChBrgFdbMacEntryAgeBitSet(devNum, index, age);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, age));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgFdbNaMsgVid1EnableSet
*
* DESCRIPTION:
*       Enable/Disable Tag1 VLAN Id assignment in vid1 field of the NA AU
*       message CPSS_MAC_UPDATE_MSG_EXT_STC.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE: vid1 field of the NA AU message is taken from
*                          Tag1 VLAN.
*                 GT_FALSE: vid1 field of the NA AU message is taken from
*                          Tag0 VLAN.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbNaMsgVid1EnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32                      regAddr;        /* register address */
    GT_U32                      bitValue;       /* bit value        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.globalUnitConfig;
    }

    bitValue = BOOL2BIT_MAC(enable);

    return prvCpssHwPpSetRegField(devNum, regAddr, 2, 1, bitValue);
}

/*******************************************************************************
* cpssDxChBrgFdbNaMsgVid1EnableSet
*
* DESCRIPTION:
*       Enable/Disable Tag1 VLAN Id assignment in vid1 field of the NA AU
*       message CPSS_MAC_UPDATE_MSG_EXT_STC.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE: vid1 field of the NA AU message is taken from
*                          Tag1 VLAN.
*                 GT_FALSE: vid1 field of the NA AU message is taken from
*                          Tag0 VLAN.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgVid1EnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbNaMsgVid1EnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbNaMsgVid1EnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbNaMsgVid1EnableGet
*
* DESCRIPTION:
*       Get the status of Tag1 VLAN Id assignment in vid1 field of the NA AU
*       message CPSS_MAC_UPDATE_MSG_EXT_STC (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE: vid1 field of the NA AU message is taken from
*                          Tag1 VLAN.
*                    GT_FALSE: vid1 field of the NA AU message is taken from
*                          Tag0 VLAN.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbNaMsgVid1EnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32                      regAddr;        /* register address */
    GT_U32                      bitValue;       /* bit value        */
    GT_STATUS                   rc;             /* return code      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.globalUnitConfig;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 2, 1, &bitValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(bitValue);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbNaMsgVid1EnableGet
*
* DESCRIPTION:
*       Get the status of Tag1 VLAN Id assignment in vid1 field of the NA AU
*       message CPSS_MAC_UPDATE_MSG_EXT_STC (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE: vid1 field of the NA AU message is taken from
*                          Tag1 VLAN.
*                    GT_FALSE: vid1 field of the NA AU message is taken from
*                          Tag0 VLAN.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgVid1EnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbNaMsgVid1EnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbNaMsgVid1EnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function returns AU message processing completion and success status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       completedPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      AU message processing completion.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation completed , otherwise not completed
*                      for multi-port groups device :
*                           when bit is set (1) - AU message was processed by PP (in the
*                                            corresponding port group).
*                           when bit is not set (0) - AU message processed is not
*                                            completed yet by PP. (in the corresponding
*                                            port group)
*       succeededPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      AU operation succeeded.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation succeeded , otherwise failed
*                      for multi-port groups device :
*                           when bit is set (1) - AU action succeeded (in the
*                                            corresponding port group).
*                           when bit is not set (0) - AU action has failed. (in the
*                                            corresponding port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DXCH devices the succeed status doesn't relevant for query
*       address messages (message type = CPSS_QA_E) -  FEr#93.
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_PORT_GROUPS_BMP  *completedPortGroupsBmpPtr,
    OUT GT_PORT_GROUPS_BMP  *succeededPortGroupsBmpPtr
)
{
    GT_U32      regAddr;        /* register address             */
    GT_U32      hwData;         /* data to read from register   */
    GT_STATUS   rc;             /* return code                  */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_PORT_GROUPS_BMP  completedPortGroupsBmp = 0;/* completed bmp */
    GT_PORT_GROUPS_BMP  succeededPortGroupsBmp = 0;/* succeeded bmp */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(completedPortGroupsBmpPtr);
    CPSS_NULL_PTR_CHECK_MAC(succeededPortGroupsBmpPtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAddrUpdateMsgs.AUMsgFromCPU.AUMsgFromCPUCtrl;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    globalRegs.addrUpdateControlReg;
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* check if the message action is completed, read Message from CPU
           Management register */
        /* check that all port groups are ready */
        rc = prvCpssPortGroupBusyWait(devNum,portGroupId,
                    regAddr,0,
                    GT_TRUE);/* only to check the bit --> no 'busy wait' */
        if(rc == GT_BAD_STATE)
        {
            /* check if need to break loop on device that supports single FDB unit instance */
            PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);

            /* this port group not completed the action */
            continue;
        }
        else if (rc != GT_OK)
        {
            return rc;
        }

        /* the action is completed on this port group */
        completedPortGroupsBmp |= 1 << portGroupId;

        /* get the succeeded bit */
        rc = prvCpssHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 1, 1, &hwData);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(hwData)
        {
            succeededPortGroupsBmp |= 1 << portGroupId;
        }

        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    *completedPortGroupsBmpPtr = completedPortGroupsBmp;
    *succeededPortGroupsBmpPtr = succeededPortGroupsBmp;

    if(portGroupsBmp == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        /* in unaware mode convert bmps of 'all active port groups' to the
           'unaware value' CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance == GT_TRUE &&
           (completedPortGroupsBmp) )
        {
            /* single port group checked and is not 0 */
            *completedPortGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

            if(completedPortGroupsBmp == succeededPortGroupsBmp)
            {
                *succeededPortGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            }
        }
        else
        if(completedPortGroupsBmp ==
            PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp)
        {
            /* all port groups completed the action */
            *completedPortGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

            if(completedPortGroupsBmp == succeededPortGroupsBmp)
            {
                *succeededPortGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function returns AU message processing completion and success status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       completedPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      AU message processing completion.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation completed , otherwise not completed
*                      for multi-port groups device :
*                           when bit is set (1) - AU message was processed by PP (in the
*                                            corresponding port group).
*                           when bit is not set (0) - AU message processed is not
*                                            completed yet by PP. (in the corresponding
*                                            port group)
*       succeededPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      AU operation succeeded.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation succeeded , otherwise failed
*                      for multi-port groups device :
*                           when bit is set (1) - AU action succeeded (in the
*                                            corresponding port group).
*                           when bit is not set (0) - AU action has failed. (in the
*                                            corresponding port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DXCH devices the succeed status doesn't relevant for query
*       address messages (message type = CPSS_QA_E) -  FEr#93.
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_PORT_GROUPS_BMP  *completedPortGroupsBmpPtr,
    OUT GT_PORT_GROUPS_BMP  *succeededPortGroupsBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, completedPortGroupsBmpPtr, succeededPortGroupsBmpPtr));

    rc = internal_cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(devNum, portGroupsBmp, completedPortGroupsBmpPtr, succeededPortGroupsBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, completedPortGroupsBmpPtr, succeededPortGroupsBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdb16BitFidHashEnableSet
*
* DESCRIPTION:
*       global enable/disable configuration for FID 16 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: hash is calculated with FID[11:0],
*                     GT_TRUE : hash is calculated with FID[15:0]
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: For systems with both new and old devices must configure
*       FID = VID in the eVLAN table.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdb16BitFidHashEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable
)
{
    GT_U32    regAddr;      /* value to write */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).
                FDBCore.FDBGlobalConfig.FDBGlobalConfig1;

    /* save info to DB , it influence the HASH calc */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.fid16BitHashEn = enable;

    return prvCpssHwPpSetRegField(devNum, regAddr, 4, 1, BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChBrgFdb16BitFidHashEnableSet
*
* DESCRIPTION:
*       global enable/disable configuration for FID 16 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: hash is calculated with FID[11:0],
*                     GT_TRUE : hash is calculated with FID[15:0]
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: For systems with both new and old devices must configure
*       FID = VID in the eVLAN table.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdb16BitFidHashEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdb16BitFidHashEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdb16BitFidHashEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdb16BitFidHashEnableGet
*
* DESCRIPTION:
*       Get global configuration for FID 16 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: hash is calculated with FID[11:0],
*                     GT_TRUE : hash is calculated with FID[15:0]
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdb16BitFidHashEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData;
    GT_U32      regAddr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).
                FDBCore.FDBGlobalConfig.FDBGlobalConfig1;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 4, 1, &hwData);

    if(rc != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdb16BitFidHashEnableGet
*
* DESCRIPTION:
*       Get global configuration for FID 16 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: hash is calculated with FID[11:0],
*                     GT_TRUE : hash is calculated with FID[15:0]
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdb16BitFidHashEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdb16BitFidHashEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdb16BitFidHashEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMaxLengthSrcIdEnableSet
*
* DESCRIPTION:
*       global enable/disable configuration for Src ID 12 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: The SrcID field in FDB table is 9b.
*                               SrcID[11:9] are used for extending the
*                               user defined bits
*                     GT_TRUE : The SrcID filed in FDB is 12b
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMaxLengthSrcIdEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;           /* return code       */
    GT_U32    regAddr;      /* register to write */
    GT_U32    data;         /* value to write    */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).
                FDBCore.FDBGlobalConfig.FDBGlobalConfig1;

    /* HW value: 0 - 12 bit, 1 - 9 bit */
    data = (enable == GT_FALSE) ? 1 : 0;

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 10, 1, data);

    if(rc != GT_OK)
        return rc;

    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.maxLengthSrcIdInFdbEn = enable;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbMaxLengthSrcIdEnableSet
*
* DESCRIPTION:
*       global enable/disable configuration for Src ID 12 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: The SrcID field in FDB table is 9b.
*                               SrcID[11:9] are used for extending the
*                               user defined bits
*                     GT_TRUE : The SrcID filed in FDB is 12b
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMaxLengthSrcIdEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMaxLengthSrcIdEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbMaxLengthSrcIdEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMaxLengthSrcIdEnableGet
*
* DESCRIPTION:
*       Get global configuration for Src ID 12 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: The SrcID field in FDB table is 9b.
*                               SrcID[11:9] are used for extending the
*                               user defined bits
*                     GT_TRUE : The SrcID filed in FDB is 12b
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMaxLengthSrcIdEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData;
    GT_U32      regAddr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).
                FDBCore.FDBGlobalConfig.FDBGlobalConfig1;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 10, 1, &hwData);

    if(rc != GT_OK)
        return rc;

    /* HW value: 0 - 12 bit, 1 - 9 bit */
    *enablePtr = (hwData == 0) ? GT_TRUE : GT_FALSE;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbMaxLengthSrcIdEnableGet
*
* DESCRIPTION:
*       Get global configuration for Src ID 12 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: The SrcID field in FDB table is 9b.
*                               SrcID[11:9] are used for extending the
*                               user defined bits
*                     GT_TRUE : The SrcID filed in FDB is 12b
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMaxLengthSrcIdEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMaxLengthSrcIdEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbMaxLengthSrcIdEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgFdbVid1AssignmentEnableSet
*
* DESCRIPTION:
*       Enable/Disable using of VID1 in FDB entry.
*       The Source ID bits [8:6], SA Security Level and DA Security Level are read as 0
*       from the FDB entry when VID1 enabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: <Tag1 VID> is not written in the FDB and is not read from the FDB.
*                             <SrcID>[8:6] can be used for src-id filtering and <SA Security Level>
*                             and <DA Security Level> are written/read from the FDB.
*                     GT_TRUE:  Tag1 VID> is written in the FDB and read from the FDB
*                             as described in Mac Based VLAN FS section.
*                             <SrcID>[8:6], <SA Security Level> and <DA Security Level>
*                             are read as 0 from the FDB entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbVid1AssignmentEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;           /* return code    */
    GT_U32    regAddr;      /* value to write */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).
                FDBCore.FDBGlobalConfig.FDBGlobalConfig1;

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 27, 1, BOOL2BIT_MAC(enable));

    if(rc != GT_OK)
        return rc;

    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.tag1VidFdbEn = enable;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbVid1AssignmentEnableSet
*
* DESCRIPTION:
*       Enable/Disable using of VID1 in FDB entry.
*       The Source ID bits [8:6], SA Security Level and DA Security Level are read as 0
*       from the FDB entry when VID1 enabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: <Tag1 VID> is not written in the FDB and is not read from the FDB.
*                             <SrcID>[8:6] can be used for src-id filtering and <SA Security Level>
*                             and <DA Security Level> are written/read from the FDB.
*                     GT_TRUE:  Tag1 VID> is written in the FDB and read from the FDB
*                             as described in Mac Based VLAN FS section.
*                             <SrcID>[8:6], <SA Security Level> and <DA Security Level>
*                             are read as 0 from the FDB entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbVid1AssignmentEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbVid1AssignmentEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgFdbVid1AssignmentEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbVid1AssignmentEnableGet
*
* DESCRIPTION:
*        Get Enable/Disable status of using of VID1 in FDB entry.
*        The Source ID bits [8:6], SA Security Level and DA Security Level are read as 0
*        from the FDB entry when VID1 enabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: <Tag1 VID> is not written in the FDB and is not read from the FDB.
*                             <SrcID>[8:6] can be used for src-id filtering and <SA Security Level>
*                             and <DA Security Level> are written/read from the FDB.
*                     GT_TRUE:  Tag1 VID> is written in the FDB and read from the FDB
*                             as described in Mac Based VLAN FS section.
*                             <SrcID>[8:6], <SA Security Level> and <DA Security Level>
*                             are read as 0 from the FDB entry.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbVid1AssignmentEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData;
    GT_U32      regAddr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).
                FDBCore.FDBGlobalConfig.FDBGlobalConfig1;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 27, 1, &hwData);

    if(rc != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbVid1AssignmentEnableGet
*
* DESCRIPTION:
*        Get Enable/Disable status of using of VID1 in FDB entry.
*        The Source ID bits [8:6], SA Security Level and DA Security Level are read as 0
*        from the FDB entry when VID1 enabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: <Tag1 VID> is not written in the FDB and is not read from the FDB.
*                             <SrcID>[8:6] can be used for src-id filtering and <SA Security Level>
*                             and <DA Security Level> are written/read from the FDB.
*                     GT_TRUE:  Tag1 VID> is written in the FDB and read from the FDB
*                             as described in Mac Based VLAN FS section.
*                             <SrcID>[8:6], <SA Security Level> and <DA Security Level>
*                             are read as 0 from the FDB entry.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbVid1AssignmentEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbVid1AssignmentEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgFdbVid1AssignmentEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgFdbSaLookupAnalyzerIndexSet
*
* DESCRIPTION:
*       Set analyzer index used when the FDB SA lookup has
*       <SA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - set mirror to analyzer index field
*                     GT_FALSE - set ZERO to the analyzer index field
*       index       - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: If a previous mechanism assigns a different analyzer index,
*             the higher index wins.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbSaLookupAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable,
    IN GT_U32  index
)
{
    GT_U32    regAddr;      /* value to write */
    GT_U32    hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(enable == GT_FALSE) /* ignore index and set 0 */
    {
        hwData = 0;
    }
    else
    {
        if(index >  PRV_CPSS_DXCH_FDB_MIRROR_TO_ANALYZER_MAX_INDEX_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        hwData = index + 1;
    }

    regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                bridgeEngineConfig.bridgeGlobalConfig0;

    return prvCpssHwPpSetRegField(devNum,regAddr ,17 ,3 ,hwData);
}

/*******************************************************************************
* cpssDxChBrgFdbSaLookupAnalyzerIndexSet
*
* DESCRIPTION:
*       Set analyzer index used when the FDB SA lookup has
*       <SA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - set mirror to analyzer index field
*                     GT_FALSE - set ZERO to the analyzer index field
*       index       - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: If a previous mechanism assigns a different analyzer index,
*             the higher index wins.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSaLookupAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable,
    IN GT_U32  index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbSaLookupAnalyzerIndexSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable, index));

    rc = internal_cpssDxChBrgFdbSaLookupAnalyzerIndexSet(devNum, enable, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbSaLookupAnalyzerIndexGet
*
* DESCRIPTION:
*       Get analyzer index used when the FDB SA lookup has
*       <SA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_TRUE  - analyzer index field is NOT ZERO
*                     GT_FALSE - analyzer index field is set to ZERO
*       indexPtr    - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbSaLookupAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData;
    GT_U32      regAddr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                bridgeEngineConfig.bridgeGlobalConfig0;

    rc = prvCpssHwPpGetRegField(devNum,regAddr ,17 ,3 ,&hwData);

    if(rc != GT_OK)
         return rc;

    if(hwData == 0) /* ignore index and set 0 */
    {
        *enablePtr = GT_FALSE;
        *indexPtr  = 0;
    }
    else
    {
        *enablePtr = GT_TRUE;
        *indexPtr = hwData-1;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbSaLookupAnalyzerIndexGet
*
* DESCRIPTION:
*       Get analyzer index used when the FDB SA lookup has
*       <SA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_TRUE  - analyzer index field is NOT ZERO
*                     GT_FALSE - analyzer index field is set to ZERO
*       indexPtr    - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSaLookupAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbSaLookupAnalyzerIndexGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr, indexPtr));

    rc = internal_cpssDxChBrgFdbSaLookupAnalyzerIndexGet(devNum, enablePtr, indexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr, indexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbDaLookupAnalyzerIndexSet
*
* DESCRIPTION:
*       Set analyzer index used when the FDB DA lookup has
*       <DA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - set mirror to analyzer index field
*                     GT_FALSE - set ZERO to the analyzer index field
*       index       - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: If a previous mechanism assigns a different analyzer index,
*             the higher index wins.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbDaLookupAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable,
    IN GT_U32  index
)
{
    GT_U32    regAddr;      /* value to write */
    GT_U32    hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(enable == GT_FALSE) /* ignore index and set 0 */
    {
        hwData = 0;
    }
    else
    {
        if(index >  PRV_CPSS_DXCH_FDB_MIRROR_TO_ANALYZER_MAX_INDEX_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        hwData = index + 1;
    }

    regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                bridgeEngineConfig.bridgeGlobalConfig0;

    return prvCpssHwPpSetRegField(devNum,regAddr ,20 ,3 ,hwData);
}

/*******************************************************************************
* cpssDxChBrgFdbDaLookupAnalyzerIndexSet
*
* DESCRIPTION:
*       Set analyzer index used when the FDB DA lookup has
*       <DA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - set mirror to analyzer index field
*                     GT_FALSE - set ZERO to the analyzer index field
*       index       - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: If a previous mechanism assigns a different analyzer index,
*             the higher index wins.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDaLookupAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable,
    IN GT_U32  index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbDaLookupAnalyzerIndexSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable, index));

    rc = internal_cpssDxChBrgFdbDaLookupAnalyzerIndexSet(devNum, enable, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbDaLookupAnalyzerIndexGet
*
* DESCRIPTION:
*       Get analyzer index used when the FDB DA lookup has
*       <DA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_TRUE  - analyzer index field is NOT ZERO
*                     GT_FALSE - analyzer index field is set to ZERO
*       indexPtr    - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbDaLookupAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData;
    GT_U32      regAddr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                bridgeEngineConfig.bridgeGlobalConfig0;

    rc = prvCpssHwPpGetRegField(devNum,regAddr ,20 ,3 ,&hwData);

    if(rc != GT_OK)
         return rc;

    if(hwData == 0) /* ignore index and set 0 */
    {
        *enablePtr = GT_FALSE;
        *indexPtr  = 0;
    }
    else
    {
        *enablePtr = GT_TRUE;
        *indexPtr = hwData-1;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbDaLookupAnalyzerIndexGet
*
* DESCRIPTION:
*       Get analyzer index used when the FDB DA lookup has
*       <DA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_TRUE  - analyzer index field is NOT ZERO
*                     GT_FALSE - analyzer index field is set to ZERO
*       indexPtr    - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDaLookupAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbDaLookupAnalyzerIndexGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr, indexPtr));

    rc = internal_cpssDxChBrgFdbDaLookupAnalyzerIndexGet(devNum, enablePtr, indexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr, indexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionActiveUserDefinedSet
*
* DESCRIPTION:
*       Set the Active userDefined and active userDefined mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "userDefined" masked by the "userDefinedmask" equal to
*       "Active userDefined"
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       actUerDefined       - action active UerDefined
*       actUerDefinedMask   - action active UerDefined Mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - actUerDefined or actUerDefinedMask out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionActiveUserDefinedSet
(
    IN GT_U8    devNum,
    IN GT_U32   actUerDefined,
    IN GT_U32   actUerDefinedMask
)
{
    GT_STATUS rc;
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(actUerDefined        > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_FDB_USER_DEFINED_MAC(devNum) ||
       actUerDefinedMask    > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_FDB_USER_DEFINED_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction3;

    data = ((actUerDefinedMask & 0xFF) << 8) | (actUerDefined & 0xFF);

    /* Set the Active UerDefined and active UerDefined mask */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 16, data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveUserDefinedSet
*
* DESCRIPTION:
*       Set the Active userDefined and active userDefined mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "userDefined" masked by the "userDefinedmask" equal to
*       "Active userDefined"
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       actUerDefined       - action active UerDefined
*       actUerDefinedMask   - action active UerDefined Mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - actUerDefined or actUerDefinedMask out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveUserDefinedSet
(
    IN GT_U8    devNum,
    IN GT_U32   actUerDefined,
    IN GT_U32   actUerDefinedMask
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionActiveUserDefinedSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, actUerDefined, actUerDefinedMask));

    rc = internal_cpssDxChBrgFdbActionActiveUserDefinedSet(devNum, actUerDefined, actUerDefinedMask);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, actUerDefined, actUerDefinedMask));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbActionActiveUserDefinedGet
*
* DESCRIPTION:
*       Get the Active userDefined and active userDefined mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "userDefined" masked by the "userDefinedmask" equal to
*       "Active userDefined"
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       actUerDefinedPtr      - pointer to action active UerDefined
*       actUerDefinedMaskPtr   - pointer to action active UerDefined Mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbActionActiveUserDefinedGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *actUerDefinedPtr,
    OUT GT_U32   *actUerDefinedMaskPtr
)
{
    GT_U32    regAddr;          /* register address     */
    GT_U32    data;             /* reg sub field data   */
    GT_STATUS rc;               /* return code          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(actUerDefinedPtr);
    CPSS_NULL_PTR_CHECK_MAC(actUerDefinedMaskPtr);

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAction.FDBAction3;

    /* get Active UerDefined and active UerDefined mask from FDB Action6 Register */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 16, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *actUerDefinedPtr       = (data & 0xFF);
    *actUerDefinedMaskPtr   = ((data >> 8) & 0xFF);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveUserDefinedGet
*
* DESCRIPTION:
*       Get the Active userDefined and active userDefined mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "userDefined" masked by the "userDefinedmask" equal to
*       "Active userDefined"
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       actUerDefinedPtr      - pointer to action active UerDefined
*       actUerDefinedMaskPtr   - pointer to action active UerDefined Mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveUserDefinedGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *actUerDefinedPtr,
    OUT GT_U32   *actUerDefinedMaskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbActionActiveUserDefinedGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, actUerDefinedPtr, actUerDefinedMaskPtr));

    rc = internal_cpssDxChBrgFdbActionActiveUserDefinedGet(devNum, actUerDefinedPtr, actUerDefinedMaskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, actUerDefinedPtr, actUerDefinedMaskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbLearnPrioritySet
*
* DESCRIPTION:
*       Set Learn priority per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       learnPriority  - Learn priority can be Low or High
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkLearnPrioritySet
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbLearnPrioritySet
(
    IN GT_U8                                devNum,
    IN GT_PORT_NUM                          portNum,
    IN CPSS_DXCH_FDB_LEARN_PRIORITY_ENT     learnPriority
)
{
    GT_U32    hwData;      /* value to write */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(learnPriority)
    {
        case CPSS_DXCH_FDB_LEARN_PRIORITY_LOW_E:
            hwData=0;
            break;
        case CPSS_DXCH_FDB_LEARN_PRIORITY_HIGH_E:
            hwData=1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* set Bridge-Ingress-ePort table */
    return prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            0, /* start bit */
                                            1,  /* 1 bit */
                                            hwData);
}

/*******************************************************************************
* cpssDxChBrgFdbLearnPrioritySet
*
* DESCRIPTION:
*       Set Learn priority per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       learnPriority  - Learn priority can be Low or High
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkLearnPrioritySet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbLearnPrioritySet
(
    IN GT_U8                                devNum,
    IN GT_PORT_NUM                          portNum,
    IN CPSS_DXCH_FDB_LEARN_PRIORITY_ENT     learnPriority
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbLearnPrioritySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, learnPriority));

    rc = internal_cpssDxChBrgFdbLearnPrioritySet(devNum, portNum, learnPriority);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, learnPriority));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbLearnPriorityGet
*
* DESCRIPTION:
*       Get Learn priority per port
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       learnPriorityPtr  - (pointer to)Learn priority can be Low or High
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkLearnPriorityGet
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbLearnPriorityGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_DXCH_FDB_LEARN_PRIORITY_ENT    *learnPriorityPtr
)
{
    GT_STATUS rc;
    GT_U32    hwData;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(learnPriorityPtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* get Bridge-Ingress-ePort table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            0, /* start bit */
                                            1,  /* 1 bit */
                                            &hwData);
    if(rc!=GT_OK)
        return rc;

    switch(hwData)
    {
        case 0:
            *learnPriorityPtr=CPSS_DXCH_FDB_LEARN_PRIORITY_LOW_E;
            break;
        case 1:
            *learnPriorityPtr=CPSS_DXCH_FDB_LEARN_PRIORITY_HIGH_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbLearnPriorityGet
*
* DESCRIPTION:
*       Get Learn priority per port
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       learnPriorityPtr  - (pointer to)Learn priority can be Low or High
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkLearnPriorityGet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbLearnPriorityGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_DXCH_FDB_LEARN_PRIORITY_ENT    *learnPriorityPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbLearnPriorityGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, learnPriorityPtr));

    rc = internal_cpssDxChBrgFdbLearnPriorityGet(devNum, portNum, learnPriorityPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, learnPriorityPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbUserGroupSet
*
* DESCRIPTION:
*       Set User Group per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       userGroup   - user group
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkUserGroupSet
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbUserGroupSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_U32           userGroup
)
{
    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(userGroup >= BIT_4)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* set Bridge-Ingress-ePort table */
    return prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            1, /* start bit */
                                            4,  /* 4 bits */
                                            userGroup);
}

/*******************************************************************************
* cpssDxChBrgFdbUserGroupSet
*
* DESCRIPTION:
*       Set User Group per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       userGroup   - user group
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkUserGroupSet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUserGroupSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_U32           userGroup
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbUserGroupSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, userGroup));

    rc = internal_cpssDxChBrgFdbUserGroupSet(devNum, portNum, userGroup);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, userGroup));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbUserGroupGet
*
* DESCRIPTION:
*       Get User Group per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       userGroupPtr  - (pointer to) user group
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkUserGroupGet
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbUserGroupGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *userGroupPtr
)
{
    GT_STATUS rc;
    GT_U32    hwData;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(userGroupPtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* get Bridge-Ingress-ePort table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            1, /* start bit */
                                            4,  /* 4 bits */
                                            &hwData);
    if(rc!=GT_OK)
        return rc;

    *userGroupPtr = hwData;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbUserGroupGet
*
* DESCRIPTION:
*       Get User Group per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       userGroupPtr  - (pointer to) user group
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkUserGroupGet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUserGroupGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *userGroupPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbUserGroupGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, userGroupPtr));

    rc = internal_cpssDxChBrgFdbUserGroupGet(devNum, portNum, userGroupPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, userGroupPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

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
)
{
    GT_U32 hwData[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]; /* words of Au hw entry */
    GT_STATUS  rc;                                  /* return status        */
    GT_U32  portGroupId ;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entryKeyPtr);

    /* build entry hw format */
    rc = prvDxChFdbToAuConvert(devNum,
                               entryKeyPtr,
                               NULL,/* the entry not needed ... only need the key */
                               CPSS_HR_E,
                               GT_FALSE,
                               &hwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* the feature supported on devices with single FDB instance */
    portGroupId = 0;

    /* write data to Hw */
    return prvDxChFdbSendAuMessage(devNum,portGroupId,&hwData[0]);
}

/*******************************************************************************
* internal_cpssDxChBrgFdbCrcHashUpperBitsModeSet
*
* DESCRIPTION:
*       Set mode how the CRC hash (CPSS_MAC_HASH_FUNC_CRC_E or
*       CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
*       will use 16 'most upper bits' for lookup key of type 'MAC+FID'
*       (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
*       relevant when using 'Independent VLAN Learning' (CPSS_IVL_E)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       mode        - the upper bits mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbCrcHashUpperBitsModeSet
(
    IN  GT_U8           devNum,
    IN  CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT     mode
)
{
    GT_U32  hwValue;/*value to write to HW */
    GT_U32  regAddr;/* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(mode)
    {
        case CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ALL_ZERO_E:
            hwValue = 0;
            break;
        case CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_FID_E:
            hwValue = 1;
            break;
        case CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_MAC_E:
            hwValue = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* save info to DB , it influence the HASH calc */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.crcHashUpperBitsMode = mode;

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig1;

    /* Set <CRCHashUpperBitsMode> */
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 2, hwValue);
}

/*******************************************************************************
* cpssDxChBrgFdbCrcHashUpperBitsModeSet
*
* DESCRIPTION:
*       Set mode how the CRC hash (CPSS_MAC_HASH_FUNC_CRC_E or
*       CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
*       will use 16 'most upper bits' for lookup key of type 'MAC+FID'
*       (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
*       relevant when using 'Independent VLAN Learning' (CPSS_IVL_E)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       mode        - the upper bits mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbCrcHashUpperBitsModeSet
(
    IN  GT_U8           devNum,
    IN  CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT     mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbCrcHashUpperBitsModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChBrgFdbCrcHashUpperBitsModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbCrcHashUpperBitsModeGet
*
* DESCRIPTION:
*       Get mode how the CRC hash (CPSS_MAC_HASH_FUNC_CRC_E or
*       CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
*       will use 16 'most upper bits' for lookup key of type 'MAC+FID'
*       (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
*       relevant when using 'Independent VLAN Learning' (CPSS_IVL_E)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       modePtr     - (pointer to) the upper bits mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbCrcHashUpperBitsModeGet
(
    IN  GT_U8           devNum,
    OUT  CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT     *modePtr
)
{
    GT_STATUS   rc;
    GT_U32  hwValue;/* value read from the HW */
    GT_U32  regAddr;/* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig1;

    /* Get <CRCHashUpperBitsMode> */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 2, &hwValue);

    switch(hwValue)
    {
        case 0:
            *modePtr = CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ALL_ZERO_E;
            break;
        case 1:
            *modePtr = CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_FID_E;
            break;
        case 2:
            *modePtr = CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_MAC_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbCrcHashUpperBitsModeGet
*
* DESCRIPTION:
*       Get mode how the CRC hash (CPSS_MAC_HASH_FUNC_CRC_E or
*       CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
*       will use 16 'most upper bits' for lookup key of type 'MAC+FID'
*       (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
*       relevant when using 'Independent VLAN Learning' (CPSS_IVL_E)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       modePtr     - (pointer to) the upper bits mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbCrcHashUpperBitsModeGet
(
    IN  GT_U8           devNum,
    OUT  CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT     *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbCrcHashUpperBitsModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChBrgFdbCrcHashUpperBitsModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbBankCounterValueGet
*
* DESCRIPTION:
*       Get the value of counter of the specific FDB table bank.
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       bankIndex   - the index of the bank
*                   (APPLICABLE RANGES: 0..15)
* OUTPUTS:
*       valuePtr    - (pointer to)the value of the counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbBankCounterValueGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          bankIndex,
    OUT GT_U32          *valuePtr
)
{
    GT_U32  regAddr;/* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    /*
        there is not 'SET' API because the CPU should not interfere with the PP
        managing the counters.

        we supply 'GET' API as 'read only' interface to the counters.
    */

    /*check bad param */
    PRV_CPSS_DXCH_FDB_BANK_INDEX_BAD_PARAM_CHECK_MAC(devNum,bankIndex);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->
                    FDB.FDBCore.FDBBankCntrs.FDBBankCntr[bankIndex];

    /* Get counter value */
    return prvCpssHwPpReadRegister(devNum, regAddr,valuePtr);
}

/*******************************************************************************
* cpssDxChBrgFdbBankCounterValueGet
*
* DESCRIPTION:
*       Get the value of counter of the specific FDB table bank.
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       bankIndex   - the index of the bank
*                   (APPLICABLE RANGES: 0..15)
* OUTPUTS:
*       valuePtr    - (pointer to)the value of the counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbBankCounterValueGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          bankIndex,
    OUT GT_U32          *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbBankCounterValueGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, bankIndex, valuePtr));

    rc = internal_cpssDxChBrgFdbBankCounterValueGet(devNum, bankIndex, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, bankIndex, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbBankCounterUpdate
*
* DESCRIPTION:
*       Update (increment/decrement) the counter of the specific bank
*       The function will fail (GT_BAD_STATE) if the PP is busy with previous
*       update.
*       the application can determine if PP is ready by calling
*       cpssDxChBrgFdbBankCounterUpdateStatusGet.
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
*       NOTE: this function is needed to be called after calling one of the next:
*           'write by index' - cpssDxChBrgFdbMacEntryWrite ,
*                              cpssDxChBrgFdbPortGroupMacEntryWrite
*           'invalidate by index' - cpssDxChBrgFdbMacEntryInvalidate ,
*                              cpssDxChBrgFdbPortGroupMacEntryInvalidate
*
*       The application logic should be:
*           if last action was 'write by index' then :
*               if the previous entry (in the index) was valid --
*                               do no call this function.
*               if the previous entry (in the index) was not valid --
*                               do 'increment'.
*           if last action was 'invalidate by index' then :
*               if the previous entry (in the index) was valid --
*                               do 'decrement'.
*               if the previous entry (in the index) was not valid --
*                               do no call this function.
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       bankIndex   - the index of the bank
*                   (APPLICABLE RANGES: 0..15)
*       incOrDec    - increment or decrement the counter by one.
*                   GT_TRUE - increment the counter
*                   GT_FALSE - decrement the counter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_OUT_OF_RANGE - bankIndex > 15
*       GT_BAD_STATE    - the PP is not ready to get a new update from CPU.
*                         (PP still busy with previous update)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbBankCounterUpdate
(
    IN GT_U8                        devNum,
    IN GT_U32                       bankIndex,
    IN GT_BOOL                      incOrDec
)
{
    GT_STATUS   rc;
    GT_U32  hwValue;/*value to write to HW */
    GT_U32  regAddr;/* register address */
    GT_U32  portGroupId ;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /*check out of range */
    PRV_CPSS_DXCH_FDB_BANK_INDEX_OUT_OF_RANGE_CHECK_MAC(devNum,bankIndex);

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBCntrsUpdate.FDBCntrsUpdateCtrl;

    /* the feature supported on devices with single FDB instance */
    portGroupId = 0;

    /* check that the device is ready */
    rc = prvCpssPortGroupBusyWait(devNum,portGroupId,
                regAddr,0,
                GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if (rc != GT_OK)
    {
        return rc;
    }

    /* < FDB Counters Update Trigger > */
    hwValue = 1;
    if(incOrDec == GT_TRUE)
    {
        /*<FDB Counters Inc >*/
        hwValue |= BIT_1;/* increment */
    }

    /*<FDB Counters Bank Update>*/
    hwValue |= bankIndex << 3;

    rc =  prvCpssHwPpSetRegField(devNum, regAddr, 0,7, hwValue);
#ifdef GM_USED
    if (rc != GT_OK)
    {
        return rc;
    }
    CPSS_TBD_BOOKMARK
    /* there is bug in the VERIFIER , that not clear the bit at end of operation !
       although operation done .
       so we will clear it for him !
       */
    rc =  prvCpssHwPpSetRegField(devNum, regAddr, 0,1,0);
#endif /*GM_USED*/
    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbBankCounterUpdate
*
* DESCRIPTION:
*       Update (increment/decrement) the counter of the specific bank
*       The function will fail (GT_BAD_STATE) if the PP is busy with previous
*       update.
*       the application can determine if PP is ready by calling
*       cpssDxChBrgFdbBankCounterUpdateStatusGet.
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
*       NOTE: this function is needed to be called after calling one of the next:
*           'write by index' - cpssDxChBrgFdbMacEntryWrite ,
*                              cpssDxChBrgFdbPortGroupMacEntryWrite
*           'invalidate by index' - cpssDxChBrgFdbMacEntryInvalidate ,
*                              cpssDxChBrgFdbPortGroupMacEntryInvalidate
*
*       The application logic should be:
*           if last action was 'write by index' then :
*               if the previous entry (in the index) was valid --
*                               do no call this function.
*               if the previous entry (in the index) was not valid --
*                               do 'increment'.
*           if last action was 'invalidate by index' then :
*               if the previous entry (in the index) was valid --
*                               do 'decrement'.
*               if the previous entry (in the index) was not valid --
*                               do no call this function.
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       bankIndex   - the index of the bank
*                   (APPLICABLE RANGES: 0..15)
*       incOrDec    - increment or decrement the counter by one.
*                   GT_TRUE - increment the counter
*                   GT_FALSE - decrement the counter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_OUT_OF_RANGE - bankIndex > 15
*       GT_BAD_STATE    - the PP is not ready to get a new update from CPU.
*                         (PP still busy with previous update)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbBankCounterUpdate
(
    IN GT_U8                        devNum,
    IN GT_U32                       bankIndex,
    IN GT_BOOL                      incOrDec
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbBankCounterUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, bankIndex, incOrDec));

    rc = internal_cpssDxChBrgFdbBankCounterUpdate(devNum, bankIndex, incOrDec);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, bankIndex, incOrDec));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbBankCounterUpdateStatusGet
*
* DESCRIPTION:
*       Get indication if PP finished processing last update of counter of a
*       specific bank. (cpssDxChBrgFdbBankCounterUpdate)
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       isFinishedPtr - (pointer to) indication that the PP finished processing
*                   the last counter update.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbBankCounterUpdateStatusGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *isFinishedPtr
)
{
    GT_STATUS   rc;
    GT_U32  regAddr;/* register address */
    GT_U32  portGroupId ;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(isFinishedPtr);

    regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBCntrsUpdate.FDBCntrsUpdateCtrl;

    /* the feature supported on devices with single FDB instance */
    portGroupId = 0;

    /* check that the device is ready */
    rc = prvCpssPortGroupBusyWait(devNum,portGroupId,
                regAddr,0,
                GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if(rc == GT_BAD_STATE)
    {
        rc = GT_OK;
        /* the device has not finished last update */
        *isFinishedPtr = GT_FALSE;
    }
    else
    {
        *isFinishedPtr = GT_TRUE;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbBankCounterUpdateStatusGet
*
* DESCRIPTION:
*       Get indication if PP finished processing last update of counter of a
*       specific bank. (cpssDxChBrgFdbBankCounterUpdate)
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       isFinishedPtr - (pointer to) indication that the PP finished processing
*                   the last counter update.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbBankCounterUpdateStatusGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *isFinishedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbBankCounterUpdateStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, isFinishedPtr));

    rc = internal_cpssDxChBrgFdbBankCounterUpdateStatusGet(devNum, isFinishedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, isFinishedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbMacEntryMove
*
* DESCRIPTION:
*       The function uses the NA message to send to the PP request to move 'valid'
*       entry from <origFdbIndex> in the FDB table to 'new index'.
*       The new index is chosen by the PP (according to the key of the 'Original entry')
*       in similar way to entry that not exists in the FDB and need to be inserted,
*       but with single difference that it can't use the <origFdbIndex>.
*       (AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*       The PP operation status can be retrieved by cpssDxChBrgFdbFromCpuAuMsgStatusGet
*       NOTE: this operation in the PP may fail on one of:
*           1. Original entry is not valid.
*           2. Original entry type is not supported.(IPv6 data/key)
*           3. No free destinations.
*
*
* APPLICABLE DEVICES:
*        Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum          - device number
*       origFdbIndex    - index of the entry that need to be moved to new index
*                       (as will be chosen by the device)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number
*       GT_OUT_OF_RANGE          - origFdbIndex out of FDB table range
*       GT_HW_ERROR              - on hardware error
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
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6_MC, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbMacEntryMove
(
    IN GT_U8       devNum,
    IN GT_U32      origFdbIndex
)
{
    GT_U32 hwData[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]; /* words of Au hw entry */
    GT_STATUS  rc;                                  /* return status        */
    GT_U32  portGroupId ;/*the port group Id - support multi-port-groups device */
    GT_PORT_GROUPS_BMP          portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* check index to the FDB */
    PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,origFdbIndex);

    hwData[0] = origFdbIndex;/* this is the only INPUT for the message */

    /* build entry hw format */
    rc = prvDxChFdbToAuConvert(devNum,
                               NULL,/* the key   not needed */
                               NULL,/* the entry not needed */
                               CPSS_NA_MOVED_E,
                               GT_FALSE,
                               &hwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* write data to Hw */
    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* write data to Hw */
        rc = prvDxChFdbSendAuMessage(devNum,portGroupId,&hwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryMove
*
* DESCRIPTION:
*       The function uses the NA message to send to the PP request to move 'valid'
*       entry from <origFdbIndex> in the FDB table to 'new index'.
*       The new index is chosen by the PP (according to the key of the 'Original entry')
*       in similar way to entry that not exists in the FDB and need to be inserted,
*       but with single difference that it can't use the <origFdbIndex>.
*       (AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*       The PP operation status can be retrieved by cpssDxChBrgFdbFromCpuAuMsgStatusGet
*       NOTE: this operation in the PP may fail on one of:
*           1. Original entry is not valid.
*           2. Original entry type is not supported.(IPv6 data/key)
*           3. No free destinations.
*
*
* APPLICABLE DEVICES:
*        Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum          - device number
*       origFdbIndex    - index of the entry that need to be moved to new index
*                       (as will be chosen by the device)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number
*       GT_OUT_OF_RANGE          - origFdbIndex out of FDB table range
*       GT_HW_ERROR              - on hardware error
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
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6_MC, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryMove
(
    IN GT_U8       devNum,
    IN GT_U32      origFdbIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbMacEntryMove);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, origFdbIndex));

    rc = internal_cpssDxChBrgFdbMacEntryMove(devNum, origFdbIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, origFdbIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
