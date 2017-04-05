/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPrvIpLpmTypes.h
*
* DESCRIPTION:
*       Include Private cpss DXCH Ip LPM library type definitions.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 21 $
*
*******************************************************************************/
#ifndef __cpssDxChPrvIpLpmTypesh
#define __cpssDxChPrvIpLpmTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macro to validate the value of shadowType parameter (DxCh format) */
#define PRV_CPSS_DXCH_IP_CHECK_SHADOW_TYPE_MAC(shadowType)           \
    if(((shadowType) != CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E) &&   \
       ((shadowType) != CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E) &&  \
       ((shadowType) != CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E) &&  \
       ((shadowType) != CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E) &&      \
       ((shadowType) != CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E) &&  \
       ((shadowType) != CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E))        \
    {                                                                \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                         \
    }

/* macro to validate the value of protocolStack parameter (DxCh format) */
#define PRV_CPSS_DXCH_IP_CHECK_PROTOCOL_STACK_MAC(protocolStack)\
    if(((protocolStack) != CPSS_IP_PROTOCOL_IPV4_E) &&          \
       ((protocolStack) != CPSS_IP_PROTOCOL_IPV6_E) &&          \
       ((protocolStack) != CPSS_IP_PROTOCOL_IPV4V6_E))          \
    {                                                           \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                    \
    }


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPrvIpLpmTypesh */

