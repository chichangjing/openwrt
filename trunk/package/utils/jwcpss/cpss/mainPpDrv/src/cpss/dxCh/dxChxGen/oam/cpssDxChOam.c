/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChOam.c
*
* DESCRIPTION:
*       CPSS DxCh OAM Engine API.
*       The OAM Engine is responsible for:
*
*           - Loss Measurement (LM) function. Allows a Maintenance End Point (MEP)
*             to compute the packet loss rate in a path to/from a peer MEP.
*           - Delay Measurement (DM) function. Enables a MEP to measure
*             the packet delay and delay variation between itself and a peer MEP.
*           - Generic Keepalive Engine.
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/oam/private/prvCpssDxChOamLog.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/policer/cpssGenPolicerTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/oam/cpssDxChOam.h>
#include <cpssCommon/private/prvCpssMath.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>

/* The size of OAM entry in words */
#define OAM_ENTRY_WORDS_CNS               4

/* check OAM entry index , when used as 'key' we return GT_BAD_PARAM on error */
/* note : index is in the range: 0..maxNum-1 */
#define PRV_CPSS_DXCH_OAM_NUM_CHECK_MAC(_devNum,_stage,_index)              \
    if((_index) >= PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.parametericTables.numEntriesOam)     \
    {                                                                            \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                     \
    }


/* Check OAM stage number */
#define PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(_devNum, _stage)                      \
switch ((_stage))                                                               \
{                                                                               \
    case CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E:                                    \
    case CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E :                                    \
        break;                                                                  \
    default:                                                                    \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                    \
}                                                                               \

/*******************************************************************************
* prvCpssDxChOamEntryToHwConvert
*
* DESCRIPTION:
*      Convert OAM entry to HW format
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       entryPtr          - pointer to OAM entry.
*
* OUTPUTS:
*       hwDataPtr         - pointer to the HW data.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_OUT_OF_RANGE   - one of the OAM entry fields are out of range.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChOamEntryToHwConvert
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_ENTRY_STC             *entryPtr,
    OUT GT_U32                              *hwDataPtr
)
{
    GT_U32 srcInterfaceCheckMode;   /* Source interface check mode (match/not matched) */
    GT_U32 lmCountingMode;          /* Determines which packets are counted by the LM counters */
    GT_U32 srcIsTrunk;              /* Source interface (trunk/port) */
    GT_U32 localDevSrcEPortTrunk;   /* Local device trunk/port of incoming packet */

    cpssOsMemSet(hwDataPtr, 0, OAM_ENTRY_WORDS_CNS * sizeof(GT_U32));

    if (entryPtr->megLevel >= BIT_3)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->ageState >= BIT_4)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->agingPeriodIndex >= BIT_3)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->agingThreshold >= BIT_4)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->flowHash >= BIT_12)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->oamPtpOffsetIndex >= BIT_7)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->packetCommandProfile >= BIT_3)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->cpuCodeOffset >= BIT_2)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    switch(entryPtr->sourceInterface.type)
    {
        case CPSS_INTERFACE_PORT_E:
            localDevSrcEPortTrunk = entryPtr->sourceInterface.devPort.portNum;
            /* check range of local device source port */
            if (localDevSrcEPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            srcIsTrunk = 0;
            break;
        case CPSS_INTERFACE_TRUNK_E:
            localDevSrcEPortTrunk = (GT_U32)entryPtr->sourceInterface.trunkId;
            /* check range of local device source trunk */
            if (localDevSrcEPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            srcIsTrunk = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(entryPtr->sourceInterfaceCheckMode)
    {
        case CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_NO_MATCH_E:
            /* No match */
            srcInterfaceCheckMode = 0;
            break;
        case CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E:
            /* Match */
            srcInterfaceCheckMode = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(entryPtr->lmCountingMode)
    {
        case CPSS_DXCH_OAM_LM_COUNTING_MODE_RETAIN_E:
            lmCountingMode = 0;
            break;
        case CPSS_DXCH_OAM_LM_COUNTING_MODE_DISABLE_E:
            lmCountingMode = 1;
            break;
        case CPSS_DXCH_OAM_LM_COUNTING_MODE_ENABLE_E:
            lmCountingMode = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (entryPtr->excessKeepalivePeriodCounter >= BIT_4)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->excessKeepalivePeriodThreshold >= BIT_4)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->excessKeepaliveMessageCounter >= BIT_3)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->excessKeepaliveMessageThreshold >= BIT_3)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->rdiStatus >= BIT_1)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (entryPtr->keepaliveTxPeriod >= BIT_3)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr,  0,  1,
                               BOOL2BIT_MAC(entryPtr->opcodeParsingEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr,  1,  1,
                               BOOL2BIT_MAC(entryPtr->lmCounterCaptureEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr,  2,  1,
                               BOOL2BIT_MAC(entryPtr->dualEndedLmEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr,  3,  2, lmCountingMode);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr,  5,  1,
                               BOOL2BIT_MAC(entryPtr->megLevelCheckEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr,  6,  3, entryPtr->megLevel);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr,  9,  1,
                               BOOL2BIT_MAC(entryPtr->keepaliveAgingEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 10,  4, entryPtr->ageState);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 14,  3, entryPtr->agingPeriodIndex);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 17,  4, entryPtr->agingThreshold);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 21,  1,
                               BOOL2BIT_MAC(entryPtr->hashVerifyEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 22,  1,
                               BOOL2BIT_MAC(entryPtr->lockHashValueEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 23, 12, entryPtr->flowHash);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 35,  7, entryPtr->oamPtpOffsetIndex);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 42,  1,
                               BOOL2BIT_MAC(entryPtr->timestampEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 43,  3, entryPtr->packetCommandProfile);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 46,  2, entryPtr->cpuCodeOffset);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 48,  1,
                               BOOL2BIT_MAC(entryPtr->sourceInterfaceCheckEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 49,  1, srcIsTrunk);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 50,  1, srcInterfaceCheckMode);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 51, 17, localDevSrcEPortTrunk);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 68,  1,
                               BOOL2BIT_MAC(entryPtr->protectionLocUpdateEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 69,  1,
                               BOOL2BIT_MAC(entryPtr->excessKeepaliveDetectionEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 70,  4, entryPtr->excessKeepalivePeriodCounter);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 74,  4, entryPtr->excessKeepalivePeriodThreshold);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 78,  3, entryPtr->excessKeepaliveMessageCounter);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 81,  3, entryPtr->excessKeepaliveMessageThreshold);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 84,  1,
                               BOOL2BIT_MAC(entryPtr->rdiCheckEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 85,  1, entryPtr->rdiStatus);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 86,  1,
                               BOOL2BIT_MAC(entryPtr->periodCheckEnable));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr, 87,  3, entryPtr->keepaliveTxPeriod);


    if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
    {
        /* <LOC Detection Enable>*/ CPSS_TBD_BOOKMARK_BOBCAT2_BOBK
        /* until explicit field from the application ... give default to allow aging */
        U32_SET_FIELD_IN_ENTRY_MAC(hwDataPtr,  93,  1, 1);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChOamEntryToSwConvert
*
* DESCRIPTION:
*      Convert HW OAM entry to SW format
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       hwDataPtr           - pointer to the HW data.
*
* OUTPUTS:
*       entryPtr            - pointer to OAM entry.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChOamEntryToSwConvert
(
    IN  GT_U32                              *hwDataPtr,
    OUT CPSS_DXCH_OAM_ENTRY_STC             *entryPtr
)
{
    GT_U32 regValue;                /* Register value */
    GT_U32 srcInterfaceCheckMode;   /* Source interface check mode (match/not matched) */
    GT_U32 srcIsTrunk;              /* Source interface (trunk/port) */
    GT_U32 localDevSrcEPortTrunk;   /* Local device trunk/port of incoming packet */

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr,  0,  1, regValue);
    entryPtr->opcodeParsingEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr,  1,  1, regValue);
    entryPtr->lmCounterCaptureEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr,  2,  1, regValue);
    entryPtr->dualEndedLmEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr,  3,  2, regValue);
    switch (regValue)
    {
        case 0:
            entryPtr->lmCountingMode = CPSS_DXCH_OAM_LM_COUNTING_MODE_RETAIN_E;
            break;
        case 1:
            entryPtr->lmCountingMode = CPSS_DXCH_OAM_LM_COUNTING_MODE_DISABLE_E;
            break;
        case 2:
            entryPtr->lmCountingMode = CPSS_DXCH_OAM_LM_COUNTING_MODE_ENABLE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr,  5,  1, regValue);
    entryPtr->megLevelCheckEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr,  6,  3, entryPtr->megLevel);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr,  9,  1, regValue);
    entryPtr->keepaliveAgingEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 10,  4, entryPtr->ageState);
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 14,  3, entryPtr->agingPeriodIndex);
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 17,  4, entryPtr->agingThreshold);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 21,  1, regValue);
    entryPtr->hashVerifyEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 22,  1, regValue);
    entryPtr->lockHashValueEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 23, 12, entryPtr->flowHash);
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 35,  7, entryPtr->oamPtpOffsetIndex);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 42,  1, regValue);
    entryPtr->timestampEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 43,  3, entryPtr->packetCommandProfile);
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 46,  2, entryPtr->cpuCodeOffset);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 48,  1, regValue);
    entryPtr->sourceInterfaceCheckEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 49,  1, srcIsTrunk);
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 50,  1, srcInterfaceCheckMode);
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 51, 17, localDevSrcEPortTrunk);

    if(srcIsTrunk)
    {
        /* Trunk */
        entryPtr->sourceInterface.trunkId = (GT_TRUNK_ID)localDevSrcEPortTrunk;
        entryPtr->sourceInterface.type = CPSS_INTERFACE_TRUNK_E;
    }
    else
    {
        /* Port */
        entryPtr->sourceInterface.devPort.portNum = localDevSrcEPortTrunk;
        entryPtr->sourceInterface.type = CPSS_INTERFACE_PORT_E;
    }

    if (srcInterfaceCheckMode == 0)
    {
        /* No match */
        entryPtr->sourceInterfaceCheckMode =
            CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_NO_MATCH_E;
    }
    else
    {
        /* Match */
        entryPtr->sourceInterfaceCheckMode =
            CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E;

    }

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 68,  1, regValue);
    entryPtr->protectionLocUpdateEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 69,  1, regValue);
    entryPtr->excessKeepaliveDetectionEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 70,  4,
                               entryPtr->excessKeepalivePeriodCounter);
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 74,  4,
                               entryPtr->excessKeepalivePeriodThreshold);
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 78,  3,
                               entryPtr->excessKeepaliveMessageCounter);
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 81,  3,
                               entryPtr->excessKeepaliveMessageThreshold);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 84,  1, regValue);
    entryPtr->rdiCheckEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 85,  1, entryPtr->rdiStatus);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 86,  1, regValue);
    entryPtr->periodCheckEnable = BIT2BOOL_MAC(regValue);

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, 87,  3, entryPtr->keepaliveTxPeriod);

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChOamExceptionTypeToHwOffsetsGet
*
* DESCRIPTION:
*      Convert exception type to the packet command and CPU bits offset
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       exceptionType               - exception type
*
* OUTPUTS:
*       exceptionConfigRegAddrPtr   - pointer to exception configuration register address
*       exceptionConfigSumRegAddrPtr
*                                   - pointer to exception configuration summary register address
*       exceptionConfigCommandOffsetPtr
*                                   - pointer to exception configuration packet command offset
*
*       exceptionConfigCpuCodeOffsetPtr
*                                   - pointer to exception configuration CPU code offset
*       exceptionConfigSumOffsetPtr - pointer to exception configuration summary offset
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChOamExceptionTypeToHwOffsetsGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT      stage,
    IN  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT  exceptionType,
    OUT GT_U32                            *exceptionConfigRegAddrPtr,
    OUT GT_U32                            *exceptionConfigSumRegAddrPtr,
    OUT GT_U32                            *exceptionConfigCommandOffsetPtr,
    OUT GT_U32                            *exceptionConfigCpuCodeOffsetPtr,
    OUT GT_U32                            *exceptionConfigSumOffsetPtr
)
{
    GT_U32 sumOffset;           /* Summary bit offset */
    GT_U32 pktCommandOffset;    /* Packet command offset */
    GT_U32 cpuOffset;           /* CPU code offset  */
    GT_U32 regAddr;             /* Register address (Packet command and CPU code) */
    GT_U32 regAddrSum;          /* Register address (Summary bit) */

    regAddrSum = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMExceptionConfigs;

    /* Convert exception type to the bits offset */
    switch(exceptionType)
    {
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_RDI_STATUS_CHANGED_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMExceptionConfigs1;
            sumOffset = 25;
            pktCommandOffset = 11;
            cpuOffset = 14;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_AGING_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMInvalidKeepaliveExceptionConfigs;
            sumOffset = 26;
            pktCommandOffset = 0;
            cpuOffset = 4;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_EXCESS_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMInvalidKeepaliveExceptionConfigs;
            sumOffset = 27;
            pktCommandOffset = 0;
            cpuOffset = 4;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_INVALID_HASH_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMInvalidKeepaliveExceptionConfigs;
            sumOffset = 28;
            pktCommandOffset = 0;
            cpuOffset = 4;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_MEG_LEVEL_E:
            regAddr = regAddrSum;
            sumOffset = 29;
            pktCommandOffset = 0;
            cpuOffset = 4;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_SOURCE_INTERFACE_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMExceptionConfigs1;
            sumOffset = 30;
            pktCommandOffset = 0;
            cpuOffset = 3;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_TX_PERIOD_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMExceptionConfigs2;
            sumOffset = 31;
            pktCommandOffset = 0;
            cpuOffset = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Assign exception register addresses and bit offsets */
    *exceptionConfigRegAddrPtr = regAddr;
    *exceptionConfigSumRegAddrPtr = regAddrSum;
    *exceptionConfigCommandOffsetPtr = pktCommandOffset;
    *exceptionConfigCpuCodeOffsetPtr = cpuOffset;
    *exceptionConfigSumOffsetPtr = sumOffset;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChOamOpcodeToHwOffsetsGet
*
* DESCRIPTION:
*      Convert opcode type to the register address
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       opcodeType                  - exception type
*       opcodeIndex                 - OAM opcode index: (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       opcodeRegAddrPtr            - pointer to register address
*       opcodeOffsetPtr             - pointer to opcode offset
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChOamOpcodeToHwOffsetsGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage,
    IN  CPSS_DXCH_OAM_OPCODE_TYPE_ENT           opcodeType,
    IN  GT_U32                                  opcodeIndex,
    OUT GT_U32                                  *opcodeRegAddrPtr,
    OUT GT_U32                                  *opcodeOffsetPtr
)
{
    GT_U32      regAddr;            /* Register address (OAM Opcode) */

    /* Convert opcode index to opcode offset bit */
    *opcodeOffsetPtr = opcodeIndex * 8;

    /* Convert opcode type to the bit offset */
    switch(opcodeType)
    {
        case CPSS_DXCH_OAM_OPCODE_TYPE_LM_COUNTED_E:
            if(opcodeIndex >= 3)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMLMCountedOpcodes[0];
            break;
        case CPSS_DXCH_OAM_OPCODE_TYPE_LM_SINGLE_ENDED_E:
            if(opcodeIndex > 3)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMLossMeasurementOpcodes[0];
            break;
        case CPSS_DXCH_OAM_OPCODE_TYPE_LM_DUAL_ENDED_E:
            if(opcodeIndex > 0)   /* Dual ended LM opcode valid index: 0 */
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).dualEndedLM;
            break;
        case CPSS_DXCH_OAM_OPCODE_TYPE_DM_E:
            if(opcodeIndex > 3)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMDelayMeasurementOpcodes[0];
            break;
        case CPSS_DXCH_OAM_OPCODE_TYPE_KEEPALIVE_E:
            if(opcodeIndex > 3)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMKeepAliveOpcodes[0];
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *opcodeRegAddrPtr = regAddr;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChOamEntrySet
*
* DESCRIPTION:
*      Set OAM Entry configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       entryIndex              - OAM entry index.
*                                 Range: see datasheet for specific device.
*       entryPtr                - pointer to OAM entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, entryIndex or entry parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_OUT_OF_RANGE          - one of the OAM entry fields are out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamEntrySet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_OAM_ENTRY_STC            *entryPtr
)
{
    return cpssDxChOamPortGroupEntrySet(devNum,
                                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        stage,
                                        entryIndex,
                                        entryPtr);
}

/*******************************************************************************
* cpssDxChOamEntrySet
*
* DESCRIPTION:
*      Set OAM Entry configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       entryIndex              - OAM entry index.
*                                 Range: see datasheet for specific device.
*       entryPtr                - pointer to OAM entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, entryIndex or entry parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_OUT_OF_RANGE          - one of the OAM entry fields are out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChOamEntrySet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_OAM_ENTRY_STC            *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, entryIndex, entryPtr));

    rc = internal_cpssDxChOamEntrySet(devNum, stage, entryIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, entryIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamPortGroupEntrySet
*
* DESCRIPTION:
*      Set OAM Entry configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       portGroupsBmp           - bitmap of Port Groups.
*                                 Bitmap must be set with at least one bit representing
*                                 valid port group(s). If a bit of non valid port group
*                                 is set then function returns GT_BAD_PARAM.
*                                 value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage                   - OAM stage type.
*       entryIndex              - OAM entry index.
*                                 Range: see datasheet for specific device.
*       entryPtr                - pointer to OAM entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, entryIndex or entry parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_OUT_OF_RANGE          - one of the OAM entry fields are out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamPortGroupEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_OAM_ENTRY_STC            *entryPtr
)
{
    GT_STATUS               rc;                /* Return code  */
    GT_U32                  hwDataArr[OAM_ENTRY_WORDS_CNS]; /* Value to write to HW */
    GT_U32                  portGroupId;       /* Port group id */
    PRV_CPSS_DXCH_TABLE_ENT tableType;         /* Table type */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_DXCH_OAM_NUM_CHECK_MAC(devNum, stage, entryIndex);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChOamEntryToHwConvert(devNum, entryPtr, hwDataArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_E;
    }
    else
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_E;
    }

    /* loop on all port groups to set the OAM entry */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDxChPortGroupWriteTableEntry(devNum, portGroupId,
                                                 tableType, entryIndex,
                                                 &hwDataArr[0]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum, portGroupsBmp,
                                                    portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChOamPortGroupEntrySet
*
* DESCRIPTION:
*      Set OAM Entry configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       portGroupsBmp           - bitmap of Port Groups.
*                                 Bitmap must be set with at least one bit representing
*                                 valid port group(s). If a bit of non valid port group
*                                 is set then function returns GT_BAD_PARAM.
*                                 value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage                   - OAM stage type.
*       entryIndex              - OAM entry index.
*                                 Range: see datasheet for specific device.
*       entryPtr                - pointer to OAM entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, entryIndex or entry parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_OUT_OF_RANGE          - one of the OAM entry fields are out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChOamPortGroupEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_OAM_ENTRY_STC            *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamPortGroupEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, stage, entryIndex, entryPtr));

    rc = internal_cpssDxChOamPortGroupEntrySet(devNum, portGroupsBmp, stage, entryIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, stage, entryIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamEntryGet
*
* DESCRIPTION:
*      Get OAM Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       entryIndex               - OAM entry index.
*                                  Range: see datasheet for specific device.
*
* OUTPUTS:
*       entryPtr                 - pointer to OAM entry.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage or entryIndex.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamEntryGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_OAM_ENTRY_STC            *entryPtr
)
{
    return cpssDxChOamPortGroupEntryGet(devNum,
                                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        stage,
                                        entryIndex,
                                        entryPtr);

}

/*******************************************************************************
* cpssDxChOamEntryGet
*
* DESCRIPTION:
*      Get OAM Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       entryIndex               - OAM entry index.
*                                  Range: see datasheet for specific device.
*
* OUTPUTS:
*       entryPtr                 - pointer to OAM entry.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage or entryIndex.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamEntryGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_OAM_ENTRY_STC            *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, entryIndex, entryPtr));

    rc = internal_cpssDxChOamEntryGet(devNum, stage, entryIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, entryIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamPortGroupEntryGet
*
* DESCRIPTION:
*      Get OAM Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       portGroupsBmp           - bitmap of Port Groups.
*                                 Bitmap must be set with at least one bit representing
*                                 valid port group(s). If a bit of non valid port group
*                                 is set then function returns GT_BAD_PARAM.
*                                 value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage                   - OAM stage type.
*       entryIndex              - OAM entry index.
*                                 Range: see datasheet for specific device.
*
* OUTPUTS:
*       entryPtr                - pointer to OAM entry.
*
* RETURNS:
*       GT_OK                   - on success.
*       GT_FAIL                 - on error.
*       GT_HW_ERROR             - on hardware error.
*       GT_BAD_PARAM            - wrong devNum, stage or entryIndex.
*       GT_BAD_PTR              - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamPortGroupEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_OAM_ENTRY_STC            *entryPtr
)
{
    GT_U32                  hwDataArr[OAM_ENTRY_WORDS_CNS]; /* Value to read from HW */
    GT_STATUS               rc;                             /* Return code  */
    PRV_CPSS_DXCH_TABLE_ENT tableType;                      /* Table type */
    GT_U32              portGroupId;                        /* Port group id */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_DXCH_OAM_NUM_CHECK_MAC(devNum, stage, entryIndex);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* Get the first active port group */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum,
                                                        portGroupsBmp,
                                                        portGroupId);

    if (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_E;
    }
    else
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_E;
    }

    rc = prvCpssDxChPortGroupReadTableEntry(devNum, portGroupId, tableType,
                                            entryIndex, &hwDataArr[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDxChOamEntryToSwConvert(hwDataArr, entryPtr);
}

/*******************************************************************************
* cpssDxChOamPortGroupEntryGet
*
* DESCRIPTION:
*      Get OAM Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       portGroupsBmp           - bitmap of Port Groups.
*                                 Bitmap must be set with at least one bit representing
*                                 valid port group(s). If a bit of non valid port group
*                                 is set then function returns GT_BAD_PARAM.
*                                 value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage                   - OAM stage type.
*       entryIndex              - OAM entry index.
*                                 Range: see datasheet for specific device.
*
* OUTPUTS:
*       entryPtr                - pointer to OAM entry.
*
* RETURNS:
*       GT_OK                   - on success.
*       GT_FAIL                 - on error.
*       GT_HW_ERROR             - on hardware error.
*       GT_BAD_PARAM            - wrong devNum, stage or entryIndex.
*       GT_BAD_PTR              - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamPortGroupEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_OAM_ENTRY_STC            *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamPortGroupEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, stage, entryIndex, entryPtr));

    rc = internal_cpssDxChOamPortGroupEntryGet(devNum, portGroupsBmp, stage, entryIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, stage, entryIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamEnableSet
*
* DESCRIPTION:
*      Enable OAM processing
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       enable                   - enable/disable OAM processing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_BOOL                             enable
)
{

    GT_U32 regAddr;                 /* Register address */
    GT_U32 value;                   /* Register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMGlobalCtrl;

    value = BOOL2BIT_MAC(enable);

    /* Set OAM enable */
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, value);
}

