/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDbDxBobcat2PhyConfig.h
*
* DESCRIPTION:
*       Initialization board phy config for the Bobcat2 - SIP5 - board.
*
* FILE REVISION NUMBER:
*       $Revision: 5$
*
*******************************************************************************/
#ifndef __gtDbDxBobcat2PhyConfig_H
#define __gtDbDxBobcat2PhyConfig_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtExtDrv/drivers/gtGenDrv.h>


typedef enum BC2_BOARD_REV_ID_ENT
{
     BOARD_REV_ID_DB_E        = 1
    ,BOARD_REV_ID_DB_TM_E     = 2
    ,BOARD_REV_ID_RDMTL_E     = 3
    ,BOARD_REV_ID_RDMTL_TM_E  = 4
    ,BOARD_REV_ID_RDMSI_E     = 11
    ,BOARD_REV_ID_RDMSI_TM_E  = 12
}BC2_SYSTEM_TYPE_ENT;

/******************************************************************************
 * typedef: enum CPSS_TM_SCENARIO
 *
 * Description: tm scenario mode.
 *
 * Enumerations:
 *
 *      CPSS_TM_48_PORT - 48 ports.
 *      CPSS_TM_2 - 10 ports.
 *      CPSS_TM_3 - 10 ports.
 ******************************************************************************/
typedef enum
{
    CPSS_TM_48_PORT = 0,
    CPSS_TM_2=2,
    CPSS_TM_3=3,
    CPSS_TM_4=4,
    CPSS_TM_5=5,
    CPSS_TM_6=6,
    CPSS_TM_7=7,
    CPSS_TM_8=8,
    CPSS_TM_9=9,
    CPSS_TM_STRESS=10,
    CPSS_TM_STRESS_1=11,
    CPSS_TM_REV4=12,
    CPSS_TM_USER_20=20
} CPSS_TM_SCENARIO;


/* Board types: 0 - DB board; 1 - RD MSI board  2 -- RD MTL board */
#define APP_DEMO_BC2_BOARD_DB_CNS     0
#define APP_DEMO_BC2_BOARD_RD_MSI_CNS 1
#define APP_DEMO_BC2_BOARD_RD_MTL_CNS 2
#define APP_DEMO_CAELUM_BOARD_DB_CNS  (EXT_DRV_BOBK_CAELUM_DB_ID)
#define APP_DEMO_CETUS_BOARD_DB_CNS   (EXT_DRV_BOBK_CETUS_DB_ID)
#define APP_DEMO_CETUS_BOARD_LEWIS_RD_CNS   (EXT_DRV_BOBK_CETUS_LEWIS_RD_ID)



/*******************************************************************************
* bobcat2BoardTypeGet
*
* DESCRIPTION:
*       This function recognizes type of Bobcat2 board.
*       The function reads PHY type for SMI#0 address 4 (port 24 on DB board,
*       port 4 on RD board).
*       If PHY is not connected or PHY ID is 1548M the board is DB.
*       Otherwise the board is RD.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       bc2BoardTypePtr - pointer to board type.
*                         0 - DB board
*                         1 - RD board
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bobcat2BoardTypeGet
(
    IN   GT_U8    devNum,
    IN   GT_U8    boardRevID,
    OUT  GT_U32  *bc2BoardTypePtr
);

 
/*******************************************************************************
* bobcat2BoardPhyConfig
*
* DESCRIPTION:
*       Board specific PHY configurations after board initialization.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devNum          - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS bobcat2BoardPhyConfig
(
    IN  GT_U8       boardRevId,
    IN  GT_U8       devNum
);


/*******************************************************************************
* bobkExternalBoardPhyConfig
*
* DESCRIPTION:
*       External PHY configuration.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS bobkExternalBoardPhyConfig
(
    IN  GT_U8       devNum,
    IN  GT_U32      phyType,
    IN  GT_U32      smiInterface,
    IN  GT_U32      smiStartAddr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

 

#endif


