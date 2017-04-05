/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCatchUpDbg.h
*
* DESCRIPTION:
*       CPSS DxCh CatchUp debug utilities
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
* 
*******************************************************************************/

#ifndef __cpssDxChCatchUpDbgh
#define __cpssDxChCatchUpDbgh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpssCommon/cpssPresteraDefs.h>

/*******************************************************************************
* cpssDxChCatchUpValidityCheckEnable
*
* DESCRIPTION:
*       Enable CatchUp validity check
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       None.
* 
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on failure
*
* COMMENTS:
*       Pre-condition for using this function:
*       CatchUp parameters in PP data structure must be equal to the HW values
*
*******************************************************************************/
GT_STATUS cpssDxChCatchUpValidityCheckEnable
(
    IN  GT_U8   devNum
);

/*******************************************************************************
* cpssDxChCatchUpValidityCheck
*
* DESCRIPTION:
*       Validates that SW params hold the correct value from HW after CatchUp
*       process.
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       None.
* 
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on failure
*
* COMMENTS:
*       To use this API, cpssDxChCatchUpValidityCheckEnable must be called before
*       performing CatchUp
*
*******************************************************************************/
GT_STATUS cpssDxChCatchUpValidityCheck
(
    IN  GT_U8   devNum
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChCatchUpDbgh */

