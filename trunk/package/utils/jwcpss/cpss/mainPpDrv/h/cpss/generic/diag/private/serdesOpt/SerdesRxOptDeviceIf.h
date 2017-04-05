/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* SerdesRxOptDeviceIf.h
*
* DESCRIPTION:
*		Serdes device interface definition
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/

#ifndef __serdesRxOptDevIf_H
#define __serdesRxOptDevIf_H

#ifdef __cplusplus
extern "C" {
#endif

#include <serdesOpt/SerdesRxOptimizer.h>
#include <serdesOpt/private/serdesOptPrivate.h>

/*******************************************************************************
* mvSerdesLaneRegistration
*
* DESCRIPTION:
*       Init all global variables for tunning algorithm.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
int mvSerdesLaneRegistration
(
    unsigned int    devNum, 
    MV_ACCESS_FUNCS *accFuncs
);

#ifdef __cplusplus
}
#endif

#endif /* __serdesRxOptDevIf_H */


