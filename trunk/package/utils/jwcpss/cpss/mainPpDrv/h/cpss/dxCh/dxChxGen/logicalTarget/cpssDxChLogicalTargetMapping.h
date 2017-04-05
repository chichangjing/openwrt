/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChLogicalTargetMapping.h
*
* DESCRIPTION:
*       Logical Target mapping.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

#ifndef __cpssDxChlogicalTargetMappingh
#define __cpssDxChlogicalTargetMappingh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/dxCh/dxChxGen/cpssDxChTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>

/*
 * typedef: enum CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT
 *
 * Description: Mode in which the Logical Ports Egress VLAN member table is
 *              accessed, according to the amount of VLANs and Logical Ports
 *              in the system.
 *
 * Enumerations:
 *
 *    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_4K_VID_32_LP_E
 *                                  - 4K VLANs and 32 Logical Ports in the system
 *    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_2K_VID_64_LP_E
 *                                  - 2K VLANs and 64 Logical Ports in the system
 *    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_1K_VID_128_LP_E
 *                                  - 1K VLANs and 128 Logical Ports in the system
 *    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_512_VID_256_LP_E
 *                                  - 512 VLANs and 256 Logical Ports in the system
 *    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_256_VID_512_LP_E
 *                                  - 256 VLANs and 512 Logical Ports in the system
 *    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_128_VID_1K_LP_E
 *                                  - 128 VLANs and 1K Logical Ports in the system
 *    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_64_VID_2K_LP_E
 *                                  - 64 VLANs and 2K Logical Ports in the system
 */
typedef enum
{
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_4K_VID_32_LP_E,
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_2K_VID_64_LP_E,
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_1K_VID_128_LP_E,
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_512_VID_256_LP_E,
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_256_VID_512_LP_E,
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_128_VID_1K_LP_E,
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_64_VID_2K_LP_E
} CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC
 *
 * Description: Defines the interface info
 *
 * Fields:
 *   outputInterface    - Defines the interface info
 *
 *   egressVlanFilteringEnable - If set to GT_TRUE, apply Egress VLAN filtering
 *                               according to LP Egress VLAN member table config.
 *   egressVlanTagStateEnable  - Enable/Disable the egress VLAN tag state for the
 *                               passenger Ethernet packet for this Pseudowire.
 *   egressVlanTagState - If <egressVlanTagStateEnable> is set to GT_TRUE,
 *                        defines the egress VLAN tag state for the passenger
 *                        Ethernet packet for this Pseudowire.
 *   egressTagTpidIndex - Index to the Egress Logical Port TPID table,
 *                        where each entry defines Tag0 TPID and Tag1 TPID.
 *   assignVid0Enable   - If set to GT_TRUE, override VID0 assignment based on target logical port.
 *   vid0               - The new VID0 assignment. Relevant only if <assignVid0Enable> is set.
 *
 */

typedef struct
{
    CPSS_DXCH_OUTPUT_INTERFACE_STC       outputInterface;

    GT_BOOL                              egressVlanFilteringEnable;
    GT_BOOL                              egressVlanTagStateEnable;
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT  egressVlanTagState;
    GT_U32                               egressTagTpidIndex;
    GT_BOOL                              assignVid0Enable;
    GT_U16                               vid0;

} CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC;

/*******************************************************************************
* cpssDxChLogicalTargetMappingEnableSet
*
* DESCRIPTION:
*       Enable/disable logical port mapping feature on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - physical device number
*       enable   - GT_TRUE:  enable
*                  GT_FALSE: disable .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
);


/*******************************************************************************
* cpssDxChLogicalTargetMappingEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status logical port mapping feature on the
*       specified  device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - Pointer to the enable/disable state.
*                   GT_TRUE : enable,
*                   GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEnableGet
(
    IN   GT_U8     devNum,
    OUT  GT_BOOL   *enablePtr
);



/*******************************************************************************
* cpssDxChLogicalTargetMappingDeviceEnableSet
*
* DESCRIPTION:
*       Enable/disable a target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                       in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*       enable        - GT_TRUE:  enable
*                       GT_FALSE: disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingDeviceEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     logicalDevNum,
    IN  GT_BOOL   enable
);


/*******************************************************************************
* cpssDxChLogicalTargetMappingDeviceEnableGet
*
* DESCRIPTION:
*       Get Enable/disable status of target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                       in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       enablePtr     - Pointer to the  Enable/disable state.
*                       GT_TRUE : enable,
*                       GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingDeviceEnableGet
(
    IN   GT_U8     devNum,
    IN  GT_U8      logicalDevNum,
    OUT  GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntrySet
*
* DESCRIPTION:
*       Set logical target mapping table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                                      in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*       logicalPortNum               - logical port number.
*                                      (APPLICABLE RANGES: 0..63)
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingTableEntrySet
(
    IN GT_U8                           devNum,
    IN GT_U8                           logicalDevNum,
    IN GT_U8                           logicalPortNum,
    IN CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC  *logicalPortMappingTablePtr
);

/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntryGet
*
* DESCRIPTION:
*       Get logical target mapping table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                                      in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*       logicalPortNum               - logical port number.
*                                      (APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingTableEntryGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            logicalDevNum,
    IN GT_U8                            logicalPortNum,
    OUT CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC  *logicalPortMappingTablePtr
);

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet
*
* DESCRIPTION:
*      This function sets the mode in which the Logical Port Egress VLAN member
*      table is accessed, according to the amount of VIDs and Logical Ports in
*      the system.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*       mode    - value to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet
(
    IN GT_U8                                                                  devNum,
    IN CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT    mode
);

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet
*
* DESCRIPTION:
*      This function gets the mode in which the Logical Port Egress VLAN member
*      table is accessed, according to the amount of VIDs and Logical Ports in
*      the system.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*
* OUTPUTS:
*       modePtr - (pointer to) value to get.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet
(
    IN  GT_U8                                                                devNum,
    OUT CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT *modePtr
);

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet
*
* DESCRIPTION:
*      This function sets the number of packets dropped due to Logical Port
*      Egress VLAN Filtering.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*       counter - value to set counter.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet
(
    IN GT_U8    devNum,
    IN GT_U32   counter
);

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet
*
* DESCRIPTION:
*      This function gets the number of packets dropped due to Logical Port
*      Egress VLAN Filtering.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - the device number
*
* OUTPUTS:
*       counterPtr - (pointer to) counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *counterPtr
);

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMappingEntrySet
*
* DESCRIPTION:
*       Function sets the mapping of any discrete VIDs of the 4K VLAN range,
*       into a continuous range of VIDs.
*       Used to accommodate the cases where the 12-bit VLAN-ID must be mapped
*       into a smaller internal VID index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Mapping Table.
*                         (Applicable Range 0..4095)
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (Applicable Range 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressMappingEntrySet
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN GT_U32                       vlanIdIndex
);

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMappingEntryGet
*
* DESCRIPTION:
*       Function gets the mapping of any discrete VIDs of the 4K VLAN range,
*       into a continuous range of VIDs.
*       Used to accommodate the cases where the 12-bit VLAN-ID must be mapped
*       into a smaller internal VID index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Mapping Table.
*                         (Applicable Range 0..4095)
*
* OUTPUTS:
*       vlanIdIndexPtr  - (pointer to) VLAN id index, use as value in
*                         the Mapping Table. (Applicable Range 0..4095)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressMappingEntryGet
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    OUT GT_U32                      *vlanIdIndexPtr
);

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMemberSet
*
* DESCRIPTION:
*       Function sets whether target logical port is a member of a given VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (APPLICABLE RANGES: 0..4095)
*       logicalDevNum   - logical target device number. (APPLICABLE RANGES: 0..31)
*       logicalPortNum  - logical target port number. (APPLICABLE RANGES: 0..63)
*       isMember        - whether target logical port is member of a given VLAN.
*                         GT_TRUE: target logical port is member of a given VLAN
*                         GT_FALSE: target logical port isn't member of a given VLAN
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressMemberSet
(
    IN GT_U8                        devNum,
    IN GT_U32                       vlanIdIndex,
    IN GT_U8                        logicalDevNum,
    IN GT_U8                        logicalPortNum,
    IN GT_BOOL                      isMember
);

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMemberGet
*
* DESCRIPTION:
*       Function gets whether target logical port is a member of a given VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (APPLICABLE RANGES: 0..4095)
*       logicalDevNum   - logical target device number. (APPLICABLE RANGES: 0..31)
*       logicalPortNum  - logical target port number. (APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       isMemberPtr     - (Pointer to) whether target logical port is member
*              of a give VLAN.
*                         GT_TRUE: target logical port is member of a given VLAN
*                         GT_FALSE: target logical port is not member of a given VLAN
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressMemberGet
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       vlanIdIndex,
    IN  GT_U8                        logicalDevNum,
    IN  GT_U8                        logicalPortNum,
    OUT GT_BOOL                     *isMemberPtr
);

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMemberTableClear
*
* DESCRIPTION:
*       Function clears Logical Port Egress VLAN member table.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressMemberTableClear
(
    IN GT_U8                        devNum
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChlogicalTargetMapping */

