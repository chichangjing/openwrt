/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortIfModeCfgResource.h
*
* DESCRIPTION:
*       CPSS implementation for Port resource configuration.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_PORT_IF_MODE_CFG_RESOURCE_H
#define __PRV_CPSS_DXCH_PORT_IF_MODE_CFG_RESOURCE_H

#include <cpss/generic/cpssTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssDxChPortResourcesInit
*
* DESCRIPTION:
*       Initialize data structure for port resource allocation
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum;
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortResourcesInit
(
    IN  GT_U8   devNum
);

/*******************************************************************************
* prvCpssDxChPortResourcesConfigDbInit
*
* DESCRIPTION:
*       Resource data structure initialization
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum;
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - physical device number
*       maxDescCredits      - physical device number
*       maxHeaderCredits    - physical device number
*       maxPayloadCredits   - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortResourcesConfigDbInit
(
    IN  GT_U8   devNum,
    IN GT_U32   dpIndex,
    IN  GT_U32  maxDescCredits,
    IN  GT_U32  maxHeaderCredits,
    IN  GT_U32  maxPayloadCredits
);


/*******************************************************************************
* prvCpssDxChPortResourcesConfigDbAvailabilityCheck
*
* DESCRIPTION:
*       Resource data structure limitations checks
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum;
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - physical device number
*       maxDescCredits      - physical device number
*       maxHeaderCredits    - physical device number
*       maxPayloadCredits   - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortResourcesConfigDbAvailabilityCheck
(
    IN  GT_U8   devNum,    
    IN GT_U32   dpIndex,
    IN  GT_U32  txQDescrCredits,
    IN  GT_U32  txFifoHeaderCredits,
    IN  GT_U32  txFifoPayloadCredits
);


/*******************************************************************************
* prvCpssDxChPortResourcesConfigDbAdd
*
* DESCRIPTION:
*       Resource data structure DB add operation.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum;
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - physical device number
*       maxDescCredits      - physical device number
*       maxHeaderCredits    - physical device number
*       maxPayloadCredits   - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortResourcesConfigDbAdd
(
    IN  GT_U8   devNum,    
    IN GT_U32   dpIndex,
    IN  GT_U32  txQDescrCredits,
    IN  GT_U32  txFifoHeaderCredits,
    IN  GT_U32  txFifoPayloadCredits
);


/*******************************************************************************
* prvCpssDxChPortResourcesConfigDbDelete
*
* DESCRIPTION:
*       Resource data structure DB delete operation.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum;
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - physical device number
*       maxDescCredits      - physical device number
*       maxHeaderCredits    - physical device number
*       maxPayloadCredits   - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortResourcesConfigDbDelete
(
    IN  GT_U8   devNum,    
    IN GT_U32   dpIndex,
    IN  GT_U32  txQDescrCredits,
    IN  GT_U32  txFifoHeaderCredits,
    IN  GT_U32  txFifoPayloadCredits
);


/*-------------------------------------------------*
 *                                                 *
 *-------------------------------------------------*/
GT_STATUS prvCpssDxChPortResourcesConfigDbCoreOverallSpeedAdd
(
    IN  GT_U8   devNum,    
    IN GT_U32   dpIndex,
    IN  GT_U32  bandwidthMbps
);

GT_STATUS prvCpssDxChPortResourcesConfigDbCoreOverallSpeedDelete
(
    IN  GT_U8   devNum,    
    IN GT_U32   dpIndex,
    IN  GT_U32  bandwidthMbps
);


/*-------------------------------------------------*
 *                                                 *
 *-------------------------------------------------*/

GT_STATUS prvCpssDxChPortResourcesConfigDbTMBWSet
(
    IN  GT_U8   devNum,    
    IN  GT_U32  tmBandwidthMbps,
    IN  GT_U32  dpIndex,
    IN  GT_U32  txQDescrCredits,
    IN  GT_U32  txFifoHeaderCredits,
    IN  GT_U32  txFifoPayloadCredits

);

GT_STATUS prvCpssDxChPortResourcesConfigDbTMBWGet
(
    IN  GT_U8   devNum,    
    IN  GT_U32  *tmBandwidthMbpsPtr
);

/*******************************************************************************
* prvCpssDxChPort_Bcat2A0_TM_ResourcesConfig
*
* DESCRIPTION:
*       Bcat A0 TM port resource configuration
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum          - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note: 
*           - API should not be called under traffic
******************************************************************************/
GT_STATUS prvCpssDxChPort_Bcat2A0_TM_ResourcesConfig
(
    IN GT_U8       devNum
);



#ifdef __cplusplus
}
#endif /* __cplusplus */





#endif

