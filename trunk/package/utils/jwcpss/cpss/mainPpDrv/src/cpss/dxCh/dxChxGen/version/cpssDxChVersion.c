/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChVersion.c
*
* DESCRIPTION:
*       Implements software CPSS DxCh version information.
*
*
* FILE REVISION NUMBER:
*       $Revision: 34 $
*
*******************************************************************************/
/* get the OS , extDrv functions*/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/version/cpssDxChVersion.h>
#include <cpss/dxCh/dxChxGen/version/private/prvCpssDxChVersionLog.h>

/* string to define the CPSS DxCh version used */
#define DXCH_VERSION_CNS        "CPSS 4.1.2016.1"

/*******************************************************************************
* internal_cpssDxChVersionGet
*
* DESCRIPTION:
*       This function returns CPSS DxCh version.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       versionPtr     - (pointer to)CPSS DxCh version info.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - one parameter is NULL pointer
*       GT_BAD_SIZE - the version name is too long
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVersionGet
(
    OUT CPSS_VERSION_INFO_STC   *versionPtr
)
{
    GT_U32  versionLen = cpssOsStrlen(DXCH_VERSION_CNS);

    if(versionPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if(versionLen > CPSS_VERSION_MAX_LEN_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_SIZE, LOG_ERROR_NO_MSG);
    }

    cpssOsMemCpy(versionPtr->version, DXCH_VERSION_CNS, versionLen + 1);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVersionGet
*
* DESCRIPTION:
*       This function returns CPSS DxCh version.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       versionPtr     - (pointer to)CPSS DxCh version info.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - one parameter is NULL pointer
*       GT_BAD_SIZE - the version name is too long
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVersionGet
(
    OUT CPSS_VERSION_INFO_STC   *versionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVersionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, versionPtr));

    rc = internal_cpssDxChVersionGet(versionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, versionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

