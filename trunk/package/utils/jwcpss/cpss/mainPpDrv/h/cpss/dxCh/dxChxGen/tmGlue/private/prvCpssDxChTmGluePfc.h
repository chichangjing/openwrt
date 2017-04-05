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
* prvCpssDxChTmGlue.h
*
* DESCRIPTION:
*       Traffic Manager Glue - common private PFC API declarations.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __prvCpssDxChTmGluePfch
#define __prvCpssDxChTmGluePfch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

#define PRV_CPSS_DXCH_TM_GLUE_PFC_MAX_PORT_CNS  64
#define PRV_CPSS_DXCH_TM_GLUE_PFC_MAX_TC_CNS     8
#define PRV_CPSS_DXCH_TM_GLUE_PFC_MAX_CNODE_CNS BIT_9

/* check that the TM physical port number is valid return GT_BAD_PARAM on error */
#define PRV_CPSS_DXCH_TM_GLUE_PFC_PHY_PORT_NUM_CHECK_MAC(_portNum)  \
    if ((_portNum) >= PRV_CPSS_DXCH_TM_GLUE_PFC_MAX_PORT_CNS)       \
    {                                                               \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                        \
    }


typedef struct
{
    GT_U16 TcPort2CnodeSwTable[PRV_CPSS_DXCH_TM_GLUE_PFC_MAX_PORT_CNS][PRV_CPSS_DXCH_TM_GLUE_PFC_MAX_TC_CNS];
} PRV_CPSS_DXCH_TM_GLUE_PFC_DEV_INFO_STC;

typedef struct
{
    GT_PHYSICAL_PORT_NUM port;
    GT_U32               tc;
    GT_U16               cNode;
} PRV_CPSS_DXCH_TM_GLUE_PFC_CNODE_INFO_STC;

extern GT_BOOL prvPfcTmGlueCnodeMappingOnce;


/*******************************************************************************
* prvCpssDxChTmGluePfcTmDbGetTcPortByCNode
*
* DESCRIPTION:
*       get traffic class and port by cNode from SW DB.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - device number. 
*       cNodeValue  - C node value (APPLICABLE RANGES: 0...511).
*
* OUTPUTS:
*       portPtr     - (poinetr to) source physical port number.
*       tcPtr       - (pointer to) traffic class.
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
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChTmGluePfcTmDbGetTcPortByCNode
(
    IN GT_U8                devNum,
    IN GT_U32               cNodeValue,
    IN GT_PHYSICAL_PORT_NUM *portPtr,
    IN GT_U32               *tcPtr

);

/*******************************************************************************
* prvCpssDxChTmGluePfcTmDbGetCNodeByTcPort
*
* DESCRIPTION:
*       get cNode by traffic class and port from SW DB.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - source physical port number.
*       tc          - traffic class (APPLICABLE RANGES: 0..7).
* OUTPUTS:
*       cNodePtr    - (pointer to) C node value.
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
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChTmGluePfcTmDbGetCNodeByTcPort
(
    IN GT_U8                devNum,
    IN GT_PHYSICAL_PORT_NUM port,
    IN GT_U32               tc,
    IN GT_U32               *cNodePtr
);

/*******************************************************************************
* prvCpssDxChTmGluePfcTmDbUpdateTcPortCNode
*
* DESCRIPTION:
*       update cNodes traffic class and port changes to SW DB.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - device number. 
*       cNodeValue  - C node value (APPLICABLE RANGES: 0...511).
*
* OUTPUTS:
*       portPtr     - (poinetr to) source physical port number.
*       tcPtr       - (pointer to) traffic class.
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
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChTmGluePfcTmDbUpdateTcPortCNode
(
    IN GT_U8                                    devNum,
    IN PRV_CPSS_DXCH_TM_GLUE_PFC_CNODE_INFO_STC *cNode1Ptr,
    IN PRV_CPSS_DXCH_TM_GLUE_PFC_CNODE_INFO_STC *cNode2Ptr
);

/*******************************************************************************
* prvCpssDxChTmGluePfcPortMappingGet
* 
* DESCRIPTION:
*       Get physical port to pfc port mapping.
*
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
*       pfcPortNum  - (pointer to) pfc port number which is used at
*                     cpssDxChTmGluePfcTmTcPort2CNodeSet. (APPLICABLE RANGES: 0..63).
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
GT_STATUS prvCpssDxChTmGluePfcPortMappingGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_PHYSICAL_PORT_NUM    *pfcPortNumPtr
);

/*******************************************************************************
* prvCpssDxChTmGluePfcInit
*
* DESCRIPTION:
*       Init TM GLUE PFC software Data Base.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTmGluePfcInit
(
    IN GT_U8 devNum
);
 

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChTmGluePfch */


