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
* prvCpssDxChBrg.h
*
* DESCRIPTION:
*       Common private bridge declarations.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/
#ifndef __prvCpssDxChBrgh
#define __prvCpssDxChBrgh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/interrupts/private/prvCpssGenIntDefs.h>

/*******************************************************************************
* prvCpssDxChBrgCpuPortBmpConvert
*
* DESCRIPTION:
*       Convert port bitmap according to physical CPU port connection.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Xcat; Xcat2.
*
* INPUTS:
*       devNum              - device number
*       isWriteConversion   - direction of conversion
*                             GT_TRUE - write conversion
*                             GT_FALSE - read conversion
*       portBitmapPtr       - (pointer to) bmp of ports members in vlan
*                             CPU port supported
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgCpuPortBmpConvert
(
    IN GT_U8                devNum,
    IN GT_BOOL              isWriteConversion,
    IN CPSS_PORTS_BMP_STC   *portBitmapPtr
);

/*******************************************************************************
* prvCpssDxChFullQueueRewind
*
* DESCRIPTION:
*   Rewind Full FU or AU queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupId     - The port group id. relevant only to 'multi-port-groups'
*                         devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       isFuQueue       - is FDB/CNC Upload Queue ?
*                         GT_TRUE  - FDB/CNC Upload Queue
*                         GT_FALSE - FDB AU Messages Queue
*       appDescCtrlPtr  - (pointer to Descriptor Control Structure passed by application)
*                         if NULL specified used primary queues descriptors
*                         This parameter is optional and allows to application
*                         to specify the secondary queues.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChFullQueueRewind
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  GT_BOOL                     isFuQueue,
    IN  PRV_CPSS_AU_DESC_CTRL_STC   *appDescCtrlPtr
);

/*******************************************************************************
* prvCpssDxChFuqOwnerSet
*
* DESCRIPTION:
*   Set FUQ used for CNC / FDB upload.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       isCnc           - is CNC need to be used ?
*                         GT_TRUE - CNC need to use the FUQ
*                         GT_FALSE - FDB upload need to use the FUQ
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChFuqOwnerSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  GT_BOOL                     isCnc
);

/*******************************************************************************
* prvCpssDxChBrgFdbSizeSet
*
* DESCRIPTION:
*       function to set the FDB size.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number.
*       fdbSize        - the FDB size to set.
*                       (each device with it's own relevant values)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad device or FDB table size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbSizeSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  fdbSize
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChBrgh */
