/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChDiag.h
*
* DESCRIPTION:
*       Internal header with diag utilities.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __prvCpssDxChDiagh
#define __prvCpssDxChDiagh

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/diag/cpssDiag.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssDxChDiagBistCheckSkipClient
*
* DESCRIPTION:
*       Check skip of DFX client for BIST
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum        - PP device number
*       pipe          - DFX pipe number
*       client        - DFX client number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE                  - skip BIST
*       GT_FALSE                 - does not skip BIST
*
* COMMENTS:
*
******************************************************************************/
GT_BOOL prvCpssDxChDiagBistCheckSkipClient
(
    IN  GT_U8       devNum,
    IN  GT_U32      pipe,
    IN  GT_U32      client
); 

/*******************************************************************************
* prvCpssDfxMemoryRegRead
*
* DESCRIPTION:
*       Function reads DFX memory registers.
*
* APPLICABLE DEVICES:
*       Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       pipeId      - DFX pipe ID
*       clientId    - DFX client ID
*       memNumber   - DFX memory ID
*       memReg      - DFX memory register address 
*
* OUTPUTS:
*       regDataPtr  - (pointer to) register data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryRegRead
(
    IN GT_U8 devNum,
    IN GT_U32 pipeId,
    IN GT_U32 clientId,
    IN GT_U32 memNumber,
    IN GT_U32 memReg,
    OUT GT_U32 *regDataPtr
);

/*******************************************************************************
* prvCpssDfxMemoryRegWrite
*
* DESCRIPTION:
*       Function writes DFX memory registers.
*
* APPLICABLE DEVICES:
*       Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       pipeId      - DFX pipe ID
*       clientId    - DFX client ID
*       memNumber   - DFX memory ID
*       memReg      - DFX memory register address 
*       regData     - register data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryRegWrite
(
    IN GT_U8 devNum,
    IN GT_U32 pipeId,
    IN GT_U32 clientId,
    IN GT_U32 memNumber,
    IN GT_U32 memReg,
    IN GT_U32 regData
);

/*******************************************************************************
* prvCpssDfxMemoryErrorInjection
*
* DESCRIPTION:
*       Inject error to a selected DFX client RAM.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum      - PP device number
*       pipeId      - DFX pipe ID
*       clientId    - DFX client ID
*       memNumber   - DFX memory ID
*       mode        - error injection mode
*       enable      - enable/disable error injection
*                         GT_TRUE - enable
*                         GT_FALSE - disable

*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryErrorInjectionSet
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           pipeId,
    IN GT_U32                                           clientId,
    IN GT_U32                                           memNumber,
    IN CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT   mode,
    IN GT_BOOL                                          enable
);

/*******************************************************************************
* prvCpssDfxClientRegRead
*
* DESCRIPTION:
*       Function reads DFX client registers.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       pipeId      - DFX pipe ID
*       clientId    - DFX client ID
*       clientReg   - DFX client register address 
*
* OUTPUTS:
*       regDataPtr  - (pointer to) register data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxClientRegRead
(
    IN GT_U8 devNum,
    IN GT_U32 pipeId,
    IN GT_U32 clientId,
    IN GT_U32 clientReg,
    OUT GT_U32 *regDataPtr
);

/*******************************************************************************
* prvCpssDfxMemoryPipeIdSet
*
* DESCRIPTION:
*       Function sets DFX pipe select register.
*
* APPLICABLE DEVICES:
*       Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       pipeId      - DFX pipe ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryPipeIdSet
(
    IN GT_U8 devNum,
    IN GT_U32 pipeId
);

/*******************************************************************************
* prvCpssDfxMemoryPipeIdGet
*
* DESCRIPTION:
*       Function sets DFX pipe select register.
*
* APPLICABLE DEVICES:
*       Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*
* OUTPUTS:
*       pipeIdPtr   - (pointer to) current DFX pipe ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryPipeIdGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *pipeIdPtr
);

/*******************************************************************************
* prvCpssDfxMemoryAllPipesSet
*
* DESCRIPTION:
*       Function sets all pipes in DFX pipe select register.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum      - PP device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryAllPipesSet
(
    IN GT_U8 devNum
);

/*******************************************************************************
* prvCpssDfxClientRegAddressGet
*
* DESCRIPTION:
*       Get DFX client register address
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       client           - DFX client number.
*       regAddr          - Relative register address in client memory space.
*
* OUTPUTS:
*       clientRegAddrPtr - (pointer to) register addrress in specific client
*                          memory space.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong client number
*       GT_BAD_PTR   - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDfxClientRegAddressGet
(
    IN  GT_U32   client,
    IN  GT_U32   regAddr,
    OUT GT_U32   *clientRegAddrPtr
);

/*******************************************************************************
* prvCpssDfxMemoryRegAddressGet
*
* DESCRIPTION:
*       Get DFX memory register address
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       client  - DFX client number.
*       memory  - DFX client RAM number.
*       regAddr - Relative register address in RAM space.
*
* OUTPUTS:
*       memoryRegAddrPtr - (pointer to) register addrress in for specific client
*                          memory.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong client or RAM number
*       GT_BAD_PTR   - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryRegAddressGet
(
    IN  GT_U32   client,
    IN  GT_U32   memory,
    IN  GT_U32   regAddr,
    OUT GT_U32   *memoryRegAddrPtr
);

/*******************************************************************************
* prvCpssDfxClientSetRegField
*
* DESCRIPTION:
*       Function set a field in a DFX client register.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - The PP device number to write to.
*       pipe            - DFX pipe the client belongs to.
*                         (not relevant for BC, MC or Data logging clients).
*       client          - DFX client to write to.
*       clientRegAddr   - The client register's address to write to.
*       fieldOffset     - The start bit number in the register.
*       fieldLength     - The number of bits to be written to register.
*       fieldData       - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Since reading BC, MC or Data logging DFX clients registers is prohibited
*       client 0 value is read to emulate the default or already existing value.
*
*******************************************************************************/
GT_STATUS prvCpssDfxClientSetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   pipe,
    IN GT_U32   client,
    IN GT_U32   clientRegAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
);

/*******************************************************************************
* prvCpssDxChDiagBistAllRamSet
*
* DESCRIPTION:
*       Use BIST to set physical RAMs with specific pattern values.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum        - PP device number
*       pattern       - parren to set in RAMs.
*                       May be one of the list: 0; 0x55; 0xAA; 0xFF.
*       skipCpuMemory - skip internal CPU related memories from MC distribution
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_TIMEOUT               - on BIST timeout
*       GT_BAD_STATE             - on BIST failure
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Triggering the BIST will cause memory content corruption!!!
*       Executing BIST cause multiple CPSS_PP_DATA_INTEGRITY_ERROR_E events
*       which can be masked by the application using cpssEventDeviceMaskSet API.
*
******************************************************************************/
GT_STATUS prvCpssDxChDiagBistAllRamSet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      pattern,
    IN  GT_BOOL                                     skipCpuMemory
);

/*******************************************************************************
* prvCpssDxChDiagBistCaelumFixBist
*
* DESCRIPTION:
*       Function fixes BIST Max Address register default value.
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat2; Bobcat3.
*
* INPUTS:
*       devNum        - PP device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_TIMEOUT               - on BIST timeout
*       GT_BAD_STATE             - on BIST failure
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS prvCpssDxChDiagBistCaelumFixBist
(
    IN  GT_U8   devNum
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChDiagh */
    

