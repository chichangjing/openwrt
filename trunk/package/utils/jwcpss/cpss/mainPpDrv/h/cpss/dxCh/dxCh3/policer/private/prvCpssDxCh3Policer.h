/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxCh3Policer.h
*
* DESCRIPTION:
*       Private CPSS DxCh3 Policing Engine declarations.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#ifndef __prvCpssDxCh3Policerh
#define __prvCpssDxCh3Policerh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>

/* Check Policer stage number, for DxCh3 it is always 0 */
#define PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(_devNum, _stage)                                  \
if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(_devNum))                                       \
{                                                                                       \
    switch ((_stage))                                                                   \
    {                                                                                   \
        case CPSS_DXCH_POLICER_STAGE_INGRESS_0_E:                                       \
            if (PRV_CPSS_DXCH_PP_MAC(_devNum)->fineTuning.tableSize.policersNum == 0)   \
            {                                                                           \
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                    \
            }                                                                           \
            break;                                                                      \
        case CPSS_DXCH_POLICER_STAGE_INGRESS_1_E:                                       \
            if (PRV_CPSS_DXCH_PP_MAC(_devNum)->fineTuning.featureInfo.iplrSecondStageSupported == GT_FALSE) \
            {                                                                           \
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                    \
            }                                                                           \
            break;                                                                      \
        case CPSS_DXCH_POLICER_STAGE_EGRESS_E:                                          \
            if (PRV_CPSS_DXCH_PP_MAC(_devNum)->fineTuning.tableSize.egressPolicersNum == 0) \
            {                                                                           \
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                    \
            }                                                                           \
            break;                                                                      \
        default:                                                                        \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                        \
    }                                                                                   \
}                                                                                       \
else                                                                                    \
    (_stage) = 0; /* the CH3 ignores stage, use stage 0 */


/*
 * typedef: enum PRV_CPSS_DXCH3_POLICER_ACCESS_CTRL_CMD_ENT
 *
 * Description: Enumeration for Type of triggered action that the Policer
 *              needs to perform. It using upon accessing the metering and
 *              counting table entries for read, write and update operations.
 *
 * Enumerations:
 *  PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_RESET_E  - The counter entry will be
 *                                                  read to the IPLR Table
 *                                                  Access Data Reg. Then reset
 *                                                  and written back to memory.
 *
 *  PRV_CPSS_DXCH3_POLICER_CNTR_READ_ONLY_E       - The counter entry will be
 *                                                  read to the IPLR Table
 *                                                  Access Data Register. No
 *                                                  write back is performed.
 *
 *  PRV_CPSS_DXCH3_POLICER_CNTR_WBF_FLUSH_E       - Flush the Counter
 *                                                  write-back buffer.
 *
 *  PRV_CPSS_DXCH3_POLICER_METERING_UPDATE_E      - Sets metering entry fields,
 *                                                  according to the information
 *                                                  placed in the IPLR Table
 *                                                  Access Data Register.
 *
 *  PRV_CPSS_DXCH3_POLICER_METERING_REFRESH_E     - Refresh the metering entry
 *                                                  status fields.
 *  PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_WRITE_E  - The counter entry will be
 *                                                  loaded to the PLR Table
 *                                                  Access Data Regs.
 *                                                  And content of  PLR Table Access
 *                                                  Data Regs is written to memory.
 *                                                  Supported for Lion and above
 *                                                  devices.
 *
 */
typedef enum {
    PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_RESET_E = 0,
    PRV_CPSS_DXCH3_POLICER_CNTR_READ_ONLY_E,
    PRV_CPSS_DXCH3_POLICER_CNTR_WBF_FLUSH_E,
    PRV_CPSS_DXCH3_POLICER_METERING_UPDATE_E = 4,
    PRV_CPSS_DXCH3_POLICER_METERING_REFRESH_E,
    PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_WRITE_E = 0x40
} PRV_CPSS_DXCH3_POLICER_ACCESS_CTRL_CMD_ENT;

/* ====== Envelopes support (MEF10.3) ===== */

/* maximal size of MEF 10.3 Envelope */
#define PRV_CPSS_DXCH_POLICER_MAX_ENVELOPE_SIZE_CNS 8

/*
 * typedef: enum PRV_CPSS_DXCH_POLICER_BUCKET_COLOR_ENT
 *
 * Description: Enumeration for Bucket Color in Bucket Envelope.
 *
 * Enumerations:
 *    PRV_CPSS_DXCH_POLICER_BUCKET_COLOR_GREEN_E   - Green Bucket.
 *    PRV_CPSS_DXCH_POLICER_BUCKET_COLOR_YELLOW_E  - Yellow Bucket.
 */
typedef enum
{
    PRV_CPSS_DXCH_POLICER_BUCKET_COLOR_GREEN_E,
    PRV_CPSS_DXCH_POLICER_BUCKET_COLOR_YELLOW_E,
} PRV_CPSS_DXCH_POLICER_BUCKET_COLOR_ENT;

/*
 * typedef: struct PRV_CPSS_DXCH_POLICER_ENVELOPE_MEMBER_CFG_STC
 *
 * Description: Structure for MEF 10.3 backet pair parameters in the
 *              Policer Metering Entry, related only to MEF1.3.
 *              (APPLICABLE DEVICES: Bobcat2).
 *
 * Fields:
 *  envelopeSize        -  amount entries in MEF 10.3 Envelope.
 *                         the same value should be configured in all entries of envelope
 *                         (APPLICABLE RANGES: 1..8)
 *  bucket0Rank         -  rank of bucket0 in the Envelope
 *                         (APPLICABLE RANGES: 1..8)
 *  bucket1Rank         -  rank of bucket1 in the Envelope
 *                         (APPLICABLE RANGES: 1..8)
 *  bucket0Color        -  color of bucket0 in the Envelope
 *  bucket1Color        -  color of bucket1 in the Envelope
 *  couplingFlag        -  The coupling flag selects between these options.
 *                         GT_FALSE = SPILL TO NEXT: The extra tokens of the CIR bucket
 *                         are spilled to the CIR bucket of the next rank.
 *                         GT_TRUE = SPILL TO SAME: The extra tokens of the CIR bucket
 *                         are spilled to the EIR bucket of the same rank.
 *  couplingFlag0       -  in MEF 10.3, the CIR bucket of the bottom rank in the
 *                         MEF 10.3 envelope may either spill the overflow to
 *                         the EIR bucket of the highest rank or not, according
 *                         to coupling flag 0.
 *                         GT_FALSE = DONT SPILL: The extra tokens of the CIR bucket
 *                         are not spilled to the EIR bucket.
 *                         GT_TRUE = SPILL: The extra tokens of the CIR bucket
 *                         are spilled to the EIR bucket.
 *  maxRateIndex        -  index in Max Rate table entry used to limit the actual rate
 *                         of backets0-1.
 *                         (APPLICABLE RANGES: 0..127)
 *
 */
typedef struct
{
    GT_U32                                  envelopeSize;
    GT_U32                                  bucket0Rank;
    GT_U32                                  bucket1Rank;
    PRV_CPSS_DXCH_POLICER_BUCKET_COLOR_ENT  bucket0Color;
    PRV_CPSS_DXCH_POLICER_BUCKET_COLOR_ENT  bucket1Color;
    GT_BOOL                                 couplingFlag;
    GT_BOOL                                 couplingFlag0;
    GT_U32                                  maxRateIndex;

} PRV_CPSS_DXCH_POLICER_ENVELOPE_MEMBER_CFG_STC;


/*******************************************************************************
* prvCpssDxCh3PolicerInternalTableAccess
*
* DESCRIPTION:
*       Triggers access operation in policer internal table.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupId     - the portGroupId. to support multi-port-groups device
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above
*                         devices and ignored by DxCh3.
*       entryOffset     - The offset from the base address of the
*                         accessed entry.
*       triggerCmd      - Triggered command that the Policer needs to perform.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_TIMEOUT                  - on time out.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong input parameters.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxCh3PolicerInternalTableAccess
(
    IN GT_U8                                        devNum,
    IN  GT_U32                                      portGroupId,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    IN GT_U32                                       entryOffset,
    IN PRV_CPSS_DXCH3_POLICER_ACCESS_CTRL_CMD_ENT   triggerCmd
);

/*******************************************************************************
* prvCpssDxCh3PolicerAccessControlBusyWait
*
* DESCRIPTION:
*       Do busy wait on Policer Access Control Register.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above
*                         devices and ignored by DxCh3.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_TIMEOUT      - on time out.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PARAM    - on wrong input parameters.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxCh3PolicerAccessControlBusyWait
(
    IN GT_U8                                        devNum,
    IN GT_PORT_GROUPS_BMP                           portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxCh3Policerh */

