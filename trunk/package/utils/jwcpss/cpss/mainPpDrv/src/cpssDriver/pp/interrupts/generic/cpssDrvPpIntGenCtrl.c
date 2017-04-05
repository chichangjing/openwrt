/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntGenCtrl.c
*
* DESCRIPTION:
*       Includes driver interrupts control functions.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>

/*******************************************************************************
* prvCpssDrvEventsMask
*
* DESCRIPTION:
*       enables / disables a given event from reaching the CPU.
*
* INPUTS:
*       devNum      - The PP's device number to mask / unmask the interrupt for.
*       intIndex    - The interrupt cause to enable/disable.
*       enable      - GT_TRUE enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL on failure.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvEventsMask
(
    IN  GT_U8   devNum,
    IN  GT_U32  intIndex,
    IN  GT_BOOL enable
)
{
    GT_STATUS retVal;

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    /* Call appropriate implementation by virtual function */
    retVal = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpEventsMask(devNum,
                                                                      intIndex,enable);

    return retVal;
}

/*******************************************************************************
* prvCpssDrvEventsMaskGet
*
* DESCRIPTION:
*       Gets enable/disable status of a given event reaching the CPU.
*
* INPUTS:
*       devNum      - The PP's device number to mask / unmask the interrupt for.
*       intIndex    - The interrupt cause to get enable/disable status.
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE enable, GT_FALSE disable.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssDrvEventsMaskGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  intIndex,
    OUT  GT_BOOL *enablePtr
)
{

    /* Call appropriate implementation by virtual function */
    prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpEventsMaskGet(
        devNum, intIndex, enablePtr);
}


