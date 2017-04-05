/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpfix.h
*
* DESCRIPTION:
*       CPSS APIs declarations and definitions for IP Flow Information Export 
*       (IPFIX).
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/

#ifndef __cpssDxChIpfixh
#define __cpssDxChIpfixh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>

/* length of array to retrieve alarm events */
#define CPSS_DXCH_IPFIX_ALARM_EVENTS_ARRAY_LENGTH_CNS   16

/*
 * typedef: enum CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT
 *
 * Description: Enumeration for IPFIX sampling action.
 *
 * Enumerations:
 *  CPSS_DXCH_IPFIX_SAMPLING_ACTION_ALARM_E   - Alarm, each sampled packet will 
 *                                              trigger an interrupt to cpu.
 *  CPSS_DXCH_IPFIX_SAMPLING_ACTION_MIRROR_E  - Mirror, each sampled packet will
 *                                              change packet command to Mirror.
 */
typedef enum{
    CPSS_DXCH_IPFIX_SAMPLING_ACTION_ALARM_E,
    CPSS_DXCH_IPFIX_SAMPLING_ACTION_MIRROR_E
} CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT;

/*
 * typedef: enum CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT
 *
 * Description: Enumeration for IPFIX sampling distribution.
 *
 * Enumerations:
 *  CPSS_DXCH_IPFIX_SAMPLING_DIST_DETERMINISTIC_E - Deterministic, the random 
 *                                                  offset is deterministic and
 *                                                  equals zero. 
 *  CPSS_DXCH_IPFIX_SAMPLING_DIST_RANDOM_E        - Random, the random_offset 
 *                                                  is a unified distributed 
 *                                                  random number which is 
 *                                                  picked for each packet.
 */
typedef enum{
    CPSS_DXCH_IPFIX_SAMPLING_DIST_DETERMINISTIC_E,
    CPSS_DXCH_IPFIX_SAMPLING_DIST_RANDOM_E
} CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT;

/*
 * typedef: enum CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT
 *
 * Description: Enumeration for IPFIX sampling mode.
 *
 * Enumerations:
 *  CPSS_DXCH_IPFIX_SAMPLING_MODE_DISABLE_E   - Disable sampling.
 *  CPSS_DXCH_IPFIX_SAMPLING_MODE_PACKET_E    - Packet count based sampling.
 *  CPSS_DXCH_IPFIX_SAMPLING_MODE_BYTE_E      - Byte count based sampling.
 *  CPSS_DXCH_IPFIX_SAMPLING_MODE_TIME_E      - Time based sampling.
 */
typedef enum{
    CPSS_DXCH_IPFIX_SAMPLING_MODE_DISABLE_E,
    CPSS_DXCH_IPFIX_SAMPLING_MODE_PACKET_E,
    CPSS_DXCH_IPFIX_SAMPLING_MODE_BYTE_E,
    CPSS_DXCH_IPFIX_SAMPLING_MODE_TIME_E
} CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT
 *
 * Description: Enumeration for IPFIX drop counter selected mode.
 *
 * Enumerations:
 *  CPSS_DXCH_IPFIX_DROP_COUNT_MODE_METER_ONLY_E - The drop counter is updated
 *                                                 only for packets dropped due
 *                                                 to the Policer engine. 
 *  CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ALL_E        - The drop counter is updated
 *                                                 for drops up to and 
 *                                                 including the Policer engine.
 */
typedef enum{
    CPSS_DXCH_IPFIX_DROP_COUNT_MODE_METER_ONLY_E,
    CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ALL_E
} CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT
 *
 * Description: Enumeration for IPFIX counter action when wraparound occurs.
 *
 * Enumerations:
 *  CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_NONE_E   - Freeze the counter at the 
 *                                               threshold value and stop 
 *                                               performing any updates. 
 *  CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_CLEAR_E  - Subtract the threshold value
 *                                               from the current value of the
 *                                               counter and continue updates.
 */
typedef enum{
    CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_NONE_E,
    CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_CLEAR_E
} CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT;

/*
 * typedef: enum CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT
 *
 * Description: Enumeration for loading of IPFIX time stamp method.
 *
 * Enumerations:
 *  CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E    - IPfix timer is 
 *                                                        uploaded to the exact 
 *                                                        value of time stamp 
 *                                                        upload registers. 
 *  CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E - IPfix timer is 
 *                                                        incremented by the 
 *                                                        value of time stamp
 *                                                        upload registers.
 */
typedef enum{
    CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E,
    CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E
} CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_IPFIX_ENTRY_STC
 *
 * Description:
 *      The IPFIX entry.
 *
 * Fields:
 *      timeStamp           - Time stamp from RTC counter. 16 bits.
 *      packetCount         - packets counter. 30 bits.
 *      byteCount           - bytes counter. 36 bits.
 *      dropCounter         - Drop packet counter. 30 bits.
 *      randomOffset        - Picked random value after each sampling. 32 bits.
 *      lastSampledValue    - Once the packet is sampled, it contains the 
 *                            summary of the previous lastSampledValue + 
 *                            samplingWindow. 36 bits.
 *      samplingWindow      - Window used for Reference value calculation. The
 *                            units of this window are according to 
 *                            samplingMode. 36 bits.
 *      samplingAction      - Command done on sampled packets.
 *      logSamplingRange    - Defines the distribution range of the 
 *                            randomOffset. Relevant only when randomFlag is 
 *                            random. 6 bits.
 *      randomFlag          - Determines the random offset used for the 
 *                            reference value calculation.
 *      samplingMode        - Sampling base method.
 *      cpuSubCode          - 2 lsb bits of the cpu code for sampled packets.
 *                            Relevant only when samplingAction is Mirror.
 * Comment:
 */
typedef struct
{
    GT_U32                              timeStamp;
    GT_U32                              packetCount;
    GT_U64                              byteCount;
    GT_U32                              dropCounter;
    GT_U32                              randomOffset;
    GT_U64                              lastSampledValue;
    GT_U64                              samplingWindow;
    CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT samplingAction;
    GT_U32                              logSamplingRange;
    CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT   randomFlag;
    CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT   samplingMode;
    GT_U32                              cpuSubCode;
} CPSS_DXCH_IPFIX_ENTRY_STC;

/*
 * typedef: struct CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC
 *
 * Description:
 *      IPFIX wraparound configuration.
 *
 * Fields:
 *      action          - The action when a counter reaches its 
 *                        threshold value.
 *      dropThreshold   - Drop packet counter threshold. 30 bits.
 *      packetThreshold - Packets counter threshold. 30 bits.
 *      byteThreshold   - Bytes counter threshold. 36 bits. This Threshold 
 *                        minimal allowed value is 0x40000 and 18 LSBs should
 *                        be 0.
 *
 * Comment:
 */
typedef struct
{
    CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT   action;
    GT_U32                                  dropThreshold;
    GT_U32                                  packetThreshold;
    GT_U64                                  byteThreshold;
} CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC;

/*
 * typedef: struct CPSS_DXCH_IPFIX_TIMER_STC
 *
 * Description:
 *      IPFIX timer.
 *
 * Fields:
 *      nanoSecondTimer - IPFIX nano second timer. 30 bits.
 *      secondTimer     - IPFIX second timer. 64 bits.
 *
 * Comment:
 */
typedef struct
{
    GT_U32  nanoSecondTimer;
    GT_U64  secondTimer;
} CPSS_DXCH_IPFIX_TIMER_STC;

/*
 * typedef: struct CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC
 *
 * Description:
 *      IPFIX timestamp upload configuration.
 *
 * Fields:
 *      uploadMode  - timestamp upload mode.
 *      timer       - IPFIX timer structure.
 *
 * Comment:
 */
typedef struct
{
    CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT  uploadMode;
    CPSS_DXCH_IPFIX_TIMER_STC                  timer;
} CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC;

/*******************************************************************************
* cpssDxChIpfixEntrySet
*
* DESCRIPTION:
*       Sets IPFIX entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of IPFIX Entry.
*                         Range: see datasheet for specific device.
*       ipfixEntryPtr   - pointer to the IPFIX Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_OUT_OF_RANGE             - on value out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In order to set IPFIX entry under traffic perform the following
*       algorithm(none-zero setting of IPFIX Counters while enabled\still 
*       under traffic may cause data coherency problems):
*           - disconnect entry from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entry back to traffic
*       This API address the same memory area (counters place) as can be set
*       by "cpssDxCh3PolicerBillingEntrySet".
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixEntrySet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_IPFIX_ENTRY_STC           *ipfixEntryPtr
);

/*******************************************************************************
* cpssDxChIpfixEntryGet
*
* DESCRIPTION:
*       Gets IPFIX entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex  - index of IPFIX Entry.
*                     Range: see datasheet for specific device.
*       reset       - reset flag (please refer to COMMENTS section for further
*                     explanations):
*                     GT_TRUE  - performing read and reset atomic operation.
*                     GT_FALSE - performing read entry operation only.
*
* OUTPUTS:
*       ipfixEntryPtr   - pointer to the IPFIX Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_BAD_STATE                - on counter entry type mismatch.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       When counters are read in clear on read mode (reset == GT_TRUE), The 
*       following actions are done on the different IPFIX entry fields:
*       - The <Packet Counter>, <Byte Counter>, and <Dropped Packet Counter> 
*         fields are cleared.
*       - The <Timestamp> field is updated to the current time.
*       - The <Last Sampled Value> is cleared.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixEntryGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                              entryIndex,
    IN  GT_BOOL                             reset,
    OUT CPSS_DXCH_IPFIX_ENTRY_STC           *ipfixEntryPtr
);

/*******************************************************************************
* cpssDxChIpfixDropCountModeSet
*
* DESCRIPTION:
*       Configures IPFIX drop counting mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode    - drop counting mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixDropCountModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT      mode
);

/*******************************************************************************
* cpssDxChIpfixDropCountModeGet
*
* DESCRIPTION:
*       Gets IPFIX drop counting mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr - pointer to drop counting mode.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixDropCountModeGet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    OUT CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT     *modePtr
);

/*******************************************************************************
* cpssDxChIpfixCpuCodeSet
*
* DESCRIPTION:
*       Sets the 6 most significant bits of the CPU Code in case of mirroring. 
*       The two least significant bits are taken from the IPfix entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       cpuCode - A cpu code to set the 6 most significant bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or cpu code.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixCpuCodeSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN CPSS_NET_RX_CPU_CODE_ENT         cpuCode
);

/*******************************************************************************
* cpssDxChIpfixCpuCodeGet
*
* DESCRIPTION:
*       Gets the 6 most significant bits of the CPU Code in case of mirroring. 
*       The two least significant bits are taken from the IPfix entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       cpuCodePtr  - pointer to a cpu code to set the 6 most significant bits.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixCpuCodeGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    OUT CPSS_NET_RX_CPU_CODE_ENT        *cpuCodePtr
);

/*******************************************************************************
* cpssDxChIpfixAgingEnableSet
*
* DESCRIPTION:
*       Enables or disabled the activation of aging for IPfix.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       enable  - GT_TRUE for enable, GT_FALSE for disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixAgingEnableSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN GT_BOOL                          enable
);

/*******************************************************************************
* cpssDxChIpfixAgingEnableGet
*
* DESCRIPTION:
*       Gets enabling status of aging for IPfix.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       enablePtr   - pointer to: GT_TRUE for enable, GT_FALSE for disable.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixAgingEnableGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_BOOL                         *enablePtr
);

/*******************************************************************************
* cpssDxChIpfixWraparoundConfSet
*
* DESCRIPTION:
*       Configures IPFIX wraparound parameters.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       confPtr   - pointer to wraparound configuration structure.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, action or threshold.
*       GT_OUT_OF_RANGE             - on value out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Since there is a HW limitation that wraparound byte threshold 18 LSBs 
*       must be set to 0, the function ignores the 18 LSBs input value and set
*       them to 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixWraparoundConfSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC       *confPtr
);

/*******************************************************************************
* cpssDxChIpfixWraparoundConfGet
*
* DESCRIPTION:
*       Get IPFIX wraparound configuration.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       confPtr   - pointer to wraparound configuration structure.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixWraparoundConfGet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    OUT CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC      *confPtr
);

/*******************************************************************************
* cpssDxChIpfixWraparoundStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of wraparound entries indexes.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       startIndex  - the first entry index to get wraparound indication on.
*       endIndex    - the last entry index to get wraparound indication on.
*       reset       - clear the bits after read: 
*                     GT_TRUE - clear,
*                     GT_FALSE - don't clear.
*
*
* OUTPUTS:
*       bmpPtr      - pointer to a bitmap indication wraparound.
*                     each bit indicate: 0 - no WA occured, 1 - WA occured.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       It is application responsibility to allocate the buffer size for the
*       bitmap(bmpPtr). Since the buffer is made from GT_U32 elements, each
*       element contains 32 indications.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixWraparoundStatusGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                           startIndex,
    IN GT_U32                           endIndex,
    IN GT_BOOL                          reset,
    OUT GT_U32                          *bmpPtr
);

/*******************************************************************************
* cpssDxChIpfixAgingStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of aged entries indexes.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       startIndex  - the first entry index to get aging indication on.
*       endIndex    - the last entry index to get aging indication on.
*       reset       - clear the bits after read: 
*                     GT_TRUE - clear,
*                     GT_FALSE - don't clear.
*
*
* OUTPUTS:
*       bmpPtr      - pointer to a bitmap indication aging.
*                     each bit indicate: 0 - no packet recieved on flow (aged).
*                                        1 - packet recieved on flow.
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       It is application responsibility to allocate the buffer size for the
*       bitmap(bmpPtr). Since the buffer is made from GT_U32 elements, each
*       element contains 32 indications.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixAgingStatusGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                           startIndex,
    IN GT_U32                           endIndex,
    IN GT_BOOL                          reset,
    OUT GT_U32                          *bmpPtr
);

/*******************************************************************************
* cpssDxChIpfixTimestampUploadSet
*
* DESCRIPTION:
*       Configures IPFIX timestamp upload per stages.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       uploadPtr   - pointer to timestamp upload configuration of a stage.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or upload mode.
*       GT_OUT_OF_RANGE             - on value out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Actual upload operation is triggered by 
*       "cpssDxChIpfixTimestampUploadTrigger".
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampUploadSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    IN CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC    *uploadPtr
);

/*******************************************************************************
* cpssDxChIpfixTimestampUploadGet
*
* DESCRIPTION:
*       Gets IPFIX timestamp upload configuration for a stage.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       uploadPtr   - pointer to timestamp upload configuration of a stage.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampUploadGet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    OUT CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC   *uploadPtr
);

/*******************************************************************************
* cpssDxChIpfixTimestampUploadTrigger
*
* DESCRIPTION:
*       Triggers the IPFIX timestamp upload operation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_BAD_STATE                - if previous upload operation is not 
*                                     finished yet.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       CPSS_DXCH_POLICER_STAGE_INGRESS_0_E triggering controls 
*       CPSS_DXCH_POLICER_STAGE_INGRESS_1 & CPSS_DXCH_POLICER_STAGE_EGRESS_E 
*       as well, all uploads are done simultenously.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampUploadTrigger
(
    IN GT_U8    devNum
);

/*******************************************************************************
* cpssDxChIpfixTimestampUploadStatusGet
*
* DESCRIPTION:
*       Gets IPFIX timestamp upload operation status.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       uploadStatusPtr - pointer to indication whether upload was done.
*                         GT_TRUE: upload is finished.
*                         GT_FALSE: upload still in progess.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampUploadStatusGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *uploadStatusPtr
);

/*******************************************************************************
* cpssDxChIpfixTimerGet
*
* DESCRIPTION:
*       Gets IPFIX timer
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       timerPtr    - pointer to IPFIX timer.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimerGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    OUT CPSS_DXCH_IPFIX_TIMER_STC       *timerPtr
);

/*******************************************************************************
* cpssDxChIpfixAlarmEventsGet
*
* DESCRIPTION:
*       Retrieves up to 16 IPFIX entries indexes where alarm events (due to
*       sampling) occured.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum       - physical device number.
*       stage        - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
*
* OUTPUTS:
*       eventsArr    - array of alarm events, each valid element contains
*                      the index of IPFIX entry which caused the alarm.
*       eventsNumPtr - The number of valid entries in eventsPtr list.
*                      (APPLICABLE RANGES: 0..15)
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixAlarmEventsGet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_U32                          eventsArr[CPSS_DXCH_IPFIX_ALARM_EVENTS_ARRAY_LENGTH_CNS],
    OUT GT_U32                          *eventsNumPtr
);

/*******************************************************************************
* cpssDxChIpfixPortGroupEntrySet
*
* DESCRIPTION:
*       Sets IPFIX entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is 
*                            IGNORED.
*                         2. for multi-port groups device :
*                         bitmap must be set with at least one bit representing
*                         valid port group(s). If a bit of non valid port group
*                         is set then function returns GT_BAD_PARAM.
*                         value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of IPFIX Entry.
*                         Range: see datasheet for specific device.
*       ipfixEntryPtr   - pointer to the IPFIX Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_OUT_OF_RANGE             - on value out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In order to set IPFIX entry under traffic perform the following
*       algorithm(none-zero setting of IPFIX Counters while enabled\still 
*       under traffic may cause data coherency problems):
*           - disconnect entry from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entry back to traffic
*       This API address the same memory area (counters place) as can be set
*       by "cpssDxCh3PolicerBillingEntrySet".
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixPortGroupEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_IPFIX_ENTRY_STC           *ipfixEntryPtr
);

/*******************************************************************************
* cpssDxChIpfixPortGroupEntryGet
*
* DESCRIPTION:
*       Gets IPFIX entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is 
*                            IGNORED.
*                         2. for multi-port groups device :
*                         bitmap must be set with at least one bit representing
*                         valid port group(s). If a bit of non valid port group
*                         is set then function returns GT_BAD_PARAM.
*                         value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of IPFIX Entry.
*                         Range: see datasheet for specific device.
*       reset           - reset flag (please refer to COMMENTS section for further
*                         explanations):
*                         GT_TRUE  - performing read and reset atomic operation.
*                         GT_FALSE - performing read entry operation only.
*
* OUTPUTS:
*       ipfixEntryPtr   - pointer to the IPFIX Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_BAD_STATE                - on counter entry type mismatch.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       When counters are read in clear on read mode (reset == GT_TRUE), The 
*       following actions are done on the different IPFIX entry fields:
*       - The <Packet Counter>, <Byte Counter>, and <Dropped Packet Counter> 
*         fields are cleared.
*       - The <Timestamp> field is updated to the current time.
*       - The <Last Sampled Value> is cleared.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixPortGroupEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                              entryIndex,
    IN  GT_BOOL                             reset,
    OUT CPSS_DXCH_IPFIX_ENTRY_STC           *ipfixEntryPtr
);

/*******************************************************************************
* cpssDxChIpfixPortGroupWraparoundStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of wraparound entries indexes.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is 
*                            IGNORED.
*                         2. for multi-port groups device :
*                         bitmap must be set with at least one bit representing
*                         valid port group(s). If a bit of non valid port group
*                         is set then function returns GT_BAD_PARAM.
*                         value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       startIndex      - the first entry index to get wraparound indication on.
*       endIndex        - the last entry index to get wraparound indication on.
*       reset           - clear the bits after read: 
*                         GT_TRUE - clear,
*                         GT_FALSE - don't clear.
*
*
* OUTPUTS:
*       bmpPtr      - pointer to a bitmap indication wraparound.
*                     each bit indicate: 0 - no WA occured, 1 - WA occured.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       It is application responsibility to allocate the buffer size for the
*       bitmap(bmpPtr). Since the buffer is made from GT_U32 elements, each
*       element contains 32 indications.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixPortGroupWraparoundStatusGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                           startIndex,
    IN GT_U32                           endIndex,
    IN GT_BOOL                          reset,
    OUT GT_U32                          *bmpPtr
);

/*******************************************************************************
* cpssDxChIpfixPortGroupAgingStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of aged entries indexes.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is 
*                            IGNORED.
*                         2. for multi-port groups device :
*                         bitmap must be set with at least one bit representing
*                         valid port group(s). If a bit of non valid port group
*                         is set then function returns GT_BAD_PARAM.
*                         value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       startIndex      - the first entry index to get aging indication on.
*       endIndex        - the last entry index to get aging indication on.
*       reset           - clear the bits after read: 
*                         GT_TRUE - clear,
*                         GT_FALSE - don't clear.
*
*
* OUTPUTS:
*       bmpPtr      - pointer to a bitmap indication aging.
*                     each bit indicate: 0 - no packet recieved on flow (aged).
*                                        1 - packet recieved on flow.
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       It is application responsibility to allocate the buffer size for the
*       bitmap(bmpPtr). Since the buffer is made from GT_U32 elements, each
*       element contains 32 indications.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixPortGroupAgingStatusGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                           startIndex,
    IN GT_U32                           endIndex,
    IN GT_BOOL                          reset,
    OUT GT_U32                          *bmpPtr
);

/*******************************************************************************
* cpssDxChIpfixPortGroupAlarmEventsGet
*
* DESCRIPTION:
*       Retrieves up to 16 IPFIX entries indexes where alarm events (due to
*       sampling) occured.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is
*                            IGNORED.
*                         2. for multi-port groups device :
*                         bitmap must be set with at least one bit representing
*                         valid port group(s). If a bit of non valid port group
*                         is set then function returns GT_BAD_PARAM.
*                         value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
*
* OUTPUTS:
*       eventsArr    - array of alarm events, each valid element contains
*                      the index of IPFIX entry which caused the alarm.
*       eventsNumPtr - The number of valid entries in eventsPtr list.
*                      (APPLICABLE RANGES: 0..15)
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixPortGroupAlarmEventsGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_U32                          eventsArr[CPSS_DXCH_IPFIX_ALARM_EVENTS_ARRAY_LENGTH_CNS],
    OUT GT_U32                          *eventsNumPtr
);

/*******************************************************************************
* cpssDxChIpfixTimestampToCpuEnableSet
*
* DESCRIPTION:
*       Configures all TO_CPU DSA tags to include a Timestamp.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       enable  - GT_TRUE: TO_CPU DSA tag contains a Timestamp.
*                 GT_FALSE: TO_CPU DSA tag does not contain a Timestamp.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampToCpuEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChIpfixTimestampToCpuEnableGet
*
* DESCRIPTION:
*       Gets enabling status of TO_CPU DSA tags including a Timestamp.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE: TO_CPU DSA tag contains a Timestamp.
*                    GT_FALSE: TO_CPU DSA tag does not contain a Timestamp.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpfixTimestampToCpuEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChIpfixh */


