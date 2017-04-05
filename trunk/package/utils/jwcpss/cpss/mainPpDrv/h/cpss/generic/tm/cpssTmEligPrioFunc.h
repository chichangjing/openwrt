/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmEligPrioFunc.h
*
* DESCRIPTION:
*       Miscellaneous functions for Eligible Priority Functions Tables configuration.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssTmEligPrioFunch
#define __cpssTmEligPrioFunch

#include <cpss/generic/tm/cpssTmPublicDefs.h>


/*******************************************************************************
* cpssTmEligPrioFuncQueueConfig
*
* DESCRIPTION:
*       Configure the Queue Eligible Priority Function according 
*       to the User Application parameters
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       eligPrioFuncPtr   - The new configured eligible function pointer (APPLICABLE RANGES: 0..63).
*       funcOutArr        - The Eligible Priority Function structure array pointer.
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
*       	The use of Eligible Function Pointer 63 on both Queue and Node level is forbidden. 
*			Eligible Function 63 is reserved to indicate that Queue/Node is not eligible.
*******************************************************************************/
GT_STATUS cpssTmEligPrioFuncQueueConfig
(
    IN GT_U8                        devNum,
    IN GT_U32                       eligPrioFuncPtr,
    IN CPSS_TM_ELIG_PRIO_FUNC_STC   *funcOutArr
);


/*******************************************************************************
* cpssTmEligPrioFuncNodeConfig
*
* DESCRIPTION:
*       Configure the Node Eligible Priority Function according 
*       to the User Application parameters
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       eligPrioFuncPtr   - The new configured eligible function pointer (APPLICABLE RANGES: 0..63).
*       level             - A level to configure the Eligible function with.
*       funcOutArr        - The Eligible Priority Function structure array pointer.
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
*       	The use of Eligible Function Pointer 63 on both Queue and Node level is forbidden. 
*			Eligible Function 63 is reserved to indicate that Queue/Node is not eligible.
*******************************************************************************/
GT_STATUS cpssTmEligPrioFuncNodeConfig
(
    IN GT_U8                        devNum,
    IN GT_U32                       eligPrioFuncPtr,
    IN CPSS_TM_LEVEL_ENT            level,
    IN CPSS_TM_ELIG_PRIO_FUNC_STC   *funcOutArr
);


/*******************************************************************************
* cpssTmEligPrioFuncConfigAllLevels
*
* DESCRIPTION:
*       Configure the Eligible Priority Function according 
*       to the User Application parameters
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       eligPrioFuncPtr   - The new configured eligible function pointer (APPLICABLE RANGES: 0..63).
*       funcOutArr        - The Eligible Priority Function structure array pointer.
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
*       	The following API configures the same Eligible Priority Functions 
*	        at all nodes (A, B, C, and Port) levels Elig. Prio. Tables 
*	        according to the user’s parameters.
*	        It has the same functionality as tm_elig_prio_func_config() 
*	        and can be used at the user convenience to configure the 
*	        same eligible function to all the Nodes levels (except for Q level)
*	
*	Note: 	The use of Eligible Function Pointer 63 on both Queue and Node level is forbidden. 
*			Eligible Function 63 is reserved to indicate that Queue/Node is not eligible.
*******************************************************************************/
GT_STATUS cpssTmEligPrioFuncConfigAllLevels
(
    IN GT_U8                        devNum,
    IN GT_U32                       eligPrioFuncPtr,
    IN CPSS_TM_ELIG_PRIO_FUNC_STC   *funcOutArr
);


#endif 	    /* __cpssTmEligPrioFunch */

