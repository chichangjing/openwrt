/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenericSrvCpuLoad.h
*
* DESCRIPTION:
*       Service CPU firmware load/start APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvCpssGenericSrvCpuLoad_h__
#define __prvCpssGenericSrvCpuLoad_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*******************************************************************************
* prvCpssGenericSrvCpuLoad
*
* DESCRIPTION:
*       Upload firmware to service CPU according to firmware table
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
GT_STATUS prvCpssGenericSrvCpuLoad
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssGenericSrvCpuStop
*
* DESCRIPTION:
*       Stop (reset) service CPU(MSYS)
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
GT_STATUS prvCpssGenericSrvCpuStop
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssGenericSrvCpuStart
*
* DESCRIPTION:
*       Start (unreset) service CPU(MSYS)
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
GT_STATUS prvCpssGenericSrvCpuStart
(
    IN GT_U8    devNum
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenericSrvCpuLoad_h__ */
