/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCatchUpDbg.c
*
* DESCRIPTION:
*       CPSS DxCh CatchUp debug utilities
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
* 
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/catchUp/private/prvCpssDxChCatchUp.h>

/*******************************************************************************
* internal_cpssDxChCatchUpValidityCheckEnable
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
static GT_STATUS internal_cpssDxChCatchUpValidityCheckEnable
(
    IN  GT_U8   devNum
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    return prvCpssDxChSystemRecoveryCatchUpValidityCheckEnable(devNum);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCatchUpValidityCheckEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChCatchUpValidityCheckEnable(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCatchUpValidityCheck
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
static GT_STATUS internal_cpssDxChCatchUpValidityCheck
(
    IN  GT_U8   devNum
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    return prvCpssDxChSystemRecoveryCatchUpValidityCheck(devNum);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCatchUpValidityCheck);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChCatchUpValidityCheck(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

