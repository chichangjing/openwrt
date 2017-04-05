/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenDiag.c
*
* DESCRIPTION:
*       Internal CPSS functions.
*
* FILE REVISION NUMBER:
*       $Revision: 21 $
*
*******************************************************************************/

#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/diag/private/prvCpssGenDiag.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#ifdef ASIC_SIMULATION
    #include <asicSimulation/SCIB/scib.h>
#endif /*ASIC_SIMULATION*/


#ifdef CPSS_DEBUG_DIAG
#define DBG_INFO(x)     cpssOsPrintf x
#else
#define DBG_INFO(x)
#endif

/*******************************************************************************
* Internal usage environment parameters
*******************************************************************************/

#define DIAG_BYTE_SWAP_MAC(doSwap, data)            \
    (doSwap == GT_TRUE) ? (BYTESWAP_MAC(data)) : data;

/* Macro to calculate the memory address from the device, memory size and memory
   base address */
#define CALC_MEM_ADDR_MAC(memBase,memSize)          \
    memBase + memSize

/* Macro to calculate address completion for PHY access */
#define PRV_DIAG_CALC_PHY_ACCESS_COMPLETION_MAC(value)  ((value) >> 27) & 0x1

/* write the DATA into the address
    do the write 3 time ... to make sure it is done ...
*/
#define PCI_3_WRITE_DATA_INTO_ADDR_MAC(_address , _doByteSwap , _data)   \
    PCI_1_WRITE_DATA_INTO_ADDR_MAC(_address , _doByteSwap , _data);      \
    PCI_1_WRITE_DATA_INTO_ADDR_MAC(_address , _doByteSwap , _data);      \
    PCI_1_WRITE_DATA_INTO_ADDR_MAC(_address , _doByteSwap , _data)

#ifdef ASIC_SIMULATION
    static GT_U32 hwUsed = 0;
#ifdef GM_USED  /* the GM supports address completion */
    static GT_U32 gmUsed = 1;
#else
    static GT_U32 gmUsed = 0;
#endif
    #define PCI_CFG_WRITE_DATA_INTO_ADDR_MAC(_address , _doByteSwap , _data)   \
        PCI_1_WRITE_DATA_INTO_ADDR_MAC(_address , _doByteSwap , _data)
    #define READ_CFG_DATA_FROM_ADDR_MAC(_address , _data) \
        scibPciRegRead(simulationDeviceId,_address,1,&_data)
    /* write the DATA into the address
        do the write 1 time .
    */
    #define PCI_1_WRITE_DATA_INTO_ADDR_MAC(_address , _doByteSwap , _data)   \
        scibWriteMemory(simulationDeviceId,_address,1,&_data)
    /* read DATA from the address */
    #define READ_DATA_FROM_ADDR_MAC(_address , _data) \
        scibReadMemory(simulationDeviceId,_address,1,&_data)
#else /* not GM_USED and not ASIC_SIMULATION*/
    static GT_U32 gmUsed = 0;
    static GT_U32 hwUsed = 1;
    /* write the DATA into the address
        do the write 1 time .
    */
    #define PCI_CFG_WRITE_DATA_INTO_ADDR_MAC(_address , _doByteSwap , _data)   \
        PCI_1_WRITE_DATA_INTO_ADDR_MAC(_address , _doByteSwap , _data)

    #define READ_CFG_DATA_FROM_ADDR_MAC(_address , _data) \
        READ_DATA_FROM_ADDR_MAC(_address , _data)

    /* read DATA from the address */
    #define READ_DATA_FROM_ADDR_MAC(_address , _data) \
            _data = *(volatile GT_U32 *)(_address)
    /* write the DATA into the address
        do the write 1 time .
    */
    #define PCI_1_WRITE_DATA_INTO_ADDR_MAC(_address , _doByteSwap , _data)   \
        *(volatile GT_U32 *)(_address) = DIAG_BYTE_SWAP_MAC(_doByteSwap, _data)

#endif



#ifdef GM_USED  /* the GM supports address completion */
#elif defined ASIC_SIMULATION   /* cause compilation error , since not supports address completion */
#else /* not GM_USED and not ASIC_SIMULATION*/
#endif
/* offset of the address completion for PEX 8 completion regions */
#define PEX_MBUS_ADDR_COMP_REG_MAC(_index) (0x120 + (4 * _index))
/* bits of address passes as is throw PCI window */
#define NOT_ADDRESS_COMPLETION_BITS_NUM_CNS 19
/* bits of address extracted from address completion registers */
#define ADDRESS_COMPLETION_BITS_MASK_CNS    (0xFFFFFFFF << NOT_ADDRESS_COMPLETION_BITS_NUM_CNS)


/* use region 2 for address completion */
#define DIAG_ADDR_COMPLETION_REGION_CNS 2

#ifdef ASIC_SIMULATION
static GT_U32   simulationDeviceId = 0;
#endif

/*
 * Typedef: PRV_CPSS_DIAG_REGS_SEARCH_TYPE_ENT
 *
 * Description: Test profile.
 *
 * Enumerations:
 *      PRV_CPSS_DIAG_REGS_NUM_ONLY_SEARCH_E - look for registers in use number
 *      PRV_CPSS_DIAG_REGS_ADDR_SEARCH_E     - look for registers addresses
 *      PRV_CPSS_DIAG_REGS_DATA_SEARCH_E     - look for registers data
 *      PRV_CPSS_DIAG_REGS_DATA_RESET_AND_INIT_CTRL_SEARCH_E - look for
 *                                      Reset and Init Controller registers data
 */
typedef enum
{
    PRV_CPSS_DIAG_REGS_NUM_ONLY_SEARCH_E,
    PRV_CPSS_DIAG_REGS_ADDR_SEARCH_E,
    PRV_CPSS_DIAG_REGS_DATA_SEARCH_E,
    PRV_CPSS_DIAG_REGS_DATA_RESET_AND_INIT_CTRL_SEARCH_E
}PRV_CPSS_DIAG_REGS_SEARCH_TYPE_ENT;

/*******************************************************************************
* Internal static function declaration
*******************************************************************************/
static void setWriteBuffer
(
    IN  GT_U32  bufferSize,
    IN  GT_U32  background,
    OUT GT_U32* writeBufferPtr
);

static GT_STATUS testAnyMemToggle
(
    IN  GT_U8                                             devNum,
    IN  GT_U32                                             portGroupId,
    IN  GT_U32                                            memBase,
    IN  GT_U32                                            size,
    IN  PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC      specialRamFuncs,
    OUT GT_BOOL                                           *testStatusPtr,
    OUT GT_U32                                            *addrPtr,
    OUT GT_U32                                            *readValPtr,
    OUT GT_U32                                            *writeValPtr
);

static GT_STATUS testAnyMemIncremental
(
    IN  GT_U8                                             devNum,
    IN  GT_U32                                            portGroupId,
    IN  GT_U32                                            memBase,
    IN  GT_U32                                            size,
    IN  PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC      specialRamFuncs,
    OUT GT_BOOL                                           *testStatusPtr,
    OUT GT_U32                                            *addrPtr,
    OUT GT_U32                                            *readValPtr,
    OUT GT_U32                                            *writeValPtr
);

static GT_STATUS testAnyMem_AA_55
(
    IN  GT_U8                                             devNum,
    IN  GT_U32                                            portGroupId,
    IN  GT_U32                                            memBase,
    IN  GT_U32                                            size,
    IN  PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC      specialRamFuncs,
    OUT GT_BOOL                                           *testStatusPtr,
    OUT GT_U32                                            *addrPtr,
    OUT GT_U32                                            *readValPtr,
    OUT GT_U32                                            *writeValPtr
);

static GT_STATUS testAnyMemRandom
(
    IN  GT_U8                                             devNum,
    IN  GT_U32                                            portGroupId,
    IN  GT_U32                                            memBase,
    IN  GT_U32                                            size,
    IN  PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC      specialRamFuncs,
    OUT GT_BOOL                                           *testStatusPtr,
    OUT GT_U32                                            *addrPtr,
    OUT GT_U32                                            *readValPtr,
    OUT GT_U32                                            *writeValPtr
);

static GT_BOOL testMemCmp
(
    IN GT_U32                  *str1Ptr,
    IN GT_U32                  *str2Ptr,
    IN GT_U32                   size,
    IN GT_U32                  *maskArrIndxPtr
);

static GT_STATUS testRegisterRandom
(
    IN GT_U8                     devNum,
    IN GT_U32                    portGroupId,
    IN GT_U32                    regAddr,
    IN GT_U32                    regMask,
    OUT GT_BOOL                  *testStatusPtr,
    OUT GT_U32                   *readValPtr,
    OUT GT_U32                   *writeValPtr
);

static GT_STATUS testRegisterIncremental
(
    IN GT_U8                     devNum,
    IN GT_U32                    portGroupId,
    IN GT_U32                    regAddr,
    IN GT_U32                    regMask,
    OUT GT_BOOL                  *testStatusPtr,
    OUT GT_U32                   *readValPtr,
    OUT GT_U32                   *writeValPtr
);

static GT_STATUS testRegisterToggle
(
    IN GT_U8                     devNum,
    IN GT_U32                    portGroupId,
    IN GT_U32                    regAddr,
    IN GT_U32                    regMask,
    OUT GT_BOOL                  *testStatusPtr,
    OUT GT_U32                   *readValPtr,
    OUT GT_U32                   *writeValPtr
);

static GT_STATUS testRegisterAA55
(
    IN GT_U8                     devNum,
    IN GT_U32                    portGroupId,
    IN GT_U32                    regAddr,
    IN GT_U32                    regMask,
    OUT GT_BOOL                  *testStatusPtr,
    OUT GT_U32                   *readValPtr,
    OUT GT_U32                   *writeValPtr
);

static GT_STATUS regPciWrite
(
    IN GT_UINTPTR                      baseAddr,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    IN GT_U32                          regData,
    IN GT_BOOL                         doByteSwap
);

static GT_STATUS regPciRead
(
    IN GT_UINTPTR                      baseAddr,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    OUT GT_U32                         *regDataPtr,
    IN GT_BOOL                         doByteSwap
);

static GT_STATUS regSmiWrite
(
    IN GT_U32                          baseAddr,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
);

static GT_STATUS regSmiRead
(
    IN GT_U32                          baseAddr,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
);

static GT_STATUS regTwsiWrite
(
    IN GT_U32                          baseAddr,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
);

static GT_STATUS regTwsiRead
(
    IN GT_U32                          baseAddr,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
);


static GT_STATUS deviceSupportPex8Regions
(
    IN GT_UINTPTR                      baseAddr,
    IN GT_BOOL                         doByteSwap
);

static GT_U32 pexNumRegionsSet
(
    IN GT_UINTPTR                      baseAddr,
    IN GT_BOOL                         doByteSwap,
    IN GT_U32                          numOfRegions
);

static GT_STATUS regPex8RegionsWrite
(
    IN GT_UINTPTR                      baseAddr,
    IN GT_U32                          offset,
    IN GT_U32                          regData,
    IN GT_BOOL                         doByteSwap
);

static GT_STATUS regPex8RegionsRead
(
    IN GT_UINTPTR                      baseAddr,
    IN GT_U32                          offset,
    OUT GT_U32                         *regDataPtr,
    IN GT_BOOL                         doByteSwap
);


static GT_STATUS regsInfoSearch
(
    IN    GT_U8                              devNum,
    IN    GT_U32                             portGroupId,
    IN    PRV_CPSS_DIAG_REGS_SEARCH_TYPE_ENT searchType,
    IN    GT_U32                             *regsListPtr,
    IN    GT_U32                             regsListSize,
    IN    GT_U32                             offset,
    IN    GT_U32                             unitIdAddrPattern,
    IN    GT_U32                             unitIdAddrMask,
    IN    PRV_CPSS_DIAG_CHECK_REG_EXIST_FUNC checkRegExistFuncPtr,
    INOUT GT_U32                             *regsNumPtr,
    OUT   GT_U32                             *regsInfoPtr
);

/* the background data is kept in here  we assume no more than 16*4=64B burst*/
static GT_U32   backgroundBuffer[PRV_DIAG_MAX_BURST_SIZE_CNS];
/* the data is kept in here  we assume no more than 16*4=64B burst*/
static GT_U32   writeBuffer[PRV_DIAG_MAX_BURST_SIZE_CNS];
/* the data is kept in here  we assume no more than 16*4=64B burst*/
static GT_U32   readBuffer[PRV_DIAG_MAX_BURST_SIZE_CNS];

/**********************************************************************
* The following Mask data is declared to diagnose a specific memory
* areas such as SALSA MAC or VLAN tables where each element contains
* several words and each word uses certain set of bits only.
* Mask Array Size is equal to area's element words number
* and every word in the Mask Array is a mask to corresponding
* element's word (mask bit n = 1 if bit n is used in the word)
* In the regular memory case Mask Array contains only one word
* with all bits = 1 (i.e. FFFFFFFF) and Mask Array Size = 1.
**********************************************************************/
static GT_U32   memMaskArSize;
static GT_U32   memMaskArray[PRV_DIAG_MAX_MASK_NUM_CNS];

static GT_U32   memDiagBurstSize = 4; /* memDiagBurstSize is in bytes */
static GT_BOOL  testInProgress = GT_FALSE ;

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
)
{
    if((testInProgress == GT_FALSE &&
       (burstSize % 4)  == 0x0)     &&
       (burstSize <= PRV_DIAG_MAX_BURST_SIZE_CNS))
    {
        memDiagBurstSize = burstSize;
        return GT_OK;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
}

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
)
{
    return memDiagBurstSize;
}

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
)
{
    GT_U32 bufferPos;

    if((testInProgress == GT_FALSE) && (maskArrSize <= PRV_DIAG_MAX_MASK_NUM_CNS))
    {
       memMaskArSize = maskArrSize;
       for (bufferPos=0; bufferPos < maskArrSize; bufferPos++)
       {
           memMaskArray[bufferPos] = maskArray[bufferPos];
       }
       return GT_OK;
    }
    else
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

}

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
)
{
    GT_U32    portGroupId;      /* port group id    */
    GT_STATUS ret = GT_OK;      /* return code      */

    *writeValPtr = 0x0;
    *readValPtr  = 0x0;
    *addrPtr     = memBase;

    if (testInProgress == GT_TRUE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    testInProgress = GT_TRUE ;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        switch (profile)
        {
            case CPSS_DIAG_TEST_BIT_TOGGLE_E:
                ret = testAnyMemToggle(devNum, portGroupId, memBase, size,
                                       specialRamFuncs, testStatusPtr, addrPtr,
                                       readValPtr, writeValPtr);
                break;
            case CPSS_DIAG_TEST_INCREMENTAL_E:
                ret = testAnyMemIncremental(devNum, portGroupId, memBase, size,
                                            specialRamFuncs, testStatusPtr, addrPtr,
                                            readValPtr, writeValPtr);
                break;
            case CPSS_DIAG_TEST_AA_55_E:
                ret = testAnyMem_AA_55(devNum, portGroupId, memBase, size,
                                       specialRamFuncs, testStatusPtr, addrPtr,
                                       readValPtr, writeValPtr);
                break;
            case CPSS_DIAG_TEST_RANDOM_E:
                ret = testAnyMemRandom(devNum, portGroupId, memBase, size,
                                       specialRamFuncs, testStatusPtr, addrPtr,
                                       readValPtr, writeValPtr);
                break;
        default:
            testInProgress = GT_FALSE;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)


    testInProgress = GT_FALSE;
    return ret;
}

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
)
{
    GT_STATUS rc;
    if (specialRamFuncs.prvCpssDiagHwPpRamBufMemWriteFuncPtr != NULL)
    {
        rc = specialRamFuncs.prvCpssDiagHwPpRamBufMemWriteFuncPtr
            (devNum, portGroupId, addr, length, dataPtr); /* Buff Mem addr space */
    }
    else if (specialRamFuncs.prvCpssDiagHwPpMemoryWriteFuncPtr != NULL)
    {
        rc = specialRamFuncs.prvCpssDiagHwPpMemoryWriteFuncPtr
            (devNum, portGroupId, addr, length, dataPtr);
    }
    else
    {
        rc = prvCpssHwPpPortGroupWriteRam(devNum, portGroupId, addr, length,
                                             dataPtr);
    }
    return rc;
}

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
)
{
    GT_STATUS rc;
    if (specialRamFuncs.prvCpssDiagHwPpRamInWordsReadFuncPtr != NULL)
    {
        rc = specialRamFuncs.prvCpssDiagHwPpRamInWordsReadFuncPtr
            (devNum, portGroupId, addr, length, dataPtr); /* Buff Mem addr space */
    }
    else if (specialRamFuncs.prvCpssDiagHwPpMemoryReadFuncPtr != NULL)
    {
        rc = specialRamFuncs.prvCpssDiagHwPpMemoryReadFuncPtr
            (devNum, portGroupId, addr, length, dataPtr);
    }
    else
    {
        rc = prvCpssHwPpPortGroupReadRam(devNum, portGroupId, addr, length,
                                            dataPtr);
    }
    return rc;
}

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
)
{
    GT_STATUS rc;
    if (specialRamFuncs.prvCpssDiagHwPpMemoryCompareFuncPtr != NULL)
    {
        rc = specialRamFuncs.prvCpssDiagHwPpMemoryCompareFuncPtr
            (writeBufferPtr, readBufferPtr, size, maskArrIndxPtr);
        return rc;
    }
    rc = testMemCmp(writeBufferPtr, readBufferPtr, size, maskArrIndxPtr);
    return rc;
}

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
)
{
    GT_STATUS status;

    if (diagMemTestFunc == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    /* AA_55 test */
    status = diagMemTestFunc (devNum, memType, 0, size, CPSS_DIAG_TEST_AA_55_E,
                              testStatusPtr, addrPtr, readValPtr, writeValPtr);
    PRV_CPSS_DIAG_CHECK_TEST_RESULT_MAC(status, *testStatusPtr);

    /* Random test */
    status = diagMemTestFunc (devNum, memType, 0, size, CPSS_DIAG_TEST_RANDOM_E,
                              testStatusPtr, addrPtr, readValPtr, writeValPtr);
    PRV_CPSS_DIAG_CHECK_TEST_RESULT_MAC(status, *testStatusPtr);

    /* incremental test */
    return diagMemTestFunc (devNum, memType, 0, size, CPSS_DIAG_TEST_INCREMENTAL_E,
                            testStatusPtr, addrPtr, readValPtr, writeValPtr);
}

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
)
{

    /* check input parameter */
    if (ifChannel >= CPSS_CHANNEL_LAST_E || regType > CPSS_DIAG_PP_REG_PCI_CFG_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(ifChannel)
    {
        case CPSS_CHANNEL_TWSI_E:
            if (regType == CPSS_DIAG_PP_REG_PCI_CFG_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            return regTwsiWrite ((GT_U32)baseAddr, offset, data, doByteSwap);
        case CPSS_CHANNEL_SMI_E:
            if (regType == CPSS_DIAG_PP_REG_PCI_CFG_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            return regSmiWrite ((GT_U32)baseAddr, offset, data, doByteSwap);
        case CPSS_CHANNEL_PEX_MBUS_E:
            if (regType == CPSS_DIAG_PP_REG_PCI_CFG_E)
            {
                return regPciWrite (baseAddr, regType, offset, data, doByteSwap);
            }
            /* do 8 regions write */
            return regPex8RegionsWrite (baseAddr, offset, data, doByteSwap);
        default:/*pci/pex/mbas*/
            return regPciWrite (baseAddr, regType, offset, data, doByteSwap);
    }
}

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
)
{
    /* check input parameter */
    CPSS_NULL_PTR_CHECK_MAC(dataPtr);
    if (ifChannel >= CPSS_CHANNEL_LAST_E || regType > CPSS_DIAG_PP_REG_PCI_CFG_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(ifChannel)
    {
        case CPSS_CHANNEL_TWSI_E:
            if (regType == CPSS_DIAG_PP_REG_PCI_CFG_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            return regTwsiRead ((GT_U32)baseAddr, offset, dataPtr, doByteSwap);
        case CPSS_CHANNEL_SMI_E:
            if (regType == CPSS_DIAG_PP_REG_PCI_CFG_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            return regSmiRead ((GT_U32)baseAddr, offset, dataPtr, doByteSwap);
        case CPSS_CHANNEL_PEX_MBUS_E:
            if (regType == CPSS_DIAG_PP_REG_PCI_CFG_E)
            {
                return regPciRead (baseAddr, regType, offset, dataPtr, doByteSwap);
            }
            /* do 8 regions read */
            return regPex8RegionsRead (baseAddr, offset, dataPtr, doByteSwap);
        default:/*pci/pex*/
            return regPciRead (baseAddr, regType, offset, dataPtr, doByteSwap);
    }


}

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
)
{
    GT_STATUS status;
    GT_U32 value;           /* value to write into the register */
    GT_U32 completion;

    /* check input parameters */
    if (ifChannel >= CPSS_CHANNEL_LAST_E || phyAddr >= BIT_5 ||
        offset >= BIT_5 || data >= BIT_16)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    value = (data) | (((phyAddr) & 0x1F) << 16) | (((offset) & 0x1F) << 21);

    /* write value */
    status = prvCpssDiagRegWrite (baseAddr, ifChannel,
                                  CPSS_DIAG_PP_REG_INTERNAL_E, smiRegOffset,
                                  value, doByteSwap);
    if (status != GT_OK)
    {
        return status;
    }

    /* check if write operation has finished */
    do
    {
        status = prvCpssDiagRegRead (baseAddr, ifChannel,
                                     CPSS_DIAG_PP_REG_INTERNAL_E, smiRegOffset,
                                     &value, doByteSwap);
        if (status != GT_OK)
        {
            return status;
        }
        completion = PRV_DIAG_CALC_PHY_ACCESS_COMPLETION_MAC(value);
    }while (completion);

    return GT_OK;
}

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
)
{
    GT_STATUS status;
    GT_U32 value;           /* value to read from the register */
    GT_U32 completion;

    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    /* check input parameters */
    if (ifChannel >= CPSS_CHANNEL_LAST_E || phyAddr >= BIT_5 ||
        offset >= BIT_5)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    value = (((phyAddr) & 0x1F) << 16) | (((offset) & 0x1F) << 21) | (1 << 26);

    status = prvCpssDiagRegWrite (baseAddr, ifChannel,
                                  CPSS_DIAG_PP_REG_INTERNAL_E, smiRegOffset,
                                  value, doByteSwap);
    if (status != GT_OK)
    {
        return status;
    }

    /* check if read operation has finished */
    do
    {
        status = prvCpssDiagRegRead (baseAddr, ifChannel,
                                     CPSS_DIAG_PP_REG_INTERNAL_E, smiRegOffset,
                                     &value, doByteSwap);
        if (status != GT_OK)
        {
            return status;
        }
        completion = PRV_DIAG_CALC_PHY_ACCESS_COMPLETION_MAC(value);
    }while (!completion);

    *dataPtr = (GT_U16)((value) & 0xFFFF);
    return GT_OK;
}

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
)
{
    return regsInfoSearch    (devNum,
                           portGroupId,
                           PRV_CPSS_DIAG_REGS_NUM_ONLY_SEARCH_E,
                           regsListPtr,
                           regsListSize,
                           offset,
                           0,
                           0,
                           checkRegExistFuncPtr,
                           regsNumPtr,
                           NULL);
}

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
)
{
    return regsInfoSearch (devNum,
                           CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                           PRV_CPSS_DIAG_REGS_NUM_ONLY_SEARCH_E,
                           regsListPtr,
                           regsListSize,
                           offset,
                           unitIdAddrPattern,
                           unitIdAddrMask,
                           NULL,
                           regsNumPtr,
                           NULL);
}

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
)
{
    return prvCpssDiagUnitRegsDataGet (devNum,
                                       portGroupId,
                                       regsListPtr,
                                       regsListSize,
                                       offset,
                                       0,
                                       0,
                                       checkRegExistFuncPtr,
                                       regsNumPtr,
                                       regAddrPtr,
                                       regDataPtr);
}


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
)
{
    GT_U32 ppRegsNum;     /* number of registers in use on the PP */

    /* Get number of registers in use */
    regsInfoSearch (devNum,
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                    PRV_CPSS_DIAG_REGS_NUM_ONLY_SEARCH_E,
                    regsListPtr,
                    regsListSize,
                    offset,
                    0,
                    0,
                    checkRegExistFuncPtr,
                    &ppRegsNum,
                    NULL);

    if (*regsNumPtr > ppRegsNum)
    {
        /* regsNumPtr is too big */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Get registers addresses */
    regsInfoSearch (devNum,
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                    PRV_CPSS_DIAG_REGS_ADDR_SEARCH_E,
                    regsListPtr,
                    regsListSize,
                    offset,
                    0,
                    0,
                    checkRegExistFuncPtr,
                    regsNumPtr,
                    regAddrPtr);

    /* Get registers data */
    return regsInfoSearch (devNum,
                           CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                           PRV_CPSS_DIAG_REGS_DATA_RESET_AND_INIT_CTRL_SEARCH_E,
                           regsListPtr,
                           regsListSize,
                           offset,
                           0,
                           0,
                           checkRegExistFuncPtr,
                           regsNumPtr,
                           regDataPtr);
}

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
)
{
    GT_U32 ppRegsNum;     /* number of registers in use on the PP */

    /* Get number of registers in use */
    regsInfoSearch (devNum,
                    portGroupId,
                    PRV_CPSS_DIAG_REGS_NUM_ONLY_SEARCH_E,
                    regsListPtr,
                    regsListSize,
                    offset,
                    unitIdAddrPattern,
                    unitIdAddrMask,
                    checkRegExistFuncPtr,
                    &ppRegsNum,
                    NULL);

    if (*regsNumPtr > ppRegsNum)
    {
        /* regsNumPtr is too big */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Get registers addresses */
    regsInfoSearch (devNum,
                    portGroupId,
                    PRV_CPSS_DIAG_REGS_ADDR_SEARCH_E,
                    regsListPtr,
                    regsListSize,
                    offset,
                    unitIdAddrPattern,
                    unitIdAddrMask,
                    checkRegExistFuncPtr,
                    regsNumPtr,
                    regAddrPtr);

    /* Get registers data */
    return regsInfoSearch (devNum,
                           portGroupId,
                           PRV_CPSS_DIAG_REGS_DATA_SEARCH_E,
                           regsListPtr,
                           regsListSize,
                           offset,
                           unitIdAddrPattern,
                           unitIdAddrMask,
                           checkRegExistFuncPtr,
                           regsNumPtr,
                           regDataPtr);
}

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
)
{
    GT_U32      originalVal;        /* register original value  */
    GT_U32      portGroupId;        /* port group id            */
    GT_STATUS   status;             /* return code              */
    GT_STATUS   status1;             /* return code              */

    /* loop on all port groups */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {

        /* Save original register value */
        status = prvCpssHwPpPortGroupReadRegister (devNum, portGroupId,
                                                      regAddr, &originalVal);
        if (status != GT_OK)
        {
            return status;
        }

        switch (profile)
        {
            case CPSS_DIAG_TEST_RANDOM_E:
                status = testRegisterRandom (devNum, portGroupId, regAddr, regMask,
                                             testStatusPtr, readValPtr,
                                             writeValPtr);
                break;

            case CPSS_DIAG_TEST_INCREMENTAL_E:
                status = testRegisterIncremental (devNum, portGroupId, regAddr,
                                                  regMask,testStatusPtr,
                                                  readValPtr, writeValPtr);
                break;

            case CPSS_DIAG_TEST_BIT_TOGGLE_E:
                status = testRegisterToggle (devNum, portGroupId, regAddr,
                                             regMask, testStatusPtr,
                                             readValPtr, writeValPtr);
                break;

            case CPSS_DIAG_TEST_AA_55_E:
                status = testRegisterAA55 (devNum, portGroupId, regAddr,
                                           regMask, testStatusPtr,
                                           readValPtr, writeValPtr);
                break;

            default:
                status = GT_BAD_PARAM;
        }

        /* Restore original register value */
        if (status != GT_OK)
        {
            status1 = prvCpssHwPpPortGroupWriteRegister (devNum, portGroupId, regAddr,
                                                            originalVal);
            if(status1 != GT_OK)
                return status1;
            else
                return status;
        }

        status = prvCpssHwPpPortGroupWriteRegister (devNum, portGroupId, regAddr,
                                                       originalVal);
        if (status != GT_OK)
        {
            return status;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

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
)
{
    GT_U32 i;
    GT_STATUS status;

    for (i = 0; i < regsArraySize; i++)
    {
        *badRegPtr = regsPtr[i];

        /* AA_55 test */
        status = prvCpssDiagRegTest (devNum, regsPtr[i], 0xFFFFFFFF,
                                     CPSS_DIAG_TEST_AA_55_E, testStatusPtr,
                                     readValPtr, writeValPtr);
        PRV_CPSS_DIAG_CHECK_TEST_RESULT_MAC(status, *testStatusPtr);

        /* Random test */
        status = prvCpssDiagRegTest (devNum, regsPtr[i], 0xFFFFFFFF,
                                     CPSS_DIAG_TEST_RANDOM_E, testStatusPtr,
                                     readValPtr, writeValPtr);
        PRV_CPSS_DIAG_CHECK_TEST_RESULT_MAC(status, *testStatusPtr);

        /* Bits toggle test */
        status = prvCpssDiagRegTest (devNum, regsPtr[i], 0xFFFFFFFF,
                                     CPSS_DIAG_TEST_BIT_TOGGLE_E, testStatusPtr,
                                     readValPtr, writeValPtr);
        PRV_CPSS_DIAG_CHECK_TEST_RESULT_MAC(status, *testStatusPtr);

        /* incremental test */
        status = prvCpssDiagRegTest (devNum, regsPtr[i], 0xFFFFFFFF,
                                     CPSS_DIAG_TEST_INCREMENTAL_E,
                                     testStatusPtr, readValPtr, writeValPtr);
        PRV_CPSS_DIAG_CHECK_TEST_RESULT_MAC(status, *testStatusPtr);
    }
    return GT_OK;
}

/***************************** Static Functions ******************************/

/*******************************************************************************
* setWriteBuffer
*
* DESCRIPTION:
*       set all the buffer with background.
*       fill position "pos" with word.
*       writeBuffer is in bytes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       bufferSize    - number of words to fill into the buffer.
*       background    - data to fill the memory with
*
* OUTPUTS:
*       writeBufferPtr  - the buffer to write into.
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void setWriteBuffer
(
    IN  GT_U32  bufferSize,
    IN  GT_U32  background,
    OUT GT_U32  *writeBufferPtr
)
{
    GT_U32 bufferPos=0;

    for (bufferPos=0 ;bufferPos< (bufferSize/4) ;bufferPos++)
    {
        writeBufferPtr[bufferPos] = background;
    }
}

/*******************************************************************************
* testAnyMemToggle
*
* DESCRIPTION:
*       This routine preforms toggle memory check.
*       it scane every bit individualy.
*       chaging every bit and checking that none of the other bit has changed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId       - the port group Id , to support multi-port-group devices
*                           that need to access specific port group
*       memBase           - memory pointer to start the test from
*       size              - the size of memory to check (in bytes)
*       specialRamFuncs   - Special RAM treatment functions
*
* OUTPUTS:
*       testStatusPtr     - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr           - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr        - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr       - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       This function depends on init the "memDiagBurstSize" by
*       prvCpssDiagSetBurstSize().
*
*******************************************************************************/
static GT_STATUS testAnyMemToggle
(
    IN  GT_U8                                             devNum,
    IN  GT_U32                                             portGroupId,
    IN  GT_U32                                            memBase,
    IN  GT_U32                                            size,
    IN  PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC      specialRamFuncs,
    OUT GT_BOOL                                           *testStatusPtr,
    OUT GT_U32                                            *addrPtr,
    OUT GT_U32                                            *readValPtr,
    OUT GT_U32                                            *writeValPtr
)
{
    GT_U32    i,j,k;               /* loops                               */
    GT_U32    phase;               /* test phase                          */
    GT_STATUS ret ;
    GT_32     retCmp;               /* ret from memCmp        */
    GT_U32    background;           /* the RAM background     */
    GT_U32    currentWord;          /* current written word   */
    GT_U32    currentAddr = 0;      /* current tested address */
    GT_U32    currentAddrScan ;     /*                        */
    GT_U32    currentAddrVerify ;   /*                        */
    GT_U32    currentAddrOverride ; /*                        */
    GT_U32    numberOfBitsInBurst ;
    GT_U32    bitNumberInWord ;
    GT_U32    wordOffsetInBurst ;
    GT_U32    wordMask ;
    GT_U32    currentToggle ;
    GT_U32    saveMaskPos;
    GT_U32    maskArrayPos;         /* current mask number to use */

    numberOfBitsInBurst =  memDiagBurstSize * 8 ;
    saveMaskPos = 0;
    *testStatusPtr = GT_TRUE;

    /*
    fill all DRAM with 0x00000000,
    for each bit, set bit to '1' verify all other bits are '0' and restore.
    fill all DRAM with 0xFFFFFFFF
    for each bit, set bit to '0' verify all other bits are '1' and restore.
    */
    DBG_INFO(("Toggle test\n"));

    /* Phase 0 - set 0x00000000 and Toggle '1' */
    /* Phase 1 - set 0xFFFFFFFF and Toggle '0' */
    for(phase = 0;phase < 2;phase++)
    {
        background = (phase == 0) ? 0x00000000:0xFFFFFFFF;
        /* reset the tested area */
        setWriteBuffer(memDiagBurstSize ,background, backgroundBuffer);
        for(i = 0; i < size; i += memDiagBurstSize)
        {
            currentAddr = CALC_MEM_ADDR_MAC(memBase, i);
            /* prvCpssDiagHwPpRamWrite get params in words */
            ret = prvCpssDiagHwPpRamWrite (devNum , portGroupId, currentAddr,
                                           memDiagBurstSize/4, specialRamFuncs,
                                           backgroundBuffer);
            if(ret != GT_OK)
            {
                DBG_INFO(("Failed on write.\n"));
                return ret;
            }
        }
        /* start toggling */
        for(i = 0; i < size; i += memDiagBurstSize)
        {
            currentAddrScan = CALC_MEM_ADDR_MAC(memBase, i);

            /* scan all word bits */
            for(j = 0; j < numberOfBitsInBurst; j++)
            {
                bitNumberInWord   = j % 32 ;
                wordOffsetInBurst = j / 32 ;
                currentToggle = (1 << bitNumberInWord);
                currentWord = (phase == 0) ? (currentToggle):
                                            ~(currentToggle);
                /*as Burst as Mask are aligned to table entries */
                maskArrayPos = wordOffsetInBurst;
                wordMask = memMaskArray[maskArrayPos];
                /* do the check only for unmasked bits */
                if((currentToggle & wordMask) != 0x0)
                {
                    /* reset the array with background which work with words */
                    setWriteBuffer(memDiagBurstSize,background,writeBuffer);
                    /* set the checked bit */
                    writeBuffer[wordOffsetInBurst] = currentWord ;
                    ret = prvCpssDiagHwPpRamWrite (devNum, portGroupId,
                                                   currentAddrScan,
                                                   (memDiagBurstSize/4),
                                                   specialRamFuncs,
                                                   writeBuffer);
                    if(ret != GT_OK)
                    {
                        DBG_INFO(("Failed on write.\n"));
                        return ret;
                    }
                    /* verify all other address are 0x00000000/0xffffffff */
                    for(k = 0; k < size; k += memDiagBurstSize)
                    {
                        maskArrayPos = saveMaskPos;
                        currentAddrVerify = CALC_MEM_ADDR_MAC(memBase, k);
                        ret = prvCpssDiagHwPpRamRead(devNum, portGroupId,
                                                     currentAddrVerify,
                                                     (memDiagBurstSize/4),
                                                     specialRamFuncs,
                                                     readBuffer);
                        if(ret != GT_OK)
                        {
                            DBG_INFO(("Failed on read.\n"));
                            return ret;
                        }

                        retCmp = prvCpssDiagHwPpRamMemoryCompare(
                                   writeBuffer
                                  ,readBuffer
                                  ,memDiagBurstSize
                                  ,&maskArrayPos
                                  ,specialRamFuncs) ;
                        if(/* its not the same burst */
                           (i != k && retCmp == GT_TRUE) ||
                           /* its the same burst     */
                           (i == k && retCmp == GT_FALSE))
                        {

                            *writeValPtr = writeBuffer[0];
                            *readValPtr = readBuffer[0];
                            *addrPtr = currentAddr;
                            *testStatusPtr = GT_FALSE;
                            DBG_INFO(("Test failed: Addr 0x%x, Val - 0x%x.\n"
                                ,currentAddrVerify
                                ,readBuffer[0]) );
                            return GT_OK;
                        }
                    }
                }
            }
            /* restore current address to zero and move to the next addr */
            currentAddrOverride = CALC_MEM_ADDR_MAC(memBase, i);
            /* reset the tested burst */
            ret = prvCpssDiagHwPpRamWrite (devNum, portGroupId,
                                           currentAddrOverride,
                                           (memDiagBurstSize/4),
                                           specialRamFuncs,
                                           backgroundBuffer);
            if(ret != GT_OK)
            {
                DBG_INFO(("Failed on write.\n"));
                return ret;
            }
        }
    }
    return GT_OK;
}

/*******************************************************************************
* testAnyMemIncremental
*
* DESCRIPTION:
*       This routine preforms Incremental memory check.
*       1st it fill the all memory with incremental numbers.
*       2nd it scane all the memory hopping to find the number we just wrote.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId       - the port group Id , to support multi-port-group devices
*                           that need to access specific port group
*       memBase           - memory pointer to start the test from
*       size              - the size of memory to check (in bytes)
*       specialRamFuncs   - Special RAM treatment functions
*
* OUTPUTS:
*       testStatusPtr     - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr           - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr        - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr       - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       This function depends on init the "memDiagBurstSize" by
*       prvCpssDiagSetBurstSize().
*
*******************************************************************************/
static GT_STATUS testAnyMemIncremental
(
    IN  GT_U8                                             devNum,
    IN GT_U32                                             portGroupId,
    IN  GT_U32                                            memBase,
    IN  GT_U32                                            size,
    IN  PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC      specialRamFuncs,
    OUT GT_BOOL                                           *testStatusPtr,
    OUT GT_U32                                            *addrPtr,
    OUT GT_U32                                            *readValPtr,
    OUT GT_U32                                            *writeValPtr
)
{
    GT_U32    i;                   /* loops                               */
    GT_STATUS ret ;
    GT_U32    currentAddr = 0;     /* current tested address              */
    GT_U32    incrementVal ;
    GT_U32    incrementLoop ;
    GT_U32    maskArrayPos;

    maskArrayPos = 0;
    *testStatusPtr = GT_TRUE;

    DBG_INFO(("Incremental test\n"));
    /* fill all the memory with incremental value */
    /* ========================================== */
    incrementVal = 0;
    for(i = 0; i < size; i += memDiagBurstSize)
    {
        /* fill the backgroundBuffer with incremental value */
        for(incrementLoop = 0 ;
            incrementLoop < (memDiagBurstSize/4) ;
            ++incrementLoop)
        {
            backgroundBuffer[incrementLoop] = incrementVal ;
            ++incrementVal ;
        }

        currentAddr = CALC_MEM_ADDR_MAC(memBase, i);

        /* prvCpssDiagHwPpRamWrite params in words */
        ret = prvCpssDiagHwPpRamWrite (devNum, portGroupId, currentAddr,
                                       (memDiagBurstSize/4), specialRamFuncs,
                                       backgroundBuffer);
        if(ret != GT_OK)
        {
            DBG_INFO(("Failed on write.\n"));
            return ret;
        }
    }
    /* check the all memory for the inserted values */
    /* ============================================ */
    incrementVal = 0;
    for(i = 0; i < size; i += memDiagBurstSize)
    {
        /* fill the backgroundBuffer with incremental value */
        for(incrementLoop = 0 ;
            incrementLoop < (memDiagBurstSize/4) ;
            ++incrementLoop)
        {
            backgroundBuffer[incrementLoop] = incrementVal ;
            ++incrementVal ;
        }
        /* The following IF operator has been removed as erroneous:
           it eliminates from check the Current memory burst when
           incrementVal is used masked bits for the first word
           of the NEXT memory burst. Moreover this method cannot
           be applied for different masks declared in Mask Array  */
        /**********************************************************/
        /* do the check only for masked bits */
        /* if((incrementVal & ~wordMask) == 0x0) */
        /**********************************************************/

        /* compare the backgroundBuffer with the Memory Burst */
        {
            currentAddr = CALC_MEM_ADDR_MAC(memBase, i);

            ret = prvCpssDiagHwPpRamRead(devNum, portGroupId, currentAddr,
                                         (memDiagBurstSize/4), specialRamFuncs,
                                         readBuffer);
            if(ret != GT_OK)
            {
                DBG_INFO(("Failed on read.\n"));
                return ret;
            }

            ret = prvCpssDiagHwPpRamMemoryCompare(
                     backgroundBuffer
                    ,readBuffer
                    ,memDiagBurstSize
                    ,&maskArrayPos
                    ,specialRamFuncs) ;
            if(ret == GT_FALSE) /* its not the same data */
            {
                *testStatusPtr = GT_FALSE;
                *writeValPtr = backgroundBuffer[0];
                *readValPtr = readBuffer[0];
                *addrPtr = currentAddr;
                DBG_INFO(("Test failed: Addr 0x%x, Val - 0x%x.\n"
                  ,currentAddr
                  ,readBuffer[0]) );
                return GT_OK;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* testAnyMem_AA_55
*
* DESCRIPTION:
*       This routine preforms check the memory with Const Var.
*       1st fill all the memry with a patern.
*       2nd it check that all the memry has that patern.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId       - the port group Id , to support multi-port-group devices
*                           that need to access specific port group
*       memBase           - memory pointer to start the test from
*       size              - the size of memory to check (in bytes)
*       specialRamFuncs   - Special RAM treatment functions
*
* OUTPUTS:
*       testStatusPtr     - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr           - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr        - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr       - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       This function depends on init the "memDiagBurstSize" by
*       prvCpssDiagSetBurstSize().
*
*******************************************************************************/
static GT_STATUS testAnyMem_AA_55
(
    IN  GT_U8                                             devNum,
    IN GT_U32                                             portGroupId,
    IN  GT_U32                                            memBase,
    IN  GT_U32                                            size,
    IN  PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC      specialRamFuncs,
    OUT GT_BOOL                                           *testStatusPtr,
    OUT GT_U32                                            *addrPtr,
    OUT GT_U32                                            *readValPtr,
    OUT GT_U32                                            *writeValPtr
)
{
    GT_U32    i;                   /* loops                               */
    GT_U32    phase;               /* test phase                          */
    GT_STATUS ret ;
    GT_32     retCmp;              /* ret from memCmp                     */
    GT_U32    background;          /* the RAM background                  */
    GT_U32    currentAddr = 0;     /* current tested address              */
    GT_U32    maskArrayPos;
    GT_U32    saveMaskPos;          /* to save the first pos in maskArray */

    saveMaskPos = 0;
    *testStatusPtr = GT_TRUE;

    /* 0xAAAAAAAA/0x55555555 test */
    /* ========================== */
    DBG_INFO(("AA-55 test\n"));
    /* set test bacground */
    background = 0x55555555;

    /* start test phase */
    for(phase = 0;phase < 4;phase++)
    {
        maskArrayPos = saveMaskPos;
        setWriteBuffer(memDiagBurstSize,background,backgroundBuffer);
        for(i=0; i < size; i+= memDiagBurstSize)
        {
            currentAddr = CALC_MEM_ADDR_MAC(memBase, i);

            ret = prvCpssDiagHwPpRamWrite (devNum, portGroupId, currentAddr,
                                           (memDiagBurstSize/4),
                                           specialRamFuncs,
                                           backgroundBuffer);
            if(ret != GT_OK)
            {
                DBG_INFO(("Failed on write.\n"));
                return ret;
            }
        }

        /* verify background and write ~background */
        for(i=0; i < size; i += memDiagBurstSize)
        {
            currentAddr = CALC_MEM_ADDR_MAC(memBase, i);

            ret = prvCpssDiagHwPpRamRead(devNum, portGroupId, currentAddr,
                                         (memDiagBurstSize/4), specialRamFuncs,
                                         readBuffer);
            if(ret != GT_OK)
            {
                DBG_INFO(("Failed on read.\n"));
                return ret;
            }

            retCmp=prvCpssDiagHwPpRamMemoryCompare(
                      backgroundBuffer
                     ,readBuffer
                     ,memDiagBurstSize
                     ,&maskArrayPos
                     ,specialRamFuncs) ;

            if(retCmp == GT_FALSE) /* not the same */
            {
                *testStatusPtr = GT_FALSE;
                *writeValPtr = backgroundBuffer[0];
                *readValPtr = readBuffer[0];
                *addrPtr = currentAddr;
                DBG_INFO(("Test failed: Addr 0x%x, Val - 0x%x.\n"
                  ,currentAddr
                  ,readBuffer[0]) );
                return GT_OK;
            }
        }
        /* change back ground polarity */
        background = ~background;
    }

    return GT_OK;
}

/*******************************************************************************
* testAnyMemRandom
*
* DESCRIPTION:
*       This routine preforms check the memory with random values.
*       1st fill all the memry with a patern.
*       2nd it check that all the memry has that patern.
*       Note that the pattern repeats itself every memDiagBurstSize.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId       - the port group Id , to support multi-port-group devices
*                           that need to access specific port group
*       memBase           - memory pointer to start the test from
*       size              - the size of memory to check (in bytes)
*       specialRamFuncs   - Special RAM treatment functions
*
* OUTPUTS:
*       testStatusPtr     - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr           - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr        - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr       - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       This function depends on init the "memDiagBurstSize" by
*       prvCpssDiagSetBurstSize().
*
*******************************************************************************/
static GT_STATUS testAnyMemRandom
(
    IN  GT_U8                                             devNum,
    IN GT_U32                                             portGroupId,
    IN  GT_U32                                            memBase,
    IN  GT_U32                                            size,
    IN  PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC      specialRamFuncs,
    OUT GT_BOOL                                           *testStatusPtr,
    OUT GT_U32                                            *addrPtr,
    OUT GT_U32                                            *readValPtr,
    OUT GT_U32                                            *writeValPtr
)
{
    GT_U32    i;                   /* loops                               */
    GT_STATUS ret ;
    GT_U32    currentAddr = 0;     /* current tested address              */
    GT_U32    incrementLoop ;
    GT_U32    maskArrayPos;

    maskArrayPos = 0;
    *testStatusPtr = GT_TRUE;

    DBG_INFO(("Random test\n"));

    /* fill the backgroundBuffer with random values */
    for(incrementLoop = 0 ;
        incrementLoop < (memDiagBurstSize/4) ;
        ++incrementLoop)
    {
        backgroundBuffer[incrementLoop] = cpssOsRand ();
    }

    /* fill all the memory with random value */
    /* ===================================== */
    for(i = 0; i < size; i += memDiagBurstSize)
    {
        currentAddr = CALC_MEM_ADDR_MAC(memBase, i);

        /* prvCpssDiagHwPpRamWrite params in words */
        ret = prvCpssDiagHwPpRamWrite (devNum, portGroupId, currentAddr,
                                       (memDiagBurstSize/4), specialRamFuncs,
                                       backgroundBuffer);
        if(ret != GT_OK)
        {
            DBG_INFO(("Failed on write.\n"));
            return ret;
        }
    }
    /* check the all memory for the inserted values */
    /* ============================================ */
    for(i = 0; i < size; i += memDiagBurstSize)
    {
        /* compare the backgroundBuffer with the Memory Burst */
        {
            currentAddr = CALC_MEM_ADDR_MAC(memBase, i);

            ret = prvCpssDiagHwPpRamRead(devNum, portGroupId, currentAddr,
                                         (memDiagBurstSize/4), specialRamFuncs,
                                         readBuffer);
            if(ret != GT_OK)
            {
                DBG_INFO(("Failed on read.\n"));
                return ret;
            }

            ret = prvCpssDiagHwPpRamMemoryCompare(
                     backgroundBuffer
                    ,readBuffer
                    ,memDiagBurstSize
                    ,&maskArrayPos
                    ,specialRamFuncs) ;
            if(ret == GT_FALSE) /* its not the same data */
            {
                *testStatusPtr = GT_FALSE;
                *writeValPtr = backgroundBuffer[0];
                *readValPtr = readBuffer[0];
                *addrPtr = currentAddr;
                DBG_INFO(("Test failed: Addr 0x%x, Val - 0x%x.\n"
                  ,currentAddr
                  ,readBuffer[0]) );
                return GT_OK;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* testMemCmp
*
* DESCRIPTION:
*       Compare 'size' characters from the object pointed to by 'str1Ptr' to
*       the object pointed to by 'str2Ptr'.
*       checking words.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       str1Ptr - first memory area
*       str2Ptr - second memory area
*       size - size of memory to compare
*       maskArrIndxPtr - pointer to mask index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_FALSE - if str1Ptr is different from str2Ptr
*       GT_TRUE  - if str1Ptr is equal to str2Ptr
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_BOOL testMemCmp
(
    IN GT_U32   *str1Ptr,
    IN GT_U32   *str2Ptr,
    IN GT_U32   size,
    IN GT_U32   *maskArrIndxPtr
)
{
    GT_U32 loop ;
    GT_UINTPTR str1_Ptr ;
    GT_UINTPTR str2_Ptr ;
    GT_U32 str1Mask;
    GT_U32 str2Mask;

    if((size % 4) != 0) /* check words */
    {
        return GT_FALSE;
    }

    for(loop = 0 ;loop < size ;loop+=4)
    {
        str1_Ptr  = (GT_UINTPTR)(str1Ptr) + loop ;
        str2_Ptr  = (GT_UINTPTR)(str2Ptr) + loop ;
        str1Mask = ((GT_U32)(*((GT_U32 *)str1_Ptr)) &
                      (memMaskArray[*maskArrIndxPtr]));
        str2Mask = ((GT_U32)(*((GT_U32 *)str2_Ptr)) &
                      (memMaskArray[*maskArrIndxPtr]));
        *maskArrIndxPtr = ((*maskArrIndxPtr) + 1) % memMaskArSize ;
        if(str1Mask != str2Mask)
        {
            DBG_INFO(("Failed on word 0x%x: str1Mask = 0x%x, str2Mask = 0x%x.\n",
                     (loop / 4), str1Mask, str2Mask));
            return GT_FALSE;
        }
    }
    return GT_TRUE ;
}

/*******************************************************************************
* testRegisterRandom
*
* DESCRIPTION:
*       Tests the device specific register, by writing a random pattern to it
*       and verifying the value.
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
*       regAddr       - Register to test
*       regMask       - Register mask. The test verifies only the non-masked
*                       bits.
*
* OUTPUTS:
*       testStatusPtr - GT_TRUE if the test succeeded or GT_FALSE for failure
*       readValPtr    - The value read from the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       writeValPtr   - The value written to the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       After the register is tested, the original value prior to the test is
*       restored.
*
*******************************************************************************/
static GT_STATUS testRegisterRandom
(
    IN GT_U8                     devNum,
    IN GT_U32                    portGroupId,
    IN GT_U32                    regAddr,
    IN GT_U32                    regMask,
    OUT GT_BOOL                  *testStatusPtr,
    OUT GT_U32                   *readValPtr,
    OUT GT_U32                   *writeValPtr
)
{
    GT_U32 writeValue = cpssOsRand();
    GT_U32 readValue;
    GT_STATUS status;

    /* Start the test - Write value into the register */
    status = prvCpssHwPpPortGroupWriteRegBitMask (devNum, portGroupId,
                                                     regAddr, regMask, writeValue);
    if (status != GT_OK)
    {
        return status;
    }
    /* Read the register value */
    status = prvCpssHwPpPortGroupReadRegBitMask (devNum, portGroupId,
                                                    regAddr, regMask, &readValue);
    if (status != GT_OK)
    {
        return status;
    }

    /* Check if the test passed */
    if ((writeValue & regMask) == (readValue & regMask))
    {
        *testStatusPtr = GT_TRUE;
    }
    else
    {
        *testStatusPtr = GT_FALSE;
        *readValPtr = readValue;
        *writeValPtr = writeValue;
    }

    return GT_OK;
}

/*******************************************************************************
* testRegisterIncremental
*
* DESCRIPTION:
*       Tests the device specific register, by writing an incremental pattern to
*       it and verifying the value.
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
*       regAddr       - Register to test
*       regMask       - Register mask. The test verifies only the non-masked
*                       bits.
*
* OUTPUTS:
*       testStatusPtr - GT_TRUE if the test succeeded or GT_FALSE for failure
*       readValPtr    - The value read from the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       writeValPtr   - The value written to the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       After the register is tested, the original value prior to the test is
*       restored.
*
*******************************************************************************/
static GT_STATUS testRegisterIncremental
(
    IN GT_U8                     devNum,
    IN GT_U32                    portGroupId,
    IN GT_U32                    regAddr,
    IN GT_U32                    regMask,
    OUT GT_BOOL                  *testStatusPtr,
    OUT GT_U32                   *readValPtr,
    OUT GT_U32                   *writeValPtr
)
{
    static GT_U32 writeValue;
    GT_U32 readValue;
    GT_STATUS status;

    /* increment writeValue */
    writeValue = (writeValue + 1) % (0xFFFFFFFF);

    /* Start the test - Write value into the register */
    status = prvCpssHwPpPortGroupWriteRegBitMask (devNum, portGroupId,
                                                     regAddr, regMask, writeValue);
    if (status != GT_OK)
    {
        return status;
    }
    /* Read the register value */
    status = prvCpssHwPpPortGroupReadRegBitMask (devNum, portGroupId,
                                                    regAddr, regMask, &readValue);
    if (status != GT_OK)
    {
        return status;
    }

    /* Check if the test passed */
    if ((writeValue & regMask) == (readValue & regMask))
    {
        *testStatusPtr = GT_TRUE;
    }
    else
    {
        *testStatusPtr = GT_FALSE;
        *readValPtr = readValue;
        *writeValPtr = writeValue;
    }

    return GT_OK;
}

/*******************************************************************************
* testRegisterToggle
*
* DESCRIPTION:
*       Tests the device specific register, by toggling each of the register
*       bits and verifying the value.
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
*       regAddr       - Register to test
*       regMask       - Register mask. The test verifies only the non-masked
*                       bits.
*
* OUTPUTS:
*       testStatusPtr - GT_TRUE if the test succeeded or GT_FALSE for failure
*       readValPtr    - The value read from the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       writeValPtr   - The value written to the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       After the register is tested, the original value prior to the test is
*       restored.
*
*******************************************************************************/
static GT_STATUS testRegisterToggle
(
    IN GT_U8                     devNum,
    IN GT_U32                    portGroupId,
    IN GT_U32                    regAddr,
    IN GT_U32                    regMask,
    OUT GT_BOOL                  *testStatusPtr,
    OUT GT_U32                   *readValPtr,
    OUT GT_U32                   *writeValPtr
)
{
    GT_U32 testedBit, writeValue, readValue;
    GT_STATUS status;

    *testStatusPtr = GT_TRUE;

    for (testedBit = 0; testedBit < 32; testedBit++)
    {
        /* Start testing the bit - Write value into the register */
        writeValue = 1 << testedBit;
        status = prvCpssHwPpPortGroupWriteRegBitMask (devNum, portGroupId,
                                                         regAddr, regMask,
                                                         writeValue);
        if (status != GT_OK)
        {
            return status;
        }

        /* Read the register value */
        status = prvCpssHwPpPortGroupReadRegBitMask (devNum, portGroupId,
                                                        regAddr, regMask,
                                                        &readValue);
        if (status != GT_OK)
        {
            return status;
        }

        /* Check if the test passed */
        if ((writeValue & regMask) != (readValue & regMask))
        {
            *testStatusPtr = GT_FALSE;
            *readValPtr = readValue;
            *writeValPtr = writeValue;
            return GT_OK;
        }

        /* Repeat the test with toggle bit */
        writeValue = ~writeValue;
        status = prvCpssHwPpPortGroupWriteRegBitMask (devNum, portGroupId,
                                                         regAddr, regMask,
                                                         writeValue);
        if (status != GT_OK)
        {
            return status;
        }

        /* Read the register value */
        status = prvCpssHwPpPortGroupReadRegBitMask (devNum, portGroupId,
                                                        regAddr, regMask,
                                                        &readValue);
        if (status != GT_OK)
        {
            return status;
        }

        /* Check if the test passed */
        if ((writeValue & regMask) != (readValue & regMask))
        {
            *testStatusPtr = GT_FALSE;
            *readValPtr = readValue;
            *writeValPtr = writeValue;
            return GT_OK;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* testRegisterAA55
*
* DESCRIPTION:
*       Tests the device specific register, by writing "AAAAAAAA" and "55555555"
*       and verifying the values.
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
*       regAddr       - Register to test
*       regMask       - Register mask. The test verifies only the non-masked
*                       bits.
*
* OUTPUTS:
*       testStatusPtr - GT_TRUE if the test succeeded or GT_FALSE for failure
*       readValPtr    - The value read from the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       writeValPtr   - The value written to the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       After the register is tested, the original value prior to the test is
*       restored.
*
*******************************************************************************/
static GT_STATUS testRegisterAA55
(
    IN GT_U8                     devNum,
    IN GT_U32                    portGroupId,
    IN GT_U32                    regAddr,
    IN GT_U32                    regMask,
    OUT GT_BOOL                  *testStatusPtr,
    OUT GT_U32                   *readValPtr,
    OUT GT_U32                   *writeValPtr
)
{
    GT_U32 writeValue = 0xAAAAAAAA;
    GT_U32 readValue;
    GT_STATUS status;

    *testStatusPtr = GT_TRUE;

    /* Start the test - Write "0xAAAAAAAA" into the register */
    status = prvCpssHwPpPortGroupWriteRegBitMask (devNum, portGroupId,
                                                     regAddr, regMask,
                                                     writeValue);
    if (status != GT_OK)
    {
        return status;
    }
    /* Read the register value */
    status = prvCpssHwPpPortGroupReadRegBitMask (devNum, portGroupId,
                                                    regAddr, regMask,
                                                    &readValue);
    if (status != GT_OK)
    {
        return status;
    }

    /* Check if the test passed */
    if ((writeValue & regMask) != (readValue & regMask))
    {
        *testStatusPtr = GT_FALSE;
        *readValPtr = readValue;
        *writeValPtr = writeValue;
        return GT_OK;
    }

    /* Write "0x55555555" into the register */
    writeValue = 0x55555555;
    status = prvCpssHwPpPortGroupWriteRegBitMask (devNum, portGroupId,
                                                     regAddr, regMask,
                                                     writeValue);
    if (status != GT_OK)
    {
        return status;
    }
    /* Read the register value */
    status = prvCpssHwPpPortGroupReadRegBitMask (devNum, portGroupId,
                                                    regAddr, regMask,
                                                    &readValue);
    if (status != GT_OK)
    {
        return status;
    }

    /* Check if the test passed */
    if ((writeValue & regMask) != (readValue & regMask))
    {
        *testStatusPtr = GT_FALSE;
        *readValPtr = readValue;
        *writeValPtr = writeValue;
    }

    return GT_OK;
}

/*******************************************************************************
* regPciWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write through PCI to one of the PP PCI
*       configuration or internal registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       regType    - The register type
*       offset     - the register offset
*       regData    - data to write
*       doByteSwap - GT_TRUE:  byte swap will be done on the written data
*                    GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static GT_STATUS regPciWrite
(
    IN GT_UINTPTR                      baseAddr,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    IN GT_U32                          regData,
    IN GT_BOOL                         doByteSwap
)
{
    GT_UINTPTR  address;
    GT_U32      data;
    GT_UINTPTR  addressCompletionReg;
    GT_U32      origAddrCompletionReg;
    GT_U32      origNumRegions;

#ifdef ASIC_SIMULATION
    simulationDeviceId = scibGetDeviceId(baseAddr);
    if(simulationDeviceId == 0xFFFFFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
#endif /* ASIC_SIMULATION */

    if (regType == CPSS_DIAG_PP_REG_PCI_CFG_E)
    {
        /* write PCI register */
        address = offset;

        if(hwUsed)
        {
            address += baseAddr;
        }

        PCI_CFG_WRITE_DATA_INTO_ADDR_MAC(address , doByteSwap , regData);
    }
    else /* CPSS_DIAG_PP_REG_INTERNAL_E */
    {
        if(GT_OK ==  deviceSupportPex8Regions(baseAddr,doByteSwap))
        {
            /* set 4 regions */
            origNumRegions = pexNumRegionsSet(baseAddr,doByteSwap,4);
        }
        else
        {
            origNumRegions = 4;
        }

        if(hwUsed == 0 && gmUsed == 0)
        {
            PCI_1_WRITE_DATA_INTO_ADDR_MAC((GT_UINTPTR)offset,doByteSwap , regData);
        }
        else
        {
            /* write PP internal register */
            /* update HW Address Completion - using completion region 2 */
            addressCompletionReg = 0;

            addressCompletionReg += baseAddr;

            /* save address completion value */
            READ_DATA_FROM_ADDR_MAC(addressCompletionReg,origAddrCompletionReg);
            GT_SYNC;

            data = ((GT_U8)(offset >> 24)) << 16;
            PCI_3_WRITE_DATA_INTO_ADDR_MAC(addressCompletionReg , doByteSwap , data);
            GT_SYNC;

            /* for PP register access use region N 2 always */
            address = (DIAG_ADDR_COMPLETION_REGION_CNS << 24) | (offset & 0x00ffffff);
            address += baseAddr;
            PCI_3_WRITE_DATA_INTO_ADDR_MAC(address , doByteSwap , regData);
            GT_SYNC;

            /* restore address completion */
            PCI_3_WRITE_DATA_INTO_ADDR_MAC(addressCompletionReg,GT_FALSE , origAddrCompletionReg);
            GT_SYNC;
        }

        if(origNumRegions != 4)
        {
            /* restore regions */
            (void)pexNumRegionsSet(baseAddr,doByteSwap,origNumRegions);
        }
    }
    return GT_OK;
}

/*******************************************************************************
* regPciRead
*
* DESCRIPTION:
*       Performs single 32 bit data read through PCI from one of the PP
*       PCI configuration or internal registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       regType    - The register type
*       offset     - the register offset
*       doByteSwap - GT_TRUE:  byte swap will be done on the read data
*                    GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       regDataPtr    - read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static GT_STATUS regPciRead
(
    IN GT_UINTPTR                      baseAddr,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    OUT GT_U32                         *regDataPtr,
    IN GT_BOOL                         doByteSwap
)
{
    GT_UINTPTR  address;
    GT_U32      data;
    GT_UINTPTR  addressCompletionReg;
    GT_U32      origAddrCompletionReg;
    GT_U32      origNumRegions;

#ifdef ASIC_SIMULATION
    simulationDeviceId = scibGetDeviceId(baseAddr);
    if(simulationDeviceId == 0xFFFFFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
#endif /* ASIC_SIMULATION */

    if (regType == CPSS_DIAG_PP_REG_PCI_CFG_E)
    {
        /* write PCI register */
        address = offset;

        if(hwUsed)
        {
            address += baseAddr;
        }
        READ_CFG_DATA_FROM_ADDR_MAC(address , data);
    }
    else /* CPSS_DIAG_PP_REG_INTERNAL_E */
    {
        if(GT_OK ==  deviceSupportPex8Regions(baseAddr,doByteSwap))
        {
            /* set 4 regions */
            origNumRegions = pexNumRegionsSet(baseAddr,doByteSwap,4);
        }
        else
        {
            origNumRegions = 4;
        }

        if(hwUsed == 0 && gmUsed == 0)
        {
            READ_DATA_FROM_ADDR_MAC((GT_UINTPTR)offset , data);
        }
        else
        {
            /* write PP internal register */
            /* update HW Address Completion - using completion region 2 */
            addressCompletionReg = 0;

            addressCompletionReg += baseAddr;

            /* save address completion value */
            READ_DATA_FROM_ADDR_MAC(addressCompletionReg,origAddrCompletionReg);
            GT_SYNC;

            data = ((GT_U8)(offset >> 24)) << 16;
            PCI_3_WRITE_DATA_INTO_ADDR_MAC(addressCompletionReg , doByteSwap , data);
            GT_SYNC;

            /* for PP register access use region N 2 always */
            address = (DIAG_ADDR_COMPLETION_REGION_CNS << 24) | (offset & 0x00ffffff);
            address += baseAddr;

            READ_DATA_FROM_ADDR_MAC(address , data);

            /* restore address completion */
            PCI_3_WRITE_DATA_INTO_ADDR_MAC(addressCompletionReg,GT_FALSE , origAddrCompletionReg);
            GT_SYNC;
        }

        if(origNumRegions != 4)
        {
            /* restore regions */
            (void)pexNumRegionsSet(baseAddr,doByteSwap,origNumRegions);
        }
    }

    *regDataPtr = DIAG_BYTE_SWAP_MAC(doByteSwap, data);
    return GT_OK;
}

/* DB to hold the device types that the CPSS support */
extern const struct {
    CPSS_PP_FAMILY_TYPE_ENT     devFamily;
    GT_U8                       numOfPorts;
    const CPSS_PP_DEVICE_TYPE   *devTypeArray;
    const CPSS_PORTS_BMP_STC    *defaultPortsBmpPtr;
    const /*CORES_INFO_STC*/void *coresInfoPtr;
    const GT_U32                *activeCoresBmpPtr;
}cpssSupportedTypes[];
#define END_OF_TABLE    0xFFFFFFFF

/*******************************************************************************
* diag_getDevFamily
*
* DESCRIPTION:
*       Gets the device family from vendor Id and device Id (read from PCI bus)
*
* INPUTS:
*       deviceId - pointer to PCI/PEX device identification data.
*
* OUTPUTS:
*       devFamilyPtr - (pointer to) CPSS PP device family.
*
*
*
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS diag_getDevFamily(
    IN  GT_U16                  deviceId,
    OUT CPSS_PP_FAMILY_TYPE_ENT *devFamilyPtr
)
{
    CPSS_PP_DEVICE_TYPE deviceType = ((deviceId) << 16) | (0x11AB);
    GT_U32                  ii;
    GT_U32                  jj;
    GT_BOOL                 found = GT_FALSE;

    /* get the info about our device */
    ii = 0;
    while (cpssSupportedTypes[ii].devFamily != END_OF_TABLE)
    {
        jj = 0;
        while (cpssSupportedTypes[ii].devTypeArray[jj] != END_OF_TABLE)
        {
            if (deviceType == cpssSupportedTypes[ii].devTypeArray[jj])
            {
                found = GT_TRUE;
                break;
            }
            jj++;
        }

        if (GT_TRUE == found)
        {
            break;
        }
        ii++;
    }

    /* check if deviceType was found */
    if (GT_TRUE == found)
    {
        /* get family type from CPSS DB */
        *devFamilyPtr = cpssSupportedTypes[ii].devFamily;
    }
    else  /* device not managed by CPSS */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}


/*******************************************************************************
* deviceIsPrestera
*
* DESCRIPTION:
*       check if the device is of Prestera family .
*       the function checks register 0x50 to check that 0x11AB exists in it.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*
* OUTPUTS:
*       needSwapPtr - (pointer to) indication that need swap (achieved by read of register 0x50)
*       devFamilyPtr - (pointer to) the device family
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static GT_STATUS deviceIsPrestera
(
    IN GT_UINTPTR                      baseAddr,
    OUT GT_BOOL                        *needSwapPtr,
    OUT CPSS_PP_FAMILY_TYPE_ENT        *devFamilyPtr
)
{
    GT_U32  data;
    GT_UINTPTR  address;

    address = 0x50;

    if(hwUsed || gmUsed)
    {
        address += baseAddr;
    }

#ifdef ASIC_SIMULATION
    scibReadMemory(simulationDeviceId,address,1,&data);
#else
    READ_DATA_FROM_ADDR_MAC(address, data);
#endif

    if(data == 0x0011AB)
    {
        *needSwapPtr = GT_FALSE;
    }
    else
    if(data == 0xAB110000)/* check with swapped bytes */
    {
        *needSwapPtr = GT_TRUE;
    }
    else
    {
        /* register 0x50 not hold 0x11AB ??? */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    address = 0x4c;
    if(hwUsed || gmUsed)
    {
        address += baseAddr;
    }
#ifdef ASIC_SIMULATION
    scibReadMemory(simulationDeviceId,address,1,&data);
#else
    READ_DATA_FROM_ADDR_MAC(address, data);
#endif

    data = DIAG_BYTE_SWAP_MAC(*needSwapPtr,data);

    data >>= 4;

    return diag_getDevFamily((GT_U16)data , devFamilyPtr);
}

/* check if the device supports 8 regions .

    returns :
*       GT_OK   - on success
*       GT_FAIL - on error
*/
static GT_STATUS deviceSupportPex8Regions
(
    IN GT_UINTPTR                      baseAddr,
    IN GT_BOOL                         doByteSwap
)
{
    GT_STATUS  rc;
    GT_BOOL    autoSwapDetect = GT_FALSE;
    CPSS_PP_FAMILY_TYPE_ENT         devFamily = CPSS_PP_FAMILY_DXCH_BOBCAT2_E;

    rc = deviceIsPrestera(baseAddr , &autoSwapDetect , &devFamily);
    if(rc != GT_OK)
    {
        /* this is not prestera device ??? */
        return rc;
    }

    if(autoSwapDetect != doByteSwap)
    {
        cpssOsPrintf("autoSwapDetect[%d] != doByteSwap[%d] \n" , autoSwapDetect , doByteSwap);
    }

    switch(devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
            return GT_OK;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
}

/* check if the device supports 8 regions .

    returns :
        mode before the change:
        4 - 4 regions
        8 - 8 regions
*/
static GT_U32 pexNumRegionsSet
(
    IN GT_UINTPTR                      baseAddr,
    IN GT_BOOL                         doByteSwap,
    IN GT_U32                          numOfRegions
)
{
    GT_U32      retVal;
    GT_U32      addCompCntrlVal;/*address of 'address completion control' register */
    GT_UINTPTR  addCompCntrlReg;/*value of 'address completion control' register */

    if(hwUsed == 0 && gmUsed == 0)
    {
        return 4;
    }

    addCompCntrlReg = baseAddr + 0x140;

    /* register addCompCntrl : get 8-region mode: regAddr = 0x140, bit16  :
        0 -->
            Legacy address completion mode.
            Notice that when working on address completion legacy mode , the user works with Address_completion register (Address = 0x0).
        0x0 = 8 regions address completion mode
        0x1 = 4 regions address completion mode; (Legacy mode)
    */
    READ_DATA_FROM_ADDR_MAC(addCompCntrlReg, addCompCntrlVal);
    addCompCntrlVal = DIAG_BYTE_SWAP_MAC(doByteSwap, addCompCntrlVal);

    if(numOfRegions == 8 && (addCompCntrlVal & BIT_16))
    {
        retVal = 4;

        /* 4 regions mode ... change it to 8 regions mode ... */
        addCompCntrlVal &= (~BIT_16);/* clear bit 16 */
        PCI_1_WRITE_DATA_INTO_ADDR_MAC(addCompCntrlReg , doByteSwap , addCompCntrlVal);
        GT_SYNC;
    }
    else
    if(numOfRegions == 4 && (0 == (addCompCntrlVal & BIT_16)))
    {
        retVal = 8;
        /* 8 regions mode ... change it to 4 regions mode ... */
        addCompCntrlVal |= BIT_16;/* set bit 16 */
        PCI_1_WRITE_DATA_INTO_ADDR_MAC(addCompCntrlReg , doByteSwap , addCompCntrlVal);
        GT_SYNC;
    }
    else
    {
        /* no need to change mode */
        retVal = (addCompCntrlVal & BIT_16) ? 4 : 8;
    }

    return retVal;

}



/*******************************************************************************
* regPex8RegionsWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write through PEX/MBUS to one of the PP registers.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; ExMxPm; ExMx.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       offset     - the register offset
*       regData    - data to write
*       doByteSwap - GT_TRUE:  byte swap will be done on the written data
*                    GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static GT_STATUS regPex8RegionsWrite
(
    IN GT_UINTPTR                      baseAddr,
    IN GT_U32                          offset,
    IN GT_U32                          regData,
    IN GT_BOOL                         doByteSwap
)
{
    GT_STATUS   rc;
    GT_UINTPTR  address;
    GT_UINTPTR  AddCompRegion2Reg;/*address of 'address completion region 2' register */
    GT_U32      AddCompRegion2Val,orig_AddCompRegion2Val;/*value of 'address completion region 2' register */
    GT_U32      origNumRegions;

#ifdef ASIC_SIMULATION
    simulationDeviceId = scibGetDeviceId(baseAddr);
    if(simulationDeviceId == 0xFFFFFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
#endif /* ASIC_SIMULATION */

    rc = deviceSupportPex8Regions(baseAddr,doByteSwap);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(hwUsed == 0 && gmUsed == 0)
    {
        return regPciWrite (baseAddr, CPSS_DIAG_PP_REG_INTERNAL_E , offset, regData, doByteSwap);
    }

    /* set 8 regions */
    origNumRegions = pexNumRegionsSet(baseAddr,doByteSwap,8);

    /* the address of the 'region 2' register */
    AddCompRegion2Reg = PEX_MBUS_ADDR_COMP_REG_MAC(DIAG_ADDR_COMPLETION_REGION_CNS);
    AddCompRegion2Reg += baseAddr;

    /* save 'region 2' register value */
    READ_DATA_FROM_ADDR_MAC(AddCompRegion2Reg,orig_AddCompRegion2Val);
    GT_SYNC;

    /* set 'region 2' register value with MSBits of the 'offset' */
    AddCompRegion2Val = offset >> NOT_ADDRESS_COMPLETION_BITS_NUM_CNS;
    PCI_1_WRITE_DATA_INTO_ADDR_MAC(AddCompRegion2Reg,doByteSwap , AddCompRegion2Val);
    GT_SYNC;

    /* for PP register access use region N 2 always */
    /* now we can access the 'offset' with it's LSBits */
    address = (DIAG_ADDR_COMPLETION_REGION_CNS << NOT_ADDRESS_COMPLETION_BITS_NUM_CNS) |
              (offset & (~ ADDRESS_COMPLETION_BITS_MASK_CNS));
    address += baseAddr;

    PCI_1_WRITE_DATA_INTO_ADDR_MAC(address , doByteSwap , regData);
    GT_SYNC;

    /* restore 'region 2' register value */
    PCI_1_WRITE_DATA_INTO_ADDR_MAC(AddCompRegion2Reg,GT_FALSE , orig_AddCompRegion2Val);
    GT_SYNC;

    if(origNumRegions != 8)
    {
        /* restore 4 regions mode */
        (void) pexNumRegionsSet(baseAddr,doByteSwap,origNumRegions);
    }

    return GT_OK;
}

/*******************************************************************************
* regPex8RegionsRead
*
* DESCRIPTION:
*       Performs single 32 bit data read through PEX/MBUS from one of the PP registers.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; ExMxPm; ExMx.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*
*       offset     - the register offset
*       doByteSwap - GT_TRUE:  byte swap will be done on the read data
*                    GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       regDataPtr    - (pointer to) read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static GT_STATUS regPex8RegionsRead
(
    IN GT_UINTPTR                      baseAddr,
    IN GT_U32                          offset,
    OUT GT_U32                         *regDataPtr,
    IN GT_BOOL                         doByteSwap
)
{
    GT_STATUS   rc;
    GT_UINTPTR  address;
    GT_UINTPTR  AddCompRegion2Reg;/*address of 'address completion region 2' register */
    GT_U32      AddCompRegion2Val,orig_AddCompRegion2Val;/*value of 'address completion region 2' register */
    GT_U32      origNumRegions;
    GT_U32      regData;

#ifdef ASIC_SIMULATION
    simulationDeviceId = scibGetDeviceId(baseAddr);
    if(simulationDeviceId == 0xFFFFFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
#endif /* ASIC_SIMULATION */

    rc = deviceSupportPex8Regions(baseAddr,doByteSwap);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(hwUsed == 0 && gmUsed == 0)
    {
        return regPciRead (baseAddr, CPSS_DIAG_PP_REG_INTERNAL_E , offset, regDataPtr, doByteSwap);
    }

    /* set 8 regions */
    origNumRegions = pexNumRegionsSet(baseAddr,doByteSwap,8);

    /* the address of the 'region 2' register */
    AddCompRegion2Reg = PEX_MBUS_ADDR_COMP_REG_MAC(DIAG_ADDR_COMPLETION_REGION_CNS);
    AddCompRegion2Reg += baseAddr;

    /* save 'region 2' register value */
    READ_DATA_FROM_ADDR_MAC(AddCompRegion2Reg,orig_AddCompRegion2Val);
    GT_SYNC;

    /* set 'region 2' register value with MSBits of the 'offset' */
    AddCompRegion2Val = offset >> NOT_ADDRESS_COMPLETION_BITS_NUM_CNS;
    PCI_1_WRITE_DATA_INTO_ADDR_MAC(AddCompRegion2Reg,doByteSwap , AddCompRegion2Val);
    GT_SYNC;

    /* for PP register access use region N 2 always */
    /* now we can access the 'offset' with it's LSBits */
    address = (DIAG_ADDR_COMPLETION_REGION_CNS << NOT_ADDRESS_COMPLETION_BITS_NUM_CNS) |
              (offset & (~ ADDRESS_COMPLETION_BITS_MASK_CNS));
    address += baseAddr;

    READ_DATA_FROM_ADDR_MAC(address , regData);
    GT_SYNC;

    *regDataPtr = DIAG_BYTE_SWAP_MAC(doByteSwap, regData);

    /* restore 'region 2' register value */
    PCI_1_WRITE_DATA_INTO_ADDR_MAC(AddCompRegion2Reg,GT_FALSE , orig_AddCompRegion2Val);
    GT_SYNC;

    if(origNumRegions != 8)
    {
        /* restore 4 regions mode */
        (void) pexNumRegionsSet(baseAddr,doByteSwap,origNumRegions);
    }

    return GT_OK;
}


/*******************************************************************************
* regSmiWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write through SMI to one of the PP PCI
*       configuration or internal registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       offset     - the register offset
*       data       - data to write
*       doByteSwap - GT_TRUE:  byte swap will be done on the written data
*                    GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static GT_STATUS regSmiWrite
(
    IN GT_U32                          baseAddr,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
)
{
    data = DIAG_BYTE_SWAP_MAC(doByteSwap, data);
    return cpssExtDrvHwIfSmiWriteReg (baseAddr, offset, data);
}

/*******************************************************************************
* regSmiRead
*
* DESCRIPTION:
*       Performs single 32 bit data read through SMI from one of the PP
*       PCI configuration or internal registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       offset     - the register offset
*       doByteSwap - GT_TRUE:  byte swap will be done on the read data
*                    GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static GT_STATUS regSmiRead
(
    IN GT_U32                          baseAddr,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
)
{
    GT_STATUS retVal;

    retVal = cpssExtDrvHwIfSmiReadReg (baseAddr, offset, dataPtr);
    *dataPtr = DIAG_BYTE_SWAP_MAC(doByteSwap, *dataPtr);

    return retVal;
}

/*******************************************************************************
* regTwsiWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write through TWSI to one of the PP PCI
*       configuration or internal registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       offset     - the register offset
*       data       - data to write
*       doByteSwap - GT_TRUE:  byte swap will be done on the written data
*                    GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static GT_STATUS regTwsiWrite
(
    IN GT_U32                          baseAddr,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
)
{
    data = DIAG_BYTE_SWAP_MAC(doByteSwap, data);
    return cpssExtDrvHwIfTwsiWriteReg (baseAddr, offset, data);
}

/*******************************************************************************
* regTwsiRead
*
* DESCRIPTION:
*       Performs single 32 bit data read through TWSI from one of the PP
*       PCI configuration or internal registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       offset     - the register offset
*       doByteSwap - GT_TRUE:  byte swap will be done on the read data
*                    GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static GT_STATUS regTwsiRead
(
    IN GT_U32                          baseAddr,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
)
{
    GT_STATUS retVal;

    retVal = cpssExtDrvHwIfTwsiReadReg (baseAddr, offset, dataPtr);
    *dataPtr = DIAG_BYTE_SWAP_MAC(doByteSwap, *dataPtr);

    return retVal;
}

/*******************************************************************************
* regsInfoSearch
*
* DESCRIPTION:
*       Search for information about the registers in use.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - The device number
*       portGroupId       - the port group Id , to support multi-port-group
*                           devices that need to access specific port group
*       searchType        - Type of information to search.
*                           This can be:
*                           (a) Number of registers in use
*                           (b) List of addresses for registers in use
*                           (c) List of data for registers in use
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
*       regsNumPtr        - for (a) - Not relevant as input parameter
*                           for (b) - Number of registers to get their addresses
*                           for (c) - Number of registers to get their data
*
* OUTPUTS:
*       regsNumPtr        - for (a) - Number of registers in use
*                           for (c) - Number of registers that their addresses were
*                                     successfully retrieved
*                           for (c) - Number of registers that their data were
*                                     successfully retrieved
*       regsInfoPtr       - for (a) - Not relevant
*                           for (b) - List of registers addresses
*                           for (c) - List of registers data
*
* RETURNS:
*       GT_OK              - on success
*       GT_BAD_PARAM       - on bad search type
*       GT_FAIL            - on error
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
static GT_STATUS regsInfoSearch
(
    IN    GT_U8                              devNum,
    IN    GT_U32                             portGroupId,
    IN    PRV_CPSS_DIAG_REGS_SEARCH_TYPE_ENT searchType,
    IN    GT_U32                             *regsListPtr,
    IN    GT_U32                             regsListSize,
    IN    GT_U32                             offset,
    IN    GT_U32                             unitIdAddrPattern,
    IN    GT_U32                             unitIdAddrMask,
    IN    PRV_CPSS_DIAG_CHECK_REG_EXIST_FUNC checkRegExistFuncPtr,
    INOUT GT_U32                             *regsNumPtr,
    OUT   GT_U32                             *regsInfoPtr
)
{
    GT_U32 i;
    GT_U32 foundRegsNum = 0;
    GT_STATUS status;
    GT_U32 tempRegsNum;
        PRV_CPSS_BOOKMARK_STC *bookmark;

    switch(searchType)
    {
        case PRV_CPSS_DIAG_REGS_NUM_ONLY_SEARCH_E:
        case PRV_CPSS_DIAG_REGS_ADDR_SEARCH_E:
        case PRV_CPSS_DIAG_REGS_DATA_SEARCH_E:
        case PRV_CPSS_DIAG_REGS_DATA_RESET_AND_INIT_CTRL_SEARCH_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    for (i = 0; i < regsListSize; i++)
    {
        if (regsListPtr[i] == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            continue; /* Skip unused entry */
        }
        if (regsListPtr[i] == PRV_CPSS_SW_PTR_BOOKMARK_CNS)
        {
            bookmark = (PRV_CPSS_BOOKMARK_STC*)(&(regsListPtr[i]));
            /*Check for write only data*/
            if (bookmark->type & PRV_CPSS_SW_TYPE_WRONLY_CNS)
            {
                /* if the pointer field data is write only skip the pointer,
                   else skip the whole range*/
                if (bookmark->type & PRV_CPSS_SW_TYPE_PTR_FIELD_CNS)
                {
                    i += sizeof(*bookmark)/sizeof(GT_U32) - 1;
                }
                else
                {
                    i += 1 + bookmark->size;
                }
            }
            else
            {
                 /* assign temp register number to be reminder from already found */
                tempRegsNum = *regsNumPtr - foundRegsNum;

                regsInfoSearch (devNum,
                                portGroupId,
                                searchType,
                                bookmark->nextPtr,
                                bookmark->size / sizeof(GT_U32),
                                offset,
                                unitIdAddrPattern,
                                unitIdAddrMask,
                                checkRegExistFuncPtr,
                                &tempRegsNum,  /* supply temp registers number for recursive call */
                                regsInfoPtr);
                i += sizeof(*bookmark)/sizeof(GT_U32) - 1;

                /* move pointer to new position after recursive call */
                regsInfoPtr += tempRegsNum;

                /* increment found registers number */
                foundRegsNum += tempRegsNum;

            }
        }
        /* regular entry - address should be bigger than offset*/
        else if ((regsListPtr[i] >= offset) && ((regsListPtr[i] & unitIdAddrMask) == unitIdAddrPattern ))
        {
            /* check if function for register existance checking is specified */
            if (checkRegExistFuncPtr != NULL)
            {
                /* check if register address exist in specific port group */
                if (checkRegExistFuncPtr(devNum,portGroupId,regsListPtr[i]) == GT_FALSE)
                {
                    continue;
                }
            }

            if ((searchType == PRV_CPSS_DIAG_REGS_DATA_SEARCH_E) ||
                (searchType == PRV_CPSS_DIAG_REGS_DATA_RESET_AND_INIT_CTRL_SEARCH_E))
            {
                if (foundRegsNum < *regsNumPtr)
                {
                    /* For data searching read the data from the hardware */
                    if(searchType == PRV_CPSS_DIAG_REGS_DATA_SEARCH_E)
                    {
                        status = prvCpssHwPpPortGroupReadRegister(devNum,
                                                                  portGroupId,
                                                                  regsListPtr[i],
                                                                  regsInfoPtr);
                    }
                    else /* searchType == PRV_CPSS_DIAG_REGS_DATA_RESET_AND_INIT_CTRL_SEARCH_*/
                    {
                        status = prvCpssDrvHwPpResetAndInitControllerReadReg(
                                                                devNum,
                                                                regsListPtr[i],
                                                                regsInfoPtr);
                    }

                    if (status != GT_OK)
                    {
                        *regsNumPtr = foundRegsNum;
                        return status;
                    }

                    regsInfoPtr++;
                    foundRegsNum += 1;
                }
                else
                    break;
            }
            else if (searchType == PRV_CPSS_DIAG_REGS_ADDR_SEARCH_E)
            {
                /* For addresses searching take the address from the input
                   registers list */
                if (foundRegsNum < *regsNumPtr)
                {
                    *regsInfoPtr = regsListPtr[i];
                    foundRegsNum += 1;
                    regsInfoPtr++;
                }
                else
                    break;
            }
            else /* PRV_CPSS_DIAG_REGS_NUM_ONLY_SEARCH_E */
            {
                foundRegsNum += 1;
            }
        }
    }

    /* store number of found registers */
    *regsNumPtr = foundRegsNum;


    return GT_OK;
}


