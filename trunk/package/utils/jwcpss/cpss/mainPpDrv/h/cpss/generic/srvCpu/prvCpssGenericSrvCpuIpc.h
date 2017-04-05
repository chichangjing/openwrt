/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenericSrvCpuIpc.h
*
* DESCRIPTION:
*       Service CPU IPC APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvCpssGenericSrvCpuIpc_h__
#define __prvCpssGenericSrvCpuIpc_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*******************************************************************************
* prvCpssGenericSrvCpuIpcInit
*
* DESCRIPTION:
*       Configure IPC, open and attach all channels
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
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
GT_STATUS prvCpssGenericSrvCpuIpcInit
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssGenericSrvCpuIpcMessageSend
*
* DESCRIPTION:
*       This API allows the application to send a message to other side,
*       using a buffer it was previously fetched from the IPC (by using the
*       Buffer-Get API)
*       It may choose to send asynchrony or synchronic and in case
*       synchronic how much tome to wait for processing.
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
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
GT_STATUS prvCpssGenericSrvCpuIpcMessageSend
(
    IN  GT_U8    devNum,
    IN  GT_U32   channel,
    IN  GT_U8   *msgData,
    IN  GT_U32   msgDataSize
);

/*******************************************************************************
* prvCpssGenericSrvCpuIpcMessageRecv
*
* DESCRIPTION:
*       This API allows the application to fetch message sent by the other
*       side. Application may choose to wait for incoming message or not
*       (if application wish to use polling it can loop with no waiting).
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
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
GT_STATUS prvCpssGenericSrvCpuIpcMessageRecv
(
    IN  GT_U8    devNum,
    IN  GT_U32   channel,
    OUT GT_U8   *msgData,
    OUT GT_U32  *msgDataSize
);

/*******************************************************************************
* prvCpssGenericSrvCpuWrite
*
* DESCRIPTION:
*       Write to Service CPU "firmware section"
*       TODO: remove, design bug
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
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
GT_STATUS prvCpssGenericSrvCpuWrite
(
    IN  GT_U8    devNum,
    IN  GT_U32   offset,
    IN  void    *data,
    IN  GT_U32   size
);

/*******************************************************************************
* prvCpssGenericSrvCpuRead
*
* DESCRIPTION:
*       Read from Service CPU "firmware section"
*       TODO: remove, design bug
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
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
GT_STATUS prvCpssGenericSrvCpuRead
(
    IN  GT_U8    devNum,
    IN  GT_U32   offset,
    OUT void    *data,
    IN  GT_U32   size
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenericSrvCpuIpc_h__ */
