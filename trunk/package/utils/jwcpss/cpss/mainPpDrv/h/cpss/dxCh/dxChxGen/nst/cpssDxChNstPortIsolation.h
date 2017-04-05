
/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNstPortIsolation.h
*
* DESCRIPTION:
*       CPSS DxCh NST Port Isolation Mechanism.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

#ifndef __cpssDxChNstPortIsolationh
#define __cpssDxChNstPortIsolationh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>


/*
 * typedef: enum CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT
 *
 * Description: Enumeration for L2 or/and L3 traffic type
 *
 * Enumerations:
 *    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E - Port isolation for L2
 *                                         packets (L2 Port Isolation table)
 *    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E - Port isolation for L3
 *                                         packets (L3 Port Isolation table)
 *
 */
typedef enum
{
   CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E,
   CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E
}CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT;


/*
 * typedef: enum CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT
 *
 * Description: Enumeration for Port Isolation feature in the TxQ per egress ePort.
 *
 * Enumerations:
 *  CPSS_DXCH_NST_PORT_ISOLATION_DISABLE_E - Port Isolation filter is disabled.
 *  CPSS_DXCH_NST_PORT_ISOLATION_L2_ENABLE_E - Trigger L2 Port Isolation filter.
 *  CPSS_DXCH_NST_PORT_ISOLATION_L3_ENABLE_E - Trigger L3 Port Isolation filter.
 *  CPSS_DXCH_NST_PORT_ISOLATION_ALL_ENABLE_E - Trigger L2 & L3 Port Isolation filter.
 *
 */
typedef enum
{
   CPSS_DXCH_NST_PORT_ISOLATION_DISABLE_E,
   CPSS_DXCH_NST_PORT_ISOLATION_L2_ENABLE_E,
   CPSS_DXCH_NST_PORT_ISOLATION_L3_ENABLE_E,
   CPSS_DXCH_NST_PORT_ISOLATION_ALL_ENABLE_E
}CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT;


/*******************************************************************************
* cpssDxChNstPortIsolationEnableSet
*
* DESCRIPTION:
*       Function enables/disables the port isolation feature.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       enable    - port isolation feature enable/disable
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);


/*******************************************************************************
* cpssDxChNstPortIsolationEnableGet
*
* DESCRIPTION:
*       Function gets enabled/disabled state of the port isolation feature.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       enablePtr - (pointer to) port isolation feature state
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntrySet
*
* DESCRIPTION:
*       Function sets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking. If it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interface.
*                              Only portDev and Trunk are supported.
*       cpuPortMember        - port isolation for CPU Port
*                               GT_TRUE - member
*                               GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written to the
*                              L2/L3 PI table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface or
*                                  localPortsMembersPtr
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationTableEntrySet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_BOOL                                        cpuPortMember,
    IN CPSS_PORTS_BMP_STC                             *localPortsMembersPtr
);

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntryGet
*
* DESCRIPTION:
*       Function gets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking if it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interfaces
*                              Only portDev and Trunk are supported.
*
* OUTPUTS:
*       cpuPortMemberPtr     - (pointer to) port isolation for CPU Port
*                                GT_TRUE - member
*                                GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written
*                              to the L2/L3 PI table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationTableEntryGet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    OUT GT_BOOL                                       *cpuPortMemberPtr,
    OUT CPSS_PORTS_BMP_STC                            *localPortsMembersPtr
);

/*******************************************************************************
* cpssDxChNstPortIsolationPortAdd
*
* DESCRIPTION:
*       Function adds single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Adding local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port isn't blocked.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be added to bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationPortAdd
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_PHYSICAL_PORT_NUM                           portNum
);

/*******************************************************************************
* cpssDxChNstPortIsolationPortDelete
*
* DESCRIPTION:
*       Function deletes single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Deleting local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port is blocked.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be deleted from bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In srcInterface parameter only portDev and Trunk are supported.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationPortDelete
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_PHYSICAL_PORT_NUM                           portNum
);



/*******************************************************************************
* cpssDxChNstPortIsolationModeSet
*
* DESCRIPTION:
*       Set Port Isolation feature mode in the TxQ per egress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       mode        - Port Isolation Mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationModeSet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_NUM                              portNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT    mode
);

/*******************************************************************************
* cpssDxChNstPortIsolationModeGet
*
* DESCRIPTION:
*       Get Port Isolation feature mode in the TxQ per egress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       modePtr     - (pointer to) Port Isolation Mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationModeGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_NUM                             portNum,
    OUT CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT   *modePtr
);

/*******************************************************************************
* cpssDxChNstPortIsolationLookupBitsSet
*
* DESCRIPTION:
*       Set the number of bits from the source Interface that are used to
*       index the port isolation table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       numberOfPortBits    - number of least significant bits of src port (APPLICABLE RANGES: 0..15)
*       numberOfDeviceBits  - number of least significant bits of src device (APPLICABLE RANGES: 0..15)
*       numberOfTrunkBits   - number of least significant bits of src trunk  (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - on value out of range for one of:
*                                   numberOfPortBits > 15 , numberOfDeviceBits > 15 ,
*                                   numberOfTrunkBits > 15
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For example, if the value of this field is 7, and <Port Isolation
*       Lookup Device Bits> is 5, then the index to the port isolation table is
*       {SrcDev[4:0], OrigSRCePort[6:0]}.
*       Note:
*           If <Port Isolation Lookup Port Bits> is 0x0, no bits from the source
*               port are used.
*           If <Port Isolation Lookup Device Bits> is 0x0, no bits from the
*               SrcDev are used.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationLookupBitsSet
(
    IN GT_U8    devNum,
    IN GT_U32   numberOfPortBits,
    IN GT_U32   numberOfDeviceBits,
    IN GT_U32   numberOfTrunkBits
);

/*******************************************************************************
* cpssDxChNstPortIsolationLookupBitsGet
*
* DESCRIPTION:
*       Get the number of bits from the source Interface that are used to
*       index the port isolation table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       numberOfPortBitsPtr    - (pointer to)number of least significant bits of src port (APPLICABLE RANGES: 0..15)
*       numberOfDeviceBitsPtr  - (pointer to)number of least significant bits of src device (APPLICABLE RANGES: 0..15)
*       numberOfTrunkBitsPtr   - (pointer to)number of least significant bits of src trunk  (APPLICABLE RANGES: 0..15)
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
GT_STATUS cpssDxChNstPortIsolationLookupBitsGet
(
    IN GT_U8    devNum,
    OUT GT_U32   *numberOfPortBitsPtr,
    OUT GT_U32   *numberOfDeviceBitsPtr,
    OUT GT_U32   *numberOfTrunkBitsPtr
);


/*******************************************************************************
* cpssDxChNstPortIsolationOnEportsEnableSet
*
* DESCRIPTION:
*       Determines how the port isolation is performed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE: port isolation is performed based on the source ePort.
*                GT_FALSE: port isolation is performed based on the source physical port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationOnEportsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChNstPortIsolationOnEportsEnableGet
*
* DESCRIPTION:
*       Returns how the port isolation is performed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*               GT_TRUE: port isolation is performed based on the source ePort.
*               GT_FALSE: port isolation is performed based on the source physical port.

*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationOnEportsEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChNstPortIsolationLookupTrunkIndexBaseSet
*
* DESCRIPTION:
*       Determines the first index of the trunk ID based lookup.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       trunkIndexBase - the first index of the trunk ID based lookup.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_OUT_OF_RANGE          - on trunkIndexBase out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The default value is 2048 (0x800) for backward compatibility.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationLookupTrunkIndexBaseSet
(
    IN GT_U8    devNum,
    IN GT_U32   trunkIndexBase
);

/*******************************************************************************
* cpssDxChNstPortIsolationLookupTrunkIndexBaseGet
*
* DESCRIPTION:
*       Returns the first index of the trunk ID based lookup.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       trunkIndexBasePtr - (pointer to) the first index of the trunk ID based lookup.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationLookupTrunkIndexBaseGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *trunkIndexBasePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChNstPortIsolationh */

