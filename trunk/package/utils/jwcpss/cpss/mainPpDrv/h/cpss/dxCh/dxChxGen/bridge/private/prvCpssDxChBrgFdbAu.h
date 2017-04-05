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
* prvCpssDxChBrgFdbAu.h
*
* DESCRIPTION:
*       MAC hash struct implementation.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#ifndef __prvCpssDxChBrgFdbAuh
#define __prvCpssDxChBrgFdbAuh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/interrupts/private/prvCpssGenIntDefs.h>


/*
 *  typedef: enum  MESSAGE_QUEUE_ENT
 *
 *  Description: hold the different types of message queues that cpss SW uses
 *
 *  Enumerations:
 *      MESSAGE_QUEUE_PRIMARY_AUQ_E - the primary AUQ
 *      MESSAGE_QUEUE_PRIMARY_FUQ_E - the primary FUQ
 *      MESSAGE_QUEUE_SECONDARY_AUQ_E - the secondary AUQ
*/
typedef enum
{
    MESSAGE_QUEUE_PRIMARY_AUQ_E,
    MESSAGE_QUEUE_PRIMARY_FUQ_E,
    MESSAGE_QUEUE_SECONDARY_AUQ_E
}MESSAGE_QUEUE_ENT;

/************************************************************************
* auqFuqClearUnreadMessages
*
* DESCRIPTION:
*       The function scan the AU/FU queues, clear all unread
*       AU/FU messages in the queue and calculate number of messages
*       that remained till the end of the queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - the device number on which AU are counted
*       portGroupId           - the portGroupId - for multi-port-groups support
*       queueType             - AUQ or FUQ. FUQ valid for DxCh2 and above.
*
* OUTPUTS:
*       numOfAuPtr            - (pointer to) number of AU messages processed in the specified queue.
*       numMsgTillQueueEndPtr - (pointer to) number of AU messages remained to fill the queue.
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, queueType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - on not initialized queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to have the accurate number of entries  application should
*       protect Mutual exclusion between HW access to the AUQ/FUQ
*       SW access to the AUQ/FUQ and calling to this API.
*       i.e. application should stop the PP from sending AU messages to CPU.
*       and should not call  the api's
*       cpssDxChBrgFdbFuMsgBlockGet, cpssDxChBrgFdbFuMsgBlockGet
*
***********************************************************************/
GT_STATUS auqFuqClearUnreadMessages
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  MESSAGE_QUEUE_ENT             queueType,
    OUT GT_U32                       *numOfAuPtr,
    OUT GT_U32                       *numMsgTillQueueEndPtr
);

/************************************************************************
* auqFuqMessagesNumberGet
*
* DESCRIPTION:
*       The function scan the AU/FU queues and returns the number of
*       AU/FU messages in the queue.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number on which AU are counted
*       portGroupId       - the portGroupId - for multi-port-groups support
*       queueType - AUQ or FUQ. FUQ valid for DxCh2 and above.
*
* OUTPUTS:
*       numOfAuPtr - (pointer to) number of AU messages in the specified queue.
*       endOfQueueReachedPtr - (pointer to) GT_TRUE: The queue reached to the end.
*                                        GT_FALSE: else
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, queueType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - on not initialized queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to have the accurate number of entries  application should
*       protect Mutual exclusion between HW access to the AUQ/FUQ
*       SW access to the AUQ/FUQ and calling to this API.
*       i.e. application should stop the PP from sending AU messages to CPU.
*       and should not call  the api's
*       cpssDxChBrgFdbFuMsgBlockGet, cpssDxChBrgFdbFuMsgBlockGet
*
***********************************************************************/
GT_STATUS auqFuqMessagesNumberGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  MESSAGE_QUEUE_ENT             queueType,
    OUT GT_U32                       *numOfAuPtr,
    OUT GT_BOOL                      *endOfQueueReachedPtr
);

/*******************************************************************************
* auDesc2UpdMsg
*
* DESCRIPTION:
*       This function is called whenever an address update message is received.
*       It translates the descriptor into CPSS_MAC_UPDATE_MSG_EXT_STC format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - number of device whose descriptor queue is processed
*       portGroupId    - the portGroupId - for multi-port-groups support
*       auDescExtPtr   - (pointer to) the AU descriptor filled by PP.
*       auDescrFromDma - AU descriptor is from DMA queue
*                        GT_TRUE - AU descriptor is from DMA queue
*                        GT_FALSE - AU descriptor is from FIFO but not DMA queue
*
* OUTPUTS:
*       addrUpMsgPtr     - MAC format of entry
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS auDesc2UpdMsg
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  PRV_CPSS_AU_DESC_EXT_8_STC    *auDescExtPtr,
    IN  GT_BOOL                       auDescrFromDma,
    OUT CPSS_MAC_UPDATE_MSG_EXT_STC   *addrUpMsgPtr
);

/*******************************************************************************
* prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet
*
* DESCRIPTION:
*       Set the field mask for 'MAC entry does NOT reside on the local port group'
*       When Enabled - AU/FU messages are not sent to the CPU if the MAC entry
*                      does NOT reside on the local portgroup, i.e.
*                      the entry port[5:4] != LocalPortGroup
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*                      --ONLY for multi-Port Group device
*
* INPUTS:
*       devNum      - device number
*       enable      - mask enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*    GT_OK           - on success
*    GT_HW_ERROR     - on HW error
*    GT_BAD_PARAM    - on bad device number.
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*       none
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableGet
*
* DESCRIPTION:
*       Get the field mask for 'MAC entry does NOT reside on the local port group'
*       When Enabled - AU/FU messages are not sent to the CPU if the MAC entry
*                      does NOT reside on the local core, i.e.
*                      the entry port[5:4] != LocalPortGroup
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*                      --ONLY for multi-Port Group device
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) mask enable/disable
*
* RETURNS:
*    GT_OK           - on success
*    GT_HW_ERROR     - on HW error
*    GT_BAD_PARAM    - on bad device number.
*    GT_BAD_PTR      - on NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*       none
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChBrgFdbAuh */
