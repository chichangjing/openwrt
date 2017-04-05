/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenCfg.c
*
* DESCRIPTION:
*       CPSS generic configuration functions.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChConfigLog.h>
#include <cpss/generic/config/cpssGenCfg.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#define DEVICE_NOT_EXISTS_CNS 0xFF

/*******************************************************************************
* internal_cpssPpCfgNextDevGet
*
* DESCRIPTION:
*
*       Return the number of the next existing device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to start from. For the first one devNum should be 0xFF.
*
* OUTPUTS:
*       nextDevNumPtr - number of next device after devNum.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NO_MORE               - devNum is the last device. nextDevNumPtr will be set to 0xFF.
*       GT_BAD_PARAM             - devNum > max device number
*       GT_BAD_PTR               - nextDevNumPtr pointer is NULL.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssPpCfgNextDevGet
(
    IN  GT_U8 devNum,
    OUT GT_U8 *nextDevNumPtr
)
{
    GT_U8 num;

    /* device should be in the allowed range but not necessary exist */
    if ((devNum > PRV_CPSS_MAX_PP_DEVICES_CNS) && (devNum != DEVICE_NOT_EXISTS_CNS))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (nextDevNumPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (devNum == 0xFF)
        num = 0;
    else
        num = (GT_U8)(devNum+1);

    for (; num<PRV_CPSS_MAX_PP_DEVICES_CNS; num++)
    {
        if (prvCpssPpConfig[num] != NULL)
        {
            *nextDevNumPtr = num;
            return GT_OK;
        }
    }

    /* no more devices found */
    *nextDevNumPtr = DEVICE_NOT_EXISTS_CNS;
    return /* it's not error for log */ GT_NO_MORE;
}

/*******************************************************************************
* cpssPpCfgNextDevGet
*
* DESCRIPTION:
*
*       Return the number of the next existing device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to start from. For the first one devNum should be 0xFF.
*
* OUTPUTS:
*       nextDevNumPtr - number of next device after devNum.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NO_MORE               - devNum is the last device. nextDevNumPtr will be set to 0xFF.
*       GT_BAD_PARAM             - devNum > max device number
*       GT_BAD_PTR               - nextDevNumPtr pointer is NULL.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPpCfgNextDevGet
(
    IN  GT_U8 devNum,
    OUT GT_U8 *nextDevNumPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssPpCfgNextDevGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, nextDevNumPtr));

    rc = internal_cpssPpCfgNextDevGet(devNum, nextDevNumPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, nextDevNumPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
