/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortMappingShadowDB.h
*
* DESCRIPTION:
*       interface to port mapping shadow DB
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/

#ifndef __PRV_CPSS_DXCH_PORTMAPPING_SHADOW_DB_H
#define __PRV_CPSS_DXCH_PORTMAPPING_SHADOW_DB_H

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h> 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

GT_STATUS prvCpssDxChPortPhysicalPortMapShadowDBClear
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM physPortNum
);

GT_STATUS prvCpssDxChPortPhysicalPortMapShadowDBSet
(
    IN GT_U8                   devNum,
    IN GT_PHYSICAL_PORT_NUM    physPortNum,
    IN CPSS_DXCH_PORT_MAP_STC *portMapPtr,
    IN GT_U32                  portArgArr[PRV_CPSS_DXCH_PORT_TYPE_MAX_E]

);

GT_STATUS prvCpssDxChPortPhysicalPortMapShadowDBGet
(
    IN   GT_U8                   devNum,
    IN   GT_PHYSICAL_PORT_NUM    physPortNum,
    OUT  CPSS_DXCH_DETAILED_PORT_MAP_STC **portMapShadowPtrPtr
);

GT_STATUS prvCpssDxChPortPhysicalPortMapShadowDBInit
(
    IN GT_U8 devNum
);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
