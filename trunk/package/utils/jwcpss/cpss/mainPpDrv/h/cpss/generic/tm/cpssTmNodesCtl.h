/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmNodesCtl.h
*
* DESCRIPTION:
*       TM nodes control APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssTmNodesCtlh
#define __cpssTmNodesCtlh

#include <cpss/generic/tm/cpssTmPublicDefs.h>


/*******************************************************************************
* cpssTmNodesCtlQueueInstall
*
* DESCRIPTION:
*       Install queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index (APPLICABLE RANGES: 0..16383).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Makes a queue capable of getting traffic.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlQueueInstall
(
    IN GT_U8    devNum,
    IN GT_U32   queueInd
);


/*******************************************************************************
* cpssTmNodesCtlQueueUninstall
*
* DESCRIPTION:
*       Uninstall queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index (APPLICABLE RANGES: 0..16383).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       - Makes a queue incapable of getting traffic. Queue must be not in use!
*       - User should flush Q before this call.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlQueueUninstall
(
    IN GT_U8    devNum,
    IN GT_U32   queueInd
);

/*******************************************************************************
* cpssTmNodesCtlQueueInstallStatusGet
*
* DESCRIPTION:
*       Retrieves install status if the queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index (APPLICABLE RANGES: 0..16383).
* OUTPUTS:
*       statusPtr         - (Pointer to) queue install status.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL statusPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlQueueInstallStatusGet
(
    IN GT_U8    devNum,
    IN GT_U32   queueInd,
    OUT GT_U32  *statusPtr
);

/*******************************************************************************
* cpssTmNodesCtlNodeDelete
*
* DESCRIPTION:
*       Delete node from scheduling hierarchy.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       level             - Scheduling level: Queue/A/B/C-node/Port.
*       index             - Queue/node index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_STATE             - on not free node's subtree.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlNodeDelete
(
    IN GT_U8                devNum,
    IN CPSS_TM_LEVEL_ENT    level,
    IN GT_U32               index
);


/*******************************************************************************
* cpssTmNodesCtlTransPortDelete
*
* DESCRIPTION:
*       Delete port and all its subtree from scheduling hierarchy.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlTransPortDelete
(
    IN GT_U8    devNum,
    IN GT_U32   index
);


/*******************************************************************************
* cpssTmQueueFlush
*
* DESCRIPTION:
*       Flush queue by a given index. All packets dequeued from this queue are dropped.
*       Wait till flush queue has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Queue index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush queue blocking API.
*******************************************************************************/
GT_STATUS cpssTmQueueFlush
(
    IN GT_U8                    devNum,
    IN GT_U32                   index
);


/*******************************************************************************
* cpssTmQueueFlushTriggerActionStart
*
* DESCRIPTION:
*       Activate Flush queue by a given index.
*       All packets dequeued from this queue are dropped.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Queue index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush queue non blocking API.
*******************************************************************************/
GT_STATUS cpssTmQueueFlushTriggerActionStart
(
    IN GT_U8                    devNum,
    IN GT_U32                   index
);


/*******************************************************************************
* cpssTmQueueFlushTriggerActionStatusGet
*
* DESCRIPTION:
*       Get whether Flush queue has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Queue index.
*
* OUTPUTS:
*       actFinishedPtr  - GT_FALSE - flush is active (packets are dropped on dequeue).
*                       - GT_TRUE  - flush off /completed.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_BAD_PTR               - on NULL actFinishedPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmQueueFlushTriggerActionStatusGet
(
    IN GT_U8                    devNum,
    IN GT_U32                   index,
    OUT GT_BOOL                 *actFinishedPtr
);


/*******************************************************************************
* cpssTmPortFlush
*
* DESCRIPTION:
*       Flush port by a given index. All packets dequeued from this port (and all its queues)
*       are dropped. Wait till flush port has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush port blocking API.
*******************************************************************************/
GT_STATUS cpssTmPortFlush
(
    IN GT_U8                    devNum,
    IN GT_U32                   index
);


/*******************************************************************************
* cpssTmPortFlushTriggerActionStart
*
* DESCRIPTION:
*       Flush port by a given index. All packets dequeued from this port (and all its queues)
*       are dropped.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush port non blocking API.
*******************************************************************************/
GT_STATUS cpssTmPortFlushTriggerActionStart
(
    IN GT_U8                    devNum,
    IN GT_U32                   index
);


/*******************************************************************************
* cpssTmPortFlushTriggerActionStatusGet
*
* DESCRIPTION:
*       Get whether Flush port has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       actFinishedPtr  - GT_FALSE - flush is active (packets are dropped on dequeue).
*                       - GT_TRUE  - flush off /completed.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_BAD_PTR               - on NULL actFinishedPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmPortFlushTriggerActionStatusGet
(
    IN GT_U8                    devNum,
    IN GT_U32                   index,
    OUT GT_BOOL                 *actFinishedPtr
);


/*******************************************************************************
* cpssTmNodesCtlReadNextChange
*
* DESCRIPTION:
*       Read next tree index/range change after reshuffling.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       changePtr         - Change structure pointer.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_BAD_PTR               - on NULL changePtr.
*       GT_NO_RESOURCE           - on empty list.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlReadNextChange
(
    IN GT_U8                        devNum,
    OUT CPSS_TM_TREE_CHANGE_STC     *changePtr
);


/*******************************************************************************
* cpssTmNodesCtlCleanList
*
* DESCRIPTION:
*       Empty list of reshuffling changes.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlCleanList
(
    IN GT_U8                        devNum
);


#endif 	    /* __cpssTmNodesCtlh */


