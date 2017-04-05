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
* wrapHws.h
*
* DESCRIPTION:
*       Galtis commands for HWS debug
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/
#ifndef __WrapHwsH
#define __WrapHwsH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cmdShell/common/cmdCommon.h>

/*******************************************************************************
*
*
* DESCRIPTION:
*     Library database initialization functions.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitDDR3();
GT_STATUS cmdLibInitHwsPortInit();
GT_STATUS cmdLibInitHwsMacIf();
GT_STATUS cmdLibInitHwsSerdesInit();
GT_STATUS cmdLibInitHwsPcsIf();
GT_STATUS cmdLibInitEvents();
GT_STATUS cmdLibInitHwUtils();
GT_STATUS cmdLibInitPortAp();
GT_STATUS cmdLibInitDDR3TrainingIP();
GT_STATUS cmdLibInitDdr3Bap();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WrapHwsH */


