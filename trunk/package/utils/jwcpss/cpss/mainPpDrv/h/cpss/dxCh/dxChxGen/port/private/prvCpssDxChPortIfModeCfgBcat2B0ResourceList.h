/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortIfModeCfgBcat2ResourceList.h
*
* DESCRIPTION:
*       CPSS BC2 B0 resource tables
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_PORT_IF_MODE_CFG_BCAT2_B0_RESOURCE_LIST_H
#define __PRV_CPSS_DXCH_PORT_IF_MODE_CFG_BCAT2_B0_RESOURCE_LIST_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortIfModeCfgBcat2Resource.c
*
* DESCRIPTION:
*       CPSS BC2 implementation for Port interface mode resource configuration.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>


GT_STATUS PRV_DXCH_BCAT2_RESOURCE_LIST_Init
(
    INOUT CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC * listPtr
);


GT_STATUS PRV_DXCH_BCAT2_RESOURCE_LIST_Append
(
    INOUT CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC * listPtr,
    IN    CPSS_DXCH_BC2_PORT_RESOURCES_FLD_ENT fldId,
    IN    GT_U32                         fldArrIdx,
    IN    GT_U32                         fldVal
);

GT_STATUS PRV_DXCH_BCAT2_RESOURCE_LIST_Get
(
    INOUT CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC * listPtr,
    IN    CPSS_DXCH_BC2_PORT_RESOURCES_FLD_ENT fldId,
    OUT   GT_U32                        *fldValPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
