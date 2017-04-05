/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssGenSystemRecovery.c
*
* DESCRIPTION:
*       Generic System Recovery ApIs
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/interrupts/private/prvCpssGenIntDefs.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/private/prvCpssDxChSystemRecoveryLog.h>

/* define the HSU  global DB */
CPSS_SYSTEM_RECOVERY_INFO_STC   systemRecoveryInfo;
PRV_CPSS_SYSTEM_RECOVERY_MSG_DISABLE_MODE_FUN     prvCpssSystemRecoveryMsgDisableModeHandleFuncPtr = NULL;
PRV_CPSS_SYSTEM_RECOVERY_CATCH_UP_FUN             prvCpssSystemRecoveryCatchUpHandleFuncPtr = NULL;

/*******************************************************************************
* internal_cpssSystemRecoveryStateSet
*
* DESCRIPTION:
*       This function inform cpss about system recovery state
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       systemRecoveryInfoPtr           - pointer to system recovery information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*       GT_BAD_PTR             - on NULL pointer
*       GT_BAD_PARAM           - on bad systemRecoveryState
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static GT_STATUS internal_cpssSystemRecoveryStateSet
(
    IN CPSS_SYSTEM_RECOVERY_INFO_STC   *systemRecoveryInfoPtr
)
{
    GT_STATUS rc = GT_OK;
    CPSS_NULL_PTR_CHECK_MAC(systemRecoveryInfoPtr);
    switch(systemRecoveryInfoPtr->systemRecoveryState)
    {
    case CPSS_SYSTEM_RECOVERY_PREPARATION_STATE_E:
    case CPSS_SYSTEM_RECOVERY_INIT_STATE_E:
    case CPSS_SYSTEM_RECOVERY_HW_CATCH_UP_STATE_E:
    case CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    systemRecoveryInfo.systemRecoveryState = systemRecoveryInfoPtr->systemRecoveryState;
    systemRecoveryInfo.systemRecoveryMode  = systemRecoveryInfoPtr->systemRecoveryMode;
    systemRecoveryInfo.systemRecoveryProcess = systemRecoveryInfoPtr->systemRecoveryProcess;

    switch(systemRecoveryInfoPtr->systemRecoveryProcess)
    {
    case CPSS_SYSTEM_RECOVERY_PROCESS_HSU_E:
        if (systemRecoveryInfoPtr->systemRecoveryMode.haCpuMemoryAccessBlocked == GT_TRUE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if ( ((systemRecoveryInfoPtr->systemRecoveryMode.continuousRx == GT_FALSE)          ||
              (systemRecoveryInfoPtr->systemRecoveryMode.continuousTx == GT_FALSE)          ||
              (systemRecoveryInfoPtr->systemRecoveryMode.continuousAuMessages == GT_FALSE)  ||
              (systemRecoveryInfoPtr->systemRecoveryMode.continuousFuMessages == GT_FALSE) ) &&
              (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_PREPARATION_STATE_E) )
        {

               /* make specific to this mode arrangements for RX,TX,AU,FU queues*/
            CPSS_FUNCPTR_CALL_NULL_COMPARED_MAC(prvCpssSystemRecoveryMsgDisableModeHandleFuncPtr,rc);
        
        }
        break;
    case CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E:
        if ( (systemRecoveryInfoPtr->systemRecoveryMode.haCpuMemoryAccessBlocked == GT_TRUE) ||
             (systemRecoveryInfoPtr->systemRecoveryMode.continuousRx == GT_TRUE)             ||
             (systemRecoveryInfoPtr->systemRecoveryMode.continuousTx == GT_TRUE)             ||
             (systemRecoveryInfoPtr->systemRecoveryMode.continuousAuMessages == GT_TRUE)     ||
             (systemRecoveryInfoPtr->systemRecoveryMode.continuousFuMessages == GT_TRUE))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_HW_CATCH_UP_STATE_E)
        {   

              CPSS_FUNCPTR_CALL_NULL_COMPARED_MAC(prvCpssSystemRecoveryCatchUpHandleFuncPtr,rc); 
        }
        break;
        
    case CPSS_SYSTEM_RECOVERY_PROCESS_HA_E:
        if ( (systemRecoveryInfoPtr->systemRecoveryMode.haCpuMemoryAccessBlocked == GT_TRUE) &&
             ((systemRecoveryInfoPtr->systemRecoveryMode.continuousRx == GT_TRUE)         ||
              (systemRecoveryInfoPtr->systemRecoveryMode.continuousTx == GT_TRUE)         ||
              (systemRecoveryInfoPtr->systemRecoveryMode.continuousAuMessages == GT_TRUE) ||
              (systemRecoveryInfoPtr->systemRecoveryMode.continuousFuMessages == GT_TRUE)) )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
        if (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_HW_CATCH_UP_STATE_E)
        {
             CPSS_FUNCPTR_CALL_NULL_COMPARED_MAC(prvCpssSystemRecoveryCatchUpHandleFuncPtr,rc); 
        }
        break;
    case CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E)
    {
        /* it means that system recovery process is finished  */
        systemRecoveryInfo.systemRecoveryProcess = CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E;
        systemRecoveryInfo.systemRecoveryMode.continuousAuMessages = GT_FALSE;
        systemRecoveryInfo.systemRecoveryMode.continuousFuMessages = GT_FALSE;
        systemRecoveryInfo.systemRecoveryMode.continuousRx = GT_FALSE;
        systemRecoveryInfo.systemRecoveryMode.continuousTx = GT_FALSE;
        systemRecoveryInfo.systemRecoveryMode.haCpuMemoryAccessBlocked = GT_FALSE;
    }

    return rc;
}

/*******************************************************************************
* cpssSystemRecoveryStateSet
*
* DESCRIPTION:
*       This function inform cpss about system recovery state
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       systemRecoveryInfoPtr           - pointer to system recovery information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*       GT_BAD_PTR             - on NULL pointer
*       GT_BAD_PARAM           - on bad systemRecoveryState
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssSystemRecoveryStateSet
(
    IN CPSS_SYSTEM_RECOVERY_INFO_STC   *systemRecoveryInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssSystemRecoveryStateSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, systemRecoveryInfoPtr));

    rc = internal_cpssSystemRecoveryStateSet(systemRecoveryInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, systemRecoveryInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


