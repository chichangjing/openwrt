/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDragoniteDrv.h
*
* DESCRIPTION:
*       Includes Dragonite functions wrappers, for reading / writing Dragonite 
*       memory
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __cpssDragoniteDrvh
#define __cpssDragoniteDrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>


/*******************************************************************************
* CPSS_EXTDRV_DRAGONITE_GET_BASE_ADDRESS_FUNC
*
* DESCRIPTION:
*       Get start address of communication structure in DTCM
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       dtcmPtr      - Pointer to beginning of DTCM, where
*                      communication structures must be placed
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_DRAGONITE_GET_BASE_ADDRESS_FUNC)
(
    OUT  GT_U32  *dtcmPtr
);


/* CPSS_EXT_DRV_DRAGONITE_STC -
    structure that hold the "external driver DMA " functions needed be
    bound to cpss.
*/
typedef struct{
    CPSS_EXTDRV_DRAGONITE_GET_BASE_ADDRESS_FUNC extDrvDragoniteShMemBaseAddrGetFunc;
}CPSS_EXT_DRV_DRAGONITE_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDragoniteDrvh */


