/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChOam.h
*
* DESCRIPTION:
*       CPSS DxCh OAM Engine API
*       The OAM Engine is responsible for:
*
*           - Loss Measurement (LM) function. Allows a Maintenance End Point (MEP)
*             to compute the packet loss rate in a path to/from a peer MEP.
*           - Delay Measurement (DM) function. Enables a MEP to measure
*             the packet delay and delay variation between itself and a peer MEP.
*           - Generic Keepalive Engine.
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
*
*******************************************************************************/

#ifndef __cpssDxChOamh
#define __cpssDxChOamh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
/*
 *
 * typedef: enum CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT
 *
 * Description: Enumeration for OAM Unit Type.
 *
 * Enumerations:
 *    CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_NO_MATCH_E -
 *              Verify that the packet’s source interface
 *              does NOT match <LocalDevSRCePort / LocalDevSrcTrunk>.
 *    CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E   -
 *              Verify that the packet’s source interface
 *              matches <LocalDevSRCePort / LocalDevSrcTrunk>.
 */
typedef enum {
    CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_NO_MATCH_E,
    CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E
} CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT;


/*
 *
 * typedef: enum CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT
 *
 * Description: Enumeration for OAM Unit Type.
 *
 * Enumerations:
 *  CPSS_DXCH_OAM_LM_COUNTING_MODE_RETAIN_E - Retain: Keep the previous decision of whether to enable
 *      Policer counting for this packet or not.
 *  CPSS_DXCH_OAM_LM_COUNTING_MODE_DISABLE_E - Disable Counting
 *  CPSS_DXCH_OAM_LM_COUNTING_MODE_ENABLE_E - Enable Counting: If <OAM Opcode Parsing Enable> is enabled,
 *      then OAM packets with an opcode that is equal to <OAM Counted Opcode<n>>
 *      are counted by the LM counter. - If <OAM Opcode Parsing Enable> is disabled,
 *      all packets that access the OAM table are counted by the LM counter.
 */
typedef enum {
    CPSS_DXCH_OAM_LM_COUNTING_MODE_RETAIN_E = 0,
    CPSS_DXCH_OAM_LM_COUNTING_MODE_DISABLE_E,
    CPSS_DXCH_OAM_LM_COUNTING_MODE_ENABLE_E
} CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT;
/*
 *  typedef: struct CPSS_DXCH_OAM_ENTRY_STC
 *
 *  Description: Structure of OAM table entry
 *
 *  Fields:
 *
 *                      Fields relevant to general OAM processing
 *
 *   opcodeParsingEnable        -   When enabled, the packet is treated as
 *                                  an OAM packet;
 *                                  GT_FALSE - disable opcode parsing;
 *                                  GT_TRUE - enable opcode parsing;
 *   megLevelCheckEnable        -   Determines if the packet’s MEG level
 *                                  is checked, using the <MEG level> field;
 *                                  GT_FALSE - disable MEG level check;
 *                                  GT_TRUE - enable MEG level check;
 *   megLevel                   -   Determines the MEG level of the current MEP;
 *                                  (APPLICABLE RANGES: 0..7)
 *   packetCommandProfile       -   This field determines the profile of the
 *                                  packet command according to the opcode
 *                                  to packet command table;
 *                                  (APPLICABLE RANGES: 0..7)
 *   cpuCodeOffset              -   If the packet is trapped/mirrored to the
 *                                  CPU by the OAM Opcode Packet Command table,
 *                                  this field is added to the CPU Code;
 *                                  (APPLICABLE RANGES: 0..3)
 *   sourceInterfaceCheckEnable -   Determines whether the packet’s
 *                                  source interface is checked;
 *                                  GT_FALSE - disable source interface check;
 *                                  GT_TRUE - enable source interface check;
 *   sourceInterfaceCheckMode   -   Source interface check mode
 *
 *   sourceInterface            -   Determines source local interface: Port/Trunk.
 *
 *                      Fields relevant to Loss Measurement
 *
 *   lmCounterCaptureEnable     -   Determines whether the OAM Loss Measurement
 *                                  counter is captured;
 *                                  GT_FALSE - disable loss measurement capturing;
 *                                  GT_TRUE - enable loss measurement capturing;
 *   dualEndedLmEnable          -   When enabled, OAM packets whose opcode
 *                                  is <Dual-Ended Loss Measurement Opcode>
 *                                  are treated as Loss Measurement packets;
 *                                  GT_FALSE - disable dual-ended
 *                                  loss measurement treatment;
 *                                  GT_TRUE - enable dual-ended
 *                                  loss measurement treatment;
 *   lmCountingMode             -   Determines which packets are counted by the LM counters.
 *
 *   oamPtpOffsetIndex          -   Relevant only for Egress OAM.
 *                                  An index to the OAM Offset Table,
 *                                  specifying the offset in the packet where
 *                                  the Loss Measurement/Delay Measurement counter
 *                                  is inserted into;
 *                                  (APPLICABLE RANGES: 0..127)
 *
 *                      Fields relevant to Delay Measurement
 *
 *   timestampEnable            -   When enabled, the current timestamp
 *                                  is copied  into the packet header,
 *                                  according to the configurable offset;
 *                                  GT_FALSE - disable OAM timestamping;
 *                                  GT_TRUE - enable OAM timestamping;
 *
 *                      Fields relevant to Keep Alive processing
 *
 *   keepaliveAgingEnable       -   Determines whether the current entry is
 *                                  subject to the aging mechanism;
 *                                  GT_FALSE - disable OAM keepalive aging;
 *                                  GT_TRUE - enable OAM keepalive aging;
 *   ageState                   -   Determines the age state of the current
 *                                  entry, i.e., the number of age periods
 *                                  that have passed since the last
 *                                  keepalive reception;
 *                                  (APPLICABLE RANGES: 0..15)
 *   agingPeriodIndex           -   This index selects one of the eight entries
 *                                  in the Aging Period Table;
 *                                  (APPLICABLE RANGES: 0..7)
 *   agingThreshold             -   The number of age periods that indicate
 *                                  loss of continuity;
 *                                  (APPLICABLE RANGES: 0..15)
 *   hashVerifyEnable -             If enabled, the correctness of the
 *                                  packet header is verified by comparing the
 *                                  packet hash[5:0] with the <Flow Hash> field;
 *                                  GT_FALSE - disable keepalive hash verification;
 *                                  GT_TRUE - enable keepalive hash verification;
 *   lockHashValueEnable        -   When enabled, the OAM engine does not change
 *                                  the value of the <Flow Hash> field.
 *                                  When disabled, and this OAN entry is
 *                                  accessed, the OAM engine updates the
 *                                  <Flow Hash> field, and sets the
 *                                  <Lock Hash Value Enable> field;
 *                                  GT_FALSE - disable hash value updating;
 *                                  GT_TRUE - enable hash value updating;
 *   protectionLocUpdateEnable  -   When enabled, the Aging Daemon updates the
 *                                  <OAM Protection LOC Status<n>> when an
 *                                  OAM flow<n> aged out;
 *                                  GT_FALSE - disable OAM Protection LOC Status<n>  updating;
 *                                  GT_TRUE - enable OAM Protection LOC Status<n>  updating;
 *   flowHash                   -   Determines the hash value of the flow;
 *                                  (APPLICABLE RANGES: 0..4095)
 *
 *   excessKeepaliveDetectionEnable - If enabled, the current entry is subject
 *                                  to excess keepalive detection;
 *                                  GT_FALSE - disable excess keepalive detection;
 *                                  GT_TRUE - enable excess keepalive detection;
 *   excessKeepalivePeriodCounter   - This counter is incremented by one
 *                                  each time the daemon updates this entry;
 *                                  (APPLICABLE RANGES: 0..15)
 *   excessKeepalivePeriodThreshold - Determines the number of Aging Periods
 *                                  used as the observation interval for
 *                                  Excess Keepalive detection;
 *                                  (APPLICABLE RANGES: 0..15)
 *   excessKeepaliveMessageCounter  - This counter is incremented each time a
 *                                  keepalive message arrives and this OAM entry
 *                                  is accessed;
 *                                  (APPLICABLE RANGES: 0..7)
 *   excessKeepaliveMessageThreshold- Determines the minimal number of
 *                                  keepalive messages during the observation
 *                                  interval that trigger an Excess event;
 *                                  (APPLICABLE RANGES: 0..7)
 *   rdiCheckEnable             -   When enabled, the packet’s RDI bit is
 *                                  compared to the <RDI Status<n>>;
 *                                  GT_FALSE - disable RDI bits comparing;
 *                                  GT_TRUE - enable RDI bits comparing;
 *   rdiStatus                  -   This bit contains the RDI status of the
 *                                  current OAM flow;
 *                                  0 - the peer switch has not detected a failure.
 *                                  1 - the peer switch has detected a failure.
 *   periodCheckEnable          -   When enabled, the packet’s Period field is
 *                                  compared to the <Keepalive TX Period> Field;
 *                                  GT_FALSE - disable packet's Period field checking;
 *                                  GT_TRUE - enable packet's Period field checking;
 *   keepaliveTxPeriod          -   The expected transmission period field;
 *                                  (APPLICABLE RANGES: 0..7)
 *
*/
typedef struct
{
    GT_BOOL opcodeParsingEnable;
    GT_BOOL megLevelCheckEnable;
    GT_U32  megLevel;
    GT_U32  packetCommandProfile;
    GT_U32  cpuCodeOffset;
    GT_BOOL sourceInterfaceCheckEnable;
    CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT sourceInterfaceCheckMode;
    CPSS_INTERFACE_INFO_STC  sourceInterface;
    GT_BOOL lmCounterCaptureEnable;
    GT_BOOL dualEndedLmEnable;
    CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT lmCountingMode;
    GT_U32  oamPtpOffsetIndex;
    GT_BOOL timestampEnable;
    GT_BOOL keepaliveAgingEnable;
    GT_U32  ageState;
    GT_U32  agingPeriodIndex;
    GT_U32  agingThreshold;
    GT_BOOL hashVerifyEnable;
    GT_BOOL lockHashValueEnable;
    GT_BOOL protectionLocUpdateEnable;
    GT_U32  flowHash;
    GT_BOOL excessKeepaliveDetectionEnable;
    GT_U32  excessKeepalivePeriodCounter;
    GT_U32  excessKeepalivePeriodThreshold;
    GT_U32  excessKeepaliveMessageCounter;
    GT_U32  excessKeepaliveMessageThreshold;
    GT_BOOL rdiCheckEnable;
    GT_U32  rdiStatus;
    GT_BOOL periodCheckEnable;
    GT_U32  keepaliveTxPeriod;
}CPSS_DXCH_OAM_ENTRY_STC;

