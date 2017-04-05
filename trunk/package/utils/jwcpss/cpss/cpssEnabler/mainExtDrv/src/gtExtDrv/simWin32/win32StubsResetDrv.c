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
*       $Revision: 2.*******************************************************************************/

#include <gtExtDrv/drivers/gtPciDrv.h>

#include <asicSimulation/SCIB/scib.h>
#ifndef SHARED_MEMORY
#include <extUtils/IOStream/IOStream.h>
#endif
#ifdef ASIC_SIMULATION
extern void SHOSTG_reset (unsigned int status);
#endif

#ifndef SHARED_MEMORY
extern IOStreamPTR defaultStdio;
#endif
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
#ifdef ASIC_SIMULATION
    GT_STATUS rc = GT_OK;
#ifndef SHARED_MEMORY
    if (defaultStdio != NULL)
    {
        defaultStdio->destroy(defaultStdio);
    }
#endif

    SCIB_SEM_TAKE;
    rc = osBindStdOut(NULL,NULL);
    SCIB_SEM_SIGNAL;
    if (rc != GT_OK)
    {
        osPrintf("bad bind\n");
    }

    SHOSTG_reset(0);
#endif
    return GT_FAIL;
}


