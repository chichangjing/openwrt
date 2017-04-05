/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChMirror.h
*
* DESCRIPTION:
*       CPSS DxCh Mirror APIs.
*
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*******************************************************************************/
#ifndef __cpssDxChMirrorh
#define __cpssDxChMirrorh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>

/*
 * typedef: struct GT_MIRROR_ANALYZER_VLAN_TAG_CFG
 *
 * Description: used to describe Analyzer Vlan Tag
 *
 * Fields:
 *      etherType - The Vlan ether type of the VLAN tag added to packet
 *                  forwarded to Analyzer
 *      vpt - The VPT of the VLAN tag added to packet forwarded to Analyzer (APPLICABLE RANGES: 0..7)
 *      cfi - The CFI of the VLAN tag added to packet forwarded to
 *            Analyzer
 *      vid - The VLAN ID of the VLAN tag added to packet forwarded to Analyzer
 *
 */
typedef struct
{
    GT_U16   etherType;
    GT_U8    vpt;
    GT_U8    cfi;
    GT_U16   vid;
}CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC;

/* deprecated legacy names that should not be used ,
    and replaced by CPSS_CSCD_PORT_DSA_MODE_1_WORD_E ,
    and CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_OVERRIDE_E and
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_END_TO_END_E */
enum
{
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E = 1
};


/*
 * Typedef: enum CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT
 *
 * Description:
 *      This enum defines mode of forwarding To Analyzer packets.
 * Enumerations:
 *     CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E - DSA tag holds
 *     the Device/Port trigerring parameters. The forwarding decision of the
 *     To Analyzer packet is performed at each hop according to the local
 *     configuration of the ingress/egress analyzer.
 *     CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E - DSA tag holds
 *     the mirroring Device/Port destination. The destination analyzer is
 *     determined at the triggered mirroring device
 *     CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_OVERRIDE_E -
 *              Backward compatible mode. The Src/Trg fields in the DSA tag are
 *              overwritten with the target analyzer interface.
 *     CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_END_TO_END_E - The Src/Trg
 *              fields are separate fields than the target analyzer.
 *              This mode is possible only in eDSA format.

 *  Comments:
 *
 */
typedef enum
{
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E,
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_OVERRIDE_E = CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E,
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_END_TO_END_E
} CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT;

/*
 * Typedef: enum CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT
 *
 * Description:
 *      This enum defines mode of Enhanced Mirroring Priority.
 * Enumerations:
 *     CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_TDM_E - TDM arbiter is
 *              selected (no client receives strict priority over the others).
 *     CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_SP_INGRESS_EGRESS_MIRROR_E - Ingress
 *              and Egress mirroring receive strict priority over other
 *              clients (regular received, not mirrored, traffic).
 */
typedef enum
{
    CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_TDM_E,
    CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_SP_INGRESS_EGRESS_MIRROR_E
} CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT;
/*
 * typedef: CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC
 *
 * Description: used to describe Analyzer interface.
 *
 * Fields:
 *      interface - Analyzer interface.
 *      Only CPSS_INTERFACE_PORT_E  interface type is supported.
 *
 */
typedef struct
{
    CPSS_INTERFACE_INFO_STC  interface;
}CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC;

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagEnable
*
* DESCRIPTION:
*       Enable/Disable Analyzer port adding additional VLAN Tag to mirrored
*       packets. The content of this VLAN tag is configured
*       by cpssDxChMirrorTxAnalyzerVlanTagConfig.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number.
*       portNum     - ports number to be set
*       enable      - GT_TRUE - additional VLAN tag inserted to mirrored packets
*                     GT_FALSE - no additional VLAN tag inserted to mirrored packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagEnable
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  enable
);

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagEnableGet
*
* DESCRIPTION:
*       Get Analyzer port VLAN Tag to mirrored packets mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number.
*       portNum     - ports number to be set
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - additional VLAN tag inserted to mirrored packets
*                     GT_FALSE - no additional VLAN tag inserted to mirrored packets
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerVlanTagConfig
*
* DESCRIPTION:
*      Set global Tx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerVlanTagConfig
(
    IN GT_U8                                       devNum,
    IN CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
);

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerVlanTagConfigGet
*
* DESCRIPTION:
*      Get global Tx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerVlanTagConfigGet
(
    IN GT_U8                                       devNum,
    OUT CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
);

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerVlanTagConfig
*
* DESCRIPTION:
*      Set global Rx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerVlanTagConfig
(
    IN GT_U8                                       devNum,
    IN CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
);

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerVlanTagConfigGet
*
* DESCRIPTION:
*      Get global Rx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerVlanTagConfigGet
(
    IN GT_U8                                       devNum,
    OUT CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
);

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a port to be an analyzer port of tx mirrored
*       ports on all system pp's.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerHwDev - pp HW Device number of analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     analyzerPort,
    IN  GT_HW_DEV_NUM   analyzerHwDev
);

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerPortGet
*
* DESCRIPTION:
*       Gets the analyzer port of tx mirrored ports on all system pp's.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*
* OUTPUTS:
*       analyzerPortPtr - (pointer to)port number of analyzer port
*       analyzerHwDevPtr - (pointer to)pp HW Device number of analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerPortGet
(
    IN  GT_U8           devNum,
    OUT GT_PORT_NUM     *analyzerPortPtr,
    OUT GT_HW_DEV_NUM   *analyzerHwDevPtr
);

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a specific port to be an analyzer port of Rx mirrored
*       ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*
*       devNum  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerHwDev - pp HW Device number of analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     analyzerPort,
    IN  GT_HW_DEV_NUM   analyzerHwDev
);

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerPortGet
*
* DESCRIPTION:
*       Gets the analyzer port of Rx mirrored ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*
*       devNum  - the device number
*
* OUTPUTS:
*       analyzerPortPtr - (pointer to)port number of analyzer port
*       analyzerHwDevPtr - (pointer to)pp HW Device number of analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerPortGet
(
    IN  GT_U8           devNum,
    OUT GT_PORT_NUM     *analyzerPortPtr,
    OUT GT_HW_DEV_NUM   *analyzerHwDevPtr
);

/*******************************************************************************
* cpssDxChMirrorTxCascadeMonitorEnable
*
* DESCRIPTION:
*       One global bit that is set to 1 when performing egress mirroring or
*       egress STC of any of the cascading ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - the device number
*       enable    - enable/disable Egress Monitoring on cascading ports.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxCascadeMonitorEnable
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
);

/*******************************************************************************
* cpssDxChMirrorTxCascadeMonitorEnableGet
*
* DESCRIPTION:
*       Get One global bit that indicate performing egress mirroring or
*       egress STC of any of the cascading ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - the device number
*
* OUTPUTS:
*       enablePtr    - enable/disable Egress Monitoring on cascading ports.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxCascadeMonitorEnableGet
(
    IN   GT_U8   devNum,
    OUT  GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChMirrorRxPortSet
*
* DESCRIPTION:
*       Sets a specific port to be Rx mirrored port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number, CPU port supported.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*       enable         - enable/disable Rx mirror on this port
*                        GT_TRUE - Rx mirroring enabled, packets
*                                  received on a mirrPort are
*                                  mirrored to Rx analyzer.
*                        GT_FALSE - Rx mirroring disabled.
*       index          - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*                        Supported for xCat and above device.
*                        Used only if forwarding mode to analyzer is Source-based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     mirrPort,
    IN  GT_BOOL         isPhysicalPort,
    IN  GT_BOOL         enable,
    IN  GT_U32          index
);

/*******************************************************************************
* cpssDxChMirrorRxPortGet
*
* DESCRIPTION:
*       Gets status of Rx mirroring (enabled or disabled) of specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number, CPU port supported.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*
* OUTPUTS:
*       enablePtr    - (pointer to) Rx mirror mode
*                   GT_TRUE - Rx mirroring enabled, packets
*                             received on a mirrPort are
*                             mirrored to Rx analyzer.
*                   GT_FALSE - Rx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index.
*                   Supported for xCat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxPortGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     mirrPort,
    IN  GT_BOOL         isPhysicalPort,
    OUT GT_BOOL         *enablePtr,
    OUT GT_U32          *indexPtr
);

/*******************************************************************************
* cpssDxChMirrorTxPortSet
*
* DESCRIPTION:
*       Enable or disable Tx mirroring per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*       enable         - enable/disable Tx mirror on this port
*                        GT_TRUE - Tx mirroring enabled, packets
*                                  transmitted from a mirrPort are
*                                  mirrored to Tx analyzer.
*                        GT_FALSE - Tx mirroring disabled.
*       index          - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*                        Supported for xCat and above device.
*                        Used only if forwarding mode to analyzer is Source-based.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     mirrPort,
    IN  GT_BOOL         isPhysicalPort,
    IN  GT_BOOL         enable,
    IN  GT_U32          index
);

/*******************************************************************************
* cpssDxChMirrorTxPortGet
*
* DESCRIPTION:
*       Get status (enabled/disabled) of Tx mirroring per port .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*
* OUTPUTS:
*       enablePtr - (pointer to) Tx mirror mode on this port
*                                GT_TRUE - Tx mirroring enabled, packets
*                                          transmitted from a mirrPort are
*                                          mirrored to Tx analyzer.
*                                GT_FALSE - Tx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index.
*                   Supported for xCat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxPortGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     mirrPort,
    IN  GT_BOOL         isPhysicalPort,
    OUT GT_BOOL         *enablePtr,
    OUT GT_U32          *indexPtr
);

/*******************************************************************************
* cpssDxChMirrRxStatMirroringToAnalyzerRatioSet
*
* DESCRIPTION:
*       Set the statistical mirroring rate in the Rx Analyzer port
*       Set Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*      ratio - Indicates the ratio of egress mirrored to analyzer port packets
*              forwarded to the analyzer port. 1 of every 'ratio' packets are
*              forwarded to the analyzer port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or ratio.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirroringToAnalyzerRatioSet
(
    IN GT_U8    devNum,
    IN GT_U32   ratio
);

/*******************************************************************************
* cpssDxChMirrRxStatMirroringToAnalyzerRatioGet
*
* DESCRIPTION:
*       Get the statistical mirroring rate in the Rx Analyzer port
*       Get Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*
* OUTPUTS:
*      ratioPtr - (pointer to) Indicates the ratio of egress mirrored to analyzer port packets
*                 forwarded to the analyzer port. 1 of every 'ratio' packets are
*                 forwarded to the analyzer port.
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirroringToAnalyzerRatioGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *ratioPtr
);

/*******************************************************************************
* cpssDxChMirrRxStatMirrorToAnalyzerEnable
*
* DESCRIPTION:
*       Enable Ingress Statistical Mirroring to the Ingress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*      enable - enable\disable Ingress Statistical Mirroring.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirrorToAnalyzerEnable
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChMirrRxStatMirrorToAnalyzerEnableGet
*
* DESCRIPTION:
*       Get Ingress Statistical Mirroring to the Ingress Analyzer Port Mode
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*
* OUTPUTS:
*      enablePtr - (pointer to) Ingress Statistical Mirroring mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirrorToAnalyzerEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChMirrRxAnalyzerDpTcSet
*
* DESCRIPTION:
*       The TC/DP assigned to the packet forwarded to the ingress analyzer port due
*       to ingress mirroring to the analyzer port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum       - device number.
*      analyzerDp   - the Drop Precedence to be set
*      analyzerTc   - traffic class on analyzer port (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxAnalyzerDpTcSet
(
    IN GT_U8             devNum,
    IN CPSS_DP_LEVEL_ENT analyzerDp,
    IN GT_U8             analyzerTc
);

/*******************************************************************************
* cpssDxChMirrRxAnalyzerDpTcGet
*
* DESCRIPTION:
*       Get TC/DP assigned to the packet forwarded to the ingress analyzer port due
*       to ingress mirroring to the analyzer port cofiguration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum          - device number.
*
* OUTPUTS:
*      analyzerDpPtr   - pointer to the Drop Precedence.
*      analyzerTcPtr   - pointer to traffic class on analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_BAD_PTR               - wrong pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxAnalyzerDpTcGet
(
    IN GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT *analyzerDpPtr,
    OUT GT_U8             *analyzerTcPtr
);

/*******************************************************************************
* cpssDxChMirrTxAnalyzerDpTcSet
*
* DESCRIPTION:
*       The TC/DP assigned to the packet forwarded to the egress analyzer port due
*       to egress mirroring to the analyzer port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum       - device number.
*      analyzerDp   - the Drop Precedence to be set
*      analyzerTc   - traffic class on analyzer port (APPLICABLE RANGES: 0..7)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxAnalyzerDpTcSet
(
    IN GT_U8              devNum,
    IN CPSS_DP_LEVEL_ENT  analyzerDp,
    IN GT_U8              analyzerTc
);

/*******************************************************************************
* cpssDxChMirrTxAnalyzerDpTcGet
*
* DESCRIPTION:
*       Get TC/DP assigned to the packet forwarded to the egress analyzer port due
*       to egress mirroring to the analyzer port cofiguration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum          - device number.
*
* OUTPUTS:
*      analyzerDpPtr   - pointer to the Drop Precedence.
*      analyzerTcPtr   - pointer to traffic class on analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_BAD_PTR               - wrong pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxAnalyzerDpTcGet
(
    IN GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT *analyzerDpPtr,
    OUT GT_U8             *analyzerTcPtr
);

/*******************************************************************************
* cpssDxChMirrTxStatMirroringToAnalyzerRatioSet
*
* DESCRIPTION:
*       Set the statistical mirroring rate in the Tx Analyzer port
*       Set Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*      ratio - Indicates the ratio of egress mirrored to analyzer port packets
*              forwarded to the analyzer port. 1 of every 'ratio' packets are
*              forwarded to the analyzer port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device or ratio.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Tx mirroring
*         ratio 1 -> all Tx mirrored packets are forwarded out the Tx
*             Analyzer port
*         ratio 10 -> 1 in 10 Tx mirrored packets are forwarded out the Tx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirroringToAnalyzerRatioSet
(
    IN GT_U8    devNum,
    IN GT_U32   ratio
);

/*******************************************************************************
* cpssDxChMirrTxStatMirroringToAnalyzerRatioGet
*
* DESCRIPTION:
*       Get the statistical mirroring rate in the Tx Analyzer port
*       Get Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*
* OUTPUTS:
*      ratioPtr - (pointer to)Indicates the ratio of egress mirrored to analyzer
*              port packets forwarded to the analyzer port. 1 of every 'ratio'
*              packets are forwarded to the analyzer port.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Tx mirroring
*         ratio 1 -> all Tx mirrored packets are forwarded out the Tx
*             Analyzer port
*         ratio 10 -> 1 in 10 Tx mirrored packets are forwarded out the Tx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirroringToAnalyzerRatioGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *ratioPtr
);

/*******************************************************************************
* cpssDxChMirrTxStatMirrorToAnalyzerEnable
*
* DESCRIPTION:
*       Enable Egress Statistical Mirroring to the Egress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*      enable - enable\disable Egress Statistical Mirroring.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirrorToAnalyzerEnable
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChMirrTxStatMirrorToAnalyzerEnableGet
*
* DESCRIPTION:
*       Get mode of Egress Statistical Mirroring to the Egress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number
*
* OUTPUTS:
*      enablePtr - (pointer to)  Egress Statistical Mirroring.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirrorToAnalyzerEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChMirrorToAnalyzerForwardingModeSet
*
* DESCRIPTION:
*       Set Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      mode   - mode of forwarding To Analyzer packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change  Forwarding mode, applicaton should
*       disable Rx/Tx mirrorred ports.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorToAnalyzerForwardingModeSet
(
    IN GT_U8     devNum,
    IN CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   mode
);

/*******************************************************************************
* cpssDxChMirrorToAnalyzerForwardingModeGet
*
* DESCRIPTION:
*       Get Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*
* OUTPUTS:
*      modePtr   - pointer to mode of forwarding To Analyzer packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorToAnalyzerForwardingModeGet
(
    IN  GT_U8     devNum,
    OUT CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   *modePtr
);

/*******************************************************************************
* cpssDxChMirrorAnalyzerInterfaceSet
*
* DESCRIPTION:
*       This function sets analyzer interface.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*      index          - index of analyzer interface. (APPLICABLE RANGES: 0..6)
*      interfacePtr   - Pointer to analyzer interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index, interface type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_RANGE          - on wrong port or device number in interfacePtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerInterfaceSet
(
    IN GT_U8     devNum,
    IN GT_U32    index,
    IN CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   *interfacePtr
);

/*******************************************************************************
* cpssDxChMirrorAnalyzerInterfaceGet
*
* DESCRIPTION:
*       This function gets analyzer interface.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*      index          - index of analyzer interface. (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*      interfacePtr   - Pointer to analyzer interface.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerInterfaceGet
(
    IN  GT_U8     devNum,
    IN GT_U32     index,
    OUT CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   *interfacePtr
);

/*******************************************************************************
* cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for ingress
*       mirroring from all engines except
*       port-mirroring source-based-forwarding mode.
*       (Port-Based hop-by-hop mode, Policy-Based, VLAN-Based,
*        FDB-Based, Router-Based).
*       If a packet is mirrored by both the port-based ingress mirroring,
*       and one of the other ingress mirroring, the selected analyzer is
*       the one with the higher index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable mirroring for
*                  Port-Based hop-by-hop mode, Policy-Based,
*                  VLAN-Based, FDB-Based, Router-Based.
*                  - GT_TRUE - enable mirroring.
*                  - GT_FALSE - No mirroring.
*      index     - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
);

/*******************************************************************************
* cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for ingress mirroring
*       from all engines except port-mirroring source-based-forwarding mode.
*       (Port-Based hop-by-hop mode, Policy-Based, VLAN-Based,
*        FDB-Based, Router-Based).
*       If a packet is mirrored by both the port-based ingress mirroring,
*       and one of the other ingress mirroring, the selected analyzer is
*       the one with the higher index.
*
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      enablePtr    - Pointer to global enable/disable mirroring for
*                     Port-Based hop-by-hop mode, Policy-Based,
*                     VLAN-Based, FDB-Based, Router-Based.
*                     - GT_TRUE - enable mirroring.
*                     - GT_FALSE - No mirroring.
*      indexPtr     - pointer to analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
);

/*******************************************************************************
* cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable mirroring for
*                  Port-Based hop-by-hop mode.
*                  - GT_TRUE - enable mirroring.
*                  - GT_FALSE - No mirroring.
*      index     - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
);

/*******************************************************************************
* cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      enablePtr    - Pointer to global enable/disable mirroring for
*                     Port-Based hop-by-hop mode.
*                     - GT_TRUE - enable mirroring.
*                     - GT_FALSE - No mirroring.
*      indexPtr     - pointer to analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
);

/*******************************************************************************
* cpssDxChMirrorAnalyzerMirrorOnDropEnableSet
*
* DESCRIPTION:
*       Enable / Disable mirroring of dropped packets.
*
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum - device number.
*      index  - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*      enable - GT_TRUE  - dropped packets are mirrored to analyzer interface.
*               GT_FALSE - dropped packets are not mirrored to analyzer interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerMirrorOnDropEnableSet
(
    IN GT_U8     devNum,
    IN GT_U32    index,
    IN GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChMirrorAnalyzerMirrorOnDropEnableGet
*
* DESCRIPTION:
*       Get mirroring status of dropped packets.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum - device number.
*      index  - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*      enablePtr - Pointer to mirroring status of dropped packets.
*                  - GT_TRUE  - dropped packets are mirrored to
*                               analyzer interface.
*                  - GT_FALSE - dropped packets are not mirrored to
*                               analyzer interface.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerMirrorOnDropEnableGet
(
    IN  GT_U8     devNum,
    IN GT_U32     index,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet
*
* DESCRIPTION:
*       Enable/Disable VLAN tag removal of mirrored traffic.
*       When VLAN tag removal is enabled for a specific analyzer port, all
*       packets that are mirrored to this port are sent without any VLAN tags.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - GT_TRUE  - VLAN tag is removed from mirrored traffic.
*                     GT_FALSE - VLAN tag isn't removed from mirrored traffic.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
);


/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet
*
* DESCRIPTION:
*       Get status of enabling/disabling VLAN tag removal of mirrored traffic.
*       When VLAN tag removal is enabled for a specific analyzer port, all
*       packets that are mirrored to this port are sent without any VLAN tags.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - pointer to status of VLAN tag removal of mirrored traffic.
*                  - GT_TRUE  - VLAN tag is removed from mirrored traffic.
*                    GT_FALSE - VLAN tag isn't removed from mirrored traffic.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChMirrorEnhancedMirroringPriorityModeSet
*
* DESCRIPTION:
*       Setting Enhanced Mirroring Priority mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       mode    - Enhanced Mirroring Priority selected working mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorEnhancedMirroringPriorityModeSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChMirrorEnhancedMirroringPriorityModeGet
*
* DESCRIPTION:
*       Getting Enhanced Mirroring Priority mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       modePtr    - (pointer to) Enhanced Mirroring Priority selected working mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorEnhancedMirroringPriorityModeGet
(
    IN GT_U8                                         devNum,
    OUT CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet
*
* DESCRIPTION:
*       Set the drop code to be used for Hop-By-Hop mirroring mode, when a packet
*       is duplicated to a target analyzer, and the global Ingress Analyzer Index
*       or Egress Analyzer Index is DISABLED.
*       In this case the packet is hard dropped with this drop code.
*
*       NOTE: not relevant to the device that generates the duplication but only
*       to device that will get 'TO_ANALYZER' DSA tag (in HOP-BY_HOP mode)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       dropCode  - the drop code. (values like 'cpu code')
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device , bad dropCode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT    dropCode
);

/*******************************************************************************
* cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet
*
* DESCRIPTION:
*       Get the drop code to be used for Hop-By-Hop mirroring mode, when a packet
*       is duplicated to a target analyzer, and the global Ingress Analyzer Index
*       or Egress Analyzer Index is DISABLED.
*       In this case the packet is hard dropped with this drop code.
*
*       NOTE: not relevant to the device that generates the duplication but only
*       to device that will get 'TO_ANALYZER' DSA tag (in HOP-BY_HOP mode)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       dropCodePtr  - (pointer to) the drop code. (values like 'cpu code')
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT    *dropCodePtr
);

/*******************************************************************************
* cpssDxChMirrorTxPortVlanEnableSet
*
* DESCRIPTION:
*       Enable or disable Tx vlan mirroring from specific physical port.
*       (define if the physical port allow/deny egress vlan mirroring)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - physical port number.
*       enable    - enable/disable Tx vlan mirroring from this physical port
*                   GT_TRUE  - Tx vlan mirroring enabled  for this physical port.
*                   GT_FALSE - Tx vlan mirroring disabled for this physical port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device , bad portNum.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxPortVlanEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChMirrorTxPortVlanEnableGet
*
* DESCRIPTION:
*       Get is the Tx vlan mirroring from specific physical port enabled/disabled.
*       (define if the physical port allow/deny egress vlan mirroring)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - physical port number.
*
* OUTPUTS:
*       enablePtr - (pointer to)enable/disable Tx vlan mirroring from this physical port
*                   GT_TRUE  - Tx vlan mirroring enabled  for this physical port.
*                   GT_FALSE - Tx vlan mirroring disabled for this physical port.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxPortVlanEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChMirrorh */

