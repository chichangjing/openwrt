/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenPpSmi.h
*
* DESCRIPTION:
*       API implementation for ASIC SMI controller
*
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*******************************************************************************/

#ifndef __cpssGenPpSmih
#define __cpssGenPpSmih

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>

/*******************************************************************************
* cpssSmiRegisterRead
*
* DESCRIPTION:
*      The function reads register of a Marvell device, which connected to
*           SMI master controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       dataPtr - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       Function specific for Marvell devices with 32-bit registers
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterRead
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    OUT GT_U32  *dataPtr
);

/*******************************************************************************
* cpssSmiRegisterWrite
*
* DESCRIPTION:
*      The function writes register of a Marvell device, which connected to SMI master
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       Function specific for Marvell devices with 32-bit registers
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterWrite
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);

/*******************************************************************************
* cpssSmiRegisterReadShort
*
* DESCRIPTION:
*      The function reads register of a device, which connected to SMI master
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       dataPtr - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*       GT_NOT_INITIALIZED - smi ctrl register callback not registered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterReadShort
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    OUT GT_U16  *dataPtr
);

/*******************************************************************************
* cpssSmiRegisterWriteShort
*
* DESCRIPTION:
*      The function writes register of a device, which connected to SMI master
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*       GT_NOT_INITIALIZED - smi ctrl register callback not registered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterWriteShort
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U16  data
);

/*******************************************************************************
* cpssXsmiPortGroupRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to write to (APPLICABLE RANGES: 0..31).
*       data        - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiPortGroupRegisterWrite
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    IN  GT_U16  data
);


/*******************************************************************************
* cpssXsmiPortGroupRegisterRead
*
* DESCRIPTION:
*       Read value of a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to read from (APPLICABLE RANGES: 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter.
*       GT_BAD_PTR               - pointer to place data is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiPortGroupRegisterRead
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    OUT GT_U16  *dataPtr
);

/*******************************************************************************
* cpssXsmiRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to write to (APPLICABLE RANGES: 0..31).
*       data        - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiRegisterWrite
(
    IN  GT_U8   devNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    IN  GT_U16  data
);


/*******************************************************************************
* cpssXsmiRegisterRead
*
* DESCRIPTION:
*       Read value of a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to read from (APPLICABLE RANGES: 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter.
*       GT_BAD_PTR               - pointer to place data is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiRegisterRead
(
    IN  GT_U8   devNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    OUT GT_U16  *dataPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenPpSmih */

