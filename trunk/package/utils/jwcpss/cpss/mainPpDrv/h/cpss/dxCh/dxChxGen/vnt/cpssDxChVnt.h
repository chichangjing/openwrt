/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChVnt.h
*
* DESCRIPTION:
*       CPSS DXCH Virtual Network Tester (VNT) Technology facility API.
*       VNT features:
*       - Operation, Administration, and Maintenance (OAM).
*       - Connectivity Fault Management (CFM).
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*
*******************************************************************************/
#ifndef __cpssDxChVnth
#define __cpssDxChVnth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
 *
 * typedef: enum CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT
 *
 * Description: Enumeration for CFM opcode change mode.
 *
 * Enumerations:
 *    CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_OPCODE_E -
 *              When replying to a CFM message, the opcode is taken from 
 *              CFM LBR Opcode value. 
 *    CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_LSBIT_E   -
 *              When replying to a CFM message, the least significant bit of 
 *              the opcode is changed to zero. 
 */
typedef enum {
    CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_OPCODE_E,
    CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_LSBIT_E
} CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT;

/*******************************************************************************
* cpssDxChVntOamPortLoopBackModeEnableSet
*
* DESCRIPTION:
*      Enable/Disable 802.3ah Loopback mode on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port,
*      - All traffic destined for  this port but was not ingressed on this port
*        is discarded, except for FROM_CPU traffic - which is to allow the CPU
*        to send OAM control packets.
*      - Egress VLAN and spanning tree filtering is bypassed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*       enable   - GT_TRUE:  Enable OAM loopback mode.
*                  GT_FALSE: Disable OAM loopback mode.
*
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*      In the Policy TCAM, after the existing rule which traps OAMPDUs,
*      add a rule to redirect all traffic received on
*      the port back to the same port for getting loopback.
*      For xCat and above not need rule for OAM PDU trap.
*      Use cpssDxChVntOamPortPduTrapEnableSet for it.
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortLoopBackModeEnableSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  enable
);

/*******************************************************************************
* cpssDxChVntOamPortLoopBackModeEnableGet
*
* DESCRIPTION:
*      Get 802.3ah Loopback mode on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port,
*      - All traffic destined for  this port but was not ingressed on this port
*        is discarded, except for FROM_CPU traffic - which is to allow the CPU
*        to send OAM control packets.
*      - Egress VLAN and spanning tree filtering is bypassed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable OAM loopback mode.
*                     GT_FALSE: Disable OAM loopback mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*      In the Policy TCAM, after the existing rule which traps OAMPDUs,
*      add a rule to redirect all traffic received on
*      the port back to the same port for getting loopback.
*      For xCat and above not need rule for OAM PDU trap.
*      Use cpssDxChVntOamPortPduTrapEnableSet for it.
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortLoopBackModeEnableGet
(
    IN  GT_U8                   devNum,
    IN GT_PORT_NUM              portNum,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChVntOamPortUnidirectionalEnableSet
*
* DESCRIPTION:
*      Enable/Disable the port for unidirectional transmit.
*      If unidirectional transmit enabled, sending
*      OAM (Operation, Administration, and Maintenance) control packets
*      and data traffic over failed links (ports with link down) is allowed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*       enable   - GT_TRUE:   Enable the port for unidirectional transmit.
*                  GT_FALSE:  Disable the port for unidirectional transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortUnidirectionalEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM    port,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChVntOamPortUnidirectionalEnableGet
*
* DESCRIPTION:
*      Gets the current status of unidirectional transmit for the port.
*      If unidirectional transmit enabled, sending
*      OAM (Operation, Administration, and Maintenance) control packets
*      and data traffic over failed links (ports with link down) is allowed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       port        - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to current status of unidirectional transmit:
*                     GT_TRUE:   Enable the port for unidirectional transmit.
*                     GT_FALSE:  Disable the port for unidirectional transmit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - if feature not supported by port
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortUnidirectionalEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM    port,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChVntCfmEtherTypeSet
*
* DESCRIPTION:
*       Sets the EtherType to identify CFM (Connectivity Fault Management) PDUs.
*       If the packet EtherType matches the CFM EtherType, the ingress Policy
*       key implicitly uses the three User-Defined-Byte (UDB0, UDB1, UDB2) to
*       contain the three CFM data fields, MD Level, Opcode, and Flags,
*       respectively.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       etherType   - CFM (Connectivity Fault Management) EtherType,
*                     (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum.
*       GT_OUT_OF_RANGE          - on wrong etherType values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType
);

/*******************************************************************************
* cpssDxChVntCfmEtherTypeGet
*
* DESCRIPTION:
*       Gets the current EtherType to identify CFM
*       (Connectivity Fault Management) PDUs.
*       If the packet EtherType matches the CFM EtherType, the ingress Policy
*       key implicitly uses the three User-Defined-Byte (UDB0, UDB1, UDB2) to
*       contain the three CFM data fields, MD Level, Opcode, and Flags,
*       respectively.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       etherTypePtr - pointer to CFM (Connectivity Fault Management) EtherType.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmEtherTypeGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *etherTypePtr
);

/*******************************************************************************
* cpssDxChVntCfmLbrOpcodeSet
*
* DESCRIPTION:
*      Sets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       opcode      - CFM LBR Opcode (APPLICABLE RANGES: 0..0xFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, opcode.
*       GT_OUT_OF_RANGE          - on wrong opcode values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmLbrOpcodeSet
(
    IN GT_U8    devNum,
    IN GT_U32   opcode
);

/*******************************************************************************
* cpssDxChVntCfmLbrOpcodeGet
*
* DESCRIPTION:
*      Gets the current CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       opcodePtr   - pointer to CFM LBR Opcode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmLbrOpcodeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *opcodePtr
);

/*******************************************************************************
* cpssDxChVntCfmReplyModeSet
*
* DESCRIPTION:
*      Sets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode change mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       mode    - CFM Opcode change mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, change mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmReplyModeSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT mode
);

/*******************************************************************************
* cpssDxChVntCfmReplyModeGet
*
* DESCRIPTION:
*      Gets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode change mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - (pointer to) CFM Opcode change mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, change mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - output parameter is NULL pointer.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmReplyModeGet
(
    IN GT_U8                            devNum,
    OUT CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT *modePtr
);

/*******************************************************************************
* cpssDxChVntPortGroupLastReadTimeStampGet
*
* DESCRIPTION:
*      Gets the last-read-time-stamp counter value that represent the exact
*      timestamp of the last read operation to the PP registers.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*
* OUTPUTS:
*       timeStampValuePtr - pointer to timestamp of the last read operation.
*                           200  MHz:   granularity - 5 nSec
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntPortGroupLastReadTimeStampGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *timeStampValuePtr
);

/*******************************************************************************
* cpssDxChVntLastReadTimeStampGet
*
* DESCRIPTION:
*      Gets the last-read-time-stamp counter value that represent the exact
*      timestamp of the last read operation to the PP registers.
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       timeStampValuePtr - pointer to timestamp of the last read operation.
*                           200  MHz:   granularity - 5 nSec
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntLastReadTimeStampGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *timeStampValuePtr
);

/*******************************************************************************
* cpssDxChVntOamPortPduTrapEnableSet
*
* DESCRIPTION:
*      Enable/Disable trap to CPU of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs destined for this port are trapped to the CPU.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*       enable   - GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                  GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol.
*
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortPduTrapEnableSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  enable
);

/*******************************************************************************
* cpssDxChVntOamPortPduTrapEnableGet
*
* DESCRIPTION:
*      Get trap to CPU status(Enable/Disable) of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs are trapped to the CPU.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*
*
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                     GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol..
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortPduTrapEnableGet
(
    IN  GT_U8                   devNum,
    IN GT_PORT_NUM              portNum,
    OUT GT_BOOL                 *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChVnth */


