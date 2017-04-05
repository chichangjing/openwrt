/********************* **********************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmDrop.c
*
* DESCRIPTION:
*       TM Configuration Library Drop APIs
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/tm/private/prvCpssGenTmLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/tm/cpssTmServices.h>
#include <cpss/generic/tm/cpssTmDrop.h>
#include <cpssCommon/cpssPresteraDefs.h>

#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <tm_drop.h>
#include <errno.h>

/*******************************************************************************
* internal_cpssTmDropWredCurveCreate
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
*       curveIndexPtr   - (pointer to) The created WRED curve index (APPLICABLE RANGES: 0..7).
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
static GT_STATUS internal_cpssTmDropWredCurveCreate
(
    IN  GT_U8                   devNum,
    IN  CPSS_TM_LEVEL_ENT       level,
    IN  GT_U32                  cos,
    IN  GT_U32                  probabilityArr[CPSS_TM_DROP_PROB_ARR_SIZE_CNS],
    OUT GT_U32                  *curveIndexPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    uint8_t prob_t[32];
    uint8_t curve_index;
    int i;

    CPSS_NULL_PTR_CHECK_MAC(curveIndexPtr);

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    for(i = 0; i < 32; i++)
        prob_t[i] = (uint8_t)probabilityArr[i];
    ret = tm_create_wred_curve(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                               level,
                               (uint8_t)cos,
                               prob_t,
                               &curve_index);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;

    *curveIndexPtr = curve_index;
    return rc;
}

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
*       curveIndexPtr        - (pointer to) The created WRED curve index.
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
GT_STATUS cpssTmDropWredCurveCreate
(
    IN  GT_U8                   devNum,
    IN  CPSS_TM_LEVEL_ENT       level,
    IN  GT_U32                  cos,
    IN  GT_U32                  probabilityArr[CPSS_TM_DROP_PROB_ARR_SIZE_CNS],
    OUT GT_U32                  *curveIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropWredCurveCreate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, cos, probabilityArr, curveIndexPtr));

    rc = internal_cpssTmDropWredCurveCreate(devNum, level, cos, probabilityArr, curveIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, cos, probabilityArr, curveIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmDropWredTraditionalCurveCreate
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
*       curveIndexPtr   - (pointer to) The created WRED curve index.
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
static GT_STATUS internal_cpssTmDropWredTraditionalCurveCreate
(
    IN  GT_U8                   devNum,
    IN  CPSS_TM_LEVEL_ENT       level,
    IN  GT_U32                  cos,
    IN  GT_U32                  maxProbability,
    OUT GT_U32                  *curveIndexPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    uint8_t curve_index;

    CPSS_NULL_PTR_CHECK_MAC(curveIndexPtr);

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_create_wred_traditional_curve(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                           level,
                                           (uint8_t)cos,
                                           (float)maxProbability,
                                           &curve_index);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;

    *curveIndexPtr = curve_index;
    return rc;
}

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
*       curveIndexPtr   - (pointer to) The created WRED curve index (APPLICABLE RANGES: 0..7).
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
GT_STATUS cpssTmDropWredTraditionalCurveCreate
(
    IN  GT_U8                   devNum,
    IN  CPSS_TM_LEVEL_ENT       level,
    IN  GT_U32                  cos,
    IN  GT_U32                  maxProbability,
    OUT GT_U32                  *curveIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropWredTraditionalCurveCreate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, cos, maxProbability, curveIndexPtr));

    rc = internal_cpssTmDropWredTraditionalCurveCreate(devNum, level, cos, maxProbability, curveIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, cos, maxProbability, curveIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmDropProfileCreate
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
*       profileStcPtr         - Drop Profile configuration structure pointer.
*
* OUTPUTS:
*       profileIndexPtr            - (pointer to) The created Drop profile index
*                                    (APPLICABLE RANGES: 0..2048  for Queues,
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
static GT_STATUS internal_cpssTmDropProfileCreate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_TM_LEVEL_ENT                       level,
    IN  GT_U32                                  cos,
    IN  CPSS_TM_DROP_PROFILE_PARAMS_STC         *profileStcPtr,
    OUT GT_U32                                  *profileIndexPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    struct tm_drop_profile_params prf;
    uint16_t prof_index;
    CPSS_DP_LEVEL_ENT i;

    CPSS_NULL_PTR_CHECK_MAC(profileStcPtr);
    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(profileStcPtr);


	prf.cbtdBytes = profileStcPtr->cbTdThresholdBytes;
    switch(profileStcPtr->dropMode)
	{
		case CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E:
			prf.drop_mode=WRED ;
			prf.wredDp.aqlExponent  = profileStcPtr->caWredDp.aqlExponent;
			for (i = CPSS_DP_GREEN_E; i <= CPSS_DP_RED_E; i++)
			{
				prf.wredDp.curveIndex[i] = profileStcPtr->caWredDp.curveIndex[i];
				prf.wredDp.dpCurveScale[i] = profileStcPtr->caWredDp.dpCurveScale[i];
				prf.wredDp.caWredTdMinThreshold[i] = profileStcPtr->caWredDp.caWredTdMinThreshold[i];
				prf.wredDp.caWredTdMaxThreshold[i] = profileStcPtr->caWredDp.caWredTdMaxThreshold[i];
 			}
			break;
		case CPSS_TM_DROP_MODE_CB_TD_CA_TD_E:
			prf.drop_mode=CATD ;
			for (i = CPSS_DP_GREEN_E; i <= CPSS_DP_RED_E; i++)
			{
				prf.caTdDp.catdBytes[i]=profileStcPtr->caTdDp.caTdThreshold[i];
			}
			break;
		case CPSS_TM_DROP_MODE_CB_TD_ONLY_E:
			prf.drop_mode=DISABLED ;
			break;
		default:
			/* invalid mode  */
			return XEL_TO_CPSS_ERR_CODE(-ENODATA);
	}

    ret = tm_create_drop_profile(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                 level,
                                 (uint8_t)cos,
                                 &prf,
                                 &prof_index);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;

    *profileIndexPtr = prof_index;
    return rc;
}

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
*       profileStcPtr         - Drop Profile configuration structure pointer.
*
* OUTPUTS:
*       profileIndexPtr            - (pointer to) The created Drop profile index
*                                    (APPLICABLE RANGES: 0..2048  for Queues,
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
    IN  GT_U8                                   devNum,
    IN  CPSS_TM_LEVEL_ENT                       level,
    IN  GT_U32                                  cos,
    IN  CPSS_TM_DROP_PROFILE_PARAMS_STC         *profileStcPtr,
    OUT GT_U32                                  *profileIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropProfileCreate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, cos, profileStcPtr, profileIndexPtr));

    rc = internal_cpssTmDropProfileCreate(devNum, level, cos, profileStcPtr, profileIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, cos, profileStcPtr, profileIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssTmDropProfileUpdate
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
*       profileIndex          - Drop profile index to update.
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
static GT_STATUS internal_cpssTmDropProfileUpdate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_TM_LEVEL_ENT                       level,
    IN  GT_U32                                  cos,
    IN  GT_U32									profileIndex,
    IN  CPSS_TM_DROP_PROFILE_PARAMS_STC			*profileStcPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    struct tm_drop_profile_params prf;
    uint16_t prof_index  = (uint16_t)profileIndex;
    CPSS_DP_LEVEL_ENT i;

    CPSS_NULL_PTR_CHECK_MAC(profileStcPtr);

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

   CPSS_NULL_PTR_CHECK_MAC(profileStcPtr);


	prf.cbtdBytes = profileStcPtr->cbTdThresholdBytes;
    switch(profileStcPtr->dropMode)
	{
		case CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E:
			prf.drop_mode=WRED ;
			prf.wredDp.aqlExponent  = profileStcPtr->caWredDp.aqlExponent;
			for (i = CPSS_DP_GREEN_E; i <= CPSS_DP_RED_E; i++)
			{
				prf.wredDp.curveIndex[i] = profileStcPtr->caWredDp.curveIndex[i];
				prf.wredDp.dpCurveScale[i] = profileStcPtr->caWredDp.dpCurveScale[i];
				prf.wredDp.caWredTdMinThreshold[i] = profileStcPtr->caWredDp.caWredTdMinThreshold[i];
				prf.wredDp.caWredTdMaxThreshold[i] = profileStcPtr->caWredDp.caWredTdMaxThreshold[i];
 			}
			break;
		case CPSS_TM_DROP_MODE_CB_TD_CA_TD_E:
			prf.drop_mode=CATD ;
			for (i = CPSS_DP_GREEN_E; i <= CPSS_DP_RED_E; i++)
			{
				prf.caTdDp.catdBytes[i]=profileStcPtr->caTdDp.caTdThreshold[i];
			}
			break;
		case CPSS_TM_DROP_MODE_CB_TD_ONLY_E:
			prf.drop_mode=DISABLED ;
			break;
		default:
			/* invalid mode  */
			return XEL_TO_CPSS_ERR_CODE(-ENODATA);
	}

    ret = tm_update_drop_profile(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                 level,
                                 (uint8_t)cos,
                                 prof_index,
								 &prf);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    return rc;
}

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
    IN  GT_U8                                   devNum,
    IN  CPSS_TM_LEVEL_ENT                       level,
    IN  GT_U32                                  cos,
    IN  GT_U32									profileIndex,
    IN  CPSS_TM_DROP_PROFILE_PARAMS_STC			*profileStcPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropProfileCreate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, cos,  profileIndex, profileStcPtr));

    rc = internal_cpssTmDropProfileUpdate(devNum, level, cos, profileIndex, profileStcPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, cos, profileIndex, profileStcPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}




/*******************************************************************************
* internal_cpssTmDropProfileDelete
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
*       profile             - An index of the Drop profile
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
static GT_STATUS internal_cpssTmDropProfileDelete
(
    IN GT_U8                    devNum,
    IN CPSS_TM_LEVEL_ENT        level,
    IN GT_U32                   cos,
    IN GT_U32                   profile
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_delete_drop_profile(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                 level,
                                 (uint8_t)cos,
                                 (uint16_t)profile);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
*       profile             - An index of the Drop profile
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
    IN GT_U8                    devNum,
    IN CPSS_TM_LEVEL_ENT        level,
    IN GT_U32                   cos,
    IN GT_U32                   profile
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropProfileDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, cos, profile));

    rc = internal_cpssTmDropProfileDelete(devNum, level, cos, profile);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, cos, profile));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmDropProfileRead
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
static GT_STATUS internal_cpssTmDropProfileRead
(
    IN GT_U8                                devNum,
    IN CPSS_TM_LEVEL_ENT                    level,
    IN GT_U32                               cos,
    IN GT_U32                               profileInd,
    OUT CPSS_TM_DROP_PROFILE_PARAMS_STC     *profileStcPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    struct tm_drop_profile_params prf;
    CPSS_DP_LEVEL_ENT i;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(profileStcPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(profileStcPtr);


    ret = tm_read_drop_profile(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                               level,
                               (uint8_t)cos,
                               (uint16_t)profileInd,
                               &prf);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;
	profileStcPtr->cbTdThresholdBytes = prf.cbtdBytes;
    switch(prf.drop_mode)
	{
		case WRED:
			profileStcPtr->dropMode = CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E;
			profileStcPtr->caWredDp.aqlExponent = prf.wredDp.aqlExponent;
			for (i = CPSS_DP_GREEN_E; i <= CPSS_DP_RED_E; i++)
			{
				profileStcPtr->caWredDp.curveIndex[i] = prf.wredDp.curveIndex[i];
				profileStcPtr->caWredDp.dpCurveScale[i] = prf.wredDp.dpCurveScale[i] ;
				profileStcPtr->caWredDp.caWredTdMinThreshold[i] = prf.wredDp.caWredTdMinThreshold[i];
				profileStcPtr->caWredDp.caWredTdMaxThreshold[i] = prf.wredDp.caWredTdMaxThreshold[i];
			}
			break;
		case CATD:
			profileStcPtr->dropMode = CPSS_TM_DROP_MODE_CB_TD_CA_TD_E;
			for (i = CPSS_DP_GREEN_E; i <= CPSS_DP_RED_E; i++)
			{
				profileStcPtr->caTdDp.caTdThreshold[i] = prf.caTdDp.catdBytes[i];
			}
			break;
		case DISABLED:
		default:
			profileStcPtr->dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
		break;
	}
    return rc;
}



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
    IN GT_U8                                devNum,
    IN CPSS_TM_LEVEL_ENT                    level,
    IN GT_U32                               cos,
    IN GT_U32                               profileInd,
    OUT CPSS_TM_DROP_PROFILE_PARAMS_STC     *profileStcPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropProfileRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, cos, profileInd, profileStcPtr));

    rc = internal_cpssTmDropProfileRead(devNum, level, cos, profileInd, profileStcPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, cos, profileInd, profileStcPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmDropColorNumSet
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
static GT_STATUS internal_cpssTmDropColorNumSet
(
    IN GT_U8                        devNum,
    IN CPSS_TM_LEVEL_ENT            level,
    IN CPSS_TM_COLOR_NUM_ENT        number
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );
    ret = tm_set_drop_color_num(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, level, number);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;

}

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
    IN GT_U8                        devNum,
    IN CPSS_TM_LEVEL_ENT            level,
    IN CPSS_TM_COLOR_NUM_ENT        number
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropColorNumSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, number));

    rc = internal_cpssTmDropColorNumSet(devNum, level, number);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, number));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmDropColorNumResolutionGet
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
* OUTPUTS:
*       colorNumPtr		- pointer to variable  where color number will be assigned.
*		resolutionPtr   - pointer to variable  where  curve resolution  will be assigned.
*
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

static GT_STATUS internal_cpssTmDropColorNumResolutionGet
(
    IN GT_U8                    devNum,
    IN CPSS_TM_LEVEL_ENT        level,
    OUT GT_U8 *					colorNumPtr,
    OUT GT_U8 *					resolutionPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
	enum tm_color_num	color_num;
	uint8_t				resolution;

    PRV_CPSS_DEV_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(colorNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(resolutionPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );
    ret = tm_get_drop_color_num_and_resolution(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, level, &color_num , &resolution);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
	if (rc==0)
	{
		*colorNumPtr	= (GT_U8)color_num+1;
		*resolutionPtr	= (GT_U8)resolution;
	}
    return rc;

}


/*******************************************************************************
* cpssTmDropColorNumResolutionGet
*
* DESCRIPTION:
*       Retrieves Drop (Egress) Colors number and  curve resolution birst count per level.
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
* OUTPUTS:
*       colorNumPtr		- pointer to variable  where color number will be assigned.
*		resolutionPtr   - pointer to variable  where  curve resolution  will be assigned.
*
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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropColorNumResolutionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, colorNumPtr, resolutionPtr));

    rc = internal_cpssTmDropColorNumResolutionGet(devNum, level, colorNumPtr , resolutionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, colorNumPtr , resolutionPtr ));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssTmAgingChangeStatus
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
static GT_STATUS internal_cpssTmAgingChangeStatus
(
    IN GT_U8         devNum,
    IN GT_U32        status
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;


    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );
    ret = tm_aging_change_status(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (uint8_t)status);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmAgingChangeStatus);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, status));

    rc = internal_cpssTmAgingChangeStatus(devNum, status);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, status));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmAgingStatusGet
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
** 
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL statusPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
static GT_STATUS internal_cpssTmAgingStatusGet
(
    IN  GT_U8         devNum,
    OUT GT_U32        *statusPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
    uint8_t		st;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );
    ret = tm_aging_status_get(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, &st);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
	*statusPtr = (GT_U32)st;
    return rc;
}

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
**
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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmAgingStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, statusPtr));

    rc = internal_cpssTmAgingStatusGet(devNum, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssTmDpSourceSet
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
static GT_STATUS internal_cpssTmDpSourceSet
(
    IN GT_U8                   devNum,
    IN CPSS_TM_LEVEL_ENT       level,
    IN CPSS_DP_LEVEL_ENT       color,
    IN CPSS_TM_DP_SOURCE_ENT   source
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;


    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_dp_source_set(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, level, (uint8_t)color, source);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDpSourceSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, color, source));

    rc = internal_cpssTmDpSourceSet(devNum, level, color, source);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, color, source));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssTmDpSourceGet
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
static GT_STATUS internal_cpssTmDpSourceGet
(
    IN GT_U8                   devNum,
    IN CPSS_TM_LEVEL_ENT       level,
    IN CPSS_DP_LEVEL_ENT       color,
    OUT CPSS_TM_DP_SOURCE_ENT  *sourcePtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
	enum tm_dp_source dp_source;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(sourcePtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_dp_source_get(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, level, (uint8_t)color, &dp_source);
	if (dp_source)	*sourcePtr = CPSS_TM_DP_SOURCE_QL_E;
	else			*sourcePtr = CPSS_TM_DP_SOURCE_AQL_E;
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDpSourceGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, color, sourcePtr));

    rc = internal_cpssTmDpSourceGet(devNum, level, color, sourcePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, color, sourcePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssTmDropQueryResponceSet
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
static GT_STATUS internal_cpssTmDropQueryResponceSet
(
    IN GT_U8                   devNum,
    IN GT_U32                  portDp,
    IN CPSS_TM_LEVEL_ENT       level
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;


    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_set_drop_query_responce(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (uint8_t)portDp, level);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropQueryResponceSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portDp, level));

    rc = internal_cpssTmDropQueryResponceSet(devNum, portDp, level);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portDp, level));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmDropQueryResponceGet
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
*       portDpPtr       - (pointer to) PortDp(0 - Global, 1 - CoS).
*       levelPtr        - (pointer to) Local response level (APPLICABLE RANGES: CPSS_TM_LEVEL_Q_E..CPSS_TM_LEVEL_P_E).
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL portDpPtr or levelPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
static GT_STATUS internal_cpssTmDropQueryResponceGet
(
    IN GT_U8                     devNum,
    OUT GT_U32                  *portDpPtr,
    OUT CPSS_TM_LEVEL_ENT       *levelPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
	uint8_t			port_dp;
	enum tm_level	lvl;


    PRV_CPSS_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(portDpPtr);
    CPSS_NULL_PTR_CHECK_MAC(levelPtr);


    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_get_drop_query_responce(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, &port_dp, &lvl);
	*portDpPtr = (GT_U32)port_dp;
	*levelPtr = (CPSS_TM_LEVEL_ENT)lvl;
	
	rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}
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
*       portDpPtr       - (pointer to) PortDp(0 - Global, 1 - CoS).
*       levelPtr        - (pointer to) Local response level (APPLICABLE RANGES: CPSS_TM_LEVEL_Q_E..CPSS_TM_LEVEL_P_E).
* 
* OUTPUTS:
*       None.
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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropQueryResponceGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portDpPtr, levelPtr));

    rc = internal_cpssTmDropQueryResponceGet(devNum, portDpPtr, levelPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portDpPtr, levelPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssTmDropQueueCosSet
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
static GT_STATUS internal_cpssTmDropQueueCosSet
(
    IN GT_U8                   devNum,
    IN GT_U32                  index,
    IN GT_U32                  cos
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;


    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_set_drop_queue_cos(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, (uint8_t)cos);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropQueueCosSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, cos));

    rc = internal_cpssTmDropQueueCosSet(devNum, index, cos);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, cos));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmDropQueueCosGet
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
*       cosPtr          - (pointer to)Cos.
* 
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL cosPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
static GT_STATUS internal_cpssTmDropQueueCosGet
(
    IN GT_U8                   devNum,
    IN GT_U32                  index,
    OUT GT_U32                 *cosPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
	uint8_t		CoS;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cosPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_get_drop_queue_cos(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &CoS);
	*cosPtr = (GT_U32)CoS;

    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}


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
*       cosPtr          - (pointer to)Cos (APPLICABLE RANGES: 0..7).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL cosPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
GT_STATUS cpssTmDropQueueCosGet
(
    IN GT_U8                   devNum,
    IN GT_U32                  index,
    OUT GT_U32                 *cosPtr
)
{
	GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropQueueCosGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, cosPtr));

    rc = internal_cpssTmDropQueueCosGet(devNum, index, cosPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, cosPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssTmDropProfileAgingBlockCreate
*
* DESCRIPTION:
*       Create Aging profile for CPSS_TM_LEVEL_Q_E.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum                - Device number.
*       profileStcPtr         - array of 4 Aging Profile configuration structure pointer.
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
*       None
*******************************************************************************/
static GT_STATUS internal_cpssTmDropProfileAgingBlockCreate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_TM_DROP_PROFILE_PARAMS_STC         profileStrArr[CPSS_TM_AGING_PROFILES_CNS],
    OUT GT_U32                                  *profileIndexPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    struct tm_drop_profile_params prf[CPSS_TM_AGING_PROFILES_CNS];
    uint16_t prof_index;
    int j;
    CPSS_DP_LEVEL_ENT i;


    PRV_CPSS_DEV_CHECK_MAC(devNum);
	CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );


    CPSS_NULL_PTR_CHECK_MAC(profileStrArr);
    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    for (j=0;j<CPSS_TM_AGING_PROFILES_CNS; j++)
	{
		prf[j].cbtdBytes = profileStrArr[j].cbTdThresholdBytes;
		switch(profileStrArr[j].dropMode)
		{
			case CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E:
				prf[j].drop_mode=WRED ;
				prf[j].wredDp.aqlExponent  = profileStrArr[j].caWredDp.aqlExponent;
				for (i = CPSS_DP_GREEN_E; i <= CPSS_DP_RED_E; i++)
				{
					prf[j].wredDp.curveIndex[i] = profileStrArr[j].caWredDp.curveIndex[i];
					prf[j].wredDp.dpCurveScale[i] = profileStrArr[j].caWredDp.dpCurveScale[i];
					prf[j].wredDp.caWredTdMinThreshold[i] = profileStrArr[j].caWredDp.caWredTdMinThreshold[i];
					prf[j].wredDp.caWredTdMaxThreshold[i] = profileStrArr[j].caWredDp.caWredTdMaxThreshold[i];
 				}
				break;
			case CPSS_TM_DROP_MODE_CB_TD_CA_TD_E:
				prf[j].drop_mode=CATD ;
				for (i = CPSS_DP_GREEN_E; i <= CPSS_DP_RED_E; i++)
				{
					prf[j].caTdDp.catdBytes[i]=profileStrArr[j].caTdDp.caTdThreshold[i];
				}
				break;
			case CPSS_TM_DROP_MODE_CB_TD_ONLY_E:
				prf[j].drop_mode=DISABLED ;
				break;
			default: 
				/* invalid mode  */
				return XEL_TO_CPSS_ERR_CODE(-ENODATA);
			} 
	}

    ret = tm_create_aging_profile(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                 prf,
                                 &prof_index);

    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;

    *profileIndexPtr = prof_index;
    return rc;
}

/*******************************************************************************
* cpssTmDropProfileAgingBlockCreate
*
* DESCRIPTION:
*       Create Aging profile for CPSS_TM_LEVEL_Q_E.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum                - Device number.
*       profileStcPtr         - array of 4 Aging Profile configuration structure pointer.
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
    IN  CPSS_TM_DROP_PROFILE_PARAMS_STC         profileStrArr[CPSS_TM_AGING_PROFILES_CNS],
    OUT GT_U32                                  *profileIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropProfileAgingBlockCreate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileStrArr, profileIndexPtr));

    rc = internal_cpssTmDropProfileAgingBlockCreate(devNum, profileStrArr, profileIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileStrArr, profileIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssTmDropProfileAgingBlockDelete
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
GT_STATUS internal_cpssTmDropProfileAgingBlockDelete
(
    IN GT_U8                    devNum,
    IN GT_U32                   profileIndex
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_delete_aging_profile(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                 (uint16_t)profileIndex);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropProfileAgingBlockDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex));

    rc = internal_cpssTmDropProfileAgingBlockDelete(devNum, profileIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssTmDropProfileAgingUpdate
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
*       profileParamsPtr    - (pointer to)Drop Profile configurations structure.
*       profileIndex        - The first created Aging profile index (APPLICABLE RANGES: 0..2044).
*       profileOffset       - The drop profile index to be update (APPLICABLE RANGES: 0..be CPSS_TM_AGING_BLOCK_SIZE).
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
static GT_STATUS internal_cpssTmDropProfileAgingUpdate
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  profileIndex,
    IN  GT_U32                                  profileOffset,
    IN  CPSS_TM_DROP_PROFILE_PARAMS_STC         *profileParamsPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    struct tm_drop_profile_params prf;
    uint16_t prof_index  = (uint16_t)profileIndex;
    uint16_t offset  = (uint16_t)profileOffset;
    CPSS_DP_LEVEL_ENT i;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
	CPSS_NULL_PTR_CHECK_MAC(profileParamsPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

   CPSS_NULL_PTR_CHECK_MAC(profileParamsPtr);


	prf.cbtdBytes = profileParamsPtr->cbTdThresholdBytes;
    switch(profileParamsPtr->dropMode)
	{
		case CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E:
			prf.drop_mode=WRED ;
			prf.wredDp.aqlExponent  = profileParamsPtr->caWredDp.aqlExponent;
			for (i = CPSS_DP_GREEN_E; i <= CPSS_DP_RED_E; i++)
			{
				prf.wredDp.curveIndex[i] = profileParamsPtr->caWredDp.curveIndex[i];
				prf.wredDp.dpCurveScale[i] = profileParamsPtr->caWredDp.dpCurveScale[i];
				prf.wredDp.caWredTdMinThreshold[i] = profileParamsPtr->caWredDp.caWredTdMinThreshold[i];
				prf.wredDp.caWredTdMaxThreshold[i] = profileParamsPtr->caWredDp.caWredTdMaxThreshold[i];
 			}
			break;
		case CPSS_TM_DROP_MODE_CB_TD_CA_TD_E:
			prf.drop_mode=CATD ;
			for (i = CPSS_DP_GREEN_E; i <= CPSS_DP_RED_E; i++)
			{
				prf.caTdDp.catdBytes[i]=profileParamsPtr->caTdDp.caTdThreshold[i];
			}
			break;
		case CPSS_TM_DROP_MODE_CB_TD_ONLY_E:
			prf.drop_mode=DISABLED ;
			break;
		default:
			/* invalid mode  */
			return XEL_TO_CPSS_ERR_CODE(-ENODATA);
	}

    ret = tm_update_aging_profile(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                 prof_index,
								 offset,
								 &prf);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    return rc;
}

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
*       profileParamsPtr    - (pointer to)Drop Profile configurations structure.
*       profileIndex        - The first created Aging profile index (APPLICABLE RANGES: 0..2044).
*       profileOffset       - The drop profile index to be update (APPLICABLE RANGES: 0..CPSS_TM_AGING_BLOCK_SIZE).
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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropProfileAgingUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex, profileOffset, profileParamsPtr));

    rc = internal_cpssTmDropProfileAgingUpdate(devNum, profileIndex, profileOffset, profileParamsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex, profileOffset, profileParamsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmNodeDropProfileIndexRead
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
*       cos                - CoS of RED Curve (APPLICABLE RANGES: 0..7).
*       nodeIndex          - The index of queue to read its drop profile.
* OUTPUTS:
*       profileIndex       - (pointer to) The drop profile currently active for the queueIndex.
*                                    (APPLICABLE RANGES: 0..2047  for Queues,
*                                                        0..255   for Anodes,
*                                                        0..63    for Bnodes,Cnodes,
*                                                        0..number of ports for Ports).
*
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
static GT_STATUS internal_cpssTmNodeDropProfileIndexRead
(
    IN GT_U8                    devNum,
    IN CPSS_TM_LEVEL_ENT        level,
    IN GT_U32                   cos,
    IN GT_U32                   nodeIndex,
    OUT GT_U32                  *profileIndexPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
	uint16_t	profile;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_read_node_drop_profile(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
									level,
									(uint8_t)cos,
									nodeIndex,
									&profile);
	*profileIndexPtr=(GT_U32)profile;
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}


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
*       cos                - CoS of RED Curve (APPLICABLE RANGES: 0..7).
*       nodeIndex          - The index of queue to read its drop profile.
* OUTPUTS:
*       profileIndex       - (pointer to) The drop profile currently active for the queueIndex.
*                                    (APPLICABLE RANGES: 0..2047  for Queues,
*                                                        0..255   for Anodes,
*                                                        0..63    for Bnodes,Cnodes,
*                                                        0..number of ports for Ports).
*
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
    OUT GT_U32                  *profileIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmNodeDropProfileIndexRead );

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum,level, cos, nodeIndex, profileIndex ));

    rc = internal_cpssTmNodeDropProfileIndexRead (devNum, level, cos, nodeIndex, profileIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, cos, nodeIndex, profileIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


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
*      none
*******************************************************************************/
GT_U32 cpssTmDropConvertBytesToBW
(
    IN  GT_U32                          cbTdBytes
)
{
	return DROP_BYTES_TO_BW_KBITS(cbTdBytes);
}


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
*       None
*******************************************************************************/
GT_U32 cpssTmDropConvertBWToBytes
(
    IN  GT_U32                          cbTdkBitsPerSec
)
{
	return DROP_BW_KBITS_TO_BYTES(cbTdkBitsPerSec);
}

/*******************************************************************************
* prvCpssTmDropProfileGetNext
*
* DESCRIPTION:
*       Get Next Drop profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum              - Device number.
*       level               - A nodes level for the next Drop profile (Port/C/B/A/Q).
*       cos                 - CoS of Drop profile (APPLICABLE RANGES: 0..7).
*       profileIndPtr       - (pointer to) Drop profile Index (APPLICABLE RANGES: 0..MAX_Drop_Profile_per_level).
*
* OUTPUTS:
*       profileStcPtr       - (pointer to) Next Drop profile index.
*
* RETURNS:
*       GT_OK                    - next drop profile does exist.
*       GT_NOT_FOUND             - next drop profile index does not exist.
*       GT_BAD_PTR               - on NULL profileIndPtr.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssTmDropProfileGetNext
(
    IN GT_U8                            devNum,
    IN CPSS_TM_LEVEL_ENT                level,
    IN GT_U32                           cos,
    INOUT GT_U32                        *profileIndPtr
)
{
    int ret;
    GT_STATUS rc = GT_OK;
    PRV_CPSS_DEV_CHECK_MAC(devNum);
	    CPSS_NULL_PTR_CHECK_MAC(profileIndPtr)

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
    CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                               | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_get_next_drop_profile(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                                   level,
                                                   (uint8_t)cos,
                                                   (uint16_t*)profileIndPtr);

    if (ret < 0)
        rc = GT_NOT_FOUND;

    return rc;
}

