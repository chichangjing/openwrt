/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmDrop.h
*
* DESCRIPTION:
*       TM Drop Unit configuration, including WRED curves and Drop Profiles. 
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#ifndef __cpssTmDroph
#define __cpssTmDroph

#include <cpss/generic/tm/cpssTmPublicDefs.h>
#include <cpss/generic/cos/cpssCosTypes.h>


#define     CPSS_TM_AGING_PROFILES_CNS  4


/*******************************************************************************
* cpssTmDropWredCurveCreate
*
* DESCRIPTION:
*       Create a WRED curve for a TM level.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - A nodes level the WRED curve is created for (Port/C/B/A/Q).
*       cos             - CoS of RED Curve (APPLICABLE RANGES: 0..7).
*       probabilityArr  - Array of 32 probability points in % (APPLICABLE RANGES: 0..100).
*
* OUTPUTS:
*       curveIndexPtr        - (pointer to) The created WRED curve index (APPLICABLE RANGES: 0..7).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL curveIndPtr.
*       GT_FULL                  - on full allocation of WRED Curves table.
*       GT_NOT_INITIALIZED       - on not configured AQM Mode params for this lvl.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. The CoS parameter is relevant for C and P level only, else set CPSS_TM_INVAL_CNS.
*       2. For P level in Global mode set 'cos' = CPSS_TM_INVAL_CNS, else curve will be created for CoS mode.
*******************************************************************************/
GT_STATUS cpssTmDropWredCurveCreate
(
    IN  GT_U8               devNum,
    IN  CPSS_TM_LEVEL_ENT   level,
    IN  GT_U32              cos,
    IN  GT_U32              probabilityArr[CPSS_TM_DROP_PROB_ARR_SIZE_CNS],
    OUT GT_U32              *curveIndexPtr
);


/*******************************************************************************
* cpssTmDropWredTraditionalCurveCreate
*
* DESCRIPTION:
*       Create a WRED traditional curve for a level.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - A nodes level the WRED curve is created for (Port/C/B/A/Q).
*       cos             - CoS of RED Curve (APPLICABLE RANGES: 0..7).
*       maxProbability  - Max probability in percents (APPLICABLE RANGES: 1..100).
*       
* OUTPUTS:
*       curveIndexPtr        - (pointer to) The created WRED curve index (APPLICABLE RANGES: 0..7).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_FULL                  - on full allocation of WRED Curves table.
*       GT_BAD_PTR               - on NULL curveIndPtr.
*       GT_NOT_INITIALIZED       - on not configured AQM Mode params for this lvl.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. The CoS parameter is relevant for C and P level only, else set CPSS_TM_INVAL_CNS.
*       2. For P level in Global mode set 'cos' = CPSS_TM_INVAL_CNS, else curve will be created for CoS mode.
*******************************************************************************/
GT_STATUS cpssTmDropWredTraditionalCurveCreate
(
    IN  GT_U8               devNum,
    IN  CPSS_TM_LEVEL_ENT   level,
    IN  GT_U32              cos,
    IN  GT_U32              maxProbability,
    OUT GT_U32              *curveIndexPtr
);


/*******************************************************************************
* cpssTmDropProfileCreate
*
* DESCRIPTION:
*       Create Drop profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum                - Device number.
*       level                 - A nodes level the Drop profile is created for (Port/C/B/A/Q).
*       cos                   - CoS of Drop profile (APPLICABLE RANGES: 0..7).
*       profileStcPtr         - (pointer to) Drop Profile configuration structure.
*       
* OUTPUTS:
*       profileIndexPtr            - (pointer to) The created Drop profile index
*                                    (APPLICABLE RANGES: 0..2047  for Queues,
*                                                        0..255   for Anodes,
*                                                        0..63    for Bnodes,Cnodes,
*                                                        0..number of ports for Ports).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL profileIndPtr or profileStcPtr.
*       GT_FULL                  - on full allocation of level's Drop profile table.
*       GT_NOT_INITIALIZED       - on not configured AQM Mode params for this lvl.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. In case of Color Blind TD disabled set cbTdThresholdBytes=CPSS_TM_MAX_DROP_THRESHOLD_CNS
*       2. Cos of Drop Profile matches Cos of given curve.
*       3. The CoS parameter is relevant for C and P level only, else set CPSS_TM_INVAL_CNS.
*       4. For P level in Global mode set 'cos' = CPSS_TM_INVAL_CNS, else profile will be created for CoS mode.
*******************************************************************************/
GT_STATUS cpssTmDropProfileCreate
(
    IN  GT_U8                           devNum,
    IN  CPSS_TM_LEVEL_ENT               level,
    IN  GT_U32                          cos,
    IN  CPSS_TM_DROP_PROFILE_PARAMS_STC *profileStcPtr,
    OUT GT_U32                          *profileIndexPtr
);

/*******************************************************************************
* cpssTmDropProfileUpdate
*
* DESCRIPTION:
*       Update Drop profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum                - Device number.
*       level                 - A nodes level the Drop profile is created for (Port/C/B/A/Q).
*       cos                   - CoS of Drop profile (APPLICABLE RANGES: 0..7).
*       profileIndex          - Drop profile index to update
*                                    (APPLICABLE RANGES: 0..2047  for Queues,
*                                                        0..255   for Anodes,
*                                                        0..63    for Bnodes,Cnodes,
*                                                        0..number of ports for Ports).
*       profileStcPtr         - (pointer to) Drop Profile configuration structure.
*       
* OUTPUTS:
*		none
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL profileStcPtr.
*       GT_FULL                  - on full allocation of level's Drop profile table.
*       GT_NOT_INITIALIZED       - on not configured AQM Mode params for this lvl.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. In case of Color Blind TD disabled set cbTdThresholdBytes=CPSS_TM_MAX_DROP_THRESHOLD_CNS
*       2. Cos of Drop Profile matches Cos of given curve.
*       3. The CoS parameter is relevant for C and P level only, else set CPSS_TM_INVAL_CNS.
*       4. For P level in Global mode set 'cos' = CPSS_TM_INVAL_CNS, else profile will be created for CoS mode.
*		5. For Q/A/B/C levels the user is aware to pause traffic during update process
*       6. For P level update is possible only if profile is not in use.
*******************************************************************************/
GT_STATUS cpssTmDropProfileUpdate
(
    IN  GT_U8                           devNum,
    IN  CPSS_TM_LEVEL_ENT               level,
    IN  GT_U32                          cos,
    IN  GT_U32                          profileIndex,
    IN  CPSS_TM_DROP_PROFILE_PARAMS_STC *profileStcPtr
);


/*******************************************************************************
* cpssTmDropConvertBytesToBW
*
* DESCRIPTION:
*		Utility function  :     Converts Tail drop  threshold in bytes to   kBits/sec .
*
* INPUTS:
*       GT_U32			cbTdBytes	 tail drop threshold value in bytes.
* RETURNS:
*       GT_U32			tail drop threshold value in  kBits/sec  units.
*
* COMMENTS:
*       none
*******************************************************************************/
GT_U32 cpssTmDropConvertBytesToBW
(
    IN  GT_U32                          cbTdBytes
);


/*******************************************************************************
* cpssTmDropConvertBWToBytes
*
* DESCRIPTION:
*		Utility function  :     Converts Tail drop  in kkBits/sec units to Bytes.
*
* INPUTS:
*       GT_U32			cbTdkBitsPerSec	 tail drop threshold value in kBits/sec  units.
* RETURNS:
*       GT_U32			tail drop threshold value in  bytes.
*
* COMMENTS:
*       none
*******************************************************************************/
GT_U32 cpssTmDropConvertBWToBytes
(
    IN  GT_U32                          cbTdkBitsPerSec
);



/*******************************************************************************
* cpssTmDropProfileDelete
*
* DESCRIPTION:
*       Delete Drop profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum              - Device number.
*       level               - A nodes level the Drop profile is deleted for (Port/C/B/A/Q).
*       cos                 - CoS of Drop profile (APPLICABLE RANGES: 0..7).
*       profile             - An index of the Drop profile.
*                                    (APPLICABLE RANGES: 0..2047  for Queues,
*                                                        0..255   for Anodes,
*                                                        0..63    for Bnodes,Cnodes,
*                                                        0..number of ports for Ports).
*       
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. The CoS parameter is relevant for C and P level only, else set CPSS_TM_INVAL_CNS.
*       2. For P level in Global mode set 'cos' = CPSS_TM_INVAL_CNS, else profile will be deleted for CoS mode.
*******************************************************************************/
GT_STATUS cpssTmDropProfileDelete
(
    IN GT_U8                            devNum,
    IN CPSS_TM_LEVEL_ENT                level,
    IN GT_U32                           cos,
    IN GT_U32                   		profile
);


/*******************************************************************************
* cpssTmDropProfileRead
*
* DESCRIPTION:
*       Read Drop profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum              - Device number.
*       level               - A nodes level the Drop profile is read for (Port/C/B/A/Q).
*       cos                 - CoS of Drop profile (APPLICABLE RANGES: 0..7).
*       profileInd          - An index of the Drop profile.
*                                    (APPLICABLE RANGES: 0..2047  for Queues,
*                                                        0..255   for Anodes,
*                                                        0..63    for Bnodes,Cnodes,
*                                                        0..number of ports for Ports).
*       
* OUTPUTS:
*       profileStcPtr       - (pointer to) Drop profile configuration structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL profileStcPtr.
*       GT_NOT_INITIALIZED       - on not configured AQM Mode params for this lvl.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. The cbTdThresholdRatio is calculated aproximately from the register's values.
*       2. The CoS parameter is relevant for C and P level only, else set CPSS_TM_INVAL_CNS.
*       3. For P level in Global mode set 'cos' = CPSS_TM_INVAL_CNS, else profile will be read for CoS mode.
*******************************************************************************/
GT_STATUS cpssTmDropProfileRead
(
    IN GT_U8                            devNum,
    IN CPSS_TM_LEVEL_ENT                level,
    IN GT_U32                           cos,
    IN GT_U32                           profileInd,
    OUT CPSS_TM_DROP_PROFILE_PARAMS_STC *profileStcPtr
);


/*******************************************************************************
* cpssTmDropColorNumSet
*
* DESCRIPTION:
*       Set Drop (Egress) Colors number per level.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - A nodes level to set colors number for (Port/C/B/A/Q).
*       number          - Colors amount.
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*   1. This API should be called before all the rest Drop APIs (if need).
*   2. By default there are two colors per each level.
*******************************************************************************/
GT_STATUS cpssTmDropColorNumSet
(
    IN GT_U8                    devNum,
    IN CPSS_TM_LEVEL_ENT        level,
    IN CPSS_TM_COLOR_NUM_ENT    number
);

/*******************************************************************************
* cpssTmDropColorNumResolutionGet
*
* DESCRIPTION:
*       Retrieves Drop (Egress) Colors number and  curve resolution birs count per level.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - A nodes level to set colors number for (Port/C/B/A/Q).
*       colorNumPtr		- (pointer to) variable  where color number will be assigned.
*		resolutionPtr   - (pointer to) variable  where  curve resolution  will be assigned.
* OUTPUTS:
*		None.
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL colorNumPtr or resolutionPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*   None.
*******************************************************************************/

GT_STATUS cpssTmDropColorNumResolutionGet
(
    IN GT_U8                    devNum,
    IN CPSS_TM_LEVEL_ENT        level,
    OUT GT_U8 *					colorNumPtr,
    OUT GT_U8 *					resolutionPtr
);

/*******************************************************************************
* cpssTmAgingChangeStatus
*
* DESCRIPTION:
*       Change the aging status.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       status          - Aging status.
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
GT_STATUS cpssTmAgingChangeStatus
(
    IN GT_U8         devNum,
    IN GT_U32        status
);

/*******************************************************************************
* cpssTmAgingStatusGet
*
* DESCRIPTION:
*       Retrieve the aging status.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number. 
* OUTPUTS:
*       statusPtr       - (pointer to) Aging status.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL statusPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
GT_STATUS cpssTmAgingStatusGet
(
    IN  GT_U8         devNum,
    OUT GT_U32        *statusPtr
);


/*******************************************************************************
* cpssTmDpSourceSet
*
* DESCRIPTION:
*       Change Drop Probability (DP) source.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - A nodes level to set dp source for (APPLICABLE RANGES: CPSS_TM_LEVEL_Q_E..CPSS_TM_LEVEL_P_E).
*       color           - A color to set dp source for (APPLICABLE RANGES: 0..2).
*       source          - Dp source (APPLICABLE RANGES: CPSS_TM_DP_SOURCE_AQL_E..CPSS_TM_DP_SOURCE_QL_E).
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
GT_STATUS cpssTmDpSourceSet
(
    IN GT_U8                   devNum,
    IN CPSS_TM_LEVEL_ENT       level,
    IN CPSS_DP_LEVEL_ENT       color,
    IN CPSS_TM_DP_SOURCE_ENT   source
);

/*******************************************************************************
* cpssTmDpSourceGet
*
* DESCRIPTION:
*       Change Drop Probability (DP) source.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - A nodes level to set dp source for (APPLICABLE RANGES: CPSS_TM_LEVEL_Q_E..CPSS_TM_LEVEL_P_E).
*       color           - A color to set dp source for (APPLICABLE RANGES: 0..2).
* OUTPUTS:
*       sourcePtr       - (pointer to)Dp source.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL sourcePtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
GT_STATUS cpssTmDpSourceGet
(
    IN GT_U8                   devNum,
    IN CPSS_TM_LEVEL_ENT       level,
    IN CPSS_DP_LEVEL_ENT       color,
    OUT CPSS_TM_DP_SOURCE_ENT  *sourcePtr
);

/*******************************************************************************
* cpssTmDropQueryResponceSet
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
*       devNum          - Device number.
*       portDp          - 0 - Global, 1 - CoS.
*       level           - Local response level (APPLICABLE RANGES: CPSS_TM_LEVEL_Q_E..CPSS_TM_LEVEL_P_E).
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
GT_STATUS cpssTmDropQueryResponceSet
(
    IN GT_U8                   devNum,
    IN GT_U32                  portDp,
    IN CPSS_TM_LEVEL_ENT       level
);

/*******************************************************************************
* cpssTmDropQueryResponceGet
*
* DESCRIPTION:
*       Retrieve Drop Query Response state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
* OUTPUTS:
*       portDpPtr       - (pointer to) PortDp.
*       levelPtr        - (pointer to) Local response level.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL portDpPtr or levelPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
GT_STATUS cpssTmDropQueryResponceGet
(
    IN GT_U8                     devNum,
    OUT GT_U32                  *portDpPtr,
    OUT CPSS_TM_LEVEL_ENT       *levelPtr
);

/*******************************************************************************
* cpssTmDropQueueCosSet
*
* DESCRIPTION:
*       Drop Queue Cos Select.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Queue index (APPLICABLE RANGES: 0..16383).
*       cos             - Cos (APPLICABLE RANGES: 0..7).
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
GT_STATUS cpssTmDropQueueCosSet
(
    IN GT_U8                   devNum,
    IN GT_U32                  index,
    IN GT_U32                  cos
);

/*******************************************************************************
* cpssTmDropQueueCosGet
*
* DESCRIPTION:
*       Retrieve Drop Queue Cos.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Queue index (APPLICABLE RANGES: 0..16383).
* OUTPUTS:
*       cosPtr          - (pointer to)CoS.
* 
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
GT_STATUS cpssTmDropQueueCosGet
(
    IN GT_U8                   devNum,
    IN GT_U32                  index,
    OUT GT_U32                 *cosPtr
);

/*******************************************************************************
* cpssTmDropProfileAgingBlockCreate
*
* DESCRIPTION:
*       Create Aging drop profile for CPSS_TM_LEVEL_Q_E.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum                - Device number.
*       profileStcArr         - array of 4 Aging Profile configuration structure pointer.
*       
* OUTPUTS:
*       profileIndexPtr       - (pointer to) The first created Aging profile index (APPLICABLE RANGES: 0..2044).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL profileIndexPtr.
*       GT_FULL                  - on full allocation of level's Drop profile table.
*       GT_NOT_INITIALIZED       - on not configured AQM Mode params for this lvl.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. In case of Color Blind TD disabled set cbTdThresholdBytes=CPSS_TM_MAX_DROP_THRESHOLD_CNS
*       2. Cos of Drop Profile matches Cos of given curve.
*       3. The CoS parameter is relevant for C and P level only, else set CPSS_TM_INVAL_CNS.
*       4. For P level in Global mode set 'cos' = CPSS_TM_INVAL_CNS, else profile will be created for CoS mode.
*******************************************************************************/
GT_STATUS cpssTmDropProfileAgingBlockCreate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_TM_DROP_PROFILE_PARAMS_STC         profileStcArr[CPSS_TM_AGING_PROFILES_CNS],
    OUT GT_U32                                  *profileIndexPtr
);

/*******************************************************************************
* cpssTmDropProfileAgingUpdate
*
* DESCRIPTION:
*       Update Aging Drop profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum              - Device number.
*       profileIndex        - The first created Aging profile index (APPLICABLE RANGES: 0..2044).
*       profileOffset       - The drop profile index to be update (APPLICABLE RANGES: 0..CPSS_TM_AGING_BLOCK_SIZE).
*       profileParamsPtr    - (pointer to)Drop Profile configurations structure.
*        
*       
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL profileParamsPtr.
*       GT_FULL                  - on full allocation of level's Drop profile table.
*       GT_NOT_INITIALIZED       - on not configured AQM Mode params for this lvl.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssTmDropProfileAgingUpdate
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  profileIndex,
    IN  GT_U32                                  profileOffset,
    IN  CPSS_TM_DROP_PROFILE_PARAMS_STC         *profileParamsPtr
);


/*******************************************************************************
* cpssTmDropProfileAgingBlockDelete
*
* DESCRIPTION:
*       Delete Aging profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum              - Device number.
*       profileIndex        - The index of the first Aging profile (APPLICABLE RANGES: 0..2044).
*       
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmDropProfileAgingBlockDelete
(
    IN GT_U8                    devNum,
    IN GT_U32                   profileIndex
);

/*******************************************************************************
* cpssTmNodeDropProfileIndexRead
*
* DESCRIPTION:
*  As a result of aging process the queue drop profile index can be changed dynamically 
* internally by TM. This function allows to monitor these changes.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum             - Device number.
*       level              - Scheduling level: Queue/A/B/C-node/Port.
*       cos                - cos  of drop profile (APPLICABLE RANGES: 0..7 for port/C-level, otherwise ignored ).
*       nodeIndex          - The index of queue to read its drop profile.
* OUTPUTS:
*       profileIndex       - (pointer to) The drop profile currently active for the nodeIndex.
*                                    (APPLICABLE RANGES: 0..2047  for Queues,
*                                                        0..255   for Anodes,
*                                                        0..63    for Bnodes,Cnodes,
*                                                        0..number of ports for Ports).
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL profileIndexPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The CoS parameter is relevant for C and P level (Global mode) only, else set CPSS_TM_INVAL_CNS.
*******************************************************************************/
GT_STATUS cpssTmNodeDropProfileIndexRead  
(
    IN GT_U8                    devNum,
    IN CPSS_TM_LEVEL_ENT        level,
    IN GT_U32                   cos,
    IN GT_U32                   nodeIndex,
    OUT GT_U32                  *profileIndexPtr
);

#endif 	    /* __cpssTmDroph */

