/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssHwRegisters.h
*
* DESCRIPTION:
*       Private API definition for register access of the generic device.
*
*       Those APIs gives next benefit :
*       for multi-port group device
*       - write of 'global config' into special units may need to we written only in
*       some of the port groups (not all)
*       - write of 'per port' may need to we written to other port group then
*       specified according to 'dest port'
*
*       still SOME functionality MUST NOT use those APIs , like:
*       - interrupts handling --> needed per specific port group
*       - prvCpssPortGroupsCounterSummary(...) , prvCpssPortGroupBusyWait(...)
*          and maybe others...
*       - maybe others
*
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/

#ifndef __prvCpssHwRegistersh
#define __prvCpssHwRegistersh

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* indication that the CPSS API level need to call directly to the driver and bypass the
   'shim layer' of the 'duplicated cores' / 'duplicated units' because the caller is
   handling unique case that is not generic. */
#define PRV_CPSS_MUST_DIRECT_CALL_TO_DRIVER_INDICATION_CNS


/* max number of supported duplicated units */
#define PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_MAX_CNS    8
/*
 * Typedef: struct PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC
 *
 * Description: structure to hold info about the 'additional' registers addresses
 *              that needs the same 'read'/'write' as the 'original' address.
 *
 * Fields:
 *              numOfAdditionalAddresses - number of (0..PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_MAX_CNS-1)
 *              additionalAddressesArr   - array of additional addresses
 *
  */
typedef struct{
    GT_U32  numOfAdditionalAddresses;
    GT_U32  additionalAddressesArr[PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_MAX_CNS];
}PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC;

/* init structure PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC */
#define PRV_CPSS_INIT_VALUE_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_CNS {0,{0}}

