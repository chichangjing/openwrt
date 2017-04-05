/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortIfModeCfgBcat2ResourceList.c
*
* DESCRIPTION:
*       CPSS BC2 implementation for Port interface mode resource configuration.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2B0ResourceList.h>


GT_STATUS PRV_DXCH_BCAT2_RESOURCE_LIST_Init
(
    INOUT CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC * listPtr
)
{
    GT_U32 i;

    if (listPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    listPtr->fldN = 0;
    for (i = 0 ; i < sizeof(listPtr->arr)/sizeof(listPtr->arr[0]); i++)
    {
        listPtr->arr[i].fldId  = BC2_PORT_FLD_INVALID_E;
        listPtr->arr[i].fldArrIdx = (GT_U32)~0;
        listPtr->arr[i].fldVal = (GT_U32)~0;
    }
    return GT_OK;
}


GT_STATUS PRV_DXCH_BCAT2_RESOURCE_LIST_Append
(
    INOUT CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC * listPtr,
    IN    CPSS_DXCH_BC2_PORT_RESOURCES_FLD_ENT fldId,
    IN    GT_U32                         fldArrIdx,
    IN    GT_U32                         fldVal
)
{
    if (listPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (fldId < 0 ||  fldId >= BC2_PORT_FLD_MAX) /* fldId is correct */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    } 
    if (listPtr->fldN == sizeof(listPtr->arr)/sizeof(listPtr->arr[0])) /* thereis a place , seems has no sense .... */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }
    if (listPtr->arr[fldId].fldId != BC2_PORT_FLD_INVALID_E)   /* if fld is already initialize, it seems as error */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }
    listPtr->arr[fldId].fldId     = fldId;
    listPtr->arr[fldId].fldArrIdx = fldArrIdx;
    listPtr->arr[fldId].fldVal    = fldVal;
    listPtr->fldN++;
    return GT_OK;
}

GT_STATUS PRV_DXCH_BCAT2_RESOURCE_LIST_Get
(
    INOUT CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC * listPtr,
    IN    CPSS_DXCH_BC2_PORT_RESOURCES_FLD_ENT fldId,
    OUT   GT_U32                        *fldValPtr
)
{
    if (listPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (fldValPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (fldId < 0 ||  fldId >= BC2_PORT_FLD_MAX)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    *fldValPtr = listPtr->arr[fldId].fldVal;
    return GT_OK;
}


