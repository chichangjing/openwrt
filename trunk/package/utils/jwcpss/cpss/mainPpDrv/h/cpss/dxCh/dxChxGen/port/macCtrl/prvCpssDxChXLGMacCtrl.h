/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChXLGMacCtrl.h
*
* DESCRIPTION:
*       bobcat2 XLG mac control reg 5 API
*
* FILE REVISION NUMBER:
*       $Revision: 3$
*******************************************************************************/

#ifndef PRV_CPSS_DXCH_MAC_CTRL_H
#define PRV_CPSS_DXCH_MAC_CTRL_H

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvCpssDxChBobcat2PortMacXLGIsSupported
*
* DESCRIPTION:
*       check whether XLG mac is supported for specific mac
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - mac number
*
* OUTPUTS:
*       isSupportedPtr. is mac supported result
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on bad ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacXLGIsSupported
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     mac,
    OUT GT_BOOL                *isSupportedPtr
);

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacCtrl5FldSet
*
* DESCRIPTION:
*       XLG mac control register 5 set specific field
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum      - physical port number
*       fldOffs  - fld offset 
*       fldLen   - fld len
*       fldVal   - value to write
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacCtrl5FldSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U32                   fldOffs,
    IN GT_U32                   fldLen,
    IN GT_U32                   fldVal
);

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacCtrl5FldGet
*
* DESCRIPTION:
*       XLG mac control register 5 get specific field
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - physical port number
*       fldOffs  - fld offset 
*       fldLen   - fld len
*
* OUTPUTS:
*       fldValPtr   - value to write
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacCtrl5FldGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     mac,
    IN  GT_U32                   fldOffs,
    IN  GT_U32                   fldLen,
    OUT GT_U32                  *fldValPtr
);

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacIPGLengthSet
*
* DESCRIPTION:
*       XLG mac control register 5 set IPG length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - physical port number
*       length   - ipg length
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacIPGLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     mac,
    IN GT_U32                   length
);


/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacIPGLengthGet
*
* DESCRIPTION:
*       XLG mac control register 5 get IPG length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - physical port number
*
* OUTPUTS:
*       lengthPtr - pointer to length
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, mac
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacIPGLengthGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     mac,
    IN GT_U32                  *lengthPtr
);


/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacPreambleLengthSet
*
* DESCRIPTION:
*       XLG mac control register 5 set Preamble length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum      - max number
*       direction - RX/TX/both
*       length   - preamble length
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
);

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacPreambleLengthGet
*
* DESCRIPTION:
*       XLG mac control register 5 get Preamble length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - max number
*       direction - RX/TX/both
*
* OUTPUTS:
*       lengthPtrr   - pointer to preamble length
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacPreambleLengthGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     mac,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                  *lengthPtr
);

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacNumCRCByteshSet
*
* DESCRIPTION:
*       XLG mac control register 5 set number of CRC bytes
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       direction - RX/TX/both
*       crcBytesNum - number of CRC bytes
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacNumCRCByteshSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   crcBytesNum
);

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacNumCRCByteshGet
*
* DESCRIPTION:
*       XLG mac control register 5 get number of CRC bytes
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - max number
*       direction - RX/TX/both
*
* OUTPUTS:
*       crcBytesNumPtr - pointer to number of CRC bytes
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacNumCRCByteshGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     mac,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                  *crcBytesNumPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