/*******************************************************************************
* prvCpssDuplicatedMultiPortGroupsGet
*
* DESCRIPTION:
*       Check if register address is duplicated in multi-port groups device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum      - the device number.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - register address to access.
*
* OUTPUTS:
*       portGroupsBmpPtr  - (pointer to) the updated port groups
*                       relevant only when function returns GT_TRUE
*       isAdditionalRegDupNeededPtr - (pointer to)
*             GT_TRUE - the caller need to duplicate the value to additional
*                       register in all 'port groups bmp'.
*             GT_FALSE - the caller NOT need to duplicate the value to additional
*                       register.
*                       can be NULL --> meaning 'don't care'
*       additionalRegDupPtr - (pointer to)
*                       register address of additional register duplication.
*                       Only relevant when isAdditionalRegDupNeeded is GT_TRUE.
*                       can be NULL --> meaning 'don't care'
*
* RETURNS:
*       GT_TRUE     - the caller need to use the updated 'port groups bmp'
*       GT_FALSE    - the caller NOT need to use the updated 'port groups bmp'
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvCpssDuplicatedMultiPortGroupsGet
(
    IN GT_U8                    devNum,
    IN GT_U32                   portGroupId,
    IN GT_U32                   regAddr,
    OUT  GT_PORT_GROUPS_BMP     *portGroupsBmpPtr,
    OUT  GT_BOOL                *isAdditionalRegDupNeededPtr,
    OUT  PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC *additionalRegDupPtr
);

/*******************************************************************************
* prvCpssHwPpReadRegister
*
* DESCRIPTION:
*       Read a register value from the given PP. - generic (non-driver) API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP to read from.
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       dataPtr - (pointer to) memory for the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    OUT GT_U32  *dataPtr
);

/*******************************************************************************
* prvCpssHwPpWriteRegister
*
* DESCRIPTION:
*       Write to a PP's given register. - generic (non-driver) API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP to write to.
*       regAddr - The register's address to write to.
*       value   - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpWriteRegister
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 value
);

/*******************************************************************************
* prvCpssHwPpGetRegField
*
* DESCRIPTION:
*       Read a selected register field. - generic (non-driver) API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       regAddr - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*
* OUTPUTS:
*       fieldDataPtr   - (pointer to) Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldDataPtr
);

/*******************************************************************************
* prvCpssHwPpSetRegField
*
* DESCRIPTION:
*       Write value to selected register field. - generic (non-driver) API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       regAddr - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpSetRegField
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 fieldData
);



/*******************************************************************************
* prvCpssHwPpReadRegBitMask
*
* DESCRIPTION:
*       Reads the unmasked bits of a register. - generic (non-driver) API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - PP device number to read from.
*       regAddr - Register address to read from.
*       mask    - Mask for selecting the read bits.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
);



/*******************************************************************************
* prvCpssHwPpWriteRegBitMask
*
* DESCRIPTION:
*       Writes the unmasked bits of a register. - generic (non-driver) API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - PP device number to write to.
*       regAddr - Register address to write to.
*       mask    - Mask for selecting the written bits.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpWriteRegBitMask
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 value
);

/*******************************************************************************
* prvCpssHwPpReadRam
*
* DESCRIPTION:
*       Read from PP's RAM. - generic (non-driver) API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       addr    - Address offset to read from.
*       length  - Number of Words (4 byte) to read.
*
* OUTPUTS:
*       dataPtr    - (pointer to) An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpReadRam
(
    IN  GT_U8   devNum,
    IN  GT_U32  addr,
    IN  GT_U32  length,
    OUT GT_U32  *dataPtr
);

/*******************************************************************************
* prvCpssHwPpWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM. - generic (non-driver) API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       addr    - Address offset to write to.
*       length  - Number of Words (4 byte) to write.
*       dataPtr - (pointer to) the array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpWriteRam
(
    IN GT_U8   devNum,
    IN GT_U32  addr,
    IN GT_U32  length,
    IN GT_U32  *dataPtr
);

/*******************************************************************************
* prvCpssHwPpPortGroupReadRegister
*
* DESCRIPTION:
*       Read a register value from the given PP. - generic (non-driver) API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*
* OUTPUTS:
*       dataPtr - (pointer to) memory for the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpPortGroupReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *dataPtr
);

/*******************************************************************************
* prvCpssHwPpPortGroupWriteRegister
*
* DESCRIPTION:
*       Write to a PP's given register. - generic (non-driver) API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum      - The PP to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to write to.
*       value       - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpPortGroupWriteRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
);

/*******************************************************************************
* prvCpssHwPpPortGroupGetRegField
*
* DESCRIPTION:
*       Read a selected register field. - generic (non-driver) API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum      - The PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*
* OUTPUTS:
*       fieldDataPtr   - (pointer to) Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpPortGroupGetRegField
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   regAddr,
    IN  GT_U32   fieldOffset,
    IN  GT_U32   fieldLength,
    OUT GT_U32   *fieldDataPtr
);

/*******************************************************************************
* prvCpssHwPpPortGroupSetRegField
*
* DESCRIPTION:
*       Write value to selected register field. - generic (non-driver) API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpPortGroupSetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
);



/*******************************************************************************
* prvCpssHwPpPortGroupReadRegBitMask
*
* DESCRIPTION:
*       Reads the unmasked bits of a register. - generic (non-driver) API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum      - PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - Register address to read from.
*       mask        - Mask for selecting the read bits.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpPortGroupReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
);



/*******************************************************************************
* prvCpssHwPpPortGroupWriteRegBitMask
*
* DESCRIPTION:
*       Writes the unmasked bits of a register. - generic (non-driver) API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum      - PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - Register address to write to.
*       mask        - Mask for selecting the written bits.
*       value       - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpPortGroupWriteRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    IN GT_U32   value
);

/*******************************************************************************
* prvCpssHwPpPortGroupReadRam
*
* DESCRIPTION:
*       Read from PP's RAM. - generic (non-driver) API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum      - The PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr        - Address offset to read from.
*       length      - Number of Words (4 byte) to read.
*
* OUTPUTS:
*       dataPtr     - (pointer to) An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpPortGroupReadRam
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *dataPtr
);

/*******************************************************************************
* prvCpssHwPpPortGroupWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM. - generic (non-driver) API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr        - Address offset to write to.
*       length      - Number of Words (4 byte) to write.
*       dataPtr     - (pointer to) An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpPortGroupWriteRam
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *dataPtr
);

/*******************************************************************************
* prvCpssHwPpPortGroupNumDuplicatedUnitsGet
*
* DESCRIPTION:
*       Get the number of duplicated units that a unit have.
*       NOTE: the returned value is 'per core'.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2 ; Lion; Lion2.
*        ExMxPm ; ExMxPm3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       unitId      - the unitId of the 'orig unit' (must be 'instance 0' of the unit)
*
* OUTPUTS:
*       numDuplicatedUnitsPtr - (pointer to) the number of duplicated units.
*                               (in addition to the 'orig unit')
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwPpPortGroupNumDuplicatedUnitsGet
(
    IN GT_U8    devNum,
    IN PRV_CPSS_DXCH_UNIT_ENT   unitId,
    IN GT_U32   *numDuplicatedUnitsPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssHwRegistersh */
