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
* mvHwsServiceCpuFwIf.h
*
* DESCRIPTION:
*           This file contains HWS Service Firmware Interface definition
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/

#ifndef __mvHwsServiceCpuFwIF_H
#define __mvHwsServiceCpuFwIF_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* mvHwsServiceCpuEnable
*
* DESCRIPTION:
*       Service CPU Enable for device
*
* INPUTS:
*       devNum - device number
*       enable - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuEnable(GT_U8 devNum, GT_U32 enable);

/*******************************************************************************
* mvHwsServiceCpuEnableGet
*
* DESCRIPTION:
*       Return Service CPU Enable state
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_U32 mvHwsServiceCpuEnableGet(GT_U8 devNum);

/*******************************************************************************
* mvHwsServiceCpuFwInit
*
* DESCRIPTION:
*       HWS FW initialization.
*
* INPUTS:
*       devNum    - system device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuFwInit(GT_U8 devNum);

/*******************************************************************************
* mvHwsServiceCpuFwPostInit
*
* DESCRIPTION:
*       Init HWS Firmware After Avago Serdes init
*
* INPUTS:
*       devNum    - system device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuFwPostInit(GT_U8 devNum);

#ifdef __cplusplus
}
#endif

#endif /* __mvHwsServiceCpuFwIF_H */