/*******************************************************************************
* cpssDxChOamEnableSet
*
* DESCRIPTION:
*      Enable OAM processing
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       enable                   - enable/disable OAM processing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, enable));

    rc = internal_cpssDxChOamEnableSet(devNum, stage, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamEnableGet
*
* DESCRIPTION:
*      Get OAM processing status
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*
* OUTPUTS:
*       enablePtr                - pointer to enable/disable OAM processing.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamEnableGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    OUT  GT_BOOL                            *enablePtr
)
{
    GT_U32      regAddr;                 /* Register address */
    GT_U32      value;                   /* Register value */
    GT_STATUS   rc;                      /* Return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMGlobalCtrl;

    /* Get OAM processing status */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChOamEnableGet
*
* DESCRIPTION:
*      Get OAM processing status
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*
* OUTPUTS:
*       enablePtr                - pointer to enable/disable OAM processing.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamEnableGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    OUT  GT_BOOL                            *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, enablePtr));

    rc = internal_cpssDxChOamEnableGet(devNum, stage, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamTableBaseFlowIdSet
*
* DESCRIPTION:
*      Set the base first Flow ID index used for OAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       baseFlowId              - base first Flow ID index used for OAM.
*                                (APPLICABLE RANGES: 0..65534)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_OUT_OF_RANGE          - base FlowId is out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       OAM indices are calculated by <Flow ID>-<OAM Table Base Flow ID>.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamTableBaseFlowIdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              baseFlowId
)
{
    GT_U32 regAddr;                                 /* Register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    if(baseFlowId >= BIT_16)  /*16 bits in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMGlobalCtrl;

    return prvCpssHwPpSetRegField(devNum, regAddr, 16, 16, baseFlowId);
}

/*******************************************************************************
* cpssDxChOamTableBaseFlowIdSet
*
* DESCRIPTION:
*      Set the base first Flow ID index used for OAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       baseFlowId              - base first Flow ID index used for OAM.
*                                (APPLICABLE RANGES: 0..65534)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_OUT_OF_RANGE          - base FlowId is out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       OAM indices are calculated by <Flow ID>-<OAM Table Base Flow ID>.
*
*******************************************************************************/
GT_STATUS cpssDxChOamTableBaseFlowIdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              baseFlowId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamTableBaseFlowIdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, baseFlowId));

    rc = internal_cpssDxChOamTableBaseFlowIdSet(devNum, stage, baseFlowId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, baseFlowId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamTableBaseFlowIdGet
*
* DESCRIPTION:
*      Get the base first Flow ID index used for OAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       stage                   - OAM stage type
*
* OUTPUTS:
*       baseFlowIdPtr           - pointer to first Flow ID index used for OAM.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       OAM indices are calculated by <Flow ID>-<OAM Table Base Flow ID>.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamTableBaseFlowIdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    OUT  GT_U32                            *baseFlowIdPtr
)
{
    GT_U32 regAddr;                                 /* Register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(baseFlowIdPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMGlobalCtrl;

    return prvCpssHwPpGetRegField(devNum, regAddr, 16, 16, baseFlowIdPtr);
}

/*******************************************************************************
* cpssDxChOamTableBaseFlowIdGet
*
* DESCRIPTION:
*      Get the base first Flow ID index used for OAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       stage                   - OAM stage type
*
* OUTPUTS:
*       baseFlowIdPtr           - pointer to first Flow ID index used for OAM.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       OAM indices are calculated by <Flow ID>-<OAM Table Base Flow ID>.
*
*******************************************************************************/
GT_STATUS cpssDxChOamTableBaseFlowIdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    OUT  GT_U32                            *baseFlowIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamTableBaseFlowIdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, baseFlowIdPtr));

    rc = internal_cpssDxChOamTableBaseFlowIdGet(devNum, stage, baseFlowIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, baseFlowIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamOpcodeProfileDedicatedMcProfileEnableSet
*
* DESCRIPTION:
*      Set (per stage) enable/disable MC packets have a dedicated packet command
*      profile (MC_profile = 1 + 'packet command profile' (from OAM table) )
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number.
*       stage     - OAM stage type.
*       enable    - GT_FALSE - Disable: MC packets do not have a dedicated packet command profile.
*                   GT_TRUE  - Enable : MC packets have a dedicated packet command profile, which
*                              is equal to the packet command profile in the OAM table + 1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamOpcodeProfileDedicatedMcProfileEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_BOOL                             enable
)
{
    GT_U32      regAddr;   /* Register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* OAM Global Control 1 register */
    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMGlobalCtrl1;

    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChOamOpcodeProfileDedicatedMcProfileEnableSet
*
* DESCRIPTION:
*      Set (per stage) enable/disable MC packets have a dedicated packet command
*      profile (MC_profile = 1 + 'packet command profile' (from OAM table) )
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number.
*       stage     - OAM stage type.
*       enable    - GT_FALSE - Disable: MC packets do not have a dedicated packet command profile.
*                   GT_TRUE  - Enable : MC packets have a dedicated packet command profile, which
*                              is equal to the packet command profile in the OAM table + 1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChOamOpcodeProfileDedicatedMcProfileEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamOpcodeProfileDedicatedMcProfileEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, enable));

    rc = internal_cpssDxChOamOpcodeProfileDedicatedMcProfileEnableSet(devNum, stage, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamOpcodeProfileDedicatedMcProfileEnableGet
*
* DESCRIPTION:
*      Get (per stage) enable/disable MC packets have a dedicated packet command
*      profile (MC_profile = 1 + 'packet command profile' (from OAM table) )
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number.
*       stage     - OAM stage type.
*
* OUTPUTS:
*       enablePtr - (pointer to) enabled/disabled value.
*                   GT_FALSE - Disable: MC packets do not have a dedicated packet command profile.
*                   GT_TRUE  - Enable : MC packets have a dedicated packet command profile, which
*                              is equal to the packet command profile in the OAM table + 1.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamOpcodeProfileDedicatedMcProfileEnableGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;   /* Register address */
    GT_U32      hwValue;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* OAM Global Control 1 register */
    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMGlobalCtrl1;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &hwValue);

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChOamOpcodeProfileDedicatedMcProfileEnableGet
*
* DESCRIPTION:
*      Get (per stage) enable/disable MC packets have a dedicated packet command
*      profile (MC_profile = 1 + 'packet command profile' (from OAM table) )
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number.
*       stage     - OAM stage type.
*
* OUTPUTS:
*       enablePtr - (pointer to) enabled/disabled value.
*                   GT_FALSE - Disable: MC packets do not have a dedicated packet command profile.
*                   GT_TRUE  - Enable : MC packets have a dedicated packet command profile, which
*                              is equal to the packet command profile in the OAM table + 1.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChOamOpcodeProfileDedicatedMcProfileEnableGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamOpcodeProfileDedicatedMcProfileEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, enablePtr));

    rc = internal_cpssDxChOamOpcodeProfileDedicatedMcProfileEnableGet(devNum, stage, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamExceptionConfigSet
*
* DESCRIPTION:
*      Set OAM exception configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       exceptionType           - OAM exception type.
*       exceptionConfigPtr      - OAM exception configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, exceptionType or
*                                  CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC fields.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamExceptionConfigSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage,
    IN  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT        exceptionType,
    IN  CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC     *exceptionConfigPtr
)
{
    GT_U32      pktCommand;          /* OAM exception packet forwarding command (HW format) */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT cpuCode;
                                     /* OAM exception CPU code (HW format) */
    GT_U32      sumBit;              /* OAM exception summary bit (HW format)*/
    GT_U32      sumOffset;           /* Summary bit offset */
    GT_U32      pktCommandOffset;    /* Packet command offset */
    GT_U32      cpuOffset;           /* CPU code offset  */
    GT_U32      regAddr;             /* Register address (Packet command and CPU code) */
    GT_U32      regAddrSum;          /* Register address (Summary bit) */
    GT_STATUS   rc;                  /* Return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(exceptionConfigPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(pktCommand, exceptionConfigPtr->command);

    /* Translate exception CPU code to HW value. */
    /* Note that CPU code is relevant only for TRAP or MIRROR commands. */
    if ((exceptionConfigPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (exceptionConfigPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        rc = prvCpssDxChNetIfCpuToDsaCode(exceptionConfigPtr->cpuCode, &cpuCode);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        cpuCode = 0;
    }

    rc = prvCpssDxChOamExceptionTypeToHwOffsetsGet(devNum, stage, exceptionType,
                                                   &regAddr, &regAddrSum,
                                                   &pktCommandOffset,
                                                   &cpuOffset, &sumOffset);
    if (rc != GT_OK)
    {
        return rc;
    }

    sumBit = BOOL2BIT_MAC(exceptionConfigPtr->summaryBitEnable);
    /* OAM Exception Summary bit */
    rc = prvCpssHwPpSetRegField(devNum, regAddrSum, sumOffset, 1, sumBit);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* OAM Exception Packet command */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, pktCommandOffset, 3, pktCommand);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* OAM Exception CPU code */
    return prvCpssHwPpSetRegField(devNum, regAddr, cpuOffset, 8, cpuCode);
}

/*******************************************************************************
* cpssDxChOamExceptionConfigSet
*
* DESCRIPTION:
*      Set OAM exception configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       exceptionType           - OAM exception type.
*       exceptionConfigPtr      - OAM exception configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, exceptionType or
*                                  CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC fields.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChOamExceptionConfigSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage,
    IN  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT        exceptionType,
    IN  CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC     *exceptionConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamExceptionConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, exceptionType, exceptionConfigPtr));

    rc = internal_cpssDxChOamExceptionConfigSet(devNum, stage, exceptionType, exceptionConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, exceptionType, exceptionConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamExceptionConfigGet
*
* DESCRIPTION:
*      Get OAM exception configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       exceptionType            - OAM exception type.
*
* OUTPUTS:
*       exceptionConfigPtr       - pointer to OAM exception configuration.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage or exceptionType.
*       GT_BAD_STATE             - on bad value found in HW
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamExceptionConfigGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage,
    IN  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT        exceptionType,
    OUT CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC     *exceptionConfigPtr
)
{
    GT_U32    pktCommand;          /* OAM exception packet forwarding command (HW format) */
    GT_U32    cpuCode;             /* OAM exception CPU code (HW format) */
    GT_U32    sumBit;              /* OAM exception summary bit (HW format)*/
    GT_U32    sumOffset;           /* Summary bit offset */
    GT_U32    pktCommandOffset;    /* Packet command offset */
    GT_U32    cpuOffset;           /* CPU code offset  */
    GT_U32    regAddr;             /* Register address (Packet command and CPU code) */
    GT_U32    regAddrSum;          /* Register address (Summary bit) */
    GT_STATUS rc;                  /* Return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(exceptionConfigPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChOamExceptionTypeToHwOffsetsGet(devNum, stage, exceptionType,
                                                   &regAddr, &regAddrSum,
                                                   &pktCommandOffset,
                                                   &cpuOffset, &sumOffset);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* OAM Exception Summary bit */
    rc = prvCpssHwPpGetRegField(devNum, regAddrSum, sumOffset, 1, &sumBit);
    if(rc != GT_OK)
    {
        return rc;
    }

    exceptionConfigPtr->summaryBitEnable = BIT2BOOL_MAC(sumBit);

    /* OAM Exception Packet command */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, pktCommandOffset, 3, &pktCommand);
    if(rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(exceptionConfigPtr->command, pktCommand);

    /* OAM Exception CPU code */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, cpuOffset, 8, &cpuCode);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* Translate  HW value to exception CPU code. */
    /* Note that CPU code is relevant only for TRAP or MIRROR commands. */
    if ((exceptionConfigPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (exceptionConfigPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        rc = prvCpssDxChNetIfDsaToCpuCode(cpuCode,
                                          &exceptionConfigPtr->cpuCode);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        exceptionConfigPtr->cpuCode = 0;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChOamExceptionConfigGet
*
* DESCRIPTION:
*      Get OAM exception configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       exceptionType            - OAM exception type.
*
* OUTPUTS:
*       exceptionConfigPtr       - pointer to OAM exception configuration.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage or exceptionType.
*       GT_BAD_STATE             - on bad value found in HW
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamExceptionConfigGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage,
    IN  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT        exceptionType,
    OUT CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC     *exceptionConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamExceptionConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, exceptionType, exceptionConfigPtr));

    rc = internal_cpssDxChOamExceptionConfigGet(devNum, stage, exceptionType, exceptionConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, exceptionType, exceptionConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamExceptionCounterGet
*
* DESCRIPTION:
*      Get OAM exception counter value
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       exceptionType           - OAM exception type.
*
* OUTPUTS:
*       counterValuePtr         - pointer to OAM exception counter value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage or exceptionType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamExceptionCounterGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage,
    IN  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT        exceptionType,
    OUT GT_U32                                  *counterValuePtr
)
{
    GT_U32      regAddr;             /* Register address (Exception counters) */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(counterValuePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* Convert exception type to the bit offset */
    switch(exceptionType)
    {
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_MEG_LEVEL_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).MEGLevelExceptionCntr;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_SOURCE_INTERFACE_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).sourceInterfaceMismatchCntr;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_RDI_STATUS_CHANGED_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).RDIStatusChangeCntr;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_AGING_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).keepaliveAgingCntr;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_EXCESS_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).excessKeepaliveCntr;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_INVALID_HASH_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).invalidKeepaliveHashCntr;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_TX_PERIOD_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).txPeriodExceptionCntr;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssPortGroupsBmpCounterSummary(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                              regAddr, 0, 32, counterValuePtr, NULL);
}

/*******************************************************************************
* cpssDxChOamExceptionCounterGet
*
* DESCRIPTION:
*      Get OAM exception counter value
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       exceptionType           - OAM exception type.
*
* OUTPUTS:
*       counterValuePtr         - pointer to OAM exception counter value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage or exceptionType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChOamExceptionCounterGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage,
    IN  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT        exceptionType,
    OUT GT_U32                                  *counterValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamExceptionCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, exceptionType, counterValuePtr));

    rc = internal_cpssDxChOamExceptionCounterGet(devNum, stage, exceptionType, counterValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, exceptionType, counterValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamExceptionGroupStatusGet
*
* DESCRIPTION:
*      Get OAM exception group status
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       exceptionType            - OAM exception type.
*
* OUTPUTS:
*       groupStatusArr           - array of OAM exception groups status.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage or
*                                  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT value.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Each bit in group status notifies exception in one of 32 OAM table entries
*       according to formula: <<1024*n+32*m>> - <<1024*n+32*m+31>>.
*       Where n - word in group status (0 < n < CPSS_DXCH_OAM_GROUP_STATUS_SIZE_IN_WORDS_CNS),
*       m - bit in group status (0 < m < 32)
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamExceptionGroupStatusGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage,
    IN  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT        exceptionType,
    OUT GT_U32                                  groupStatusArr[CPSS_DXCH_OAM_GROUP_STATUS_SIZE_IN_WORDS_CNS]
)
{
    GT_U32      regAddr;             /* Register address (Exception counters) */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(groupStatusArr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* Convert exception type to the bit offset */
    switch(exceptionType)
    {
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_MEG_LEVEL_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).MEGLevelGroupStatus[0];
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_SOURCE_INTERFACE_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).sourceInterfaceGroupStatus[0];
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_RDI_STATUS_CHANGED_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).RDIStatusChangeExceptionGroupStatus[0];
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_AGING_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).agingEntryGroupStatus[0];
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_EXCESS_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).excessKeepaliveGroupStatus[0];
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_INVALID_HASH_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).invalidKeepaliveGroupStatus[0];
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_TX_PERIOD_E:
            regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).txPeriodExceptionGroupStatus[0];
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Read OAM exception group status */
    return prvCpssHwPpReadRam(devNum, regAddr,
                              CPSS_DXCH_OAM_GROUP_STATUS_SIZE_IN_WORDS_CNS,
                              &groupStatusArr[0]);
}

/*******************************************************************************
* cpssDxChOamExceptionGroupStatusGet
*
* DESCRIPTION:
*      Get OAM exception group status
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       exceptionType            - OAM exception type.
*
* OUTPUTS:
*       groupStatusArr           - array of OAM exception groups status.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage or
*                                  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT value.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Each bit in group status notifies exception in one of 32 OAM table entries
*       according to formula: <<1024*n+32*m>> - <<1024*n+32*m+31>>.
*       Where n - word in group status (0 < n < CPSS_DXCH_OAM_GROUP_STATUS_SIZE_IN_WORDS_CNS),
*       m - bit in group status (0 < m < 32)
*
*******************************************************************************/
GT_STATUS cpssDxChOamExceptionGroupStatusGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage,
    IN  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT        exceptionType,
    OUT GT_U32                                  groupStatusArr[CPSS_DXCH_OAM_GROUP_STATUS_SIZE_IN_WORDS_CNS]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamExceptionGroupStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, exceptionType, groupStatusArr));

    rc = internal_cpssDxChOamExceptionGroupStatusGet(devNum, stage, exceptionType, groupStatusArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, exceptionType, groupStatusArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamOpcodeSet
*
* DESCRIPTION:
*      Set OAM opcode value
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       opcodeType                  - OAM opcode type.
*       opcodeIndex                 - OAM opcode index: (APPLICABLE RANGES: 0..3)
*       opcodeValue                 - OAM opcode value: (APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage, opcodeType or opcodeIndex.
*       GT_OUT_OF_RANGE             - opcode value is out of range.
*       GT_BAD_PTR                  - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamOpcodeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT             stage,
    IN CPSS_DXCH_OAM_OPCODE_TYPE_ENT            opcodeType,
    IN GT_U32                                   opcodeIndex,
    IN GT_U32                                   opcodeValue
)
{
    GT_U32      regAddr;            /* Register address (OAM Opcode) */
    GT_U32      opcodeOffset;       /* Opcode bit offset */
    GT_STATUS   rc;                 /* Return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(opcodeValue >= BIT_8)  /* 8 bits in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChOamOpcodeToHwOffsetsGet(devNum, stage, opcodeType,
                                            opcodeIndex,
                                            &regAddr, &opcodeOffset);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Set OAM opcode */
    return prvCpssHwPpSetRegField(devNum, regAddr, opcodeOffset, 8, opcodeValue);
}

/*******************************************************************************
* cpssDxChOamOpcodeSet
*
* DESCRIPTION:
*      Set OAM opcode value
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       opcodeType                  - OAM opcode type.
*       opcodeIndex                 - OAM opcode index: (APPLICABLE RANGES: 0..3)
*       opcodeValue                 - OAM opcode value: (APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage, opcodeType or opcodeIndex.
*       GT_OUT_OF_RANGE             - opcode value is out of range.
*       GT_BAD_PTR                  - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamOpcodeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT             stage,
    IN CPSS_DXCH_OAM_OPCODE_TYPE_ENT            opcodeType,
    IN GT_U32                                   opcodeIndex,
    IN GT_U32                                   opcodeValue
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamOpcodeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, opcodeType, opcodeIndex, opcodeValue));

    rc = internal_cpssDxChOamOpcodeSet(devNum, stage, opcodeType, opcodeIndex, opcodeValue);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, opcodeType, opcodeIndex, opcodeValue));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamOpcodeGet
*
* DESCRIPTION:
*      Get OAM opcode value
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       opcodeType                  - OAM opcode type.
*       opcodeIndex                 - OAM opcode index: (APPLICABLE RANGES: 0..3).
*
* OUTPUTS:
*       opcodeValuePtr              - pointer to OAM opcode value
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage, opcodeType or opcodeIndex.
*       GT_BAD_PTR                  - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamOpcodeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage,
    IN  CPSS_DXCH_OAM_OPCODE_TYPE_ENT           opcodeType,
    IN  GT_U32                                  opcodeIndex,
    OUT GT_U32                                 *opcodeValuePtr
)
{
    GT_U32      regAddr;            /* Register address (OAM Opcode) */
    GT_U32      opcodeOffset;       /* Opcode bit offset */
    GT_STATUS   rc;                 /* Return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(opcodeValuePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChOamOpcodeToHwOffsetsGet(devNum, stage, opcodeType,
                                            opcodeIndex,
                                            &regAddr, &opcodeOffset);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Get OAM opcode */
    return prvCpssHwPpGetRegField(devNum, regAddr, opcodeOffset, 8, opcodeValuePtr);
}

/*******************************************************************************
* cpssDxChOamOpcodeGet
*
* DESCRIPTION:
*      Get OAM opcode value
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       opcodeType                  - OAM opcode type.
*       opcodeIndex                 - OAM opcode index: (APPLICABLE RANGES: 0..3).
*
* OUTPUTS:
*       opcodeValuePtr              - pointer to OAM opcode value
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage, opcodeType or opcodeIndex.
*       GT_BAD_PTR                  - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamOpcodeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage,
    IN  CPSS_DXCH_OAM_OPCODE_TYPE_ENT           opcodeType,
    IN  GT_U32                                  opcodeIndex,
    OUT GT_U32                                 *opcodeValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamOpcodeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, opcodeType, opcodeIndex, opcodeValuePtr));

    rc = internal_cpssDxChOamOpcodeGet(devNum, stage, opcodeType, opcodeIndex, opcodeValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, opcodeType, opcodeIndex, opcodeValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamAgingPeriodEntrySet
*
* DESCRIPTION:
*      Set OAM aging period entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       entryIndex                  - aging period entry index:
*                                     (APPLICABLE RANGES: 0..7).
*       agingPeriodValue            - expected time interval between
*                                     two received keepalive
*                                     messages from a given source in nanoseconds.
*                                     The granularity of the parameter is 40 ns.
*                                     Thus, the 34 bit field allows a period of
*                                     up to ~10 minutes
*                                     The  value is rounded down to nearest HW value.
*                                     (APPLICABLE RANGES: 0..0x3FFFFFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage or entryIndex.
*       GT_OUT_OF_RANGE             - aging period value is out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamAgingPeriodEntrySet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT     stage,
    IN GT_U32                           entryIndex,
    IN GT_U64                           agingPeriodValue
)
{
    GT_U32      regAddr;                /* Register address (Aging Period Entry) */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(entryIndex > 7)  /* 8 entries in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* There are only 2 msb */
    if (agingPeriodValue.l[1] >= BIT_2)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).agingPeriodEntryLow[entryIndex];

    /* Set Aging Period - two words */
    return prvCpssHwPpWriteRam(devNum, regAddr, 2, agingPeriodValue.l);
}

