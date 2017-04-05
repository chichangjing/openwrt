/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCutThrough.h
*
* DESCRIPTION:
*       CPSS DXCH Cut Through facility API.
*
* FILE REVISION NUMBER:
*       $Revision: 13 $
*
*******************************************************************************/
#ifndef __cpssDxChCutThroughh
#define __cpssDxChCutThroughh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

/*
 * Typedef: CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT
 *
 * Description: Bypass modes for Ingress PCL.
 *
 * Enumerations:
 *      CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_NO_BYPASS_E -
 *           No bypass for Ingress PCL.
 *
 *      CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ONE_LOOKUP_E -
 *          Lookup CPSS_PCL_LOOKUP_0_0_E is enabled, CPSS_PCL_LOOKUP_0_1_E and
 *          CPSS_PCL_LOOKUP_1_E are bypassed.
 *
 *      CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_TWO_LOOKUP_E -
 *          For Ingress PCL engine only.
 *          Lookup CPSS_PCL_LOOKUP_0_0_E and CPSS_PCL_LOOKUP_0_1_E is enabled,
 *          CPSS_PCL_LOOKUP_1_E is bypassed.
 *
 *      CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_FULL_BYPASS_E -
 *          Ingress PCL is completely bypassed.
 *
 */
typedef enum
{
    CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_NO_BYPASS_E,
    CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ONE_LOOKUP_E,
    CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_TWO_LOOKUP_E,
    CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_FULL_BYPASS_E
} CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT;

/*
 * Typedef: CPSS_DXCH_CUT_THROUGH_BYPASS_STC
 *
 * Description: used to describe Bypass mode per engine.
 *
 * Fields:
 *      bypassRouter - GT_TRUE  - Enable bypass for router engine.
 *                   - GT_FALSE - No bypass for router engine.
 *      bypassIngressPolicerStage0 - GT_TRUE - Enable bypass for Ingress policer0 engine.
 *                                 - GT_FALSE - No bypass for Ingress policer0 engine.
 *      bypassIngressPolicerStage1 - GT_TRUE - Enable bypass for Ingress policer1 engine.
 *                                 - GT_FALSE - No bypass for Ingress policer1 engine.
 *      bypassEgressPolicer        - GT_TRUE - Enable bypass for Egress policer engine.
 *                                 - GT_FALSE - No bypass for Egress policer engine.
 *      bypassEgressPcl            - GT_TRUE - Enable bypass for Egress PCL engine.
 *                                 - GT_FALSE - No bypass for Egress PCL engine.
 *      bypassIngressPcl           - Bypass Ingress PCL engine mode.
 *
 */

typedef struct{
    GT_BOOL bypassRouter;
    GT_BOOL bypassIngressPolicerStage0;
    GT_BOOL bypassIngressPolicerStage1;
    GT_BOOL bypassEgressPolicer;
    GT_BOOL bypassEgressPcl;
    CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT bypassIngressPcl;
}CPSS_DXCH_CUT_THROUGH_BYPASS_STC;

/*******************************************************************************
* cpssDxChCutThroughPortEnableSet
*
* DESCRIPTION:
*      Enable/Disable Cut Through forwarding for packets received on the port.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*       enable   - GT_TRUE:  Enable the port for Cut Through.
*                  GT_FALSE: Disable the port for Cut Through.
*       untaggedEnable -  GT_TRUE:  Enable Cut Through forwarding for
*                                   untagged packets received on the port.
*                         GT_FALSE: Disable Cut Through forwarding for
*                                    untagged packets received on the port.
*                         Used only if enable == GT_TRUE.
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       Cut Through forwarding for tagged packets is enabled
*       per source port and UP. See cpssDxChCutThroughUpEnableSet.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL  enable,
    IN GT_BOOL  untaggedEnable
);

/*******************************************************************************
* cpssDxChCutThroughPortEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode on the specified port.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status:
*                    - GT_TRUE:  Enable the port for Cut Through.
*                      GT_FALSE: Disable the port for Cut Through.
*       untaggedEnablePtr - pointer to Cut Through forwarding mode status
*                            for untagged packets.
*                            Used only if *enablePtr == GT_TRUE.
*                            GT_TRUE:  Enable Cut Through forwarding for
*                                      untagged packets received on the port.
*                            GT_FALSE: Disable Cut Through forwarding for
*                                      untagged packets received on the port.
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_BOOL  *untaggedEnablePtr
);

/*******************************************************************************
* cpssDxChCutThroughUpEnableSet
*
* DESCRIPTION:
*       Enable / Disable tagged packets with the specified UP
*       to be Cut Through.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       up              - user priority of a VLAN or DSA tagged
*                         packet (APPLICABLE RANGES: 0..7).
*       enable   - GT_TRUE:  tagged packets, with the specified UP
*                            may be subject to Cut Through forwarding.
*                  GT_FALSE: tagged packets, with the specified UP
*                            aren't subject to Cut Through forwarding.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or up.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The ingress port should be enabled for Cut Through forwarding.
*       Use cpssDxChCutThroughPortEnableSet for it.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughUpEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    up,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChCutThroughUpEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode for tagged packets
*      with the specified UP.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       up              - user priority of a VLAN or DSA tagged
*                         packet (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status
*                     for tagged packets, with the specified UP.:
*                     - GT_TRUE:  tagged packets, with the specified UP
*                                 may be subject to Cut Through forwarding.
*                       GT_FALSE: tagged packets, with the specified UP
*                                 aren't subject to Cut Through forwarding.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or up
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughUpEnableGet
(
    IN  GT_U8    devNum,
    IN GT_U8     up,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChCutThroughVlanEthertypeSet
*
* DESCRIPTION:
*       Set VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum           - device number.
*       etherType0       - VLAN EtherType0 (APPLICABLE RANGES: 0..0xFFFF)
*       etherType1       - VLAN EtherType1 (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong etherType0 or etherType1
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughVlanEthertypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType0,
    IN GT_U32   etherType1
);

/*******************************************************************************
* cpssDxChCutThroughVlanEthertypeGet
*
* DESCRIPTION:
*       Get VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum           - device number.
*
* OUTPUTS:
*       etherType0Ptr    - Pointer to VLAN EtherType0
*       etherType1Ptr    - Pointer to VLAN EtherType1.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughVlanEthertypeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *etherType0Ptr,
    OUT GT_U32   *etherType1Ptr
);

/*******************************************************************************
* cpssDxChCutThroughMinimalPacketSizeSet
*
* DESCRIPTION:
*       Set minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       size        - minimal packet size in bytes for Cut Through (APPLICABLE RANGES: 129..16376).
*                     Granularity - 8 bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When using cut-through to 1G port, and bypassing
*       of Router And Ingress Policer engines is disabled,
*       the minimal packet size should be 512 bytes.
*       When bypassing of Router And Ingress Policer engines is enabled,
*       the minimal cut-through packet size should be:
*       - for 10G or faster ports - at least 257 bytes.
*       - for ports slower than 10 G  - at least 513 bytes
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMinimalPacketSizeSet
(
    IN GT_U8    devNum,
    IN GT_U32   size
);

/*******************************************************************************
* cpssDxChCutThroughMinimalPacketSizeGet
*
* DESCRIPTION:
*       Get minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       sizePtr     - pointer to minimal packet size in bytes for Cut Through.
*                     Granularity - 8 bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMinimalPacketSizeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *sizePtr
);

/*******************************************************************************
* cpssDxChCutThroughMemoryRateLimitSet
*
* DESCRIPTION:
*       Set rate limiting of read operations from the memory
*       per target port in Cut Through mode according to the port speed.
*       To prevent congestion in egress pipe, buffer memory read operations
*       are rate limited according to the target port speed.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number including CPU port.
*       enable       - GT_TRUE - rate limiting is enabled.
*                    - GT_FALSE - rate limiting is disabled.
*       portSpeed    - port speed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portNum or portSpeed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Rate limit is recommended be enabled
*       on all egress ports for cut-through traffic.
*       Rate limit to the CPU port should be configured as a 1G port.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMemoryRateLimitSet
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL enable,
    IN CPSS_PORT_SPEED_ENT  portSpeed
);

/*******************************************************************************
* cpssDxChCutThroughMemoryRateLimitGet
*
* DESCRIPTION:
*       Get rate limiting of read operations from the memory
*       per target port in Cut Through mode.
*       To prevent congestion in egress pipe, buffer memory read operations
*       are rate limited according to the target port speed.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number.
*       portNum       - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr     - pointer to rate limiting mode status.
*                        - GT_TRUE - rate limiting is enabled.
*                        - GT_FALSE - rate limiting is disabled.
*       portSpeedPtr  - pointer to port speed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or portNum
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMemoryRateLimitGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL *enablePtr,
    OUT CPSS_PORT_SPEED_ENT  *portSpeedPtr
);

/*******************************************************************************
* cpssDxChCutThroughBypassRouterAndPolicerEnableSet
*
* DESCRIPTION:
*       Enable / Disable bypassing the Router and Ingress Policer engines.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       enable       - GT_TRUE -  bypassing of Router and Ingress Policer
*                                 engines is enabled.
*                    - GT_FALSE - bypassing of Router and Ingress Policer
*                                 engines is disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughBypassRouterAndPolicerEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChCutThroughBypassRouterAndPolicerEnableGet
*
* DESCRIPTION:
*       Get mode of bypassing the Router and Ingress Policer engines.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       enablePtr     - pointer to bypassing the Router and Ingress
*                       Policer engines status.
*                      - GT_TRUE  -  bypassing of Router and Ingress Policer
*                                    engines is enabled.
*                      - GT_FALSE -  bypassing of Router and Ingress Policer
*                                  engines is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughBypassRouterAndPolicerEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChCutThroughBypassModeSet
*
* DESCRIPTION:
*       Set Bypass mode.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number.
*       bypassModePtr - pointer to Bypass mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, Bypass Ingress PCL mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughBypassModeSet
(
    IN GT_U8                             devNum,
    IN CPSS_DXCH_CUT_THROUGH_BYPASS_STC  *bypassModePtr
);

/*******************************************************************************
* cpssDxChCutThroughBypassModeGet
*
* DESCRIPTION:
*       Get Bypass mode.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number.
*
* OUTPUTS:
*       bypassModePtr - pointer to Bypass mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughBypassModeGet
(
    IN  GT_U8                              devNum,
    OUT CPSS_DXCH_CUT_THROUGH_BYPASS_STC   *bypassModePtr
);

/*******************************************************************************
* cpssDxChCutThroughPortGroupMaxBuffersLimitSet
*
* DESCRIPTION:
*        Set max buffers limit for Cut-Through packets.
*        Packets are handled as non-Cut-Through when number of allocated buffers more then limit.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - device number.
*       portGroupsBmp             - bitmap of Port Groups.
*                                   NOTEs:
*                                    1. for non multi-port groups device this parameter is IGNORED.
*                                    2. for multi-port groups device :
*                                       bitmap must be set with at least one bit representing
*                                       valid port group(s). If a bit of non valid port group
*                                       is set then function returns GT_BAD_PARAM.
*                                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       buffersLimitEnable        - Buffers limit enable.
*                                     - GT_TRUE  - enable.
*                                     - GT_FALSE - disable.
*       buffersLimit              - Max buffers limit for Cut-Trough traffic.
*                                     The resolution is one buffer.
*                                     (APPLICABLE RANGES: 0..0x3FFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong maxSharedBuffersLimit
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B1
*       buffersLimit = (buffers number per port group) – (guard band).
*       where guard band is (MTU) * (number of cut-through ports in port group)
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortGroupMaxBuffersLimitSet
(
    IN GT_U8                            devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN GT_BOOL                          buffersLimitEnable,
    IN GT_U32                           buffersLimit
);

/*******************************************************************************
* cpssDxChCutThroughPortGroupMaxBuffersLimitGet
*
* DESCRIPTION:
*        Get max buffers limit for Cut-Through packets.
*        Packets are handled as non-Cut-Through when number of allocated buffers more then limit.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - device number.
*       portGroupsBmp             - bitmap of Port Groups.
*                                   NOTEs:
*                                    1. for non multi-port groups device this parameter is IGNORED.
*                                    2. for multi-port groups device :
*                                       bitmap must be set with at least one bit representing
*                                       valid port group(s). If a bit of non valid port group
*                                       is set then function returns GT_BAD_PARAM.
*                                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       buffersLimitEnablePtr        - (pointer to)Buffers limit enable.
*                                        - GT_TRUE  - enable.
*                                        - GT_FALSE - disable.
*       buffersLimitPtr              - (pointer to)Max buffers limit for Cut-Trough traffic.
*                                        The resolution is one buffer.
*                                        (APPLICABLE RANGES: 0..0x3FFF)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on Null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B1
*       buffersLimit = (buffers number per port group) – (guard band).
*       where guard band is (MTU) * (number of cut-through ports in port group)
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortGroupMaxBuffersLimitGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    OUT GT_BOOL                         *buffersLimitEnablePtr,
    OUT GT_U32                          *buffersLimitPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ____cpssDxChCutThroughh */




