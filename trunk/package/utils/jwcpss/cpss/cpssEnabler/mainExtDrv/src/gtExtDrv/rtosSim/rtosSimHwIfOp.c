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
* genDrvHwIfOp.c
*
* DESCRIPTION:
*       API implementation for Hw interface operations.
*
* DEPENDENCIES:
*
*       $Revision: 1.1.2.1 $
*******************************************************************************/

/*Includes*/
#include <gtExtDrv/os/extDrvOs.h>
#include <gtExtDrv/drivers/gtHwIfOp.h>
#include <gtExtDrv/drivers/gtSmiHwCtrl.h>


/*******************************************************************************
* extDrvHwIfOperExec
*
* DESCRIPTION:
*       This routine performs co-processor operations according to data in the 
*       operationLength entries. The routine can be blocking or non-blocking.
*
* INPUTS:
*       arrayLength     - Array length.
*       operationArr    - Array of operations to be perform by the co-processor.
*       nonBlockCbPtr   - A struct containing the data needed for non blocking
*                         operation (callback routine and parameter). If NULL,
*                         the routine call is blocking.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvHwIfOperExec
(
    IN   GT_U32                         arrayLength,
    IN   GT_HW_IF_UNT                   operationArr[],
    IN   GT_HW_IF_NON_BLOCK_CB_STC      *nonBlockCbPtr
)
{
    GT_U32      i;
    GT_STATUS   retVal;
    
    for (i = 0; i < arrayLength; i++)
    {
        switch (operationArr->coProc[i].opCode)
        {
            case GT_READ_SMI_BUFFER_E:
                retVal = hwIfSmiReadReg(operationArr->smiOper[i].smiDeviceAddr,
                                        operationArr->smiOper[i].regAddress,
                                        &operationArr->smiOper[i].smiData);
                break;
                    
            case GT_WRITE_SMI_BUFFER_E:
                retVal = hwIfSmiWriteReg(operationArr->smiOper[i].smiDeviceAddr,
                                         operationArr->smiOper[i].regAddress,
                                         operationArr->smiOper[i].smiData);
                break;
                                     
            case GT_COPROC_READ_COUNTERS_E:
            case GT_COPROC_READ_MIBS_E:
            case GT_COPROC_CLEAR_SRAM_E:
                retVal = GT_FAIL;
                break;
            
            default:
                return GT_FAIL;
        }
        
        if (retVal != GT_OK)
        {
            return GT_FAIL;
        }
    }
    
    return GT_OK;
}

