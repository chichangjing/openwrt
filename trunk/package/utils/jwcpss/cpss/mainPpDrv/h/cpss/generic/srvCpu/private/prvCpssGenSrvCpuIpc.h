/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenSrvCpuIpc.h
*
* DESCRIPTION:
*       Service CPU IPC API's
*
*
* FILE REVISION NUMBER:
*       $Revision: 00 $
*
*******************************************************************************/

#ifndef __prvCpssGenSrvCpuIpch
#define __prvCpssGenSrvCpuIpch

#include <cpss/generic/cpssTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssGenSrvCpuIpcSendMessage
*
* DESCRIPTION:
*       Send a message to the service CPU.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; 
*       Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum                   - device number.
*       msgPtr                   - (pointer to) message.
*       msgSize                  - message size.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenSrvCpuIpcMessageSend
(
    IN GT_U8    devNum,
    IN GT_U8    *msgPtr,
    IN GT_U32   msgSize
);

/*******************************************************************************
* prvCpssGenSrvCpuIpcMessageGet
*
* DESCRIPTION:
*       Get a message from the service CPU.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       msgPtr                   - (pointer to) message.
*       msgSizePtr               - (pointer to) message size.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenSrvCpuIpcMessageGet
(
    IN GT_U8    devNum,
    OUT GT_U8   *msgPtr,
    OUT GT_U32  *msgSizePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenSrvCpuIpch */
