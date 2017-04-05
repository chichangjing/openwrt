/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
********************************************************************************
* appDemoExMxPmHsuUtils.h
*
* DESCRIPTION:
*       App demo ExMxPm External TCAM configuration API header.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#ifndef __appDemoExMxPmHsuUtilsh
#define __appDemoExMxPmHsuUtilsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
/*******************************************************************************
* appDemoExMxPmHsuOldImagePreUpdatePreparation
*
* DESCRIPTION:
*       This function perform following steps of HSU process:
*       1. Disable interrupts
*       2. export of all hsu datatypes
*       3. warm restart
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       isMultipleIteration - GT_TRUE - multiIteration HSU operation
*                             GT_FALSE - single iteration HSU operation
*       origIterationSize   - hsu iteration size
*       systemRecoveryMode  - hsu mode defines if application want to process
*                             messages arrived during HSU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place to save the request
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmHsuOldImagePreUpdatePreparation
(
  IN GT_BOOL                               isMultipleIteration,
  IN GT_U32                                origIterationSize,
  IN CPSS_SYSTEM_RECOVERY_MODE_STC         systemRecoveryMode
);

/*******************************************************************************
* appDemoExMxPmHsuNewImageUpdateAndSynch
*
* DESCRIPTION:
*       This function perform following steps of HSU process:
*       1. import of all hsu datatypes
*       2. setting HSU state in "CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E"
*       4. open pp interrupts
*       3. enable interrupts
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       isMultipleIteration - GT_TRUE - multiIteration HSU operation
*                             GT_FALSE - single iteration HSU operation
*       origIterationSize   - hsu iteration size
*       hsuDataSize         - hsu import data size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place to save the request
*
* COMMENTS:
*       When appDemoExMxPmHsuOldImagePreUpdatePreparation is finished and new image
*       have arisen after warm restart, user should perform
*       cpssSystemRecoveryStateSet
(CPSS_SYSTEM_RECOVERY_INIT_STATE_E), cpssInitSystem and only
*       after that call appDemoExMxPmHsuNewImageUpdateAndSynch.
*******************************************************************************/
GT_STATUS appDemoExMxPmHsuNewImageUpdateAndSynch
(
  IN GT_BOOL                               isMultipleIteration,
  IN GT_U32                                origIterationSize,
  IN GT_U32                                hsuDataSize
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __appDemoExMxPmHsuUtilsh */

