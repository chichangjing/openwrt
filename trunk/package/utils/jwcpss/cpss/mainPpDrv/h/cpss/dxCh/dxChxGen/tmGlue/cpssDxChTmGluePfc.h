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
* cpssDxChTmGluePfc.h
*
* DESCRIPTION:
*       Traffic Manager Glue - PFC API declaration.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#ifndef __cpssDxChTmGluePfch
#define __cpssDxChTmGluePfch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
 * typedef: enum CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT
 *
 * Description: Enumeration of PFC response mode.
 *
 * Enumerations:
 *  CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_TXQ_E -
 *                          TXQ responds to PFC message.
 *  CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_TM_E -
 *                          TM responds to PFC message.
 *
 * Comments:
 *         None.
 */
typedef enum
{
    CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_TXQ_E,
    CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_TM_E
}CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT;

/*******************************************************************************
* cpssDxChTmGluePfcTmTcPort2CNodeSet
*
* DESCRIPTION:
*       Sets C node value for given pfc port and traffic class.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       pfcPortNum  - pfc port number. (APPLICABLE RANGES: 0..63)
*       tc          - traffic class (APPLICABLE RANGES: 0..7).
*       cNodeValue  - C node value (APPLICABLE RANGES: 0...511).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number,
*                                  port or traffic class.
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*   To enable PFC response by the TM, the Timer values of the received PFC frames must be
*   redirected to the TM unit (using cpssDxChTmGluePfcResponseModeSet).
*   C-nodes must be allocated so that a C-node serves a specific {port, priority}. 
*   For example, to support 8 PFC priorities on a specific port, 8 C-Nodes must be associated with that port.
*   Queues that serve a specific port and a PFC priority must be associated with the respective C-node.
* 
*   64 ingress ports (pfcPort) are available to redirect the Timer values of the received
*   PFC frames to the TM unit, this API is used along with cpssDxChTmGluePfcPortMappingSet 
*   to map physical port to its pfcPort. 
* 
*   Specific CNode must have one instance in TcPortToCnode Table,
*   CNodes are switched by the API to have such behaviour.
*******************************************************************************/
GT_STATUS cpssDxChTmGluePfcTmTcPort2CNodeSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     pfcPortNum,
    IN GT_U32                   tc,
    IN GT_U32                   cNodeValue
);

/*******************************************************************************
* cpssDxChTmGluePfcTmTcPort2CNodeGet
*
* DESCRIPTION:
*       Gets C node for given traffic class and pfc port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       pfcPortNum  - pfc port number. (APPLICABLE RANGES: 0..63).
*       tc          - traffic class (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       cNodeValuePtr - (pointer to) C node value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number, C node,
*                                  traffic class or port.
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTmGluePfcTmTcPort2CNodeGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     pfcPortNum,
    IN GT_U32                   tc,
    OUT GT_U32                 *cNodeValuePtr
);

/*******************************************************************************
* cpssDxChTmGluePfcResponseModeSet
*
* DESCRIPTION:
*       Enable/Disable PFC (Priority Flow Control) support with Traffic Manager (TM).
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - physical port number, relevant for Caelum only. (APPLICABLE RANGES: 0..255). 
*       responseMode    - PFC response mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number or PFC response mode.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Determine whether Timer values of the received PFC frames are redirected to be responded
*       by the TM unit.(used along with cpssDxChTmGluePfcTmTcPort2CNodeSet)
*       for Bobcat2 configuration is applied at Device level (portNum is ignored).
*       for Caelum configuration is applied at portNum only. (used along with cpssDxChTmGluePfcPortMappingSet)
*******************************************************************************/
GT_STATUS cpssDxChTmGluePfcResponseModeSet
(
    IN GT_U8                devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT responseMode
);

/*******************************************************************************
* cpssDxChTmGluePfcResponseModeGet
*
* DESCRIPTION:
*       Get PFC (Priority Flow Control) for TM support status.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum  - device number.
*       portNum - physical port number, relevant for Caelum only. (APPLICABLE RANGES: 0..255). 
* OUTPUTS:
*       responseModePtr          - (pointert to) PFC response mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number number.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_STATE             - on wrong response mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Get whether Timer values of the received PFC frames are redirected to be
*       responded by the TM unit.
*       for Bobcat2 configuration is retrived by Device level (portNum is ignored).
*       for Caelum configuration is retrieved by portNum.
*******************************************************************************/
GT_STATUS cpssDxChTmGluePfcResponseModeGet
(
    IN GT_U8                devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT *responseModePtr
);

/*******************************************************************************
* cpssDxChTmGluePfcPortMappingSet
*
* DESCRIPTION:
*       Map physical port to pfc port, used to map physical ports 0..255
*       to pfc ports 0..63.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number. (APPLICABLE RANGES: 0..255). 
*       pfcPortNum  - ingress pfc port number. (APPLICABLE RANGES: 0..63). 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number, or port.
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       64 ingress ports (pfcPort) are available to redirect the Timer values of the received
*       PFC frames to the TM unit, this API is used to map physical port to its pfcPort. 
*******************************************************************************/
GT_STATUS cpssDxChTmGluePfcPortMappingSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_PHYSICAL_PORT_NUM     pfcPortNum
);

/*******************************************************************************
* cpssDxChTmGluePfcPortMappingGet
* 
* DESCRIPTION:
*       Get physical port to pfc port mapping.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number. (APPLICABLE RANGES: 0..255). 
* 
* OUTPUTS:
*       pfcPortNum  - (pointer to) pfc port number (APPLICABLE RANGES: 0..63).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number, or port.
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTmGluePfcPortMappingGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_PHYSICAL_PORT_NUM    *pfcPortNumPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTmGluePfch */


