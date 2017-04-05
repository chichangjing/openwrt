/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamDbg.h
*
* DESCRIPTION:
*       Internal debug functions
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmTcamDbgh
#define __prvCpssDxChLpmTcamDbgh

#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamCommonTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssDxChLpmTcamDbgDumpRouteTcam
*
* DESCRIPTION:
*     This func makes physical router tcam scanning and prints its contents.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*     dump   -  parameter for debugging purposes
*
* OUTPUTS:
*     None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_INITIALIZED       - if the driver was not initialized
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - on failure.
*
* COMMENTS:
*     None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbgDumpRouteTcam
(
    IN GT_BOOL dump
);

/*******************************************************************************
* prvCpssDxChLpmTcamDbgPatTriePrint
*
* DESCRIPTION:
*     This function prints Patricia trie contents.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*     lpmDbPtr    - the LPM DB information
*     vrId        - The virtual router identifier.
*     protocol    - ip protocol
*     prefixType  - uc/mc prefix type
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_NOT_FOUND             - if vrId is not found
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbgPatTriePrint
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr,
    IN GT_U32                                vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT            protocol,
    IN CPSS_UNICAST_MULTICAST_ENT            prefixType
);

/*******************************************************************************
* prvCpssDxChLpmTcamDbgPatTrieValidityCheck
*
* DESCRIPTION:
*   This function checks Patricia trie validity.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr    - the LPM DB information
*       vrId        - The virtual router identifier.
*       protocol    - ip protocol
*       prefixType  - uc/mc prefix type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_STATE             - on bad state in patricia trie
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbgPatTrieValidityCheck
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr,
    IN GT_U32                                vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT            protocol,
    IN CPSS_UNICAST_MULTICAST_ENT            prefixType
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmTcamDbgh */

