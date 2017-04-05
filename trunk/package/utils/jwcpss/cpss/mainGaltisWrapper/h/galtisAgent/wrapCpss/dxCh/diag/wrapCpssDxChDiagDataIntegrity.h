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
* wrapCpssDxChDiagDataIntegrity.h
*
* DESCRIPTION:
*       This file implements DxCh data integrity event increment callback function
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
******************************************************************************/

#ifndef __wrapCpssDxChDiagDataIntegrity_h__
#define __wrapCpssDxChDiagDataIntegrity_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/diag/cpssDiag.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDataIntegrity.h>


typedef GT_STATUS DXCH_DATA_INTEGRITY_EVENT_COUNTER_INCREMENT_FUNC
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC     *eventPtr
);

extern GT_STATUS wrCpssDxChDiagDataIntegrityCountersCbGet
(
    DXCH_DATA_INTEGRITY_EVENT_COUNTER_INCREMENT_FUNC **dataIntegrityEventCounterBlockGetCB
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__wrapCpssDxChDiagDataIntegrity_h__*/


