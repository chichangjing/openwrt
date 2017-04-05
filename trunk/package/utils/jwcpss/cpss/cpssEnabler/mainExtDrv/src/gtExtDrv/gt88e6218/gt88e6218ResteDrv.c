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
*       $Revision: 3 $
*
*******************************************************************************/

#include <gtExtDrv/drivers/gtPciDrv.h>
#include <gtExtDrv/drivers/gtSmiHwCtrl.h>

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
    /* #warning "extDrvResetInit not yet implemented" */
    /* Soft rest trigger */
    hwIfSmiTaskWriteReg(0, 0x30, 1 << 16);
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
#if !(defined (ASIC_SIMULATION))
    /* #warning "extDrvResetInit not yet implemented use reboot for now" */
    extern void reboot(int);
    reboot(2); /* vxWorks reboot with clear */
#else
    extern void exit(int);
    exit(0);
#endif

    return GT_OK;
}

