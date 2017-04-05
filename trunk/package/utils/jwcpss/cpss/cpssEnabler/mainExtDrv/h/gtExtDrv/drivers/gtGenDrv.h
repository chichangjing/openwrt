/*******************************************************************************
*                Copyright 2015, MARVELL SEMICONDUCTOR, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), MARVELL ISRAEL LTD. (MSIL).                                          *
********************************************************************************
* gtGenDrv.h
*
* DESCRIPTION:
*       Generic extDrv functions
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __gtGenDrvh
#define __gtGenDrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtExtDrv/os/extDrvOs.h>

/* BobK Customer Boards */
#define EXT_DRV_BOBK_CUSTOMER_BOARD_ID_BASE         0x40
#define EXT_DRV_BOBK_CETUS_CUSTOMER_BOARD_ID0       (EXT_DRV_BOBK_CUSTOMER_BOARD_ID_BASE + 0)
#define EXT_DRV_BOBK_CAELUM_CUSTOMER_BOARD_ID1      (EXT_DRV_BOBK_CUSTOMER_BOARD_ID_BASE + 1)
#define EXT_DRV_BOBK_CUSTOMER_MAX_BOARD_ID          (EXT_DRV_BOBK_CUSTOMER_BOARD_ID_BASE + 2)
#define EXT_DRV_BOBK_CUSTOMER_BOARD_NUM             (EXT_DRV_BOBK_CUSTOMER_MAX_BOARD_ID - EXT_DRV_BOBK_CUSTOMER_BOARD_ID_BASE)

/* BobK Marvell Boards */
#define EXT_DRV_BOBK_MARVELL_BOARD_ID_BASE          0x50
#define EXT_DRV_BOBK_CETUS_DB_ID                    (EXT_DRV_BOBK_MARVELL_BOARD_ID_BASE + 0)
#define EXT_DRV_BOBK_CAELUM_DB_ID                   (EXT_DRV_BOBK_MARVELL_BOARD_ID_BASE + 1)
#define EXT_DRV_BOBK_CETUS_LEWIS_RD_ID              (EXT_DRV_BOBK_MARVELL_BOARD_ID_BASE + 2)
#define EXT_DRV_BOBK_MARVELL_MAX_BOARD_ID           (EXT_DRV_BOBK_MARVELL_BOARD_ID_BASE + 3)
#define EXT_DRV_BOBK_MARVELL_BOARD_NUM              (EXT_DRV_BOBK_MARVELL_MAX_BOARD_ID - EXT_DRV_BOBK_MARVELL_BOARD_ID_BASE)


/*******************************************************************************
* extDrvBoardIdGet
*
* DESCRIPTION:
*       Get BoardId value
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       boardIdPtr  - boardId
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvBoardIdGet
(
    OUT GT_U32  *boardIdPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtGenDrvh */


