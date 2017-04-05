/*******************************************************************************H
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChStc.h
*
* DESCRIPTION:
*       CPSS DxCh Sampling To CPU (STC) APIs
*
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/


#ifndef __cpssDxChStch
#define __cpssDxChStch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/********* include ************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>

/*
 * typedef: enum CPSS_DXCH_STC_COUNT_MODE_ENT
 *
 * Description: Enum for Sampling To CPU (STC) Count mode.
 *
 * Enumerations:
 *      CPSS_DXCH_STC_COUNT_ALL_PACKETS_E - All packets received without any
 *                                            MAC-level error are subject to
 *                                            Sampling To CPU (STC).
 *      CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E - Only non-dropped packets
 *                                            are subject to Sampling To CPU (STC)
 */

typedef enum
{
    CPSS_DXCH_STC_COUNT_ALL_PACKETS_E,
    CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E
} CPSS_DXCH_STC_COUNT_MODE_ENT;


/*
 * typedef: enum CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT
 *
 * Description: Enum for Sampling To CPU (STC) count reload mode.
 *
 * Enumerations:
 *      CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E - The port ingress/egress STC
 *                  mechanism reloads the sampling frequency value that was last
 *                  configured by the CPU.
 *      CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E - The port ingress/egress STC
 *                  mechanism only reloads a new sampling frequency value if it
 *                  was updated by the CPU since the previous reload operation.
 *                  If the CPU did not update the frequency value since the
 *                  previous reload, sampling is halted until a new frequency
 *                  value is updated by the CPU.
 *                  When a new sampling frequency is loaded into the port
 *                  internal counter, a CPU interrupt is generated, indicating
 *                  that a new port sampling frequency can be configured, to be
 *                  used on the next internal counter reload. Alternatively,
 *                  the CPU can read a status register field, to learn whether
 *                  the last sampling frequency has been loaded into the
 *                  internal counter.
 *
 */

typedef enum
{
    CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E,
    CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E
} CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT;


/*
 * typedef: enum CPSS_DXCH_STC_TYPE_ENT
 *
 * Description: Enum for Sampling To CPU (STC) type.
 *
 * Enumerations:
 *      CPSS_DXCH_STC_INGRESS_E - Ingress Sampling To CPU (STC)
 *      CPSS_DXCH_STC_EGRESS_E - Egress Sampling To CPU (STC)
 */
typedef enum
{
    CPSS_DXCH_STC_INGRESS_E,
    CPSS_DXCH_STC_EGRESS_E
} CPSS_DXCH_STC_TYPE_ENT;


/*******************************************************************************
* cpssDxChStcIngressCountModeSet
*
* DESCRIPTION:
*       Set the type of packets subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       mode      - CPSS_DXCH_STC_COUNT_ALL_PACKETS_E - All packets without
*                                                         any MAC-level errors.
*                   CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E -
*                                                       only non-dropped packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcIngressCountModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_STC_COUNT_MODE_ENT    mode
);

/*******************************************************************************
* cpssDxChStcIngressCountModeGet
*
* DESCRIPTION:
*       Get the type of packets that are subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       modePtr   - CPSS_DXCH_STC_COUNT_ALL_PACKETS_E = All packets without
*                                                           any MAC-level errors.
*                   CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E -
*                                                       only non-dropped packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcIngressCountModeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_STC_COUNT_MODE_ENT    *modePtr
);

/*******************************************************************************
* cpssDxChStcReloadModeSet
*
* DESCRIPTION:
*       Set the type of Sampling To CPU (STC) count reload mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       mode      - Sampling to CPU (STC) Reload mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,STC type or mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcReloadModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT                  stcType,
    IN  CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT     mode
);

/*******************************************************************************
* cpssDxChStcReloadModeGet
*
* DESCRIPTION:
*       Get the type of Sampling To CPU (STC) count reload mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       modePtr   - (pointer to) Sampling to CPU (STC) Reload mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcReloadModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT                  stcType,
    OUT CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT     *modePtr
);

/*******************************************************************************
* cpssDxChStcEnableSet
*
* DESCRIPTION:
*       Global Enable/Disable Sampling To CPU (STC).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       enable    - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcEnableSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChStcEnableGet
*
* DESCRIPTION:
*       Get the global status of Sampling To CPU (STC) (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       enablePtr - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcEnableGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_BOOL                     *enablePtr
);

/*******************************************************************************
* cpssDxChStcPortLimitSet
*
* DESCRIPTION:
*       Set Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       limit     - Count Down Limit (APPLICABLE RANGES: 0..0x3FFFFFFF)
*                   when limit value is 1 - every packet is sampled to CPU
*                   when limit value is 0 - there is no sampling
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - limit is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortLimitSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_U32                      limit
);

/*******************************************************************************
* cpssDxChStcPortLimitGet
*
* DESCRIPTION:
*       Get Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       limitPtr  - pointer to Count Down Limit
*                   when limit value is 1 - every packet is sampled to CPU
*                   when limit value is 0 - there is no sampling
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortLimitGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *limitPtr
);

/*******************************************************************************
* cpssDxChStcPortReadyForNewLimitGet
*
* DESCRIPTION:
*       Check whether Sampling to CPU is ready to get new STC Limit Value per
*       port.
*       The function cpssDxChStcPortLimitSet sets new limit value.
*       But only after device finishes handling of new limit value the
*       cpssDxChStcPortLimitSet may be called once more.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       isReadyPtr - (pointer to) Is Ready value
*                    GT_TRUE = Sampling to CPU ready to get new STC Limit Value
*                    GT_FALSE = Sampling to CPU handles STC Limit Value yet and
*                               is not ready to get new value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortReadyForNewLimitGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_BOOL                     *isReadyPtr
);

/*******************************************************************************
* cpssDxChStcPortCountdownCntrGet
*
* DESCRIPTION:
*       Get STC Countdown Counter per port.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Count down counter. This is the number of
*                   packets left to send/receive in order that a packet will be
*                   sampled to CPU and a new value will be loaded.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortCountdownCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *cntrPtr
);

/*******************************************************************************
* cpssDxChStcPortSampledPacketsCntrSet
*
* DESCRIPTION:
*       Set the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       cntr      - STC Sampled to CPU packet's counter (APPLICABLE RANGES: 0..0xFFFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_OUT_OF_RANGE          - on wrong cntr
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortSampledPacketsCntrSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_U32                      cntr
);

/*******************************************************************************
* cpssDxChStcPortSampledPacketsCntrGet
*
* DESCRIPTION:
*       Get the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Sampled to CPU packet's counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortSampledPacketsCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *cntrPtr
);

/*******************************************************************************
* cpssDxChStcEgressAnalyzerIndexSet
*
* DESCRIPTION:
*       Set the egress STC analyzer index.
*
*       feature behavior:
*       If a packet is marked for both egress STC and egress mirroring , the
*       packet is only replicated once, depending on a configurable priority
*       between the mechanisms. The configurable <STC analyzer index> determines
*       the priority of STC compared to mirroring to the analyzer.
*       Note:
*       1. that STC does not actually use this index to determine the target
*          of the mirrored packet.
*       2. the indexes used by egress mirroring engines are in range 0..6.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable analyzer index for egress STC.
*                  - GT_TRUE - enable analyzer index.
*                  - GT_FALSE - no analyzer index.
*      index     - Analyzer index. (APPLICABLE RANGES: 0..14)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function uses same parameters as mirroring functions like
*       cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet(devNum,enable,index).
*
*******************************************************************************/
GT_STATUS cpssDxChStcEgressAnalyzerIndexSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
);

/*******************************************************************************
* cpssDxChStcEgressAnalyzerIndexGet
*
* DESCRIPTION:
*       Get the egress STC analyzer index.
*
*       feature behavior:
*       If a packet is marked for both egress STC and egress mirroring , the
*       packet is only replicated once, depending on a configurable priority
*       between the mechanisms. The configurable <STC analyzer index> determines
*       the priority of STC compared to mirroring to the analyzer.
*       Note:
*       1. that STC does not actually use this index to determine the target
*          of the mirrored packet.
*       2. the indexes used by egress mirroring engines are in range 0..6.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*      devNum    - device number.
*
*
* OUTPUTS:
*      enablePtr    - (pointer to) global enable/disable analyzer index for egress STC.
*                  - GT_TRUE - enable analyzer index.
*                  - GT_FALSE - no analyzer index.
*      indexPtr     - (pointer to) Analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function uses same parameters as mirroring functions like
*       cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet(devNum,enablePtr,indexPtr).
*
*******************************************************************************/
GT_STATUS cpssDxChStcEgressAnalyzerIndexGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChStch */


