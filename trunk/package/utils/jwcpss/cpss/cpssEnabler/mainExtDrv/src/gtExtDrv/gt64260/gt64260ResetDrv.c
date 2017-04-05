/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
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
* gt64260ResetDrv.c
*
* DESCRIPTION:
*       Includes two function for reset of CPU.
*
* DEPENDENCIES:
*       -   System controller.
*       -   BSP.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*
*******************************************************************************/

#include <gtExtDrv/drivers/gtPciDrv.h>
#include <coreDrv/gtCore.h>

#include <sysLib.h>

/*******************************************************************************
* extDrvResetInit
*
* DESCRIPTION:
*       This routine calls in init to do system init config for reset.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvResetInit
(
)
{
    GT_U32 data;

    GT_REG_READ(0xf00c, &data);
    data &= 0xfffffff0;
    GT_REG_WRITE(0xf00c, data);
    
    GT_REG_READ(0xf100, &data);
    data &= ~(1<<24);
    GT_REG_WRITE(0xf100, data);
    
    return GT_OK;
}

/*******************************************************************************
* extDrvReset
*
* DESCRIPTION:
*       This routine calls to reset of CPU.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_FAIL    - always.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvReset
(
)
{
    GT_U32 data;
    
    GT_REG_READ(0xf100, &data);
    data |= 1<<24;
    GT_REG_WRITE(0xf100, data);
    
    GT_REG_READ(0xf104, &data);
    data |= 1<<24;
    GT_REG_WRITE(0xf104, data);

    /*Reset*/
    GT_REG_READ(0xf104, &data);
    data &= ~(1<<24);
    GT_REG_WRITE(0xf104, data);
    
    osTimerWkAfter(100);

    GT_REG_READ(0xf104, &data);
    data |= (1<<24);
    GT_REG_WRITE(0xf104, data);
    
    /* Reset the board from the BSP sysToMonitor function */
    sysToMonitor(0);
    while(1) data++;

    return GT_FAIL;
}


