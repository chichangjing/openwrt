/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTm2Tm.h
*
* DESCRIPTION:
*       TM to TM configuration. 
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssTm2Tmh
#define __cpssTm2Tmh

#include <cpss/generic/tm/cpssTmPublicDefs.h>


/*******************************************************************************
* cpssTm2TmGlobalConfig
*
* DESCRIPTION:
*       TM2TM Global Configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       cosSelector       - C level CoS DP selector (APPLICABLE RANGES: 0..7).
*       extHeadersPtr     - External Headers structure pointer.
*       ctrlPacketStrPtr  - Control Packet structure pointer.
*       rangePtr          - Delta Range Mapping to Priority structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTm2TmGlobalConfig
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          cosSelector,
    IN  CPSS_TM2TM_EXTERNAL_HDR_STC     *extHeadersPtr,
    IN  CPSS_TM2TM_CNTRL_PKT_STRUCT_STC *ctrlPacketStrPtr,
    IN  CPSS_TM2TM_DELTA_RANGE_STC      *rangePtr
);


/*******************************************************************************
* cpssTm2TmChannelPortConfig
*
* DESCRIPTION:
*       TM2TM Port Channel Configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx. 
* 
* INPUTS:
*       devNum            - Device number.
*       egressElements    - Number of Elements in Egress TMs.
*       srcLevel          - TM2TM Source Level (Port/C/B/A).
*       bpLevel           - BP Destination Level (C/Q).
*       bpOffset          - BP Base Offset.  
*       bpXon             - BP Xon Thresholds.
*       bpXoff            - BP Xoff Thresholds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTm2TmChannelPortConfig
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          egressElements,
    IN  CPSS_TM_LEVEL_ENT               srcLevel,
    IN  CPSS_TM_LEVEL_ENT               bpLevel,
    IN  GT_U32                          bpOffset,
    IN  GT_U32                          bpXon,
    IN  GT_U32                          bpXoff
);


/*******************************************************************************
* cpssTm2TmChannelNodeConfig
*
* DESCRIPTION:
*       TM2TM Node Channel Configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       egressElements    - Number of Elements in Egress TMs.
*       srcLevel          - TM2TM Source Level (Port/C/B/A).
*       bpLevel           - BP Destination Level (C/Q).
*       bpOffset          - BP Base Offset.  
*       bpXon             - BP Xon Thresholds.
*       bpXoff            - BP Xoff Thresholds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTm2TmChannelNodeConfig
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          egressElements,
    IN  CPSS_TM_LEVEL_ENT               srcLevel,
    IN  CPSS_TM_LEVEL_ENT               bpLevel,
    IN  GT_U32                          bpOffset,
    IN  GT_U32                          bpXon,
    IN  GT_U32                          bpXoff
);


/*******************************************************************************
* cpssTm2TmDpQueryResponceLevelSet
*
* DESCRIPTION:
*       Drop Query Response Select.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       dpType            - 0 - local only, 1 - remote only, 2 - both local & remote.
*       portDp            - 0 - Global, 1 - CoS.
*       localLevel        - Local response level (Q/A/B/C/Port).
*       remoteLevel       - Remote response level (Node/Port).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTm2TmDpQueryResponceLevelSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          dpType,
    IN  GT_U32                          portDp,
    IN  CPSS_TM_LEVEL_ENT               localLevel,
    IN  CPSS_TM2TM_CHANNEL_ENT          remoteLevel
);


/*******************************************************************************
* cpssTm2TmLcConfig
*
* DESCRIPTION:
*       TM2TM LC Configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       quantum           - Generation Data Quantum.
*       qmapHeader        - Qmap Header.
*       ctrlHeader        - Control Header (Line Card ID).
*       tmPort            - TM Port for Control Packets.  
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTm2TmLcConfig
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          quantum,
    IN  GT_U64                          qmapHeader,
    IN  GT_U32                          ctrlHeader,
    IN  GT_U32                          tmPort
);


/*******************************************************************************
* cpssTm2TmEgressDropAqmModeSet
*
* DESCRIPTION:
*       TM2TM Egress Drop AQM Mode Configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       level             - Level (Q/A/B/C/Port).
*       colorNum          - Number of colors for this level.
*       dpSourcePtr       - Array of DP sources for each color.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTm2TmEgressDropAqmModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_TM_LEVEL_ENT               level,
    IN  CPSS_TM_COLOR_NUM_ENT           colorNum,
    IN  CPSS_TM_DP_SOURCE_ENT           *dpSourcePtr
);


/*******************************************************************************
* cpssTm2TmIngressDropAqmModeSet
*
* DESCRIPTION:
*       TM2TM Ingress Drop AQM Mode Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       channel           - Channel (Node/Port).
*       colorNum          - Number of colors for this channel.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or color number.
*       GT_NOT_INITIALIZED       - on not configured channel at system setup.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTm2TmIngressDropAqmModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_TM2TM_CHANNEL_ENT          channel,
    IN  CPSS_TM_COLOR_NUM_ENT           colorNum
);


/*******************************************************************************
* cpssTm2TmEnable
*
* DESCRIPTION:
*       TM2TM Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTm2TmEnable
(
    IN  GT_U8                           devNum
);

#endif 	    /* __cpssTm2Tmh */

