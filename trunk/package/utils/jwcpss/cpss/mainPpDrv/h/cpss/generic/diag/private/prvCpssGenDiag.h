/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenDiag.h
*
* DESCRIPTION:
*       Internal CPSS functions.
*
* FILE REVISION NUMBER:
*       $Revision: 13 $
*
*******************************************************************************/
#ifndef __prvCpssGenDiagh
#define __prvCpssGenDiagh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/diag/cpssDiag.h>
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>


/* defines the maximum number of words (masks) in one entry */
#define PRV_DIAG_MAX_MASK_NUM_CNS     32

/* defines the maximum size of one entry in bytes */
#define PRV_DIAG_MAX_BURST_SIZE_CNS   128

/* Macro for checking if a test passed or failed:
   status holds the return code status.
   testStatus holds the result of the test (GT_TRUE - pass, GT_FALSE - fail).
   If the test failed, the function should exit with return code GT_OK. */
#define PRV_CPSS_DIAG_CHECK_TEST_RESULT_MAC(status,testStatus)         \
            if ((status) != GT_OK)                                     \
            {                                                          \
                return (status);                                       \
            }                                                          \
            if ((testStatus) == GT_FALSE)                              \
            {                                                          \
                return GT_OK;                                          \
            }


/*******************************************************************************
* PRV_CPSS_DIAG_HW_PP_RAM_BUF_MEM_WRITE_FUNC
*
* DESCRIPTION:
*       BUFFER_DRAM (buff mem) memory write.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - the port group Id , to support multi-port-group devices 
*                     that need to access specific port group
*       addr        - Address offset to write to.
*       length      - length in WORDS (32 bit) must be in portion of 8.
*       dataPtr     - An array containing the data to be written.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DIAG_HW_PP_RAM_BUF_MEM_WRITE_FUNC)
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32_PTR dataPtr
);

/*******************************************************************************
* PRV_CPSS_DIAG_HW_PP_RAM_BUF_MEM_READ_FUNC
*
* DESCRIPTION:
*       Read the memory WORD by WORD.(32 bits).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   : device number.
*       portGroupId : the port group Id , to support multi-port-group devices 
*                     that need to access specific port group
*       addr     : addr to start writing from.
*       length   : length in WORDS (32 bit) must be in portion of 8.
*
* OUTPUTS:
*       dataPtr  : An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DIAG_HW_PP_RAM_BUF_MEM_READ_FUNC)
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    OUT GT_U32_PTR dataPtr
);

/*******************************************************************************
* PRV_CPSS_DIAG_HW_PP_RAM_MEM_COMPARE_FUNC
*
* DESCRIPTION:
*       Memory Compare function (for TCAM memories).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       writeBufferPtr   : write buffer.
*       readBufferPtr    : read buffer.
*       sizeInBytes      : size of the memory to compare in bytes.
*       maskIndexArrPtr  : the index to start, in the masks array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE   - when read and write buffers are equal
*       GT_FALSE  - when read and write buffers are not equal
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_BOOL (*PRV_CPSS_DIAG_HW_PP_RAM_MEM_COMPARE_FUNC)
(
    IN GT_U32   *writeBufferPtr,
    IN GT_U32   *readBufferPtr,
    IN GT_U32   sizeInBytes,
    IN GT_U32   *maskIndexArrPtr
);

/*******************************************************************************
* PRV_CPSS_DIAG_MEMORY_TEST_FUNC
*
* DESCRIPTION:
*       Performs memory test on a specified memory location and size for a
*       specified memory type.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number to test
*       memType     - The packet processor memory type to verify.
*       startOffset - The offset address to start the test from.
*       size        - The memory size in byte to test (start from offset).
*       profile     - The test profile.
*
* OUTPUTS:
*       testStatusPtr  - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The test is done by writing and reading a test pattern.
*       startOffset must be aligned to 4 Bytes and size must be in 4 bytes
*       resolution.
*       The function may be called after Phase 2 initialization.
*       The test is destructive and leaves the memory corrupted.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DIAG_MEMORY_TEST_FUNC)
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         startOffset,
    IN GT_U32                         size,
    IN CPSS_DIAG_TEST_PROFILE_ENT     profile,
    OUT GT_BOOL                       *testStatusPtr,
    OUT GT_U32                        *addrPtr,
    OUT GT_U32                        *readValPtr,
    OUT GT_U32                        *writeValPtr
);

/*
 * Typedef: struct PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_BIND_FUNC_STC
 *
 * Description: Special RAM treatment functions
 *
 * Fields:
 *      prvCpssDiagHwPpRamBufMemWriteFuncPtr - BUFFER_DRAM (buff mem) memory
 *                                             write function
 *      prvCpssDiagHwPpRamInWordsReadFuncPtr - WORD by WORD memory reading
 *                                             function
 *      prvCpssDiagHwPpMemoryWriteFuncPtr    - writing to table entry function
 *      prvCpssDiagHwPpMemoryReadFuncPtr     - reading from table entry function
 *      prvCpssDiagHwPpMemoryCompareFuncPtr  - Special comparing function for 
 *                                             memories when read data is different 
 *                                             from writen one.
 *                                             E.g. TCAM where device converts 
 *                                             writen mask/pattern data to X/Y 
 *                                             representation. But read data is 
 *                                             always in X/Y format.
 *
 */
typedef struct
{
    PRV_CPSS_DIAG_HW_PP_RAM_BUF_MEM_WRITE_FUNC
                                          prvCpssDiagHwPpRamBufMemWriteFuncPtr;
    PRV_CPSS_DIAG_HW_PP_RAM_BUF_MEM_READ_FUNC
                                          prvCpssDiagHwPpRamInWordsReadFuncPtr;
    PRV_CPSS_DIAG_HW_PP_RAM_BUF_MEM_WRITE_FUNC
                                              prvCpssDiagHwPpMemoryWriteFuncPtr;
    PRV_CPSS_DIAG_HW_PP_RAM_BUF_MEM_READ_FUNC
                                              prvCpssDiagHwPpMemoryReadFuncPtr;
    PRV_CPSS_DIAG_HW_PP_RAM_MEM_COMPARE_FUNC
                                              prvCpssDiagHwPpMemoryCompareFuncPtr;

} PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC;

/*******************************************************************************
* prvCpssDiagBurstSizeSet
*
* DESCRIPTION:
*       This interface change the burst size.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       burstSize   : new burst size (in bytes)
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDiagBurstSizeSet
(
    IN GT_U32 burstSize
);

/*******************************************************************************
* prvCpssDiagBurstSizeGet
*
* DESCRIPTION:
*       This interface routine return the burst size.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       GT_U32   - burst size (in bytes)
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvCpssDiagBurstSizeGet
(
    GT_VOID
);

/*******************************************************************************
* prvCpssDiagMemMaskArraySet
*
* DESCRIPTION:
*       fill memMaskArSize with value of maskArrSize
*       fill memMaskArray with data recieved from maskArray
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS: 
*       maskArrSize   - number of words to use as masks
*       maskArray     - original data to fill the maskMemArray with.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         mask array is declared to diagnose specific memory areas
*         such as SALSA MAC or VLAN tables where each element contains
*         several words and each word uses certain set of bits only.
*         Mask array size is equal to area's element words number
*         and every word in the array is a mask to corresponding
*         element's word (mask bit n = 1 if bit n is used in the word)
*         In the regular memory case mask array should contain only
*         one word with all bits = 1, i.e. FFFFFFFF
*******************************************************************************/
GT_STATUS prvCpssDiagMemMaskArraySet
(
    IN  GT_U32 maskArrSize,
    IN  GT_U32 maskArray[]
);

/*******************************************************************************
* prvCpssDiagAnyMemTest
*
* DESCRIPTION:
*       This routine preforms a memory check.
*       calling that function with the memory profile.
*       like random / toggle / aa_55 / incremental.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of only valid Port Groups.
*       memBase           - memory pointer to start the test from
*       size              - the size of memory to check (in bytes)
*       profile           - the test profile
*       specialRamFuncs   - Special RAM treatment functions
*
* OUTPUTS:
*       testStatusPtr  - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function depends on init the "memDiagBurstSize" by
*       prvCpssDiagSetBurstSize().
*
*******************************************************************************/
GT_STATUS prvCpssDiagAnyMemTest
(
    IN GT_U8                                             devNum,
    IN GT_U32                                            portGroupsBmp,
    IN GT_U32                                            memBase,
    IN GT_U32                                            size,
    IN CPSS_DIAG_TEST_PROFILE_ENT                        profile,
    IN PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC      specialRamFuncs,
    OUT GT_BOOL                                          *testStatusPtr,
    OUT GT_U32                                           *addrPtr,
    OUT GT_U32                                           *readValPtr,
    OUT GT_U32                                           *writeValPtr
);

/*******************************************************************************
* prvCpssDiagHwPpRamWrite
*
* DESCRIPTION:
*       Write to PP's RAM.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - The PP device number to write to.
*       portGroupId       - the port group Id , to support multi-port-group 
*                           devices that need to access specific port group
*       addr              - Address offset to write to.
*       length            - Number of Words (4 byte) to write.
*       specialRamFuncs   - Special RAM treatment functions
*       dataPtr           - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDiagHwPpRamWrite
(
    IN GT_U8                                             devNum,
    IN GT_U32                                            portGroupId,
    IN GT_U32                                            addr,
    IN GT_U32                                            length,
    IN PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC      specialRamFuncs,
    IN GT_U32_PTR                                        dataPtr
);

/*******************************************************************************
* prvCpssDiagHwPpRamRead
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - The PP device number to read from.
*       portGroupId       - the port group Id , to support multi-port-group 
*                           devices that need to access specific port group
*       addr              - Address offset to read from.
*       length            - Number of Words (4 byte) to read.
*       specialRamFuncs   - Special RAM treatment functions
*
* OUTPUTS:
*       dataPtr - An array containing the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDiagHwPpRamRead
(
    IN GT_U8                                             devNum,
    IN GT_U32                                            portGroupId,
    IN GT_U32                                            addr,
    IN GT_U32                                            length,
    IN PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC      specialRamFuncs,
    OUT GT_U32                                           *dataPtr
);

/*******************************************************************************
* prvCpssDiagHwPpRamMemoryCompare
*
* DESCRIPTION:
*       Compare characters from the object pointed to by 'writeBufferPtr' to
*       the object pointed to by 'readBufferPtr', by checking words and applying
*       masks.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       writeBufferPtr      - writen data
*       readBufferPtr       - read data
*       size                - size of memory to compare in bytes
*       maskArrIndxPtr      - pointer to mask index
*       specialRamFuncs     - Special RAM treatment functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE   - when content of read and write buffers are equal
*       GT_FALSE  - when content of read and write buffers are not equal

*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL prvCpssDiagHwPpRamMemoryCompare
(
    IN GT_U32                                           *writeBufferPtr,
    IN GT_U32                                           *readBufferPtr,
    IN GT_U32                                           size,
    IN GT_U32                                           *maskArrIndxPtr,
    IN  PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC    specialRamFuncs
);

/*******************************************************************************
* prvCpssDiagMemoryForAllProfiles
*
* DESCRIPTION:
*       Performs all the pattern tests for a specified memory type and size.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The device number to test
*       memType         - The packet processor memory type to verify.
*       size            - The memory size in byte to test (start from offset).
*       diagMemTestFunc - Memory test function pointer
*
* OUTPUTS:
*       testStatusPtr  - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - diagMemTestFunc is NULL
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The test is done by calling cpssExMxDiagMemTest for all the patterns.
*
*******************************************************************************/
GT_STATUS prvCpssDiagMemoryForAllProfiles
(
    IN GT_U8                           devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT       memType,
    IN GT_U32                          size,
    IN PRV_CPSS_DIAG_MEMORY_TEST_FUNC  diagMemTestFunc,
    OUT GT_BOOL                        *testStatusPtr,
    OUT GT_U32                         *addrPtr,
    OUT GT_U32                         *readValPtr,
    OUT GT_U32                         *writeValPtr
);

/*******************************************************************************
* prvCpssDiagRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       ifChannel  - interface channel (PCI/SMI/TWSI)
*       regType    - The register type
*       offset     - the register offset
*       data       - data to write
*       doByteSwap - GT_TRUE:  byte swap will be done on the written data
*                    GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS prvCpssDiagRegWrite
(
    IN GT_UINTPTR                      baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
);

/*******************************************************************************
* prvCpssDiagRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       ifChannel  - interface channel (PCI/SMI/TWSI)
*       regType    - The register type
*       offset     - the register offset
*       doByteSwap - GT_TRUE:  byte swap will be done on the read data
*                    GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - read data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS prvCpssDiagRegRead
(
    IN GT_UINTPTR                      baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
);

/*******************************************************************************
* prvCpssDiagPhyRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PHY registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr     - The base address to access the device
*       ifChannel    - interface channel (PCI/SMI/TWSI)
*       smiRegOffset - The SMI register offset
*       phyAddr      - phy address to access
*       offset       - PHY register offset
*       data         - data to write
*       doByteSwap   - GT_TRUE:  byte swap will be done on the written data
*                      GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS prvCpssDiagPhyRegWrite
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN GT_U32                          smiRegOffset,
    IN GT_U32                          phyAddr,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
);

/*******************************************************************************
* prvCpssDiagPhyRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PHY registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr     - The base address to access the device
*       ifChannel    - interface channel (PCI/SMI/TWSI)
*       smiRegOffset - The SMI register offset
*       phyAddr      - phy address to access
*       offset       - PHY register offset
*       doByteSwap   - GT_TRUE:  byte swap will be done on the read data
*                      GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - read data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS prvCpssDiagPhyRegRead
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN GT_U32                          smiRegOffset,
    IN GT_U32                          phyAddr,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
);


/*
 * typedef: PRV_CPSS_DIAG_CHECK_REG_EXIST_FUNC
 *
 * Description:
 *      Check register existance in particular port group.
 *
 * INPUTS:
 *       devNum          - device number
 *       portGroupId     - the port group Id , to support multi-port-group 
 *                           devices that need to access specific port group
 *       regAddr         - Register address
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_TRUE    - register exists in specific port group
 *       GT_FALSE   - register doesn't exist in specific port group
 *
 * Comment:
 */
typedef GT_BOOL (*PRV_CPSS_DIAG_CHECK_REG_EXIST_FUNC)
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 regAddr
);

/*******************************************************************************
* prvCpssDiagRegsNumGet
*
* DESCRIPTION:
*       Gets the number of registers in use for the PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*       portGroupId   - the port group Id , to support multi-port-group devices 
*                       that need to access specific port group
*       regsListPtr   - Pointer to the regsiters list
*       regsListSize  - Size of registers list
*       offset        - Only addresses of registers bigger than this value
*                       will be taken into account
*       checkRegExistFuncPtr - Pointer to function that checks register existance in 
*                              particular port group.
*
* OUTPUTS:
*       regsNumPtr    - Number of registers in use
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS prvCpssDiagRegsNumGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              portGroupId,
    IN  GT_U32                              *regsListPtr,
    IN  GT_U32                              regsListSize,
    IN  GT_U32                              offset,
    IN  PRV_CPSS_DIAG_CHECK_REG_EXIST_FUNC  checkRegExistFuncPtr,
    OUT GT_U32                              *regsNumPtr
);

/*******************************************************************************
* prvCpssDiagUnitRegsNumGet
*
* DESCRIPTION:
*       Gets the number of registers in use for the the given unit in use.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - The device number
*       regsListPtr       - Pointer to the regsiters list
*       regsListSize      - Size of registers list
*       offset            - Only addresses of registers bigger than this value
*                           will be taken into account
*       unitIdAddrPattern - pattern to search registers os specific unit. Only registers
*                           with address that satisfies the formula 
*                           (register_address & unitIdAddrPattern) == unitIdAddrPattern 
*                           will be counted/dumped.
*       unitIdAddrMask    - mask to search registers of specific unit.
*
* OUTPUTS:
*       regsNumPtr    - Number of registers in use
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS prvCpssDiagUnitRegsNumGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              *regsListPtr,
    IN  GT_U32                              regsListSize,
    IN  GT_U32                              offset,
    IN  GT_U32                              unitIdAddrPattern,
    IN  GT_U32                              unitIdAddrMask,  
    OUT GT_U32                              *regsNumPtr
);

/*******************************************************************************
* prvCpssDiagRegsDataGet
*
* DESCRIPTION:
*       Get the addresses and data of the registers in use.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*       portGroupId   - the port group Id , to support multi-port-group devices 
*                       that need to access specific port group
*       regsListPtr   - Pointer to the regsiters list
*       regsListSize  - Size of registers list
*       offset        - Only addresses of registers bigger than this value
*                       will be taken into account
*       checkRegExistFuncPtr - Pointer to function that checks register existance in 
*                              particular port group.
*       regsNumPtr    - Number of registers to get their addresses and data
*
* OUTPUTS:
*       regsNumPtr    - Number of registers that their data were successfully
*                       retrieved
*       regAddrPtr    - List of registers addresses
*       regDataPtr    - List of registers data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS prvCpssDiagRegsDataGet
(
    IN    GT_U8                                 devNum,
    IN    GT_U32                                portGroupId,
    IN    GT_U32                                *regsListPtr,
    IN    GT_U32                                regsListSize,
    IN    GT_U32                                offset,
    IN    PRV_CPSS_DIAG_CHECK_REG_EXIST_FUNC    checkRegExistFuncPtr,
    INOUT GT_U32                                *regsNumPtr,
    OUT   GT_U32                                *regAddrPtr,
    OUT   GT_U32                                *regDataPtr
);

/*******************************************************************************
* prvCpssDiagResetAndInitControllerRegsDataGet
*
* DESCRIPTION:
*       Get the addresses and data of the registers in use for Reset and Init
*       Controller.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - The device number
*       regsListPtr   - Pointer to the regsiters list
*       regsListSize  - Size of registers list
*       offset        - Only addresses of registers bigger than this value
*                       will be taken into account
*       checkRegExistFuncPtr - Pointer to function that checks register existance in
*                              particular port group.
*       regsNumPtr    - Number of registers to get their addresses and data
*
* OUTPUTS:
*       regsNumPtr    - Number of registers that their data were successfully
*                       retrieved
*       regAddrPtr    - List of registers addresses
*       regDataPtr    - List of registers data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS prvCpssDiagResetAndInitControllerRegsDataGet
(
    IN    GT_U8                                 devNum,
    IN    GT_U32                                *regsListPtr,
    IN    GT_U32                                regsListSize,
    IN    GT_U32                                offset,
    IN    PRV_CPSS_DIAG_CHECK_REG_EXIST_FUNC    checkRegExistFuncPtr,
    INOUT GT_U32                                *regsNumPtr,
    OUT   GT_U32                                *regAddrPtr,
    OUT   GT_U32                                *regDataPtr
);

/*******************************************************************************
* prvCpssDiagUnitRegsDataGet
*
* DESCRIPTION:
*       Get the addresses and data of the registers in use.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - The device number
*       portGroupId       - the port group Id , to support multi-port-group devices 
*                           that need to access specific port group
*       regsListPtr       - Pointer to the regsiters list
*       regsListSize      - Size of registers list
*       offset            - Only addresses of registers bigger than this value
*                           will be taken into account
*       unitIdAddrPattern - pattern to search registers os specific unit. Only registers
*                           with address that satisfies the formula 
*                           (register_address & unitIdAddrPattern) == unitIdAddrPattern 
*                           will be counted/dumped.
*       unitIdAddrMask    - mask to search registers of specific unit.
*       checkRegExistFuncPtr - Pointer to function that checks register existance in 
*                              particular port group.
*       regsNumPtr        - Number of registers to get their addresses and data
*                        
* OUTPUTS:               
*       regsNumPtr        - Number of registers that their data were successfully
*                           retrieved
*       regAddrPtr        - List of registers addresses
*       regDataPtr        - List of registers data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS prvCpssDiagUnitRegsDataGet
(
    IN    GT_U8                                 devNum,
    IN    GT_U32                                portGroupId,
    IN    GT_U32                                *regsListPtr,
    IN    GT_U32                                regsListSize,
    IN    GT_U32                                offset,
    IN    GT_U32                                unitIdAddrPattern,
    IN    GT_U32                                unitIdAddrMask,  
    IN    PRV_CPSS_DIAG_CHECK_REG_EXIST_FUNC    checkRegExistFuncPtr,
    INOUT GT_U32                                *regsNumPtr,
    OUT   GT_U32                                *regAddrPtr,
    OUT   GT_U32                                *regDataPtr
);

/*******************************************************************************
* prvCpssDiagRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of a specific register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*       regAddr       - Register to test
*       regMask       - Register mask. The test verifies only the non-masked
*                       bits.
*       profile       - The test profile
*
* OUTPUTS:
*       testStatusPtr - GT_TRUE if the test succeeded or GT_FALSE for failure
*       readValPtr    - The value read from the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       writeValPtr   - The value written to the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       After the register is tested, the original value prior to the test is
*       restored.
*
*******************************************************************************/
GT_STATUS prvCpssDiagRegTest
(
    IN GT_U8                          devNum,
    IN GT_U32                         regAddr,
    IN GT_U32                         regMask,
    IN CPSS_DIAG_TEST_PROFILE_ENT     profile,
    OUT GT_BOOL                       *testStatusPtr,
    OUT GT_U32                        *readValPtr,
    OUT GT_U32                        *writeValPtr
);

/*******************************************************************************
* prvCpssDiagAllRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of all the registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*       regsPtr       - The registers to test
*       regsArraySize - Number of registers in regsArray
*
* OUTPUTS:
*       testStatusPtr - GT_TRUE if the test succeeded or GT_FALSE for failure
*       badRegPtr     - Address of the register which caused the failure if
*                       testStatusPtr is GT_FALSE. Irrelevant if testStatusPtr
*                       is GT_TRUE.
*       readValPtr    - The value read from the register which caused the
*                       failure if testStatusPtr is GT_FALSE. Irrelevant if
*                       testStatusPtr is GT_TRUE
*       writeValPtr   - The value written to the register which caused the
*                       failure if testStatusPtr is GT_FALSE. Irrelevant if
*                       testStatusPtr is GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The test is done by invoking prvCpssDiagRegTest in loop for all the
*       diagnostics registers and for all the patterns.
*       After each register is tested, the original value prior to the test is
*       restored.
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS prvCpssDiagAllRegTest
(
    IN  GT_U8   devNum,
    IN  GT_U32  *regsPtr,
    IN  GT_U32  regsArraySize,
    OUT GT_BOOL *testStatusPtr,
    OUT GT_U32  *badRegPtr,
    OUT GT_U32  *readValPtr,
    OUT GT_U32  *writeValPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenDiagh */

