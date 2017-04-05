/*******************************************************************************
*              Copyright 2002, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* noPpBoards.c
*
* DESCRIPTION:
*       Includes board specific initialization definitions and data-structures.
*       Boars without PP.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.0.0.3 $
*
*******************************************************************************/
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>



/*******************************************************************************
 * Local usage variables
 ******************************************************************************/
const GT_BOARD_LIST_ELEMENT   boardsList[] =
{

    {gtFpgaRhodesFaBoard,                "DB-RHODES-2FA",     /*  1,1,0,0  - Rhodes  */
     1,                                     {"Rev 0.1"}}

};

const GT_U32 boardListLen = sizeof(boardsList) / sizeof(GT_BOARD_LIST_ELEMENT);

