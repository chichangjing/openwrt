/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortPizzaArbiterDevInfo.c
*
* DESCRIPTION:
*       CPSS implementation for SerDes configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 61 $
*******************************************************************************/

#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/prvCpssDxChPortSlices.h> 
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterProfileStorage.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterDevInfo.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>



#if 0
    const DevRevType_SliceManipulFuns_STC g_DevRevType_SliceEnDisFunArr[] = 
    {
        /* family                     rev              sliceEn Dis  GetState */
        { CPSS_PP_FAMILY_DXCH_LION2_E , 0 , /* out*/ { prvLion2PortPizzaArbiterConfigureSliceOccupy, 
                                                       prvLion2PortPizzaArbiterConfigureSliceRelease, 
                                                       prvLion2PortPizzaArbiterConfigureSliceGetState}  } 
    };

    const ListDevRevType_SliceManipulFuns_STC g_listDevRevType_SliceEnDisFun = 
    {
         sizeof(g_DevRevType_SliceEnDisFunArr)/sizeof(g_DevRevType_SliceEnDisFunArr[0])
        ,(DevRevType_SliceManipulFuns_STC *)&g_DevRevType_SliceEnDisFunArr[0]
    };
#endif

GT_STATUS prvCpssDxChPortPizzaArbiterIfAddDevGetListActivePortGroup
(
    IN  GT_U8  devNum, 
    OUT PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC * activePortGroupListPtr
)
{
    GT_STATUS rc;

    rc = ActivePortGroupListBuildFromBmp(activePortGroupListPtr,
                                          PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp,
                                          PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup,
                                          PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfDevInfoBuild
*
* DESCRIPTION:
*       build relevant dev info from central data base 
*
* APPLICABLE DEVICES:
*       Lion2 Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - dev number
*
* OUTPUTS:
*       pizzaDevInfoPtr - pointer to dev info structure
*
* RETURNS:
*       	GT_BAD_PTR  - on pizzaDevInfoPtr is zero
*           GT_OK       - on success          
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfDevInfoBuild
(
    IN    GT_U8 devNum,
    OUT   PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr
)
{
    GT_STATUS rc;
    GT_U32    pg;
    GT_U32    pgBmp;
    GT_STATUS pgN;

    PRV_CPSS_GEN_PP_CONFIG_STC* devPtr; /* pionter to device to be processed*/
    CPSS_DXCH_PIZZA_PROFILE_STC * pPizzaProfilePtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (pizzaDevInfoPtr == NULL) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    devPtr = PRV_CPSS_PP_MAC(devNum);

    pizzaDevInfoPtr->devFamilyType         = devPtr->devFamily;
    pizzaDevInfoPtr->devRevision           = devPtr->revision;
    pizzaDevInfoPtr->devCoreClock          = devPtr->coreClock;
    pizzaDevInfoPtr->totalPortGroups       = devPtr->portGroupsInfo.numOfPortGroups;

    pizzaDevInfoPtr->sliceModFun.portGroupSlicesInitFunPtr = prvLion2PortPizzaArbiterConfigureSlicesInit;
    pizzaDevInfoPtr->sliceModFun.sliceOccupyFunPtr         = prvLion2PortPizzaArbiterConfigureSliceOccupy;
    pizzaDevInfoPtr->sliceModFun.sliceReleaseFunPtr        = prvLion2PortPizzaArbiterConfigureSliceRelease; 
    pizzaDevInfoPtr->sliceModFun.sliceGetStateFunPtr       = prvLion2PortPizzaArbiterConfigureSliceGetState;

    rc = prvCpssDxChPortPizzaArbiterIfAddDevGetListActivePortGroup(devNum, 
                                                                  /*OUT */&pizzaDevInfoPtr->activePortGroupList);
    if (rc != GT_OK) 
    {
        return rc;
    }
    /* Build profile Info */
    pgN = sizeof(pizzaDevInfoPtr->portGroupPizzaProfile)/sizeof(pizzaDevInfoPtr->portGroupPizzaProfile[0]);
    for (pg = 0 ; pg < pgN; pg++) 
    {
        pizzaDevInfoPtr->portGroupPizzaProfile[pg] = NULL;
    }

    for (pg = 0 ; pg < pizzaDevInfoPtr->totalPortGroups; pg++) 
    {
        pgBmp = 1 << pizzaDevInfoPtr->activePortGroupList.list[pg];
        rc = cpssDxChPortPizzaArbiterIfUserTableGet(/*IN*/devNum,pgBmp,/*OUT*/&pPizzaProfilePtr,/*OUT*/NULL);
        if (rc != GT_OK) /* Not found,  catastrophic */
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
        pizzaDevInfoPtr->portGroupPizzaProfile[pg] = pPizzaProfilePtr;
    }

    return GT_OK;
}



 