/*
 *
 * typedef: enum CPSS_DXCH_OAM_STAGE_TYPE_ENT
 *
 * Description: Enumeration for OAM Unit Type.
 *
 * Enumerations:
 *    CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E - Ingress OAM stage.
 *    CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E - Egress OAM stage.
 */
typedef enum {
    CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
    CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E
} CPSS_DXCH_OAM_STAGE_TYPE_ENT;

/*
 *  typedef: enum CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT
 *
 *  Description: Enumeratiom of OAM exception types
 *
 *  Enumerations:
 *
 *      CPSS_DXCH_OAM_EXCEPTION_TYPE_MEG_LEVEL_E
 *                                      - MEG level check exception.
 *
 *      CPSS_DXCH_OAM_EXCEPTION_TYPE_SOURCE_INTERFACE_E
 *                                      - OAM source interface  exception.
 *
 *      CPSS_DXCH_OAM_EXCEPTION_TYPE_RDI_STATUS_CHANGED_E
 *                                      - Keepalive RDI exception.
 *
 *      CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_AGING_E
 *                                      - Keepalive aging exception.
 *
 *      CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_EXCESS_E
 *                                      - Keepalive excess exception.
 *
 *      CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_INVALID_HASH_E
 *                                      - Invalid keepalive exception.
 *
 *      CPSS_DXCH_OAM_EXCEPTION_TYPE_TX_PERIOD_E
 *                                      - TX period exception.
 *
 *      CPSS_DXCH_OAM_EXCEPTION_TYPE_SUMMARY_E
 *                                      - OAM summary exception.
 *
 *      CPSS_DXCH_OAM_EXCEPTION_TYPE_LAST_E
 *                                      - Last OAM exception enumerator.
 *
*/
typedef enum {
    CPSS_DXCH_OAM_EXCEPTION_TYPE_MEG_LEVEL_E = 0,
    CPSS_DXCH_OAM_EXCEPTION_TYPE_SOURCE_INTERFACE_E,
    CPSS_DXCH_OAM_EXCEPTION_TYPE_RDI_STATUS_CHANGED_E,
    CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_AGING_E,
    CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_EXCESS_E,
    CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_INVALID_HASH_E,
    CPSS_DXCH_OAM_EXCEPTION_TYPE_TX_PERIOD_E,
    CPSS_DXCH_OAM_EXCEPTION_TYPE_SUMMARY_E,
    CPSS_DXCH_OAM_EXCEPTION_TYPE_LAST_E
}CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT;

/*
 *  typedef: struct CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC
 *
 *  Description: Structure of OAM exception.
 *
 *  Fields:
 *      command             - OAM exception packet command.
 *      cpuCode             - OAM exception packet CPU code.
 *                            used for MIRROR_TO_CPU and TRAP packet commands.
 *      summaryBitEnable    - OAM exception summary bit.
 *                            GT_TRUE - exception status changing is included
 *                            in the OAM exception bitmap.
 *                            GT_FALSE - exception status changing is NOT included
 *                            in the OAM summary exception bitmap.
 *
*/
typedef struct {
    CPSS_PACKET_CMD_ENT         command;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode;
    GT_BOOL                     summaryBitEnable;
}CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC;

/*
 *  typedef: enum CPSS_DXCH_OAM_OPCODE_TYPE_ENT
 *
 *  Description: Structure of OAM opcode types
 *
 *  Enumerations:  OAM opcode types
 *
 *      CPSS_DXCH_OAM_OPCODE_TYPE_LM_COUNTED_E
 *                              - OAM Loss Measurement Counted Opcodes.
 *
 *      CPSS_DXCH_OAM_OPCODE_TYPE_LM_SINGLE_ENDED_E
 *                              - OAM Single-Ended Loss Measurement Opcodes.
 *
 *      CPSS_DXCH_OAM_OPCODE_TYPE_LM_DUAL_ENDED_E
 *                              - OAM Dual-Ended Loss Measurement Opcodes.
 *
 *      CPSS_DXCH_OAM_OPCODE_TYPE_DM_E
 *                              - OAM Delay Measurement Opcodes.
 *
 *      CPSS_DXCH_OAM_OPCODE_TYPE_KEEPALIVE_E
 *                              - OAM Keep Alive Opcodes.
 *
*/
typedef enum {
    CPSS_DXCH_OAM_OPCODE_TYPE_LM_COUNTED_E = 0,
    CPSS_DXCH_OAM_OPCODE_TYPE_LM_SINGLE_ENDED_E,
    CPSS_DXCH_OAM_OPCODE_TYPE_LM_DUAL_ENDED_E,
    CPSS_DXCH_OAM_OPCODE_TYPE_DM_E,
    CPSS_DXCH_OAM_OPCODE_TYPE_KEEPALIVE_E
}CPSS_DXCH_OAM_OPCODE_TYPE_ENT;

/*
 *  typedef: enum CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT
 *
 *  Description: Determines whether to update only entries that fail, or each entry.
 *
 *  Enumerations:
 *
 *      CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ONLY_FAILURES_E
 *                              - updates the OAM Keepalive Aging Bitmap
 *                              only when a flow fails.
 *
 *      CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ALL_E
 *                              - updates the OAM Keepalive Aging Bitmap,
 *                              including when a status is modified from "failed" to "okay".
 */
typedef enum {
    CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ONLY_FAILURES_E = 0,
    CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ALL_E
}CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT;

/* OAM group status size */
#define CPSS_DXCH_OAM_GROUP_STATUS_SIZE_IN_WORDS_CNS 2

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);


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
);

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
);


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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
    OUT GT_U32                                     *offsetPtr
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChOamh */

