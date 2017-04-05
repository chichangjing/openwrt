/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPhySmiPreInit.h
*
* DESCRIPTION:
*        API implementation for PHY SMI pre init configuration.
*        All functions should be called after cpssDxChHwPpPhase1Init() and
*        before cpssDxChPhyPortSmiInit().
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/
#ifndef __cpssDxChPhySmiPreInith
#define __cpssDxChPhySmiPreInith

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>

/*
 * typedef: enum CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT
 *
 * Description: Enumeration of Auto Poll numbers of port
 *
 * Enumerations:
 *      CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E  - 8 ports polling
 *      CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E - 12 ports polling
 *      CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E - 16 ports polling
 */
typedef enum
{
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E = 8,
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E = 12,
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E = 16
} CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT;


/*******************************************************************************
* cpssDxChPhyPortAddrSet
*
* DESCRIPTION:
*       Configure the port's default phy address, this function should be
*       used to change the default port's phy address.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number.
*       phyAddr     - The new phy address, (APPLICABLE RANGES: 0...31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For flex ports this API and cpssDxChPhyPortSmiInterfaceSet() must be
*       used before any access to flex port phy related registers.
*       This function should be called after cpssDxChHwPpPhase1Init()
*       and before cpssDxChPhyPortSmiInit().
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortAddrSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U8   phyAddr
);

/*******************************************************************************
* cpssDxChPhyPortAddrGet
*
* DESCRIPTION:
*       Gets port's phy address from hardware and from database.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number.
*
* OUTPUTS:
*       phyAddFromHwPtr - (pointer to) phy address in HW.
*       phyAddFromDbPtr - (pointer to) phy address in DB.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For flex ports no hardware value exists and this field should be
*       ignored.
*
*******************************************************************************/

GT_STATUS cpssDxChPhyPortAddrGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U8   *phyAddFromHwPtr,
    OUT GT_U8   *phyAddFromDbPtr
);

/*******************************************************************************
* cpssDxChPhyAutoPollNumOfPortsSet
*
* DESCRIPTION:
*       Configure number of auto poll ports for SMI interfaces.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*       devNum                  - physical device number.
*       autoPollNumOfPortsSmi0  - number of ports for SMI0.
*       autoPollNumOfPortsSmi1  - number of ports for SMI1.
*       autoPollNumOfPortsSmi2  - number of ports for SMI2.
*                                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
*       autoPollNumOfPortsSmi3  - number of ports for SMI3.
*                                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, autoPollNumOfPortsSmi0,
*                                                   autoPollNumOfPortsSmi1,
*                                                   autoPollNumOfPortsSmi2,
*                                                   autoPollNumOfPortsSmi3
*
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable combinations for Auto Poll number of ports are:
*       |-----------------------|
*       |   SMI 0   |   SMI 1   |
*       |-----------|-----------|
*       |     8     |    16     |
*       |     8     |    12     |
*       |    12     |    12     |
*       |    16     |    8      |
*       |-----------------------|
*       |-----------------------|
*       |   SMI 2   |   SMI 3   |
*       |-----------|-----------|
*       |     8     |    16     |
*       |     8     |    12     |
*       |    12     |    12     |
*       |    16     |    8      |
*       |-----------------------|
*
*       This function should be called after cpssDxChHwPpPhase1Init()
*       and before cpssDxChPhyPortSmiInit().
*
*******************************************************************************/
GT_STATUS cpssDxChPhyAutoPollNumOfPortsSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi0,
    IN  CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi1,
    IN  CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi2,
    IN  CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi3
);

/*******************************************************************************
* cpssDxChPhyAutoPollNumOfPortsGet
*
* DESCRIPTION:
*       Get number of auto poll ports for SMI interfaces.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*       devNum                  - physical device number.
*
* OUTPUTS:
*       autoPollNumOfPortsSmi0Ptr  - number of ports for SMI0.
*       autoPollNumOfPortsSmi1Ptr  - number of ports for SMI1.
*       autoPollNumOfPortsSmi2Ptr  - number of ports for SMI2.
*                                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
*       autoPollNumOfPortsSmi3Ptr  - number of ports for SMI3.
*                                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, autoPollNumOfPortsSmi0,
*                                                   autoPollNumOfPortsSmi1,
*                                                   autoPollNumOfPortsSmi2,
*                                                   autoPollNumOfPortsSmi3
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPhyAutoPollNumOfPortsGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi0Ptr,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi1Ptr,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi2Ptr,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi3Ptr
);

/*******************************************************************************
* cpssDxChPhyPortSmiInterfaceSet
*
* DESCRIPTION:
*       Configure port SMI interface.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*       smiInterface    - port SMI interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum or SMI interface.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For flex ports this API and cpssDxChPhyPortAddrSet() must be
*       used before any access to flex port PHY related registers.
*       This function should be called after cpssDxChHwPpPhase1Init()
*       and before cpssDxChPhyPortSmiInit().
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiInterfaceSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PHY_SMI_INTERFACE_ENT  smiInterface
);

/*******************************************************************************
* cpssDxChPhyPortSmiInterfaceGet
*
* DESCRIPTION:
*       Get port SMI interface.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*
* OUTPUTS:
*       smiInterfacePtr - (pointer to) port SMI interface.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - port SMI interface was not initialized
*                                  correctly.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiInterfaceGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PHY_SMI_INTERFACE_ENT  *smiInterfacePtr
);

/*******************************************************************************
* cpssDxChPhyPortSmiInterfacePortGroupSet
*
* DESCRIPTION:
*       This API maps a port to SMI interface in a multi port Groups devices.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                - physical device number.
*       portNum               - port number.
*       smiInterfacePortGroup - the port group of SMI interface to configure the port.
*                               (APPLICABLE RANGES: Lion 0..3; Lion2 0..7)
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       this API doesn't access the HW but only updates an internal database.
*       After calling this API the application must access the PHYs by
*       cpssDxChPhyPortSmiRegisterRead and cpssDxChPhyPortSmiRegisterWrite
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiInterfacePortGroupSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U32  smiInterfacePortGroup
);
/*******************************************************************************
* cpssDxChPhyPortSmiInterfacePortGroupGet
*
* DESCRIPTION:
*       This API gets the SMI interface connected to a port in a multi port Groups
*       devices.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                - physical device number.
*       portNum               - port number.
*
* OUTPUTS:
*       smiInterfacePortGroupPtr - (pointer to) port group of SMI interface.
*                                  (APPLICABLE RANGES: Lion 0..3; Lion2 0..7)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiInterfacePortGroupGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT  GT_U32  *smiInterfacePortGroupPtr
);

/*******************************************************************************
* cpssDxChPhyPortXSmiInterfaceSet
*
* DESCRIPTION:
*       Configure port XSMI interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*       smiInterface    - port XSMI interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum or XSMI interface.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on wrong port type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortXSmiInterfaceSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT  smiInterface
);

/*******************************************************************************
* cpssDxChPhyPortXSmiInterfaceGet
*
* DESCRIPTION:
*       Get port XSMI interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*
* OUTPUTS:
*       smiInterfacePtr - (pointer to) port XSMI interface.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum or XSMI interface.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on wrong port type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortXSmiInterfaceGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_PHY_XSMI_INTERFACE_ENT  *smiInterfacePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPhySmiPreInith */

