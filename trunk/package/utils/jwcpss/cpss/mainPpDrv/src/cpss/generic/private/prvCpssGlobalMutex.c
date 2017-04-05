/*******************************************************************************
*              (c), Copyright 2013, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGlobalMutex.c
*
* DESCRIPTION:
*       This file provides lock/unlock with CPSS global mutex
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/private/prvCpssGlobalMutex.h>

static CPSS_OS_MUTEX prvCpssGlobalMtx = (CPSS_OS_MUTEX)0;

/*******************************************************************************
* cpssGlobalMtxInit
*
* DESCRIPTION:
*        Initialize CPSS global mutex object
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssGlobalMtxInit(GT_VOID)
{
    GT_STATUS rc;
    if (prvCpssGlobalMtx != (CPSS_OS_MUTEX)0)
    {
         /* Already initialized */
         return GT_OK;
    }
    rc = cpssOsMutexCreate("prvCpssGlobalMtx",&prvCpssGlobalMtx);
    return rc;
}

/*******************************************************************************
* cpssGlobalMtxLock
*
* DESCRIPTION:
*        Lock CPSS global mutex
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_VOID cpssGlobalMtxLock(GT_VOID)
{
    cpssOsMutexLock(prvCpssGlobalMtx);
}

/*******************************************************************************
* cpssGlobalMtxUnlock
*
* DESCRIPTION:
*        Unlock CPSS global mutex
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_VOID cpssGlobalMtxUnlock(GT_VOID)
{
    cpssOsMutexUnlock(prvCpssGlobalMtx);
}

/*******************************************************************************
* cpssGlobalMtxDelete
*
* DESCRIPTION:
*        Destroy CPSS global mutex object
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssGlobalMtxDelete(GT_VOID)
{
    cpssOsMutexDelete(prvCpssGlobalMtx);
    prvCpssGlobalMtx = 0;

    return GT_OK;
}