/*******************************************************************************
* cpssDxChOamAgingPeriodEntrySet
*
* DESCRIPTION:
*      Set OAM aging period entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       entryIndex                  - aging period entry index:
*                                     (APPLICABLE RANGES: 0..7).
*       agingPeriodValue            - expected time interval between
*                                     two received keepalive
*                                     messages from a given source in nanoseconds.
*                                     The granularity of the parameter is 40 ns.
*                                     Thus, the 34 bit field allows a period of
*                                     up to ~10 minutes
*                                     The  value is rounded down to nearest HW value.
*                                     (APPLICABLE RANGES: 0..0x3FFFFFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage or entryIndex.
*       GT_OUT_OF_RANGE             - aging period value is out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamAgingPeriodEntrySet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT     stage,
    IN GT_U32                           entryIndex,
    IN GT_U64                           agingPeriodValue
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamAgingPeriodEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, entryIndex, agingPeriodValue));

    rc = internal_cpssDxChOamAgingPeriodEntrySet(devNum, stage, entryIndex, agingPeriodValue);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, entryIndex, agingPeriodValue));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamAgingPeriodEntryGet
*
* DESCRIPTION:
*      Get OAM aging period entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       entryIndex                  - aging period entry index
*                                     (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       agingPeriodValuePtr         - pointer to expected time interval between
*                                     two received keepalive messages from a given source.
*                                     The period is measured using a 25 MHz clock,
*                                     i.e., in a granularity of 40 ns.
*                                     Thus, the 34 bit field allows a period of
*                                     up to ~10 minutes
*                                     The  value is rounded down to nearest HW value.
*                                     (APPLICABLE RANGES: 0..0x3FFFFFFFF).
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage or entryIndex.
*       GT_BAD_PTR                  - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamAgingPeriodEntryGet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT         stage,
    IN GT_U32                               entryIndex,
    OUT GT_U64                             *agingPeriodValuePtr
)
{
    GT_U32      regAddr;                /* Register address (Aging Period Entry) */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(agingPeriodValuePtr);

    if(entryIndex > 7)  /* 8 entries in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).agingPeriodEntryLow[entryIndex];

    /* Get Aging Period - two words */
    return prvCpssHwPpReadRam(devNum, regAddr, 2, agingPeriodValuePtr->l);
}

/*******************************************************************************
* cpssDxChOamAgingPeriodEntryGet
*
* DESCRIPTION:
*      Get OAM aging period entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       entryIndex                  - aging period entry index
*                                     (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       agingPeriodValuePtr         - pointer to expected time interval between
*                                     two received keepalive messages from a given source.
*                                     The period is measured using a 25 MHz clock,
*                                     i.e., in a granularity of 40 ns.
*                                     Thus, the 34 bit field allows a period of
*                                     up to ~10 minutes
*                                     The  value is rounded down to nearest HW value.
*                                     (APPLICABLE RANGES: 0..0x3FFFFFFFF).
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage or entryIndex.
*       GT_BAD_PTR                  - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamAgingPeriodEntryGet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT         stage,
    IN GT_U32                               entryIndex,
    OUT GT_U64                             *agingPeriodValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamAgingPeriodEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, entryIndex, agingPeriodValuePtr));

    rc = internal_cpssDxChOamAgingPeriodEntryGet(devNum, stage, entryIndex, agingPeriodValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, entryIndex, agingPeriodValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamOpcodeProfilePacketCommandEntrySet
*
* DESCRIPTION:
*      Set OAM Opcode Packet Command Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       opcode                      - opcode value: (APPLICABLE RANGES: 0..255).
*       profile                     - profile index: (APPLICABLE RANGES: 0..7).
*       command                     - OAM packet command.
*       cpuCodeOffset               - two least significant bits of OAM CPU code:
*                                     (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage, opcode, profile or command.
*       GT_OUT_OF_RANGE             - CPU offset is out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamOpcodeProfilePacketCommandEntrySet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              opcode,
    IN  GT_U32                              profile,
    IN  CPSS_PACKET_CMD_ENT                 command,
    IN  GT_U32                              cpuCodeOffset
)
{
    GT_U32                  hwDataArr[2];       /* Value to write to HW */
    PRV_CPSS_DXCH_TABLE_ENT tableType;          /* Table type */
    GT_STATUS               rc;                 /* Return code */
    GT_U32                  packetCommand;      /* Packet command */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(opcode > 255)  /* 256 entries in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(profile > 7)  /* 8 entries in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(cpuCodeOffset >= BIT_2)  /* 2 bits in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    switch(command)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            packetCommand = 0;
            break;
        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            packetCommand = 1;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            packetCommand = 2;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            packetCommand = 3;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            packetCommand = 4;
            break;
        case CPSS_PACKET_CMD_LOOPBACK_E:
            packetCommand = 7;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_OPCODE_PACKET_COMMAND_E;
    }
    else
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_OPCODE_PACKET_COMMAND_E;
    }

    /* Read OAM opcode packet command entry */
    rc = prvCpssDxChReadTableEntry(devNum, tableType, opcode, &hwDataArr[0]);
    if(rc != GT_OK)
    {
        return GT_OK;
    }

    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,  (profile * 5),  3, packetCommand);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,  (profile * 5) + 3,  2, cpuCodeOffset);

    /* Set packet command and two least significant bits of CPU code per opcode profile */
    return prvCpssDxChWriteTableEntry(devNum, tableType, opcode, &hwDataArr[0]);
}

/*******************************************************************************
* cpssDxChOamOpcodeProfilePacketCommandEntrySet
*
* DESCRIPTION:
*      Set OAM Opcode Packet Command Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - OAM stage type.
*       opcode                      - opcode value: (APPLICABLE RANGES: 0..255).
*       profile                     - profile index: (APPLICABLE RANGES: 0..7).
*       command                     - OAM packet command.
*       cpuCodeOffset               - two least significant bits of OAM CPU code:
*                                     (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage, opcode, profile or command.
*       GT_OUT_OF_RANGE             - CPU offset is out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamOpcodeProfilePacketCommandEntrySet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              opcode,
    IN  GT_U32                              profile,
    IN  CPSS_PACKET_CMD_ENT                 command,
    IN  GT_U32                              cpuCodeOffset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamOpcodeProfilePacketCommandEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, opcode, profile, command, cpuCodeOffset));

    rc = internal_cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum, stage, opcode, profile, command, cpuCodeOffset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, opcode, profile, command, cpuCodeOffset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamOpcodeProfilePacketCommandEntryGet
*
* DESCRIPTION:
*      Get OAM Opcode Packet Command Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       opcode                   - opcode value: (APPLICABLE RANGES: 0..255).
*       profile                  - profile index: (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       commandPtr               - pointer to OAM packet command.
*       cpuCodeOffsetPtr         - pointer to two least significant bits
*                                  of OAM CPU code.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, entryIndex or entry parameter.
*       GT_BAD_STATE             - on bad value found in HW
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamOpcodeProfilePacketCommandEntryGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              opcode,
    IN  GT_U32                              profile,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr,
    OUT GT_U32                              *cpuCodeOffsetPtr
)
{
    GT_U32                  hwDataArr[2];                   /* Value to write to HW */
    GT_STATUS               rc;                             /* Return code  */
    PRV_CPSS_DXCH_TABLE_ENT tableType;                      /* Table type */
    GT_U32                  value;                          /* Register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(commandPtr);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodeOffsetPtr);

    if(opcode > 255)  /* 256 entries in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(profile > 7)  /* 8 entries in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_OPCODE_PACKET_COMMAND_E;
    }
    else
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_OPCODE_PACKET_COMMAND_E;
    }

    cpssOsMemSet(hwDataArr, 0, 2 * sizeof(GT_U32));

    /* Get packet command and two least significant bits of CPU code */
    rc = prvCpssDxChReadTableEntry(devNum, tableType, opcode, &hwDataArr[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,  (profile * 5),  3, value);
    switch (value)
    {
        case 0:
            *commandPtr = CPSS_PACKET_CMD_FORWARD_E;
            break;
        case 1:
            *commandPtr = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            break;
        case 2:
            *commandPtr = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            *commandPtr = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            *commandPtr = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        case 7:
            *commandPtr = CPSS_PACKET_CMD_LOOPBACK_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,  (profile * 5) + 3,  2, value);
    *cpuCodeOffsetPtr = value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChOamOpcodeProfilePacketCommandEntryGet
*
* DESCRIPTION:
*      Get OAM Opcode Packet Command Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       opcode                   - opcode value: (APPLICABLE RANGES: 0..255).
*       profile                  - profile index: (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       commandPtr               - pointer to OAM packet command.
*       cpuCodeOffsetPtr         - pointer to two least significant bits
*                                  of OAM CPU code.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, entryIndex or entry parameter.
*       GT_BAD_STATE             - on bad value found in HW
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamOpcodeProfilePacketCommandEntryGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_U32                              opcode,
    IN  GT_U32                              profile,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr,
    OUT GT_U32                              *cpuCodeOffsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamOpcodeProfilePacketCommandEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, opcode, profile, commandPtr, cpuCodeOffsetPtr));

    rc = internal_cpssDxChOamOpcodeProfilePacketCommandEntryGet(devNum, stage, opcode, profile, commandPtr, cpuCodeOffsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, opcode, profile, commandPtr, cpuCodeOffsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamTimeStampEtherTypeSet
*
* DESCRIPTION:
*      Set the ethertype which is used to identify Timestamp Tag
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*       etherType                - the Ethertype of the Timestamp Tag.
*                                  (APPLICABLE RANGES: 0..0xFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or etherType.
*       GT_OUT_OF_RANGE          - Ethertype is out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamTimeStampEtherTypeSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      etherType
)
{
    GT_U32      regAddr;                /* Register address (Timestamp Tag configuration) */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    if(etherType >= BIT_16)  /* 16 bits in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.timestampEtherTypes;

    /* Set Timestamp Tag Ethertype */
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 16, etherType);
}

/*******************************************************************************
* cpssDxChOamTimeStampEtherTypeSet
*
* DESCRIPTION:
*      Set the ethertype which is used to identify Timestamp Tag
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*       etherType                - the Ethertype of the Timestamp Tag.
*                                  (APPLICABLE RANGES: 0..0xFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or etherType.
*       GT_OUT_OF_RANGE          - Ethertype is out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamTimeStampEtherTypeSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      etherType
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamTimeStampEtherTypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherType));

    rc = internal_cpssDxChOamTimeStampEtherTypeSet(devNum, etherType);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherType));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamTimeStampEtherTypeGet
*
* DESCRIPTION:
*      Get the ethertype used to identify Timestamp Tag
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       etherTypePtr             - pointer to Ethertype of the Timestamp Tag.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamTimeStampEtherTypeGet
(
    IN  GT_U8        devNum,
    OUT  GT_U32      *etherTypePtr
)
{
    GT_U32 regAddr;                 /* Register address (Timestamp Tag configuration) */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.timestampEtherTypes;

    /* Set Timestamp Tag Ethertype */
    return prvCpssHwPpGetRegField(devNum, regAddr, 0, 16, etherTypePtr);
}

/*******************************************************************************
* cpssDxChOamTimeStampEtherTypeGet
*
* DESCRIPTION:
*      Get the ethertype used to identify Timestamp Tag
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       etherTypePtr             - pointer to Ethertype of the Timestamp Tag.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamTimeStampEtherTypeGet
(
    IN  GT_U8        devNum,
    OUT  GT_U32      *etherTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamTimeStampEtherTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherTypePtr));

    rc = internal_cpssDxChOamTimeStampEtherTypeGet(devNum, etherTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamTimeStampParsingEnableSet
*
* DESCRIPTION:
*      Enable timestamp parsing
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       enable                   - enable/disable timestamp parsing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamTimeStampParsingEnableSet
(
    IN  GT_U8        devNum,
    IN  GT_BOOL      enable
)
{

    GT_U32 regAddr;                 /* Register address (Timestamp Tag configuration) */
    GT_U32 value;                   /* Register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.timestampConfigs;

    value = BOOL2BIT_MAC(enable);

    /* Set Timestamp Tag Parsing enable */
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, value);
}

/*******************************************************************************
* cpssDxChOamTimeStampParsingEnableSet
*
* DESCRIPTION:
*      Enable timestamp parsing
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       enable                   - enable/disable timestamp parsing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamTimeStampParsingEnableSet
(
    IN  GT_U8        devNum,
    IN  GT_BOOL      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamTimeStampParsingEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChOamTimeStampParsingEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamTimeStampParsingEnableGet
*
* DESCRIPTION:
*      Get timestamp parsing status
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       enablePtr                - pointer to enable/disable timestamp parsing.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamTimeStampParsingEnableGet
(
    IN  GT_U8        devNum,
    OUT  GT_BOOL     *enablePtr
)
{
    GT_U32      regAddr;                 /* Register address (Timestamp Tag configuration) */
    GT_U32      value;                   /* Register value */
    GT_STATUS   rc;                      /* Return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.timestampConfigs;

    /* Get Timestamp Tag Parsing */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChOamTimeStampParsingEnableGet
*
* DESCRIPTION:
*      Get timestamp parsing status
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       enablePtr                - pointer to enable/disable timestamp parsing.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamTimeStampParsingEnableGet
(
    IN  GT_U8        devNum,
    OUT  GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamTimeStampParsingEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChOamTimeStampParsingEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamLmOffsetTableSet
*
* DESCRIPTION:
*      Set Loss Measurement Offset Table Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       entryIndex               - entry index: (APPLICABLE RANGES: 0..127).
*       offset                   - LM offset in bytes: (APPLICABLE RANGES: 0..127).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, entryIndex or offset.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_OUT_OF_RANGE          - on  offset out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamLmOffsetTableSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    entryIndex,
    IN  GT_U32    offset
)
{
    GT_U32      hwDataArr[1];                /* Value to write to HW */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if(offset >= BIT_7)  /* 7 bits in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    hwDataArr[0] = offset;

    return prvCpssDxChWriteTableEntry(devNum,
                                      PRV_CPSS_DXCH_LION3_TABLE_OAM_LM_OFFSET_E,
                                      entryIndex, &hwDataArr[0]);
}

/*******************************************************************************
* cpssDxChOamLmOffsetTableSet
*
* DESCRIPTION:
*      Set Loss Measurement Offset Table Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       entryIndex               - entry index: (APPLICABLE RANGES: 0..127).
*       offset                   - LM offset in bytes: (APPLICABLE RANGES: 0..127).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, entryIndex or offset.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_OUT_OF_RANGE          - on  offset out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamLmOffsetTableSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    entryIndex,
    IN  GT_U32    offset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamLmOffsetTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, offset));

    rc = internal_cpssDxChOamLmOffsetTableSet(devNum, entryIndex, offset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, offset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamLmOffsetTableGet
*
* DESCRIPTION:
*      Get Loss Measurement Offset Table Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       entryIndex               - entry index: (APPLICABLE RANGES: 0..127).
*
* OUTPUTS:
*       offsetPtr                - pointer to LM offset in bytes.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or entryIndex.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamLmOffsetTableGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      entryIndex,
    OUT GT_U32                                      *offsetPtr
)
{
    GT_U32      hwData[1];                  /* Value to write to HW */
    GT_STATUS   rc;                         /* Return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(offsetPtr);

    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_OAM_LM_OFFSET_E,
                                   entryIndex, &hwData[0]);

    U32_GET_FIELD_IN_ENTRY_MAC(&hwData[0], 0, 7, *offsetPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChOamLmOffsetTableGet
*
* DESCRIPTION:
*      Get Loss Measurement Offset Table Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       entryIndex               - entry index: (APPLICABLE RANGES: 0..127).
*
* OUTPUTS:
*       offsetPtr                - pointer to LM offset in bytes.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or entryIndex.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamLmOffsetTableGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      entryIndex,
    OUT GT_U32                                      *offsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamLmOffsetTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, offsetPtr));

    rc = internal_cpssDxChOamLmOffsetTableGet(devNum, entryIndex, offsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, offsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamCpuCodeBaseSet
*
* DESCRIPTION:
*      Set CPU code of an OAM packet that is trapped or mirrored to the CPU.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       baseCpuCode              - CPU code of an OAM packet that is trapped
*                                  or mirrored to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage or baseCpuCode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamCpuCodeBaseSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT                 stage,
    IN CPSS_NET_RX_CPU_CODE_ENT                     baseCpuCode
)
{
    GT_U32      regAddr;                    /* Register address */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT  cpuCodeBase;
                                            /* OAM  CPU code base(HW format) */
    GT_STATUS   rc;                         /* Return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChNetIfCpuToDsaCode(baseCpuCode, &cpuCodeBase);
    if( rc != GT_OK )
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMExceptionConfigs;

    return prvCpssHwPpSetRegField(devNum, regAddr, 12, 8, cpuCodeBase);
}

/*******************************************************************************
* cpssDxChOamCpuCodeBaseSet
*
* DESCRIPTION:
*      Set CPU code of an OAM packet that is trapped or mirrored to the CPU.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       baseCpuCode              - CPU code of an OAM packet that is trapped
*                                  or mirrored to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage or baseCpuCode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamCpuCodeBaseSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT                 stage,
    IN CPSS_NET_RX_CPU_CODE_ENT                     baseCpuCode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamCpuCodeBaseSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, baseCpuCode));

    rc = internal_cpssDxChOamCpuCodeBaseSet(devNum, stage, baseCpuCode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, baseCpuCode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamCpuCodeBaseGet
*
* DESCRIPTION:
*      Get CPU code of an OAM packet that is trapped or mirrored to the CPU.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*
* OUTPUTS:
*       baseCpuCodePtr           - pointer to CPU code of an OAM packet that is trapped
*                                  or mirrored to the CPU.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamCpuCodeBaseGet
(
    IN  GT_U8                                        devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT                 stage,
    OUT CPSS_NET_RX_CPU_CODE_ENT                    *baseCpuCodePtr
)
{
    GT_STATUS   rc;                         /* Return code  */
    GT_U32      cpuCodeBase;                /* OAM  CPU code base(HW format) */
    GT_U32      regAddr;                    /* Register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(baseCpuCodePtr);

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMExceptionConfigs;

    /* OAM Exception CPU code */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 12, 8, &cpuCodeBase);
    if(rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDxChNetIfDsaToCpuCode(cpuCodeBase, baseCpuCodePtr);
}

/*******************************************************************************
* cpssDxChOamCpuCodeBaseGet
*
* DESCRIPTION:
*      Get CPU code of an OAM packet that is trapped or mirrored to the CPU.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*
* OUTPUTS:
*       baseCpuCodePtr           - pointer to CPU code of an OAM packet that is trapped
*                                  or mirrored to the CPU.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamCpuCodeBaseGet
(
    IN  GT_U8                                        devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT                 stage,
    OUT CPSS_NET_RX_CPU_CODE_ENT                    *baseCpuCodePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamCpuCodeBaseGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, baseCpuCodePtr));

    rc = internal_cpssDxChOamCpuCodeBaseGet(devNum, stage, baseCpuCodePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, baseCpuCodePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamDualEndedLmPacketCommandSet
*
* DESCRIPTION:
*      Set Packet Command for Dual-Ended Loss Measurement packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       command                  - Packet Command for Dual-Ended Loss Measurement packets.
*       cpuCodeLsBits            - Two least significant bits of the CPU code
*                                  (APPLICABLE RANGES: 0..3).
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, command.
*       GT_OUT_OF_RANGE          - Packet command or two least significant bits
*                                  of CPU code are out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamDualEndedLmPacketCommandSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT                 stage,
    IN CPSS_PACKET_CMD_ENT                          command,
    IN GT_U32                                       cpuCodeLsBits
)
{
    GT_U32      pktCommand;          /* Dual-ended LM packet command (HW format) */
    GT_U32      regAddr;             /* Register address */
    GT_STATUS   rc;                  /* Return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(cpuCodeLsBits >= BIT_2)  /* 2 bits in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(pktCommand, command);

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).dualEndedLM;

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 8, 3, pktCommand);
    if(rc != GT_OK)
    {
        return rc;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 11, 2, cpuCodeLsBits);
}

/*******************************************************************************
* cpssDxChOamDualEndedLmPacketCommandSet
*
* DESCRIPTION:
*      Set Packet Command for Dual-Ended Loss Measurement packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       command                  - Packet Command for Dual-Ended Loss Measurement packets.
*       cpuCodeLsBits            - Two least significant bits of the CPU code
*                                  (APPLICABLE RANGES: 0..3).
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, command.
*       GT_OUT_OF_RANGE          - Packet command or two least significant bits
*                                  of CPU code are out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamDualEndedLmPacketCommandSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT                 stage,
    IN CPSS_PACKET_CMD_ENT                          command,
    IN GT_U32                                       cpuCodeLsBits
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamDualEndedLmPacketCommandSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, command, cpuCodeLsBits));

    rc = internal_cpssDxChOamDualEndedLmPacketCommandSet(devNum, stage, command, cpuCodeLsBits);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, command, cpuCodeLsBits));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamDualEndedLmPacketCommandGet
*
* DESCRIPTION:
*      Get Packet Command for Dual-Ended Loss Measurement packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
* OUTPUTS:
*       commandPtr               - pointer to Packet Command for
*                                  Dual-Ended Loss Measurement packets.
*       cpuCodeLsBitsPtr         - pointer to two least significant bits
*                                  of the CPU code.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_STATE             - on bad value found in HW
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamDualEndedLmPacketCommandGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage,
    OUT CPSS_PACKET_CMD_ENT                         *commandPtr,
    OUT GT_U32                                      *cpuCodeLsBitsPtr
)
{
    GT_U32      pktCommand;          /* Dual-ended LM packet command (HW format) */
    GT_U32      regAddr;             /* Register address */
    GT_STATUS   rc;                  /* Return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(commandPtr);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodeLsBitsPtr);

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).dualEndedLM;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 8, 3, &pktCommand);
    if(rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(*commandPtr, pktCommand);

    return prvCpssHwPpGetRegField(devNum, regAddr, 11, 2, cpuCodeLsBitsPtr);
}

/*******************************************************************************
* cpssDxChOamDualEndedLmPacketCommandGet
*
* DESCRIPTION:
*      Get Packet Command for Dual-Ended Loss Measurement packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
* OUTPUTS:
*       commandPtr               - pointer to Packet Command for
*                                  Dual-Ended Loss Measurement packets.
*       cpuCodeLsBitsPtr         - pointer to two least significant bits
*                                  of the CPU code.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_STATE             - on bad value found in HW
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamDualEndedLmPacketCommandGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage,
    OUT CPSS_PACKET_CMD_ENT                         *commandPtr,
    OUT GT_U32                                      *cpuCodeLsBitsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamDualEndedLmPacketCommandGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, commandPtr, cpuCodeLsBitsPtr));

    rc = internal_cpssDxChOamDualEndedLmPacketCommandGet(devNum, stage, commandPtr, cpuCodeLsBitsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, commandPtr, cpuCodeLsBitsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamExceptionStatusGet
*
* DESCRIPTION:
*      Get exception status entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       exceptionType            - OAM exception type.
*       entryIndex               - index of OAM exception table entry
*
* OUTPUTS:
*       entryBmpPtr              - pointer to OAM exception table entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, exceptionType or entryIndex.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Each bit in the entry notifies exception per OAM flow according to formula:
*       <<32*m+n>>; where m - table entry index (0 < m < 64). n - exception bit (0 < n < 32)
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamExceptionStatusGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage,
    IN  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT            exceptionType,
    IN  GT_U32                                      entryIndex,
    OUT GT_U32                                      *entryBmpPtr
)
{
    PRV_CPSS_DXCH_TABLE_ENT tableType;      /* Table type */
    GT_U32  portGroupId;                    /* Port group Id */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    /* each entry hold 32 bits representing 'OAM entry' , so check validity */
    PRV_CPSS_DXCH_OAM_NUM_CHECK_MAC(devNum,stage,(entryIndex * 32));

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entryBmpPtr);

    /* Convert exception type to the bit offset */
    switch(exceptionType)
    {
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_MEG_LEVEL_E:
            tableType = (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E) ?
                PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_MEG_EXCEPTION_E :
                PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_MEG_EXCEPTION_E;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_SOURCE_INTERFACE_E:
            tableType = (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E) ?
                PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E :
                PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_RDI_STATUS_CHANGED_E:
            tableType = (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E) ?
                PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E :
                PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_AGING_E:
            tableType = (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E) ?
                PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_KEEPALIVE_AGING_E :
                PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_KEEPALIVE_AGING_E;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_EXCESS_E:
            tableType = (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E) ?
                PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_KEEPALIVE_EXCESS_E :
                PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_KEEPALIVE_EXCESS_E;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_INVALID_HASH_E:
            tableType = (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E) ?
                PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_INVALID_KEEPALIVE_HASH_E :
                PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_INVALID_KEEPALIVE_HASH_E;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_TX_PERIOD_E:
            tableType = (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E) ?
                PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_TX_PERIOD_EXCEPTION_E :
                PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_TX_PERIOD_EXCEPTION_E;
            break;
        case CPSS_DXCH_OAM_EXCEPTION_TYPE_SUMMARY_E:
            tableType = (stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E) ?
                PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_EXCEPTION_SUMMARY_E :
                PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_EXCEPTION_SUMMARY_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, portGroupId)
    {
        /* need to read the info from a specific port group */
        rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                        portGroupId,
                                        tableType,
                                        entryIndex,
                                        entryBmpPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(entryBmpPtr[0])
        {
            /* Bitmap is not empty */
            break;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChOamExceptionStatusGet
*
* DESCRIPTION:
*      Get exception status entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       exceptionType            - OAM exception type.
*       entryIndex               - index of OAM exception table entry
*
* OUTPUTS:
*       entryBmpPtr              - pointer to OAM exception table entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum, stage, exceptionType or entryIndex.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Each bit in the entry notifies exception per OAM flow according to formula:
*       <<32*m+n>>; where m - table entry index (0 < m < 64). n - exception bit (0 < n < 32)
*
*******************************************************************************/
GT_STATUS cpssDxChOamExceptionStatusGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage,
    IN  CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT            exceptionType,
    IN  GT_U32                                      entryIndex,
    OUT GT_U32                                      *entryBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamExceptionStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, exceptionType, entryIndex, entryBmpPtr));

    rc = internal_cpssDxChOamExceptionStatusGet(devNum, stage, exceptionType, entryIndex, entryBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, exceptionType, entryIndex, entryBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamAgingDaemonEnableSet
*
* DESCRIPTION:
*      Enable or disable OAM aging daemon.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       enable                  - enable/disable OAM aging daemon:
*                                 GT_TRUE  - Aging Daemon periodically access
*                                 keepalive table and check the age state.
*                                 GT_FALSE - Aging Daemon doesn't request access
*                                 to keepalive table.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamAgingDaemonEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_BOOL                             enable
)
{
    GT_U32 regAddr;                                 /* Register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMGlobalCtrl;

    return prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChOamAgingDaemonEnableSet
*
* DESCRIPTION:
*      Enable or disable OAM aging daemon.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       enable                  - enable/disable OAM aging daemon:
*                                 GT_TRUE  - Aging Daemon periodically access
*                                 keepalive table and check the age state.
*                                 GT_FALSE - Aging Daemon doesn't request access
*                                 to keepalive table.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamAgingDaemonEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamAgingDaemonEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, enable));

    rc = internal_cpssDxChOamAgingDaemonEnableSet(devNum, stage, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamAgingDaemonEnableGet
*
* DESCRIPTION:
*      Get OAM aging daemon status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*
* OUTPUTS:
*       enablePtr                - pointer to OAM aging daemon status:
*                                  GT_TRUE  - Aging Daemon periodically access
*                                  keepalive table  and check the age state.
*                                  GT_FALSE - Aging Daemon doesn't request access
*                                  to keepalive table.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamAgingDaemonEnableGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      regAddr;                    /* Register address */
    GT_U32      enable;                     /* Enable daemon(HW bit) */
    GT_STATUS   rc;                         /* Return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMGlobalCtrl;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 1, 1, &enable);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(enable);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChOamAgingDaemonEnableGet
*
* DESCRIPTION:
*      Get OAM aging daemon status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*
* OUTPUTS:
*       enablePtr                - pointer to OAM aging daemon status:
*                                  GT_TRUE  - Aging Daemon periodically access
*                                  keepalive table  and check the age state.
*                                  GT_FALSE - Aging Daemon doesn't request access
*                                  to keepalive table.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamAgingDaemonEnableGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamAgingDaemonEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, enablePtr));

    rc = internal_cpssDxChOamAgingDaemonEnableGet(devNum, stage, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamPduCpuCodeSet
*
* DESCRIPTION:
*       Set the CPU code assigned to OAM PDU frames
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       cpuCode                 - CPU code
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or cpuCode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamPduCpuCodeSet
(
    IN GT_U8                            devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT         cpuCode
)
{
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT  dsaCpuCode;
    GT_U32                                  regAddr;
    GT_STATUS                               rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChNetIfCpuToDsaCode(cpuCode, &dsaCpuCode);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfigExt;

    return prvCpssHwPpSetRegField(devNum, regAddr, 17, 8, (GT_U32)dsaCpuCode);
}

/*******************************************************************************
* cpssDxChOamPduCpuCodeSet
*
* DESCRIPTION:
*       Set the CPU code assigned to OAM PDU frames
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       cpuCode                 - CPU code
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or cpuCode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamPduCpuCodeSet
(
    IN GT_U8                            devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT         cpuCode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamPduCpuCodeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCode));

    rc = internal_cpssDxChOamPduCpuCodeSet(devNum, cpuCode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamPduCpuCodeGet
*
* DESCRIPTION:
*       Get the CPU code assigned to OAM PDU frames
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       cpuCodePtr              - (pointer to) the CPU code
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamPduCpuCodeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT        *cpuCodePtr
)
{
    GT_U32    regAddr, value;
    GT_STATUS rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfigExt;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 17, 8, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDxChNetIfDsaToCpuCode((PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT)value,
                                      cpuCodePtr);
}

/*******************************************************************************
* cpssDxChOamPduCpuCodeGet
*
* DESCRIPTION:
*       Get the CPU code assigned to OAM PDU frames
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       cpuCodePtr              - (pointer to) the CPU code
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamPduCpuCodeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT        *cpuCodePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamPduCpuCodeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCodePtr));

    rc = internal_cpssDxChOamPduCpuCodeGet(devNum, cpuCodePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCodePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamMplsCwChannelTypeProfileSet
*
* DESCRIPTION:
*       Set an MPLS Control Word Channel Type to Channel Type Profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       profileIndex    - channel type profile index (APPLICABLE RANGES: 1..15)
*       channelTypeId   - channel type id      (APPLICABLE RANGES: 0..65535)
*       profileEnable   - enable/disable the channel id:
*                         GT_TRUE: the packet Control Word Channel Type is
*                                  compared with the channel type profile value
*                         GT_FALSE: the packet Control Word Channel Type is not
*                                   compared with the channel type profile value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - out-of-range in channelTypeId parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The MPLS packet is assumed to have a Control Word if the first nibble of
*       the MPLS payload is 0x1.
*       The Channel Type ID placed in 16 LSBs of a Control Word.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamMplsCwChannelTypeProfileSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    profileIndex,
    IN  GT_U32    channelTypeId,
    IN  GT_BOOL   profileEnable
)
{
    GT_STATUS   rc;        /* return code      */
    GT_U32      regAddr;   /* register address */
    GT_U32      value;     /* HW value         */
    GT_U32      regIdx;    /* register index   */
    GT_U32      bitOffset; /* bit offset       */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if ((profileIndex == 0) || (profileIndex > 15))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((CHECK_BITS_DATA_RANGE_MAC(channelTypeId, 16)) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* Ingress OAM */

    regAddr =
        PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.MPLSOAMChannelTypeProfile[profileIndex - 1];
    value = BOOL2BIT_MAC(profileEnable) | (channelTypeId << 1);

    rc = prvCpssHwPpSetRegField(
        devNum, regAddr, 0, 17, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Egress OAM */

    if (profileIndex < 15)
    {
        regIdx = (profileIndex - 1) / 2;
        bitOffset = ((profileIndex - 1) % 2) * 16;
        regAddr =
            PRV_DXCH_REG1_UNIT_HA_MAC(devNum).MPLSChannelTypeProfileReg[regIdx];
    }
    else
    {
        /*profileIndex == 15*/
        bitOffset = 0;
        regAddr =
            PRV_DXCH_REG1_UNIT_HA_MAC(devNum).MPLSChannelTypeProfileReg7;
    }
    value = channelTypeId;


    /* channel type id */
    rc = prvCpssHwPpSetRegField(
        devNum, regAddr, bitOffset, 16, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    bitOffset = 16 + (profileIndex - 1);
    regAddr =
        PRV_DXCH_REG1_UNIT_HA_MAC(devNum).MPLSChannelTypeProfileReg7;
    value = BOOL2BIT_MAC(profileEnable);

    rc = prvCpssHwPpSetRegField(
        devNum, regAddr, bitOffset, 1, value);

    return rc;
}

/*******************************************************************************
* cpssDxChOamMplsCwChannelTypeProfileSet
*
* DESCRIPTION:
*       Set an MPLS Control Word Channel Type to Channel Type Profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       profileIndex    - channel type profile index (APPLICABLE RANGES: 1..15)
*       channelTypeId   - channel type id      (APPLICABLE RANGES: 0..65535)
*       profileEnable   - enable/disable the channel id:
*                         GT_TRUE: the packet Control Word Channel Type is
*                                  compared with the channel type profile value
*                         GT_FALSE: the packet Control Word Channel Type is not
*                                   compared with the channel type profile value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - out-of-range in channelTypeId parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The MPLS packet is assumed to have a Control Word if the first nibble of
*       the MPLS payload is 0x1.
*       The Channel Type ID placed in 16 LSBs of a Control Word.
*
*******************************************************************************/
GT_STATUS cpssDxChOamMplsCwChannelTypeProfileSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    profileIndex,
    IN  GT_U32    channelTypeId,
    IN  GT_BOOL   profileEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamMplsCwChannelTypeProfileSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex, channelTypeId, profileEnable));

    rc = internal_cpssDxChOamMplsCwChannelTypeProfileSet(devNum, profileIndex, channelTypeId, profileEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex, channelTypeId, profileEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamMplsCwChannelTypeProfileGet
*
* DESCRIPTION:
*       Get an MPLS Control Word Channel Type to Channel Type Profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       profileIndex    - channel type profile index (APPLICABLE RANGES: 1..15)
*
* OUTPUTS:
*       channelTypeIdPtr   - (pointer to) channel type id
*       profileEnablePtr   - (pointer to) enable/disable the channel id:
*                         GT_TRUE: the packet Control Word Channel Type is
*                                  compared with the channel type profile value
*                         GT_FALSE: the packet Control Word Channel Type is not
*                                   compared with the channel type profile value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer in any output parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The MPLS packet is assumed to have a Control Word if the first nibble of
*       the MPLS payload is 0x1.
*       The Channel Type ID placed in 16 LSBs of a Control Word.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamMplsCwChannelTypeProfileGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    profileIndex,
    OUT GT_U32    *channelTypeIdPtr,
    OUT GT_BOOL   *profileEnablePtr
)
{
    GT_STATUS   rc;        /* return code      */
    GT_U32      regAddr;   /* register address */
    GT_U32      value;     /* HW value         */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(channelTypeIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(profileEnablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if ((profileIndex == 0) || (profileIndex > 15))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Ingress OAM */

    regAddr =
        PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.MPLSOAMChannelTypeProfile[profileIndex - 1];

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 0, 17, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *profileEnablePtr = BIT2BOOL_MAC((value & 1));
    *channelTypeIdPtr = ((value >> 1) & 0xFFFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChOamMplsCwChannelTypeProfileGet
*
* DESCRIPTION:
*       Get an MPLS Control Word Channel Type to Channel Type Profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       profileIndex    - channel type profile index (APPLICABLE RANGES: 1..15)
*
* OUTPUTS:
*       channelTypeIdPtr   - (pointer to) channel type id
*       profileEnablePtr   - (pointer to) enable/disable the channel id:
*                         GT_TRUE: the packet Control Word Channel Type is
*                                  compared with the channel type profile value
*                         GT_FALSE: the packet Control Word Channel Type is not
*                                   compared with the channel type profile value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer in any output parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The MPLS packet is assumed to have a Control Word if the first nibble of
*       the MPLS payload is 0x1.
*       The Channel Type ID placed in 16 LSBs of a Control Word.
*
*******************************************************************************/
GT_STATUS cpssDxChOamMplsCwChannelTypeProfileGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    profileIndex,
    OUT GT_U32    *channelTypeIdPtr,
    OUT GT_BOOL   *profileEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamMplsCwChannelTypeProfileGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex, channelTypeIdPtr, profileEnablePtr));

    rc = internal_cpssDxChOamMplsCwChannelTypeProfileGet(devNum, profileIndex, channelTypeIdPtr, profileEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex, channelTypeIdPtr, profileEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamAgingBitmapUpdateModeSet
*
* DESCRIPTION:
*      Set aging bitmap update mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       mode                     - aging bitmap update mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong stage or aging bitmap update mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamAgingBitmapUpdateModeSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage,
    IN  CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT  mode
)
{
    GT_U32 regAddr;                 /* register address  */
    GT_U32 value;                   /* register HW field value */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(mode)
    {
        case CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ONLY_FAILURES_E:
            value = 0;
            break;
        case CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ALL_E:
            value = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* OAM Global Control 1 register */
    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMGlobalCtrl1;

    /* Aging Bitmap Update Mode  */
    return prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, value);
}

/*******************************************************************************
* cpssDxChOamAgingBitmapUpdateModeSet
*
* DESCRIPTION:
*      Set aging bitmap update mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       mode                     - aging bitmap update mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong stage or aging bitmap update mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamAgingBitmapUpdateModeSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage,
    IN  CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT  mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamAgingBitmapUpdateModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, mode));

    rc = internal_cpssDxChOamAgingBitmapUpdateModeSet(devNum, stage, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamAgingBitmapUpdateModeGet
*
* DESCRIPTION:
*      Get aging bitmap update mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*
* OUTPUTS:
*       modePtr                  - (pointer to) aging bitmap update mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_STATE             - on bad value found in HW
*       GT_BAD_PARAM             - on wrong stage.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - output parameter is NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamAgingBitmapUpdateModeGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage,
    OUT CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT  *modePtr
)
{
    GT_U32 regAddr;                 /* register address  */
    GT_U32 value;                   /* register HW field value */
    GT_STATUS rc;                   /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* OAM Global Control 1 register */
    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMGlobalCtrl1;
    /* Aging Bitmap Update Mode  */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 1, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (value == 0) ?
        CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ONLY_FAILURES_E :
        CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ALL_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChOamAgingBitmapUpdateModeGet
*
* DESCRIPTION:
*      Get aging bitmap update mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*
* OUTPUTS:
*       modePtr                  - (pointer to) aging bitmap update mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_STATE             - on bad value found in HW
*       GT_BAD_PARAM             - on wrong stage.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - output parameter is NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamAgingBitmapUpdateModeGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage,
    OUT CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT  *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamAgingBitmapUpdateModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, modePtr));

    rc = internal_cpssDxChOamAgingBitmapUpdateModeGet(devNum, stage, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamHashBitSelectionSet
*
* DESCRIPTION:
*      Set indexes of the first and last bits of the 32 bits hash.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       hashFirstBit             - hash first index
*                                  (APPLICABLE RANGES: 0..31)
*       hashLastBit              - hash last index
*                                  (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on bad stage, bad first or last index.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_OUT_OF_RANGE          - on hash index is out of range.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamHashBitSelectionSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT    stage,
    IN  GT_U32                          hashFirstBit,
    IN  GT_U32                          hashLastBit
)
{
    GT_U32 regAddr;                     /* register address */
    GT_U32 value;                       /* register HW value */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(hashFirstBit >= BIT_5 || hashLastBit >= BIT_5)  /*5 bits in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(hashFirstBit >= hashLastBit)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    value = hashFirstBit | (hashLastBit << 5);

    /* Hash Bits Selection */
    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).hashBitsSelection;

    /* Hash first and hash last bits */
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 10, value);
}

/*******************************************************************************
* cpssDxChOamHashBitSelectionSet
*
* DESCRIPTION:
*      Set indexes of the first and last bits of the 32 bits hash.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*       hashFirstBit             - hash first index
*                                  (APPLICABLE RANGES: 0..31)
*       hashLastBit              - hash last index
*                                  (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on bad stage, bad first or last index.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_OUT_OF_RANGE          - on hash index is out of range.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamHashBitSelectionSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT    stage,
    IN  GT_U32                          hashFirstBit,
    IN  GT_U32                          hashLastBit
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamHashBitSelectionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, hashFirstBit, hashLastBit));

    rc = internal_cpssDxChOamHashBitSelectionSet(devNum, stage, hashFirstBit, hashLastBit);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, hashFirstBit, hashLastBit));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamHashBitSelectionGet
*
* DESCRIPTION:
*      Get indexes of the first and last bits of the 32 bits hash.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*
* OUTPUTS:
*       hashFirstBitPtr          - pointer to hash first index
*       hashLastBitPtr           - pointer to hash last index
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on bad stage
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamHashBitSelectionGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT    stage,
    OUT GT_U32                          *hashFirstBitPtr,
    OUT GT_U32                          *hashLastBitPtr
)
{
    GT_U32 regAddr;                 /* register address  */
    GT_U32 value;                   /* register HW field value */
    GT_STATUS rc;                   /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(hashFirstBitPtr);
    CPSS_NULL_PTR_CHECK_MAC(hashLastBitPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    /* Hash Bits Selection */
    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).hashBitsSelection;
    /* Hash first and hash last bits */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 10, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *hashFirstBitPtr = (value & 0x1F);
    *hashLastBitPtr = (value >> 5);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChOamHashBitSelectionGet
*
* DESCRIPTION:
*      Get indexes of the first and last bits of the 32 bits hash.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number.
*       stage                    - OAM stage type.
*
* OUTPUTS:
*       hashFirstBitPtr          - pointer to hash first index
*       hashLastBitPtr           - pointer to hash last index
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on bad stage
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamHashBitSelectionGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT    stage,
    OUT GT_U32                          *hashFirstBitPtr,
    OUT GT_U32                          *hashLastBitPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamHashBitSelectionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, hashFirstBitPtr, hashLastBitPtr));

    rc = internal_cpssDxChOamHashBitSelectionGet(devNum, stage, hashFirstBitPtr, hashLastBitPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, hashFirstBitPtr, hashLastBitPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamKeepaliveForPacketCommandEnableSet
*
* DESCRIPTION:
*      Enable or disable OAM keepalive processing for specific packet command.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       command                 - packet command.
*                                  Ingress OAM (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_DROP_HARD_E;
*                                   CPSS_PACKET_CMD_DROP_SOFT_E)
*                                  Egress OAM (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_DROP_HARD_E)
*       enable                  - enable/disable OAM keepalive processing:
*                                 GT_TRUE  - Enable keepalive processing.
*                                 GT_FALSE - Disable keepalive processing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum, stage or command.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamKeepaliveForPacketCommandEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  CPSS_PACKET_CMD_ENT                 command,
    IN  GT_BOOL                             enable
)
{
    GT_U32      fieldOffset;         /* offset of field in register */
    GT_U32      fieldValue;          /* value of field in register  */
    GT_U32      regAddr;             /* Register address            */
    GT_STATUS   rc;                  /* Return code                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);

    switch(command)
    {
        case CPSS_PACKET_CMD_DROP_HARD_E:
            fieldOffset = 12;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            if (stage == CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E)
            {
                /* Egress OAM does not support Soft Drop */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            fieldOffset = 13;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMInvalidKeepaliveExceptionConfigs;
    fieldValue = BOOL2BIT_MAC(enable);

    rc = prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, fieldValue);
    return rc;
}

/*******************************************************************************
* cpssDxChOamKeepaliveForPacketCommandEnableSet
*
* DESCRIPTION:
*      Enable or disable OAM keepalive processing for specific packet command.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       command                 - packet command.
*                                  Ingress OAM (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_DROP_HARD_E;
*                                   CPSS_PACKET_CMD_DROP_SOFT_E)
*                                  Egress OAM (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_DROP_HARD_E)
*       enable                  - enable/disable OAM keepalive processing:
*                                 GT_TRUE  - Enable keepalive processing.
*                                 GT_FALSE - Disable keepalive processing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum, stage or command.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamKeepaliveForPacketCommandEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  CPSS_PACKET_CMD_ENT                 command,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamKeepaliveForPacketCommandEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, command, enable));

    rc = internal_cpssDxChOamKeepaliveForPacketCommandEnableSet(devNum, stage, command, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, command, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChOamKeepaliveForPacketCommandEnableGet
*
* DESCRIPTION:
*      Get status of OAM keepalive processing for specific packet command.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       command                 - packet command.
*                                  Ingress OAM (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_DROP_HARD_E;
*                                   CPSS_PACKET_CMD_DROP_SOFT_E)
*                                  Egress OAM (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_DROP_HARD_E)
*
* OUTPUTS:
*       enablePtr               - (pointer to) enable/disable OAM keepalive
*                                 processing:
*                                 GT_TRUE  - Enable keepalive processing.
*                                 GT_FALSE - Disable keepalive processing.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum, stage or command.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChOamKeepaliveForPacketCommandEnableGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  CPSS_PACKET_CMD_ENT                 command,
    OUT GT_BOOL                            *enablePtr
)
{
    GT_U32      fieldOffset;         /* offset of field in register */
    GT_U32      fieldValue;          /* value of field in register  */
    GT_U32      regAddr;             /* Register address            */
    GT_STATUS   rc;                  /* Return code                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_OAM_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch(command)
    {
        case CPSS_PACKET_CMD_DROP_HARD_E:
            fieldOffset = 12;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            if (stage == CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E)
            {
                /* Egress OAM does not support Soft Drop */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            fieldOffset = 13;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_OAM_MAC(devNum,stage).OAMInvalidKeepaliveExceptionConfigs;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &fieldValue);

    *enablePtr = BIT2BOOL_MAC(fieldValue);

    return rc;
}

/*******************************************************************************
* cpssDxChOamKeepaliveForPacketCommandEnableGet
*
* DESCRIPTION:
*      Get status of OAM keepalive processing for specific packet command.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       stage                   - OAM stage type.
*       command                 - packet command.
*                                  Ingress OAM (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_DROP_HARD_E;
*                                   CPSS_PACKET_CMD_DROP_SOFT_E)
*                                  Egress OAM (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_DROP_HARD_E)
*
* OUTPUTS:
*       enablePtr               - (pointer to) enable/disable OAM keepalive
*                                 processing:
*                                 GT_TRUE  - Enable keepalive processing.
*                                 GT_FALSE - Disable keepalive processing.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum, stage or command.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamKeepaliveForPacketCommandEnableGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage,
    IN  CPSS_PACKET_CMD_ENT                 command,
    OUT GT_BOOL                            *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChOamKeepaliveForPacketCommandEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stage, command, enablePtr));

    rc = internal_cpssDxChOamKeepaliveForPacketCommandEnableGet(devNum, stage, command, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stage, command, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

