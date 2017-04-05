/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenBrgFdbIsrSig.h
*
* DESCRIPTION:
*       Includes ISR and FDB signalling mechanism create/signal/wait/delete API
*       This file provide the devices ability to get indication about 2 events:
*       1. FDB AU-message from CPU action or query action ended (NA/QA/QI)
*       2. FDB mac trigger action ended (flush/transplant/trig aging)
*
*       1. When the CPSS try to operate FDB manipulation via the "AU messages
*       mechanism" of the PP, the CPSS first check that the last operation was
*       ended - this is done by waiting for the event of
*               CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E
*       2. When the CPSS try to operate FDB mac trigger action
*          (Flush/transplant/trigger aging) in the PP , the CPSS trigger the
*          action and wait for the action to end. - this is done by waiting for
*           the event of CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __prvCpssGenBrgFdbIsrSigh
#define __prvCpssGenBrgFdbIsrSigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
/*
 * Typedef: enum CPSS_FDB_ISR_SIG_ENT
 *
 * Description: Enumeration that defines the FDB signalling app type.
 *
 * Enumerations:
 *      CPSS_QUERY_E  - Query operation signal.
 *                    relate to the event of CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E
 *      CPSS_MAC_TRIG_ACTION_E - MAC trigger Action Signal.
 *                    relate to the event of CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E
 */
typedef enum {
    CPSS_QUERY_E,
    CPSS_MAC_TRIG_ACTION_E
} CPSS_FDB_ISR_SIG_ENT;


/*******************************************************************************
* prvCpssFdbIsrSigCreate
*
* DESCRIPTION:
*       This routine creates a signalling mechanism for FDB ISR.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       sigType  - The event signal type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigCreate
(
    IN CPSS_FDB_ISR_SIG_ENT       sigType
);


/*******************************************************************************
* prvCpssFdbIsrSigDelete
*
* DESCRIPTION:
*       This routine deletes a signalling mechanism for FDB ISR.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       sigType  - The event signal type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigDelete
(
    IN CPSS_FDB_ISR_SIG_ENT       sigType
);


/*******************************************************************************
* prvCpssFdbIsrSigSignal
*
* DESCRIPTION:
*       This routine signals a waiting task.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       sigType  - The event signal type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigSignal
(
    IN CPSS_FDB_ISR_SIG_ENT       sigType
);


/*******************************************************************************
* prvCpssFdbIsrSigWait
*
* DESCRIPTION:
*       This routine waits for a signal for the event to occur.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       sigType  - The event signal type.
*       timeout  - Time-out in milliseconds or 0 to wait forever.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigWait
(
    IN CPSS_FDB_ISR_SIG_ENT sigType,
    IN GT_U32               timeout
);

/*******************************************************************************
* prvCpssFdbIsrSigHaModeSet
*
* DESCRIPTION:
*       function to set CPU High Availability mode of operation.
*
* APPLICABLE DEVICES:  
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - the device number.
*       mode - active or standby
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - on failure.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigHaModeSet
(
    IN  GT_U8   devNum,
    IN  CPSS_SYS_HA_MODE_ENT mode
);


/*******************************************************************************
* prvCpssFdbIsrSigUnmaskDevice
*
* DESCRIPTION:
*       This routine unmasks the unified events of "mac update done" /
*       "mac trigger action done" on specific device.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sigType  - The event signal type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigUnmaskDevice
(
    IN GT_U8                      devNum,
    IN CPSS_FDB_ISR_SIG_ENT       sigType
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenBrgFdbIsrSigh */
