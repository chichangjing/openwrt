/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
********************************************************************************
* appDemoDxChFastBootUtils.h
*
* DESCRIPTION:
*       App demo DxCh appdemo FastBoot API header.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __appDemoDxChFastBootUtilsh
#define __appDemoDxChFastBootUtilsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* appDemoDxChMacCounterGet_HardCodedRegAddr
*
* DESCRIPTION:
*       Gets Ethernet MAC counter /  MAC Captured counter for a
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number,
*                        CPU port if getFromCapture is GT_FALSE
*       cntrName       - specific counter name
*       getFromCapture -  GT_TRUE -  Gets the captured Ethernet MAC counter
*                         GT_FALSE - Gets the Ethernet MAC counter
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide.
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*     This function is written for pexOnly mode --> these is no register initialization
*     All register addresses are hard coded
*     Based on prvCpssDxChMacCounterGet
*******************************************************************************/
GT_STATUS appDemoDxChMacCounterGet_HardCodedRegAddr
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
);

/*******************************************************************************
* appDemoDxChCfgIngressDropCntrGet_HardCodedRegAddr
*
* DESCRIPTION:
*       Get the Ingress Drop Counter value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device number.
*
* OUTPUTS:
*       counterPtr  - (pointer to) Ingress Drop Counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on cpssDxChCfgIngressDropCntrGet
*
*******************************************************************************/
GT_STATUS appDemoDxChCfgIngressDropCntrGet_HardCodedRegAddr
(
    IN  GT_U8       devNum,
    OUT GT_U32      *counterPtr
);

/*******************************************************************************
* appDemoDxChMirrorRxPortSet_HardCodedRegAddr
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
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on cpssDxChMirrorRxPortSet
*******************************************************************************/
GT_STATUS appDemoDxChMirrorRxPortSet_HardCodedRegAddr
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     mirrPort,
    IN  GT_BOOL         enable,
    IN  GT_U32          index
);

/*******************************************************************************
* setOwnDevice_HardCodedRegAddr
*
* DESCRIPTION:
*       Sets the device Device_ID within a Prestera chipset.
*
* INPUTS:
*       devNum         - the device number
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
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*******************************************************************************/
GT_STATUS setOwnDevice_HardCodedRegAddr
(
    IN  GT_U8           devNum
);

/*******************************************************************************
* appDemoDxChMirrorAnalyzerInterfaceSet_HardCodedRegAddr
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
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on cpssDxChMirrorAnalyzerInterfaceSet
*
*******************************************************************************/
GT_STATUS appDemoDxChMirrorAnalyzerInterfaceSet_HardCodedRegAddr
(
    IN GT_U8     devNum,
    IN GT_U32    index,
    IN CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   *interfacePtr
);

/*******************************************************************************
* appDemoDxChMirrorRxGlobalAnalyzerInterfaceIndexSet_HardCodedRegAddr
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
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet
*
*******************************************************************************/
GT_STATUS appDemoDxChMirrorRxGlobalAnalyzerInterfaceIndexSet_HardCodedRegAddr
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
);

/*******************************************************************************
* appDemoDxChIngressCscdPortSet_HardCodedRegAddr
*
* DESCRIPTION:
*           Set ingress port as cascaded/non-cascade .
*           (effect all traffic on ingress pipe only)
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - The physical port number.
*                   NOTE: this port is not covered and is NOT checked for 'mapping'
*       portRxDmaNum  - The RXDMA number for cascading.
*       portType - cascade  type regular/extended DSA tag port or network port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on prvCpssDxChIngressCscdPortSet
*
*******************************************************************************/
GT_STATUS appDemoDxChIngressCscdPortSet_HardCodedRegAddr
(
    IN GT_U8                        devNum,
    IN GT_PHYSICAL_PORT_NUM         portNum,
    IN GT_U32                       portRxDmaNum,
    IN CPSS_CSCD_PORT_TYPE_ENT      portType
);

/*******************************************************************************
* appDemoDxChPortEgressCntrsGet_HardCodedRegAddr
*
* DESCRIPTION:
*       Gets a egress counters from specific counter-set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - physical device number
*       cntrSetNum - counter set number : 0, 1
*
* OUTPUTS:
*       egrCntrPtr - (pointer to) structure of egress counters current values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on cpssDxChPortEgressCntrsGet
*
*******************************************************************************/
GT_STATUS appDemoDxChPortEgressCntrsGet_HardCodedRegAddr
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       cntrSetNum,
    OUT CPSS_PORT_EGRESS_CNTR_STC   *egrCntrPtr
);

/*******************************************************************************
* appDemoDxChSdmaTxPacketSend_HardCodedRegAddr
*
* DESCRIPTION:
*       This function sends a single packet.
*       The packet is sent through interface port type to port given by
*       dstPortNum with VLAN vid.
*
* INPUTS:
*       isPacketWithVlanTag - indication that the packetDataPtr[12..15] hold vlan tag.
*       vid                 - VLAN ID. (used when isPacketWithVlanTag ==GT_FALSE,
*                             otherwise taken from packetDataPtr[14..15])
*       packetPtr           - pointer to the packet data and length in bytes.
*       dstPortNum          - Destination port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*
*******************************************************************************/
GT_STATUS appDemoDxChSdmaTxPacketSend_HardCodedRegAddr
(
    IN GT_U8    devNum,
    IN GT_BOOL  isPacketWithVlanTag,
    IN GT_U8    vid,
    IN TGF_PACKET_PAYLOAD_STC  *packetPtr,
    IN GT_U32   dstPortNum
);

/*******************************************************************************
* appDemoDxChPortForceLinkPassEnableSet_HardCodedRegAddr
*
* DESCRIPTION:
*       Enable/disable Force Link Pass on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link pass, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on cpssDxChPortForceLinkPassEnableSet
*
*******************************************************************************/
GT_STATUS appDemoDxChPortForceLinkPassEnableSet_HardCodedRegAddr
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   state
);

/*******************************************************************************
* appDemoDxChPortLoopbackModeEnableSet_HardCodedRegAddr
*
* DESCRIPTION:
*       Set port in 'loopback' mode
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum    - physical device number
*       portInterfacePtr - (pointer to) port interface
*       enable           - enable / disable (loopback/no loopback)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   based on cpssDxChPortInternalLoopbackEnableSet
*
*******************************************************************************/
GT_STATUS appDemoDxChPortLoopbackModeEnableSet_HardCodedRegAddr
(
    IN  GT_U8                   devNum,
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                   enable
);

/*******************************************************************************
* appDemoDxChPortMappingTxQPort2TxDMAMapSet_HardCodedRegAddr
*
* DESCRIPTION:
*       configure TxQ port 2 TxDMA  mapping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       txqNum     - txq port
*       txDmaNum   - txDMA port
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on prvCpssDxChPortMappingTxQPort2TxDMAMapSet
*
*******************************************************************************/
GT_STATUS appDemoDxChPortMappingTxQPort2TxDMAMapSet_HardCodedRegAddr
(
    IN GT_U8  devNum,
    IN GT_U32 txqNum,
    IN GT_U32 txDmaNum
);

/*******************************************************************************
* appDemoDxChPortMappingBMAMapOfLocalPhys2RxDMASet_HardCodedRegAddr
*
* DESCRIPTION:
*       configure BMA local physical port 2 RxDMA port  mapping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       physPort    - physical port
*       rxDmaNum   - rxDMA port
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on prvCpssDxChPortMappingBMAMapOfLocalPhys2RxDMASet
*
*******************************************************************************/
GT_STATUS appDemoDxChPortMappingBMAMapOfLocalPhys2RxDMASet_HardCodedRegAddr
(
    IN GT_U8  devNum,
    IN GT_U32 physPort,
    IN GT_U32 rxDMAPort
);

/*******************************************************************************
* appDemoDxChPortMappingRxDMA2LocalPhysSet_HardCodedRegAddr
*
* DESCRIPTION:
*       configure RxDMA 2 phys port mapping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       rxDmaNum    - rxDMA port
*       physPort    - physical port
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on prvCpssDxChPortMappingRxDMA2LocalPhysSet
*
*******************************************************************************/
GT_STATUS appDemoDxChPortMappingRxDMA2LocalPhysSet_HardCodedRegAddr
(
    IN GT_U8  devNum,
    IN GT_U32 rxDmaNum,
    IN GT_U32 physPort
);

/*******************************************************************************
* appDemoDxChBobcat2PortMappingConfigSet_HardCodedRegAddr
*
* DESCRIPTION:
*       Bobcat2 Port mapping initial configuration
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum          - The Pp's device number.
*       physPort    - physical port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on prvCpssDxChBobcat2PortMappingConfigSet
*
*******************************************************************************/
GT_STATUS appDemoDxChBobcat2PortMappingConfigSet_HardCodedRegAddr
(
    IN  GT_U8  devNum,
    IN  GT_U32 physPort
);

/*******************************************************************************
* appDemoDxChBrgEportToPhysicalPortTargetMappingTableSet_HardCodedRegAddr
*
* DESCRIPTION:
*       Set physical info for the given ePort in the E2PHY mapping table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       physicalInfoPtr - (pointer to) physical Information related to the ePort
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    E2PHY table is accessed by the packet's target ePort
*   This function is written for pexOnly mode --> these is no register initialization
*   All register addresses are hard coded
*   Based on cpssDxChBrgEportToPhysicalPortTargetMappingTableSet
*
*******************************************************************************/
GT_STATUS appDemoDxChBrgEportToPhysicalPortTargetMappingTableSet_HardCodedRegAddr
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_INTERFACE_INFO_STC          *physicalInfoPtr
);

/*******************************************************************************
* appDemoDxChPortFastInit
*
* DESCRIPTION:
*       Fast ports configuration
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2: xCat3; Lion2.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS appDemoDxChPortFastInit
(
    IN  GT_U8 devNum
);

/*******************************************************************************
* appDemoDxChPortModeSpeedSet
*
* DESCRIPTION:
*       Fast ports configuration
*
* APPLICABLE DEVICES:
*        Bobcat2;Lion2
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2: xCat3; Lion; Caelum;Bobcat3.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS appDemoDxChPortModeSpeedSet
(
    IN  GT_U8 devNum
);

/*******************************************************************************
* appDemoDxChCatchUpValiditySync
*
* DESCRIPTION:
*       Sync some part of SW DB with HW for CatchUp Validation Check
*
* APPLICABLE DEVICES:
*        xCat; xCat2; xCat3; Lion; Lion2; Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Caelum; Bobcat3.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       Call this before cpssDxChCatchUpValidityCheckEnable()
*
*******************************************************************************/
GT_STATUS appDemoDxChCatchUpValiditySync
(
    IN  GT_U8 devNum
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __appDemoDxChFastBootUtilsh */

