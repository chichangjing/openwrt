/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmShaping.h
*
* DESCRIPTION:
*       TM Shaping APIs. 
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssTmShapingh
#define __cpssTmShapingh

#include <cpss/generic/tm/cpssTmPublicDefs.h>


/*******************************************************************************
* cpssTmShapingProfileCreate
*
* DESCRIPTION:
*       Create a Shaping Profile.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - Level to configure the profile for.
*       profileStrPtr   - Shaping profile configuration struct pointer.
*
* OUTPUTS:
*       profileIndPtr   - (pointer to) The created Shaping profile index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_FULL                  - on full allocation of Shaping profile table.
*       GT_NOT_INITIALIZED       - on not configured periodic update rate for the given level
*       GT_BAD_SIZE              - on too large min/max token.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmShapingProfileCreate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_TM_LEVEL_ENT                       level,
    IN  CPSS_TM_SHAPING_PROFILE_PARAMS_STC      *profileStrPtr,
    OUT GT_U32                                  *profileIndPtr
);


/*******************************************************************************
* cpssTmShapingProfileDelete
*
* DESCRIPTION:
*       Delete a Shaping Profile.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       profileInd      - An index of the Shaping profile.
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
GT_STATUS cpssTmShapingProfileDelete
(
    IN  GT_U8   devNum,
    IN  GT_U32  profileInd
);


/*******************************************************************************
* cpssTmShapingProfileRead
*
* DESCRIPTION:
*       Read Shaping profile.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - Level to read the profile for.
*       profileInd      - An index of the Shaping profile.
*
* OUTPUTS:
*       profileStrPtr   - (pointer to) Shaping profile configuration structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The cirBw and eirBw may deviate from the originally configured
*       by cpssTmShapingProfileCreate or cpssTmShapingProfileUpdate by the 
*       bw accuracy parameter provisioned in cpssTmSchedPeriodicSchemeConfig API.
*******************************************************************************/
GT_STATUS cpssTmShapingProfileRead
(
    IN  GT_U8                                   devNum,
    IN  CPSS_TM_LEVEL_ENT                       level,
    IN  GT_U32                                  profileInd,
    OUT CPSS_TM_SHAPING_PROFILE_PARAMS_STC      *profileStrPtr
);


/*******************************************************************************
* cpssTmShapingProfileUpdate
*
* DESCRIPTION:
*       Update a Shaping Profile.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - Level to update the profile for.
*       profileInd      - An index of the Shaping profile.
*       profileStrPtr   - Shaping profile configuration struct pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_INITIALIZED       - on not configured periodic update rate for the given level
*       GT_BAD_SIZE              - on too large min/max token.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmShapingProfileUpdate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_TM_LEVEL_ENT                       level,
    IN  GT_U32                                  profileInd,
    IN  CPSS_TM_SHAPING_PROFILE_PARAMS_STC      *profileStrPtr
);



/* */

/*******************************************************************************
* cpssTmShapingProfileValidate
*
* DESCRIPTION:
*       Validates if shaping profile can be configured and returns updated bursts values if possible
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - Level to configure the profile for.
*       profileStrPtr   - Shaping profile configuration struct pointer.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_INITIALIZED       - on not configured periodic update rate for the given level
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_OUT_OF_RANGE          - on too large min/max token or thy are to different to cofigure them together
*       GT_BAD_SIZE              - on cps/ebs value is too small for required bandwidth
*                                  in this case   cbs/ebs values are updated to minimum possible value.
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmShapingProfileValidate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_TM_LEVEL_ENT                       level,
    IN  CPSS_TM_SHAPING_PROFILE_PARAMS_STC      *profileStcPtr
);

/*******************************************************************************
* cpssTmPortShapingValidate
*
* DESCRIPTION:
*       Validate Port Shaping parameters.and  returns updated bursts values if possible 
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       cirBw           - Port CIR bandwidth.
*       eirBw           - Port EIR bandwidth.
* INPUT/OUTPUT
*       cbsPtr          - (pointer to)Port CIR burst size.
*       ebsPtr          - (pointer to)Port EIR burst size.
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_OUT_OF_RANGE          - on too large min/max token or thy are to different to cofigure them together
*       GT_BAD_SIZE              - on cps/ebs value is too small for required bandwidth
*                                  in this case cbs/ebs values are updated to minimum possible value.
*
* COMMENTS:
*       none
*******************************************************************************/
GT_STATUS cpssTmPortShapingValidate
(
    IN GT_U8               devNum,
    IN CPSS_TM_PORT_BW_ENT portSpeed,
    IN GT_U32              cirBw,
    IN GT_U32              eirBw,
    INOUT GT_U32 *         cbsPtr,
    INOUT GT_U32 *         ebsPtr
);






#endif 	    /* __cpssTmShapingh */

