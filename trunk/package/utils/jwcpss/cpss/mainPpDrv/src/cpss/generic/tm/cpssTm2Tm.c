/********************* **********************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTm2Tm.c
*
* DESCRIPTION:
*       TM2TM Configuration Library APIs
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/tm/private/prvCpssGenTmLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/tm/cpssTmServices.h>
#include <cpss/generic/tm/cpssTm2Tm.h>
#include <tm2tm.h>
#include <tm_defs.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>


/*******************************************************************************
* internal_cpssTm2TmGlobalConfig
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
static GT_STATUS internal_cpssTm2TmGlobalConfig
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          cosSelector,
    IN  CPSS_TM2TM_EXTERNAL_HDR_STC     *extHeadersPtr,
    IN  CPSS_TM2TM_CNTRL_PKT_STRUCT_STC *ctrlPacketStrPtr,
    IN  CPSS_TM2TM_DELTA_RANGE_STC      *rangePtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    struct tm_ext_hdr str1;
    struct tm_ctrl_pkt_str str2;
    struct tm_delta_range str3;
 	/* copy header data - 32 bytes */

	cpssOsMemCpy(str1.content, extHeadersPtr->contentArr, sizeof(char)*32);
	
    str1.size = (uint8_t)extHeadersPtr->size;
    str2.nodes = (uint8_t)ctrlPacketStrPtr->nodesNum;
    str2.ports = (uint8_t)ctrlPacketStrPtr->portsNum;
    str3.upper_range0 = (uint8_t)rangePtr->upperRange0;
    str3.upper_range1 = (uint8_t)rangePtr->upperRange1;
    str3.upper_range2 = (uint8_t)rangePtr->upperRange2;
    ret = tm2tm_global_config(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                              (uint8_t)cosSelector,
                              &str1,
                              &str2,
                              &str3);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;   
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTm2TmGlobalConfig);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cosSelector, extHeadersPtr, ctrlPacketStrPtr, rangePtr));

    rc = internal_cpssTm2TmGlobalConfig(devNum, cosSelector, extHeadersPtr, ctrlPacketStrPtr, rangePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cosSelector, extHeadersPtr, ctrlPacketStrPtr, rangePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTm2TmChannelPortConfig
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
static GT_STATUS internal_cpssTm2TmChannelPortConfig
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          egressElements,
    IN  CPSS_TM_LEVEL_ENT               srcLevel,
    IN  CPSS_TM_LEVEL_ENT               bpLevel,
    IN  GT_U32                          bpOffset,
    IN  GT_U32                          bpXon,
    IN  GT_U32                          bpXoff
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    ret = tm2tm_port_channel_config(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                    (uint8_t)egressElements,
                                    srcLevel,
                                    1, /* TM2TM_BP */
                                    bpLevel,
                                    (uint16_t)bpOffset,
                                    (uint8_t)bpXon,
                                    (uint8_t)bpXoff);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;    
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTm2TmChannelPortConfig);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, egressElements, srcLevel, bpLevel, bpOffset, bpXon, bpXoff));

    rc = internal_cpssTm2TmChannelPortConfig(devNum, egressElements, srcLevel, bpLevel, bpOffset, bpXon, bpXoff);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, egressElements, srcLevel, bpLevel, bpOffset, bpXon, bpXoff));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTm2TmChannelNodeConfig
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
static GT_STATUS internal_cpssTm2TmChannelNodeConfig
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          egressElements,
    IN  CPSS_TM_LEVEL_ENT               srcLevel,
    IN  CPSS_TM_LEVEL_ENT               bpLevel,
    IN  GT_U32                          bpOffset,
    IN  GT_U32                          bpXon,
    IN  GT_U32                          bpXoff
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    ret = tm2tm_node_channel_config(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                    (uint16_t)egressElements,
                                    srcLevel,
                                    1, /* TM2TM_BP */
                                    bpLevel,
                                    (uint16_t)bpOffset,
                                    (uint8_t)bpXon,
                                    (uint8_t)bpXoff);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;    
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTm2TmChannelNodeConfig);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, egressElements, srcLevel, bpLevel, bpOffset, bpXon, bpXoff));

    rc = internal_cpssTm2TmChannelNodeConfig(devNum, egressElements, srcLevel, bpLevel, bpOffset, bpXon, bpXoff);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, egressElements, srcLevel, bpLevel, bpOffset, bpXon, bpXoff));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTm2TmDpQueryResponceLevelSet
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
static GT_STATUS internal_cpssTm2TmDpQueryResponceLevelSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          dpType,
    IN  GT_U32                          portDp,
    IN  CPSS_TM_LEVEL_ENT               localLevel,
    IN  CPSS_TM2TM_CHANNEL_ENT          remoteLevel
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    ret = tm_set_dp_query_resp_lvl(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                   (uint8_t)dpType,
                                   (uint8_t)portDp,
                                   localLevel,
                                   remoteLevel);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTm2TmDpQueryResponceLevelSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dpType, portDp, localLevel, remoteLevel));

    rc = internal_cpssTm2TmDpQueryResponceLevelSet(devNum, dpType, portDp, localLevel, remoteLevel);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dpType, portDp, localLevel, remoteLevel));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTm2TmLcConfig
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
static GT_STATUS internal_cpssTm2TmLcConfig
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          quantum,
    IN  GT_U64                          qmapHeader,
    IN  GT_U32                          ctrlHeader,
    IN  GT_U32                          tmPort
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    ret = tm2tm_lc_config(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                          (uint16_t)quantum,
                          &qmapHeader,
                          (uint8_t)ctrlHeader,
                          (uint8_t)tmPort); 
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTm2TmLcConfig);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, quantum, qmapHeader, ctrlHeader, tmPort));

    rc = internal_cpssTm2TmLcConfig(devNum, quantum, qmapHeader, ctrlHeader, tmPort);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, quantum, qmapHeader, ctrlHeader, tmPort));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTm2TmEgressDropAqmModeSet
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
static GT_STATUS internal_cpssTm2TmEgressDropAqmModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_TM_LEVEL_ENT               level,
    IN  CPSS_TM_COLOR_NUM_ENT           colorNum,
    IN  CPSS_TM_DP_SOURCE_ENT           *dpSourcePtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
    enum tm_dp_source src = *dpSourcePtr;

    ret = tm2tm_set_egress_drop_aqm_mode(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                         level,
                                         colorNum,
                                         &src);   
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTm2TmEgressDropAqmModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, colorNum, dpSourcePtr));

    rc = internal_cpssTm2TmEgressDropAqmModeSet(devNum, level, colorNum, dpSourcePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, colorNum, dpSourcePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTm2TmIngressDropAqmModeSet
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
static GT_STATUS internal_cpssTm2TmIngressDropAqmModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_TM2TM_CHANNEL_ENT          channel,
    IN  CPSS_TM_COLOR_NUM_ENT           colorNum
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    ret = tm2tm_set_ingress_drop_aqm_mode(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, channel, colorNum);   
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTm2TmIngressDropAqmModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, channel, colorNum));

    rc = internal_cpssTm2TmIngressDropAqmModeSet(devNum, channel, colorNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, channel, colorNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTm2TmEnable
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
static GT_STATUS internal_cpssTm2TmEnable
(
    IN  GT_U8                           devNum
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    ret = tm2tm_enable(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle);   
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTm2TmEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssTm2TmEnable(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
