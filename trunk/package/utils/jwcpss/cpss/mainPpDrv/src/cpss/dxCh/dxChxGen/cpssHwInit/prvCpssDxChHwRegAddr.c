/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwRegAddr.c
*
* DESCRIPTION:
*       This file includes definitions of all different Cheetah registers
*       addresses to be used by all functions accessing PP registers.
*       a variable of type PRV_CPSS_DXCH_PP_REGS_ADDR_STC should be allocated
*       for each PP type.
*
* FILE REVISION NUMBER:
*       $Revision: 264 $
*
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChModuleConfig.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>
#include <cpss/generic/port/private/prvCpssPortTypes.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwRegAddrVer1_ppDb.h>

static GT_STATUS prvCpssDxChHwBcat2RegAddrPortMacUpdate
(
    IN  GT_U8   devNum,
    IN  GT_U32  macNum
);

/* set register address for register that is bmp of ports BUT for only 28 ports ,
   CPU in bit 31

   macro duplicate address to the 2 register addresses , so CPU port can be use
   as 'bit 63' --> like in device that TRULY support bmp of 64 ports
*/
#define PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(regArr,startAddr) \
    regArr[0] = regArr[1] = (startAddr)

/* set registers addresses for register that are 'ports bmp' with consecutive addresses */
#define LOCAL_PORTS_BMP_REG_SET_MAC(regArr,startAddr) \
    regArr[0] = (startAddr);                    \
    regArr[1] = (startAddr) + 4;

/* set registers addresses for register that are 'ports bmp' with consecutive addresses */
#define PORTS_BMP_REG_SET_MAC(_devNum,regArr,startAddr)                                \
    {                                                                          \
        GT_U32  _ii;                                                           \
        for(_ii = 0 ; _ii < (PORTS_BMP_NUM_DEV_MAC(_devNum)) ; _ii++)          \
        {                                                                      \
            regArr[_ii] = ((startAddr) + (_ii * 4));                           \
        }                                                                      \
    }

/* set registers addresses according to formula */
#define FORMULA_SINGLE_REG_MAC(regArr,startAddr,step,numSteps) \
    {                                                          \
        GT_U32  _ii;                                           \
        for(_ii = 0 ; _ii < (numSteps) ; _ii++)                \
        {                                                      \
            (regArr)[_ii] = (startAddr) + (_ii * (step));        \
        }                                                      \
    }

/* set registers addresses according to formula */
#define FORMULA_DOUBLE_REG_MAC(regArr,startAddr,step1,numSteps1,step2,numSteps2) \
    {                                                          \
        GT_U32  _ii,_jj;                                       \
        for(_ii = 0 ; _ii < (numSteps1) ; _ii++)               \
        {                                                      \
            for(_jj = 0 ; _jj < (numSteps2) ; _jj++)           \
            {                                                  \
                (regArr)[_ii][_jj] = (startAddr) + (_ii * (step1)) + (_jj * (step2)); \
            }                                                  \
        }                                                      \
    }


/* set registers addresses according to formula , with start index in array of addresses */
#define FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,numSteps,startIndex) \
    {                                                          \
        GT_U32  _ii;                                           \
        for(_ii = 0 ; _ii < (numSteps) ; _ii++)                \
        {                                                      \
            regArr[(startIndex)+_ii] = (startAddr) + (_ii * (step)); \
        }                                                      \
    }


/* macro for Lion (4 port groups) for 16 local port registers , duplicated for
   all port groups */
#define LION_16_LOCAL_PORTS_TO_64_GLOBAL_PORTS_REG_SET_MAC(regArr,startAddr,step) \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16, 0);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,16);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,32);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,48)

#define LION2_16_LOCAL_PORTS_TO_128_GLOBAL_PORTS_REG_SET_MAC(regArr,startAddr,step) \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16, 0);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,16);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,32);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,48);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,64);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,80);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,96);           \
    FORMULA_SINGLE_REG_WITH_INDEX_MAC(regArr,startAddr,step,16,112)

/* number of per port registers */
#define PORTS_NUM_DEV_MAC(_devNum)                                              \
    (PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PHY_PORT_MAC(_devNum)+1)


/* registers for bmp of ports */
/* for device that supports 256 ports bmp , uses 8 words . CPU port at bit '63' */
/* for device that supports 64 ports bmp , uses 2 words . CPU port at bit '63' */
/* for device that supports 28 ports bmp + cpu bit '31' , uses only first word */
#define PORTS_BMP_NUM_DEV_MAC(_devNum)                                         \
    ((PORTS_NUM_DEV_MAC(_devNum) + 31) / 32)

/* registers for bmp of local ports include cpu port */
/* CPU port at bit '63' */
#define LOCAL_PORTS_WITH_CPU_BMP_NUM_DEV_CNS                                   \
    ((LOCAL_PORTS_WITH_CPU_NUM_DEV_CNS + 31) / 32)

/* number of local ports include cpu port */
#define LOCAL_PORTS_WITH_CPU_NUM_DEV_CNS 64

/* Units base addresses of xCat3 (place [0] in line) and xCat (place[1] in line) */
static GT_U32 prvCpssDxChUnitsBaseAddrXcat3AndXcat[][2] =
{
    {0x00000000, 0x00000000}, /* MG */
    {0x01000000, 0x02000000}, /* L2I */
    {0x02800000, 0x01800000}, /* Egress & Txq */
    {0x03400000, 0x0A400000}, /* CCFC - override by next Unit*/
    {0x03800000, 0x0A000000}, /* Egress & Txq - VLAN */
    {0x04000000, 0x02800000}, /* IPvX */
    {0x05000000, 0x03000000}, /* BM */
    {0x06000000, 0x03800000}, /* EPLR */
    {0x07000000, 0x04000000}, /* LMS0 - Group 0 */
    {0x08000000, 0x04800000}, /* LMS0 - Group 1 */
    {0x09000000, 0x05000000}, /* LMS1 - Group 0 */
    {0x0A000000, 0x05800000}, /* LMS1 - Group 1 */
    {0x0B000000, 0x06000000}, /* FDB */
    {0x0C000000, 0x06800000}, /* MPPM Bank 0 */
    {0x0D000000, 0x07000000}, /* MPPM Bank 1 */
    {0x0E000000, 0x07800000}, /* MEM */
    {0x10000000, 0x08000000}, /* CNC */
    {0x11000000, 0x11000000}, /* MSM MIB - init correctly, no convert needed */
    {0x12000000, 0x12000000}, /* GOP - init correctly, no convert needed*/
    {0x13000000, 0x13000000}, /* SD - init correctly, no convert needed*/
    {0x14000000, 0x0B000000}, /* Eq */
    {0x15000000, 0x0B800000}, /* IPCL */
    {0x16000000, 0x0C000000}, /* TTI */
    {0x17000000, 0x0C800000}, /* IPLR0 */
    {0x18000000, 0x0D000000}, /* IPLR1 */
    {0x19000000, 0x0D800000}, /* MLL */
    {0x1A000000, 0x0E000000}, /* IPCL TCC */
    {0x1B000000, 0x0E800000}, /* IPvX TCC */
    {0x1C000000, 0x0F000000}, /* HA */
    {0x1D000000, 0x0F800000}, /* EPCL */
    {0xFF800000, 0xFF800000}
};

/* Units base adddress is derived from the 9 MSbits.                            */
/* xCat3 registers and tables DB initialization is derived from xCat ones with  */
/* respective modification to the units base address.                           */
/* In fact all the necessary information for the conversion is found in         */
/* prvCpssDxChUnitsBaseAddrXcat3AndXcat (defined here above). Doing each        */
/* conversion directly based on prvCpssDxChUnitsBaseAddrXcat3AndXcat will       */
/* required address with mask matching and then some additional bit operations. */
/* In order to make the conversion more efficient and eliminating the search    */
/* for matching activity(done in O(n)), an auxiliary array is defined (the one  */
/* here below) which enables finding the new base address in O(1) -             */
/* the 9 MSbits are the reason for the array size of 512 entries (2^9 = 512).   */
GT_U32 prvCpssDxChBaseCovertAddrXcatToXcat3[BIT_9];

/*******************************************************************************
* prvPortsBmpRegisterSetEmulate128Ports
*
* DESCRIPTION:
*       Set registers addresses for register that are 'ports bmp' with
*       consecutive addresses for devices with upto 64 ports or up to 128 ports
*
* INPUTS:
*       devNum    - The PP's device number to init the struct for.
*       startAddr - Start address
*
* OUTPUTS:
*       regArrPtr - ports bmp register
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvPortsBmpRegisterSetEmulate128Ports
(
    IN  GT_U32  devNum,
    OUT GT_U32  *regArrPtr,
    IN  GT_U32  startAddr
)
{
    GT_U32  ii;
    GT_U32  numRegs = 1; /* init for devices with upto 64 ports */

    if (PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_2_MAC(devNum))
    {
        numRegs = 2;
    }
    else
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* the BMP of ports not support more then 128 ports , but
           macro PORTS_BMP_REG_SET_MAC will try to set 256 ports ...*/
        /* so WA for bobcat2 */
        numRegs = 2;
    }


    if(numRegs < 2)
    {
        /* support device with upto 64 ports*/
        PORTS_BMP_REG_SET_MAC(devNum,regArrPtr,startAddr);
    }
    else
    {
        /* support device with upto 128 ports */
        for(ii = 0 ; ii < numRegs ; ii++)
        {
            regArrPtr[ii] =
            regArrPtr[ii + numRegs] =
                ((startAddr) + (ii * 4));
        }
    }
}


/*******************************************************************************
* defaultAddressUnusedSet
*
* DESCRIPTION:
*       This function set all the register addresses as 'unused'.
*
* INPUTS:
*       startPtr    - pointer to start of register addresses
*       numOfRegs   - number of registers to initialize
* OUTPUTS:
*       startPtr    - pointer to start of register addresses that where initialized
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void defaultAddressUnusedSet
(
    INOUT void   *startPtr,
    IN GT_U32   numOfRegs
)
{
    GT_U32        ii;
    GT_U32       *regsAddrPtr32;
    GT_U32        regsAddrPtr32Size;

    /*Fill the all words in the pointer with initial value*/
    regsAddrPtr32       = startPtr;
    regsAddrPtr32Size   = numOfRegs;

    for( ii = 0; ii < regsAddrPtr32Size; ii++ )
    {
        regsAddrPtr32[ii] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    return;
}

#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
    #define allocAddressesMemory(_numOfBytes) internlAllocAddressesMemory(_numOfBytes,__FILE__,__LINE__)
#else /*! OS_MALLOC_MEMORY_LEAKAGE_DBG*/
    #define allocAddressesMemory(_numOfBytes) internlAllocAddressesMemory(_numOfBytes)
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG*/
/*******************************************************************************
* allocAddressesMemory
*
* DESCRIPTION:
*       allocate memory for registers addresses and initialize as 'unused'.
*
* INPUTS:
*       numOfBytes   - number of bytes to initialize
* OUTPUTS:
*       none
*
* RETURNS:
*       pointer to start of allocated and initialized memory
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void* internlAllocAddressesMemory
(
    IN GT_U32   numOfBytes
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
    ,IN const char*    fileNamePtr
    ,IN GT_U32   line
#endif /*OS_MALLOC_MEMORY_LEAKAGE_DBG*/
)
{
    void* memoryAlloc =
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
        cpssOsMalloc_MemoryLeakageDbg(numOfBytes,fileNamePtr,line);
#else /*! OS_MALLOC_MEMORY_LEAKAGE_DBG*/
        cpssOsMalloc(numOfBytes);
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG*/

    if(memoryAlloc)
    {
        /* initialize the memory */
        defaultAddressUnusedSet(memoryAlloc,numOfBytes / 4);
    }

    return memoryAlloc;
}


/*******************************************************************************
* cheetahBridgeRegsAlloc
*
* DESCRIPTION:
*       This function allocates memory for bridge registers struct for devices
*       belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahBridgeRegsAlloc
(
    IN  GT_U32                              devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;

    /* SIP5 device does not use this old BridgeRegs DB */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        return GT_OK;
    }

    size = sizeof(GT_U32) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    /* allocate all arrays for per port registers */
    addrPtr->bridgeRegs.portControl = allocAddressesMemory(size);
    if (addrPtr->bridgeRegs.portControl == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    addrPtr->bridgeRegs.portControl1 = allocAddressesMemory(size);
    if (addrPtr->bridgeRegs.portControl1 == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    addrPtr->bridgeRegs.swPortControlBookmark  = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->bridgeRegs.swPortControlType      = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->bridgeRegs.swPortControlSize      = size;

    addrPtr->bridgeRegs.swPortControl1Bookmark  = addrPtr->bridgeRegs.swPortControlBookmark;
    addrPtr->bridgeRegs.swPortControl1Type      = addrPtr->bridgeRegs.swPortControlType;
    addrPtr->bridgeRegs.swPortControl1Size      = addrPtr->bridgeRegs.swPortControlSize;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* need to use table PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E
           instead of registers */
        addrPtr->sFlowRegs.egrStcTblWord0 = allocAddressesMemory(size);
        if (addrPtr->sFlowRegs.egrStcTblWord0 == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        addrPtr->sFlowRegs.swEgrStcTblW0Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW0Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW0Size     = size;

        addrPtr->sFlowRegs.egrStcTblWord1 = allocAddressesMemory(size);
        if (addrPtr->sFlowRegs.egrStcTblWord1 == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        addrPtr->sFlowRegs.swEgrStcTblW1Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW1Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW1Size     = size;

        addrPtr->sFlowRegs.egrStcTblWord2 = allocAddressesMemory(size);
        if (addrPtr->sFlowRegs.egrStcTblWord2 == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        addrPtr->sFlowRegs.swEgrStcTblW2Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW2Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW2Size     = size;
    }

    return GT_OK;
}

/*******************************************************************************
* cheetahBridgeRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahBridgeRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  offset;
    /* init all arrays of per port registers */
    for (i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        offset = i * 0x1000;

        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E))
        {
            offset = (i & 0xf) * 0x1000;

            if((i & 0xf) > 0xb)
            {
                addrPtr->bridgeRegs.portControl[i]          =
                addrPtr->bridgeRegs.portControl1[i]         = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
                continue;
            }
        }

        /* bridge configuration registers 0..1 */
        addrPtr->bridgeRegs.portControl[i] = 0x02000000 + offset;
        addrPtr->bridgeRegs.portControl1[i] = 0x02000010 + offset;

        /* both "port based" and "port and protocol based" */
        /* VLAN and QoS configuration tables */
        /* have only indirect and "control + data" access */
        /* not used portVid and protoVidBased*/

        /* brdByteLimit - is in bridge configuration register1 */
        /* use portPrvVlan instead of brdByteLimit */

        /* brdByteWindow - in global register 0x02040140 */
    }

    addrPtr->bridgeRegs.cpuPortControl = 0x0203F000;
    addrPtr->bridgeRegs.cpuPortControl1 = 0x0203F010;

    /* QoS Profile registers */
    addrPtr->haRegs.qosProfile2ExpTable = 0x07800300;
    addrPtr->bridgeRegs.qosProfileRegs.dscp2QosProfileMapTable = 0x0B800450;
    addrPtr->bridgeRegs.qosProfileRegs.up2QosProfileMapTable = 0x0B8004A0;
    addrPtr->bridgeRegs.qosProfileRegs.dscp2DscpMapTable = 0x0B800400;
    addrPtr->bridgeRegs.qosProfileRegs.exp2QosProfileMapTable = 0x0B8004A8;
    addrPtr->bridgeRegs.qosProfileRegs.macQosTable = 0x02000200;

    for (i = 0; i < 2; i++)
    {
        offset = i * 20;
        addrPtr->bridgeRegs.brgCntrSet[i].cntrSetCfg   = 0x020400DC + offset;
        addrPtr->bridgeRegs.brgCntrSet[i].inPckt       = 0x020400E0 + offset;
        addrPtr->bridgeRegs.brgCntrSet[i].inFltPckt    = 0x020400E4 + offset;
        addrPtr->bridgeRegs.brgCntrSet[i].secFltPckt   = 0x020400E8 + offset;
        addrPtr->bridgeRegs.brgCntrSet[i].brgFltPckt   = 0x020400EC + offset;
    }

    addrPtr->bridgeRegs.brgMacCntr0 = 0x020400B0;
    /* UP mapped not to TC (and to DP) only, but to QOS profile */
    /* see regs 0x0B8004A0, 0x0B8004A4 */
    /* map TC to ... not relevant to cheetah */
    /* the CpuCode table entry contais TC and DP */
    /* regs 0x0B000030 and 0x0B003000 */

    addrPtr->bridgeRegs.deviceTableBaseAddr = 0x06000068;

    /* Bridge Ingress Drop Counter Reg */
    addrPtr->bridgeRegs.dropIngrCntr = 0x02040150;

    addrPtr->bridgeRegs.hostInPckt = 0x020400BC;
    addrPtr->bridgeRegs.hostOutPckt = 0x020400C0;
    addrPtr->bridgeRegs.hostOutMcPckt = 0x020400CC;
    addrPtr->bridgeRegs.hostOutBrdPckt = 0x020400D0;

    addrPtr->bridgeRegs.ingFltStatus = 0x020401A8;/*Security Breach Status Register2*/
    addrPtr->bridgeRegs.egressFilter = 0x01800010;
    addrPtr->bridgeRegs.egressFilter2 = 0x01800014;
    addrPtr->bridgeRegs.mcastLocalEnableConfig = 0x01800008;
    addrPtr->bridgeRegs.egressFilterIpMcRouted = 0x018001D0;

    addrPtr->bridgeRegs.l2IngressControl = 0x0B800000;
    addrPtr->bridgeRegs.learnedDisc = 0x06000030;

    /*
        Classified Distribution Table 379:
        IEEE Reserved Multicast Configuration Register<n> (0<=n<16)
        Offset:  Reg0: 0x02000800, Reg1:0x02001800... Reg15:0x0200F800
        (16 Registers in step of 0x1000)
    */
    addrPtr->bridgeRegs.macRangeFltrBase[0] = 0x02000800;

    /*
     *  There are two IPv6 ICMP Message Type Configuration registers
     *  with the offset 0x1000. Only the base is allocated
    */
    addrPtr->bridgeRegs.ipv6IcmpMsgTypeBase  = 0x02010800;
    /* IPv6 ICMP Message Type Cmd */
    addrPtr->bridgeRegs.ipv6IcmpMsgCmd      = 0x02012800;

    /*
     *  There are 8 IPv6 Link Local Configuration registers
     *  with the offset 0x1000. Only the base is allocated
    */
    addrPtr->bridgeRegs.ipv6McastLinkLocalBase  = 0x02028800;

    /*
     *  There are 8 IPv4 Link Local Configuration registers
     *  with the offset 0x1000. Only the base is allocated
    */
    addrPtr->bridgeRegs.ipv4McastLinkLocalBase  = 0x02020800;

    addrPtr->bridgeRegs.ipv6McastSolicitNodeAddrBase = 0x0B800050;

    addrPtr->bridgeRegs.ipv6McastSolicitNodeMaskBase = 0x0B800060;

    /* Number of Trunk Members Table Entry<n> (0<=n<16)  */
    /* set only the base address */
     /* 8 trunks in each address  starting trunkId = 1 */
    addrPtr->bridgeRegs.trunkTable = 0x02000804;

    addrPtr->bridgeRegs.macTblAction0 = 0x06000004;
    addrPtr->bridgeRegs.macTblAction1 = 0x06000008;
    addrPtr->bridgeRegs.macTblAction2 = 0x06000020;

    addrPtr->bridgeRegs.matrixPckt = 0x020400D4;

    /* two VLAN ranges */
    addrPtr->bridgeRegs.vlan0 = 0x020400A8;
    /* two protocol IDs 0x8100 */
    addrPtr->bridgeRegs.vlan1 = 0x0B800004;

    addrPtr->bridgeRegs.vlanRegs.protoEncBased = 0x0B800520;
    addrPtr->bridgeRegs.vlanRegs.protoEncBased1 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    /* 4 registers 0x0B800500-0x0B80050C for 8 protocols */
    addrPtr->bridgeRegs.vlanRegs.protoTypeBased = 0x0B800500;

    /* Egress Vlan Ether Type Configuration Register */
    addrPtr->bridgeRegs.egressVlanEtherTypeConfig = 0x07800018;

    /* Egress Vlan Ether Type Select Register */
    addrPtr->bridgeRegs.egressVlanEtherTypeSel = 0x0780001C;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* override */
        addrPtr->bridgeRegs.egressVlanEtherTypeConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.egressVlanEtherTypeSel    = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        /* MAC error indication port configuration register */
        addrPtr->bridgeRegs.macErrorIndicationPortConfReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        /* MAC error indication status register */
        addrPtr->bridgeRegs.macErrorIndicationStatusReg[0] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.macErrorIndicationStatusReg[1] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
    else
    {
        /* MAC error indication port configuration register */
        addrPtr->bridgeRegs.macErrorIndicationPortConfReg = 0x0780004C;

        /* MAC error indication status register */
        addrPtr->bridgeRegs.macErrorIndicationStatusReg[0] = 0x07800050;
        addrPtr->bridgeRegs.macErrorIndicationStatusReg[1] = 0x07800054;
    }

    addrPtr->bridgeRegs.bridgeGlobalConfigRegArray[0] = 0x02040000;
    addrPtr->bridgeRegs.bridgeGlobalConfigRegArray[1] = 0x02040004;
    addrPtr->bridgeRegs.bridgeGlobalConfigRegArray[2] = 0x0204000C;

    addrPtr->bridgeRegs.bridgeGlobalSecurBreachDropCounter   = 0x02040104;
    addrPtr->bridgeRegs.bridgePortVlanSecurBreachDropCounter = 0x02040108;

    addrPtr->bridgeRegs.bridgeRateLimitConfigRegArray[0] = 0x02040140;
    addrPtr->bridgeRegs.bridgeRateLimitConfigRegArray[1] = 0x02040144;

    addrPtr->bridgeRegs.bridgeSecurBreachStatusRegArray[0] = 0x020401A0;
    addrPtr->bridgeRegs.bridgeSecurBreachStatusRegArray[1] = 0x020401A4;
    addrPtr->bridgeRegs.bridgeSecurBreachStatusRegArray[2] = 0x020401A8;
    addrPtr->bridgeRegs.bridgeRateLimitDropCounter[0] = 0x02040148;
    addrPtr->bridgeRegs.bridgeRateLimitDropCounter[1] = 0x0204014C;

    addrPtr->bridgeRegs.macControlReg  = 0x06000000;

    /* IPv6 MC Bridging Bytes Selection Configuration Register */
    addrPtr->bridgeRegs.bridgeIpv6BytesSelection = 0x02040010;

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.haVlanTransEnReg,0x07800130);

    for (i = 0; i < 4; i++)
    {
        addrPtr->bridgeRegs.bridgeIngressVlanEtherTypeTable[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->haRegs.bridgeEgressVlanEtherTypeTable[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->haRegs.bridgeEgressPortTag0TpidSelect[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->haRegs.bridgeEgressPortTag1TpidSelect[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    for (i = 0; i < 32; i++)
    {
        addrPtr->bridgeRegs.bridgeIngressVlanSelect[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

}

/*******************************************************************************
* cheetahGlobalRegsInit
*
* DESCRIPTION:
*       This function initializes the global registers struct for devices that
*       belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with global regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahGlobalRegsInit
(
    IN GT_U32            devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;

    for (i = 0; i < 4; i++)
    {
        addrPtr->globalRegs.addrUpdate[i] = 0x06000040 + i * 4;
    }
    addrPtr->globalRegs.addrUpdateControlReg = 0x06000050;

    addrPtr->globalRegs.globalControl          = 0x00000058;
    addrPtr->globalRegs.extendedGlobalControl  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->globalRegs.extendedGlobalControl2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->globalRegs.sampledAtResetReg      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->globalRegs.addrCompletion         = 0x00000000;
    addrPtr->globalRegs.auQBaseAddr            = 0x000000C0;
    addrPtr->globalRegs.auQControl             = 0x000000C4;
    addrPtr->globalRegs.fuQBaseAddr            = 0x000000C8;
    addrPtr->globalRegs.fuQControl             = 0x000000CC;
    addrPtr->globalRegs.interruptCoalescing    = 0x000000E0;
    addrPtr->globalRegs.dummyReadAfterWriteReg = 0x000000F0;

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)
    {
        addrPtr->globalRegs.cpuPortCtrlReg         = 0x000000A0;
    }
}


/*******************************************************************************
* cheetahBufMngRegsAlloc
*
* DESCRIPTION:
*       This function allocates  memory for the buffer management registers
*       struct for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahBufMngRegsAlloc
(
    IN  GT_U32                              devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;

    /* SIP5 device does not use this old bufMngRegs DB */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        return GT_OK;
    }

    /* allocate array for per profile registers */
    size = sizeof(struct dxch_bufMngPerPort) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    addrPtr->bufferMng.bufMngPerPort = allocAddressesMemory(size);
    if (addrPtr->bufferMng.bufMngPerPort == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    return GT_OK;
}

/*******************************************************************************
* cheetahBufMngRegsInit
*
* DESCRIPTION:
*       This function initializes the buffer management registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahBufMngRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  offset;
    /* allocate array for per port registers */
    for(i = 0; i < 32; i++)
    {
        addrPtr->bufferMng.devMapTbl[i] = 0x01A40004 + i*0x10;
        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
           (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
        {
            /* between every target device we have 0x80 (the first 0x10 already added) */
            addrPtr->bufferMng.devMapTbl[i] += i*0x70;
        }
    }

    for(i = 0; i < 32; i++)
    {
        addrPtr->bufferMng.srcIdEggFltrTbl[i] = 0x01A4000C + i*0x10;
    }


    /* Trunk<n> Non-Trunk Members Table (0<=n<128) */
    /* set base address for low bits , base address for low bits
        ---> in index 0 */
    /* use CHEETAH_TRUNK_NON_TRUNK_TABLE_OFFSET_CNS for offset */
    addrPtr->bufferMng.trunkTblCfgRegs.baseAddrNonTrunk = 0x01A40000;

    /* use CHEETAH_TRUNK_DESIGNATED_PORTS_TABLE_OFFSET_CNS for offset */
    addrPtr->bufferMng.trunkTblCfgRegs.baseAddrTrunkDesig = 0x01A40008;

    /* Port<4n,4n+1,4n+2,4n+3> TrunkNum Configuration Register<n> (0<=n<7) */
    /* num of registers : (corePpDevs[devNum]->numOfPorts / 4) + 1 */
    /* use CHEETAH_TRUNK_ID_CONIFG_TABLE_OFFSET_CNS for offset */
    for (i = 0; i < 7; i++)
    {
        addrPtr->haRegs.trunkNumConfigReg[i] = 0x0780002C + 4*i;
    }

    /* init all arrays of per profiles registers */
    addrPtr->bufferMng.swBufMngPerPortBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->bufferMng.swBufMngPerPortType     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->bufferMng.swBufMngPerPortSize     =
        sizeof(struct dxch_bufMngPerPort) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    addrPtr->bufferMng.bufMngAgingConfig = 0x0300000C;
    addrPtr->bufferMng.bufMngGlobalLimit = 0x03000000;
    addrPtr->bufferMng.cpuRxBufLimit = 0x03000020;
    addrPtr->bufferMng.cpuRxBufCount = 0x0300017C;

    addrPtr->bufferMng.eqBlkCfgRegs.trapSniffed = 0x0B000000;
    addrPtr->bufferMng.eqBlkCfgRegs.sniffQosCfg = 0x0B000004;
    addrPtr->bufferMng.eqBlkCfgRegs.sniffPortsCfg = 0x0B000008;

    addrPtr->bufferMng.eqBlkCfgRegs.cpuTargDevConfReg0 = 0x0B000010;
    addrPtr->bufferMng.eqBlkCfgRegs.cpuTargDevConfReg1 = 0x0B000014;

    addrPtr->bufferMng.eqBlkCfgRegs.trSrcSniff = 0x0B00000C;

    addrPtr->bufferMng.eqBlkCfgRegs.ingressDropCntrReg = 0x0B000040;
    addrPtr->bufferMng.eqBlkCfgRegs.ingressDropCntrConfReg = 0x0B00003C;


    addrPtr->bufferMng.bufLimitsProfile.associationReg0 = 0x03000030;
    addrPtr->bufferMng.bufLimitsProfile.associationReg1 = 0x03000034;

    addrPtr->bufferMng.bufMngGigaPortGroupLimit  = 0x03000004;
    addrPtr->bufferMng.bufMngHyperGStackPortGroupLimit  = 0x03000008;

    addrPtr->bufferMng.bufMngMetalFix = 0x03000060;

    for (i = 0; i < PRV_CPSS_DXCH_PROFILES_NUM_CNS; i++)
    {
        addrPtr->bufferMng.bufLimitProfileConfig[i] = 0x03000020 + i*4;
    }

    for (i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        offset = i * 4;
        if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            /* all ports are valid */
        }
        else
        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
           (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
        {
            offset = (i & 0xf) * 4;
            if((i & 0xf) > 0xb)
            {
                addrPtr->bufferMng.bufMngPerPort[i].portBufAllocCounter =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;
                continue;
            }
        }
        addrPtr->bufferMng.bufMngPerPort[i].portBufAllocCounter = 0x03000100 + offset;
    }
    addrPtr->bufferMng.cpuBufMngPerPort.portBufAllocCounter = 0x0300017C;
    addrPtr->bufferMng.totalBufAllocCntr = 0x03000058;
    addrPtr->bufferMng.bufMngPacketCntr  = 0x0300005C;
    addrPtr->bufferMng.netPortGroupAllocCntr = 0x03000080;
    addrPtr->bufferMng.stackPortGroupAllocCntr = 0x03000084;
    addrPtr->bufferMng.bufMngAgedBuffersCntr = 0x03000054;
}

/*******************************************************************************
* cheetahEgrTxRegsAlloc
*
* DESCRIPTION:
*       This function allocates memory for the Egress and Transmit (Tx) Queue
*       Configuration registers struct for devices that belong to
*       REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahEgrTxRegsAlloc
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_OK;
    }

    size = sizeof(dxch_txPortRegs) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    addrPtr->egrTxQConf.txPortRegs = allocAddressesMemory(size);
    if (addrPtr->egrTxQConf.txPortRegs == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cheetahEgrTxRegsInit
*
* DESCRIPTION:
*       This function initializes the Egress and Transmit (Tx) Queue
*       Configuration registers struct for devices that belong to
*       REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahEgrTxRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i,j;
    GT_U32  size;
    GT_U32  offset;

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return;
    }

    size = sizeof(dxch_txPortRegs) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;

    addrPtr->egrTxQConf.swTxPortRegsBookmark   = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->egrTxQConf.swTxPortRegsType   = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->egrTxQConf.swTxPortRegsSize       = size;

    /* init all arrays of per profiles registers */
    for (i = 0; i < PRV_CPSS_DXCH_PROFILES_NUM_CNS; i++)
    {
        /* WRR output algorithm configured */
        /* via 4 Transmit scheduler  profiles */
        offset = i * 0x10;
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights0      = 0x01800040 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights1      = 0x01800044 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].wrrStrictPrio    = 0x01800048 + offset;

    }

    /* init not used regs */
    for (/* i continue from prev loop */;
         i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        /* WRR output algorithm configured */
        /* via 4 Transmit scheduler  profiles */
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights1 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->egrTxQConf.txPortRegs[i].wrrStrictPrio = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    addrPtr->egrTxQConf.l2PortIsolationTableBase = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->egrTxQConf.l3PortIsolationTableBase = PRV_CPSS_SW_PTR_ENTRY_UNUSED;


    /* init all arrays of per port registers */
    for (i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        /* per traffic class */
        for (j = 0; j < 8; j++)
        {
            offset = j * 0x8000 + i * 0x200;
            if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
            {
                /* all ports are valid */
            }
            else
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
               (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
            {
                offset = j * 0x8000 + (i & 0xf) * 0x200;

                if((i & 0xf) > 0xb)
                {
                    addrPtr->egrTxQConf.txPortRegs[i].prioTokenBuck[j]   =
                    addrPtr->egrTxQConf.txPortRegs[i].prioTokenBuckLen[j]=
                    addrPtr->egrTxQConf.txPortRegs[i].tcDescrCounter[j]  =
                    addrPtr->egrTxQConf.txPortRegs[i].tcBufferCounter[j] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
                    continue;
                }
            }
            addrPtr->egrTxQConf.txPortRegs[i].prioTokenBuck[j]     = 0x01AC0000 + offset;
            /* such current state info unreachable */
            addrPtr->egrTxQConf.txPortRegs[i].prioTokenBuckLen[j]  = (0x01AC0004 + offset);
            addrPtr->egrTxQConf.txPortRegs[i].tcDescrCounter[j]    = 0x01880000 + offset;
            addrPtr->egrTxQConf.txPortRegs[i].tcBufferCounter[j]   = 0x01900000 + offset;
        }

        offset = i * 0x200;
        if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            /* all ports are valid */
        }
        else
        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
           (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
        {
            offset = (i & 0xf) * 0x200;

            if((i & 0xf) > 0xb)
            {
                addrPtr->egrTxQConf.txPortRegs[i].tokenBuck      =
                addrPtr->egrTxQConf.txPortRegs[i].tokenBuckLen   =
                addrPtr->egrTxQConf.txPortRegs[i].txConfig       =
                addrPtr->egrTxQConf.txPortRegs[i].wdTxFlush      =
                addrPtr->egrTxQConf.txPortRegs[i].descrCounter   =
                addrPtr->egrTxQConf.txPortRegs[i].bufferCounter  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
                continue;
            }
        }
        addrPtr->egrTxQConf.txPortRegs[i].tokenBuck        = 0x01AC0008 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].tokenBuckLen     = (0x01AC000C + offset);
        addrPtr->egrTxQConf.txPortRegs[i].txConfig         = 0x01800080 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].descrCounter     = 0x01840000 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].bufferCounter    = 0x018C0000 + offset;

        if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            addrPtr->egrTxQConf.txPortRegs[i].wdTxFlush = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->egrTxQConf.txPortRegs[i].l1OverheadConfig = 0x01A80000 + offset;
        }
        else
        {
            addrPtr->egrTxQConf.txPortRegs[i].wdTxFlush = 0x01A80000 + offset;
            addrPtr->egrTxQConf.txPortRegs[i].l1OverheadConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }

    }

    /* Tail drop Limits: 4 profiles, 0x01800030 */
    /* see also 0x01800080 */
    for (j = 0; j < PRV_CPSS_DXCH_PROFILES_NUM_CNS; j++)
    {

        addrPtr->egrTxQConf.setsConfigRegs.portLimitsConf[j] = 0x01800030 + j * 4;

        for (i = 0; i < 8; i++)
        {
            offset = j * 0x0400 + i * 0x08;
            addrPtr->egrTxQConf.setsConfigRegs.tcDp0Red[i][j] = 0x01940000 + offset;
            addrPtr->egrTxQConf.setsConfigRegs.tcDp1Red[i][j] = 0x01940004 + offset;
        }
    }

    for (i = 0; i < 8; i++)
    {
        offset = i * 0x8000;
        addrPtr->egrTxQConf.txCpuRegs.prioTokenBuck[i]     = 0x01AC7E00 + offset;
        addrPtr->egrTxQConf.txCpuRegs.prioTokenBuckLen[i]  = 0x01AC7E00 + offset;
        addrPtr->egrTxQConf.txCpuRegs.tcDescrCounter[i]    = 0x01887E00 + offset;
        addrPtr->egrTxQConf.txCpuRegs.tcBufferCounter[i]   = 0x01907E00 + offset;
    }

    addrPtr->egrTxQConf.txCpuRegs.tokenBuck = 0x01AC7E08;
    addrPtr->egrTxQConf.txCpuRegs.tokenBuckLen = 0x01AC7E0C;
    addrPtr->egrTxQConf.txCpuRegs.txConfig = 0x01807E80;
    addrPtr->egrTxQConf.txCpuRegs.descrCounter = 0x01847E00;
    addrPtr->egrTxQConf.txCpuRegs.bufferCounter = 0x018C7E00;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        addrPtr->egrTxQConf.txCpuRegs.wdTxFlush = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->egrTxQConf.txCpuRegs.l1OverheadConfig = 0x01A87E00;
    }
    else
    {
        addrPtr->egrTxQConf.txCpuRegs.wdTxFlush = 0x01A87E00;
        addrPtr->egrTxQConf.txCpuRegs.l1OverheadConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }


    addrPtr->egrTxQConf.txQueueConfig = 0x01800000;
    addrPtr->egrTxQConf.txQMcFifoEccConfig = 0x0180000C;
    addrPtr->egrTxQConf.trStatSniffAndStcReg = 0x01800020;
    addrPtr->egrTxQConf.txQueueMiscCtrl = 0x018001B0;

    /* outBytePkts is amount of BC packets both TWIST and Cheetah */
    for (i = 0; i < 2; i++)
    {
        offset = i * 0x20;
        addrPtr->egrTxQConf.txQCountSet[i].txQConfig       = 0x01800140 + offset;
        addrPtr->egrTxQConf.txQCountSet[i].outUcPkts       = 0x01B40144 + offset;
        addrPtr->egrTxQConf.txQCountSet[i].outMcPkts       = 0x01B40148 + offset;
        addrPtr->egrTxQConf.txQCountSet[i].outBytePkts     = 0x01B4014C + offset;
        addrPtr->egrTxQConf.txQCountSet[i].brgEgrFiltered  = 0x01B40150 + offset;
        addrPtr->egrTxQConf.txQCountSet[i].txQFiltered     = 0x01B40154 + offset;
        addrPtr->egrTxQConf.txQCountSet[i].outCtrlPkts     = 0x01B40158 + offset;
    }

    addrPtr->egrTxQConf.sniffTailDropCfgReg = 0x01800024;
    addrPtr->egrTxQConf.txQueueConfigExt = 0x01800004;
    addrPtr->egrTxQConf.tcDp0CscdDataRemap = 0x01800018;
    addrPtr->egrTxQConf.tcDp1CscdDataRemap = 0x0180001C;

    addrPtr->egrTxQConf.totalBufferLimitConfig = 0x01800084;

    addrPtr->txqVer1.sht.global.portIsolationLookupConfiguration = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetahIntrRegsInit
*
* DESCRIPTION:
*       This function initializes the interrupt registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with interrupt regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahIntrRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* for interrupt connection via PCI */
    addrPtr->interrupts.intSumCauseBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->interrupts.intSumCauseType= PRV_CPSS_SW_TYPE_WRONLY_CNS ;
    addrPtr->interrupts.intSumCauseSize= 3;
    addrPtr->interrupts.intSumCause    = 0x0114;  /* for interrupt connection via PCI */
    addrPtr->interrupts.intSumMask     = 0x0118; /* for interrupt connection via PCI */

    /* in the Cheetah the Bridge Interrupts
       are moved to FDB interrupt registers */
    addrPtr->interrupts.ethBrgCause    = 0x06000018;
    addrPtr->interrupts.ethBrgMask     = 0x0600001C;
    addrPtr->interrupts.bmIntCause     = 0x03000040;
    addrPtr->interrupts.bmIntMask      = 0x03000044;
    addrPtr->interrupts.miscIntCause   = 0x0038;
    addrPtr->interrupts.miscIntMask    = 0x003C;
    addrPtr->interrupts.txqIntCause    = 0x01800118;
    addrPtr->interrupts.txqIntMask     = 0x0180011C;
    addrPtr->interrupts.rxDmaIntCause  = 0x280C;
    addrPtr->interrupts.rxDmaIntMask   = 0x2814;
    addrPtr->interrupts.txDmaIntCause  = 0x2810;
    addrPtr->interrupts.txDmaIntMask   = 0x2818;
}

/*******************************************************************************
* cheetahTwsiRegsInit
*
* DESCRIPTION:
*       This function initializes the TWSI registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with TWSI regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahTwsiRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->swTwsiRegBookmark   = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->swTwsiRegType      = PRV_CPSS_SW_TYPE_WRONLY_CNS;
    addrPtr->swTwsiRegSize      = sizeof(addrPtr->twsiReg) / sizeof(GT_U32);
    addrPtr->twsiReg.slaveAddr          = 0x00400000;
    addrPtr->twsiReg.slaveAddrExtend    = 0x00400010;
    addrPtr->twsiReg.data               = 0x00400004;
    addrPtr->twsiReg.ctrl               = 0x00400008;
    addrPtr->twsiReg.status             = 0x0040000c;
    addrPtr->twsiReg.baudRate           = 0x0040000C;
    addrPtr->twsiReg.softReset          = 0x0040001C;
    addrPtr->twsiReg.serInitCtrl        = 0x0010;
    addrPtr->twsiReg.serFinishAddr      = 0x0014;
}

/*******************************************************************************
* cheetaSdmaRegsInit
*
* DESCRIPTION:
*       This function initializes the SDMA registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with sdma regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetaSdmaRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  offset;

    addrPtr->sdmaRegs.sdmaCfgReg           = 0x00002800;
    addrPtr->sdmaRegs.rxQCmdReg            = 0x00002680;
    addrPtr->sdmaRegs.txQCmdReg            = 0x00002868;
    addrPtr->sdmaRegs.rxDmaResErrCnt[0]    = 0x00002860;
    addrPtr->sdmaRegs.rxDmaResErrCnt[1]    = 0x00002864;
    addrPtr->sdmaRegs.txQFixedPrioConfig   = 0x00002870;
    addrPtr->sdmaRegs.txSdmaWrrTokenParameters = 0x00002874;

    for (i = 0; i < 8; i++)
    {
        offset = i * 0x10;
        addrPtr->sdmaRegs.rxDmaCdp[i]          = 0x260C + offset;
        addrPtr->sdmaRegs.txQWrrPrioConfig[i]  = 0x2708 + offset;
        addrPtr->sdmaRegs.txSdmaTokenBucketQueueCnt[i]    = 0x2700 + offset;
        addrPtr->sdmaRegs.txSdmaTokenBucketQueueConfig[i] = 0x2704 + offset;

        offset = i * 4;
        addrPtr->sdmaRegs.rxDmaPcktCnt[i]      = 0x2820 + offset;
        addrPtr->sdmaRegs.rxDmaByteCnt[i]      = 0x2840 + offset;
        addrPtr->sdmaRegs.txDmaCdp[i]          = 0x26C0 + offset;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE ||
       CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        for (i = 0; i < 8; i++)
        {
            offset = i * 4;
            addrPtr->sdmaRegs.txSdmaPacketGeneratorConfigQueue[i] =
                                                                0x28B0 + offset;
            addrPtr->sdmaRegs.txSdmaPacketCountConfigQueue[i]     =
                                                                0x28D0 + offset;
        }

        for (i = 0; i < 2; i++)
        {
            offset = i * 4;
            addrPtr->sdmaRegs.rxSdmaResourceErrorCountAndMode[i] =
                                                                0x2860 + offset;
        }

        for (i = 2; i < 8; i++)
        {
            offset = i * 4;
            addrPtr->sdmaRegs.rxSdmaResourceErrorCountAndMode[i] =
                                                                0x2870 + offset;
        }
    }
}

/*******************************************************************************
* cheetahSerdesRegsAlloc
*
* DESCRIPTION:
*       This function allocates memory for SERDES registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahSerdesRegsAlloc
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;
    GT_U32  numOfSerdes;

    numOfSerdes = prvCpssDxChHwInitNumOfSerdesGet((GT_U8)devNum);

    if (numOfSerdes == 0)
    {
        addrPtr->serdesConfig = NULL;
        addrPtr->serdesConfigRegsBookmark   = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->serdesConfigRegsType       = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->serdesConfigRegsSize       = 0;
        return GT_OK;
    }

    size = sizeof(struct _lpSerdesConfig) * numOfSerdes;
    addrPtr->serdesConfig = allocAddressesMemory(size);
    if (addrPtr->serdesConfig == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    addrPtr->serdesConfigRegsBookmark   = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->serdesConfigRegsType       = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->serdesConfigRegsSize       = size;

    return GT_OK;
}


/*******************************************************************************
* cheetahMacRegsAlloc
*
* DESCRIPTION:
*       This function allocates memory for MAC related registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahMacRegsAlloc
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;

    size = sizeof(struct dxch_perPortRegs) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    addrPtr->macRegs.perPortRegs = allocAddressesMemory(size);
    if (addrPtr->macRegs.perPortRegs == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    addrPtr->macRegs.swPerPortRegsBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->macRegs.swPerPortRegsType     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->macRegs.swPerPortRegsSize     = size;

    return GT_OK;
}

/*******************************************************************************
* cheetahMacRegsInit
*
* DESCRIPTION:
*       This function initializes MAC related registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahMacRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  lane;
    GT_U32  offset;

    addrPtr->macRegs.srcAddrMiddle = 0x04004024;
    addrPtr->macRegs.srcAddrHigh   = 0x04004028;

    addrPtr->addrSpace.buffMemWriteControl = 0x06800000;
    addrPtr->addrSpace.buffMemBank0Write = 0x06900000;
    addrPtr->addrSpace.buffMemBank1Write = 0x06980000;

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
       (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        addrPtr->addrSpace.buffMemClearType  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
    else
    {
        addrPtr->addrSpace.buffMemClearType  = 0x07800010;
    }

    /* Mac counters Control registers   */
    for(i = 0; i < 4; i++)
    {
        addrPtr->macRegs.macCountersCtrl[i] = 0x04004020 + (i * 0x800000) ;
        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
           (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
        {
            /* in lion the GE ports uses the same counters mechanism as the
               XG ports ! */
            addrPtr->macRegs.macCountersCtrl[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }
    }

    /* HyperG.Stack Ports MIB Counters and XSMII Configuration Register */
    addrPtr->macRegs.hgXsmiiCountersCtr = 0x01800180;

    /* per-port registers   */
    for (i = 0; (i < PRV_CPSS_PP_MAC(devNum)->numOfPorts); i++)
    {
        offset = i * 0x100;

        /* serialParameters - per port (0..23) */
        addrPtr->macRegs.perPortRegs[i].serialParameters=
            ((i < 24)
             ? (0x0A800014 + offset)
             : PRV_CPSS_SW_PTR_ENTRY_UNUSED) ;
        /* MIB MAC counters */

        addrPtr->macRegs.perPortRegs[i].macCounters =
            ((i < 24)
             ? (0x04010000 + ((i / 6) * 0x0800000) + ((i % 6) * 0x0080))
             : (0x01C00000 + ((i - 24) * 0x040000))) ;

        addrPtr->macRegs.perPortRegs[i].macCaptureCounters =
            ((i < 24)
             ? (0x04010300 + ((i / 6) * 0x0800000))
             : (0x01C00080 + ((i - 24) * 0x040000))) ;

        /* Mac control registers    */
        if(i >= 24)
        {
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl =
                    0x0A800000 + offset;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl1 =
                    0x0A800004 + offset;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl2 =
                    0x0A800008 + offset;
        }
        else
        {
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl =
                     0x0A800000 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl1 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 =
                    0x0A800004 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl2 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 =
                    0x0A800008 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl1 =
                    PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl2 =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }

        addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl1 =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl2 =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl3 =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        /* Mac status registers     */
        addrPtr->macRegs.perPortRegs[i].macStatus =
            ((i < 24)
             ? (0x0A800010 + offset)
             : (0x0A80180C + offset - (24 * 0x100))) ;
        addrPtr->macRegs.perPortRegs[i].autoNegCtrl =
            ((i < 24)
             ? (0x0A80000C + offset)
             : PRV_CPSS_SW_PTR_ENTRY_UNUSED) ;

        /* PRBS registers */
        addrPtr->macRegs.perPortRegs[i].prbsCheckStatus = 0x0A800038 + offset;
        addrPtr->macRegs.perPortRegs[i].prbsErrorCounter = 0x0A80003C + offset;

        addrPtr->macRegs.perPortRegs[i].xgMibCountersCtrl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].xlgMibCountersCtrl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].xlgExternalControlRegister = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        for(lane = 0; lane < 4; lane++)
        {
            addrPtr->macRegs.perPortRegs[i].serdesPowerUp1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesPowerUp2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesReset[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesCommon[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].laneConfig0[lane] =
                 PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].laneConfig1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].disparityErrorCounter[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesTxSyncConf[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesSpeed1[lane] =
                 PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesSpeed2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesSpeed3[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesTxConfReg1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesTxConfReg2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesRxConfReg1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesRxConfReg2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg3[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg5[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesMiscConf[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesReservConf[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesIvrefConf1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesIvrefConf2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesProcessMon[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesCalibConf1[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].serdesCalibConf2[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].prbsErrorCounterLane[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].laneStatus[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;

            addrPtr->macRegs.perPortRegs[i].cyclicData[lane] =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }

        addrPtr->macRegs.perPortRegs[i].xgGlobalConfReg0 =
                 PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->macRegs.perPortRegs[i].hxPortConfig0[0]  =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].hxPortConfig0[1]  =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->macRegs.perPortRegs[i].hxPortConfig1[0]  =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].hxPortConfig1[1]  =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->macRegs.perPortRegs[i].ccfcFcTimerBaseGig =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.perPortRegs[i].ccfcFcTimerBaseXg =
            PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        /* set the Ethernet CPU port registers -- port 63 */
        offset = 63 * 0x100;
        addrPtr->macRegs.cpuPortRegs.macStatus         = 0x0A800010 + offset;
        addrPtr->macRegs.cpuPortRegs.macCounters = 0x00000060;
        addrPtr->macRegs.cpuPortRegs.autoNegCtrl       = 0x0A80000C + offset;
        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl     = 0x0A800000 + offset;
        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1    = 0x0A800004 + offset;
        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2    = 0x0A800008 + offset;
        addrPtr->macRegs.cpuPortRegs.serialParameters  = 0x0A800014 + offset;
    }

    /* Config HX ports */
    if ((PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX249_CNS) ||
        (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX269_CNS) ||
        (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX169_CNS))
    {
        addrPtr->macRegs.perPortRegs->hxPortConfig0[0]  = 0x0A803900; /* port 25 */
        addrPtr->macRegs.perPortRegs->hxPortConfig0[1]  = 0x0A803A00; /* port 26 */

        addrPtr->macRegs.perPortRegs->hxPortConfig1[0]  = 0x0A803904; /* port 25 */
        addrPtr->macRegs.perPortRegs->hxPortConfig1[1]  = 0x0A803A04; /* port 26 */
    }

    /* HXPorts Global Configuration register - relevant only for DX269 */
    if (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX269_CNS)
    {
        addrPtr->macRegs.hxPortGlobalConfig = 0x0A802000;
    }
    else
    {
        addrPtr->macRegs.hxPortGlobalConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }


    for(i=0 ; i < 6 ; i++)
    {
        addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIStatusRegister   = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIPRBSErrorCounter = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister1 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

}

/*******************************************************************************
* cheetahLedRegsInit
*
* DESCRIPTION:
*       This function initializes the Led registers struct for devices that
*       belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with SMI regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahLedRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ledRegs.ledControl[0] = 0x04004100 ; /*ports 0-11,CPU,26*/

    addrPtr->ledRegs.ledClass[0][0] = 0x04004108; /*ports 0-11, class 0*/
    addrPtr->ledRegs.ledClass[0][1] = 0x04004108; /*ports 0-11, class 1*/
    addrPtr->ledRegs.ledClass[0][2] = 0x04804108; /*ports 0-11, class 2*/
    addrPtr->ledRegs.ledClass[0][3] = 0x04804108; /*ports 0-11, class 3*/
    addrPtr->ledRegs.ledClass[0][4] = 0x0400410C; /*ports 0-11, class 4*/
    addrPtr->ledRegs.ledClass[0][5] = 0x0480410C; /*ports 0-11, class 5*/
    addrPtr->ledRegs.ledClass[0][6] = 0x04804100; /*ports 0-11, class 6*/
    /* OUT of table 0x05805100 - ports 24-25, classes 10-11*/

    addrPtr->ledRegs.ledGroup[0][0] = 0x04004104; /* ports 0-11, group 0*/
    addrPtr->ledRegs.ledGroup[0][1] = 0x04004104; /* ports 0-11, group 1*/
    addrPtr->ledRegs.ledGroup[0][2] = 0x04804104; /* ports 0-11, group 2*/
    addrPtr->ledRegs.ledGroup[0][3] = 0x04804104; /* ports 0-11, group 3*/


    if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)&&
       (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E) &&
       (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE))
    {
        addrPtr->ledRegs.ledControl[1] = 0x05004100 ; /*ports 12-23,24,25*/

        addrPtr->ledRegs.ledClass[1][0] = 0x05004108; /*ports 12-23, class 0*/
        addrPtr->ledRegs.ledClass[1][1] = 0x05004108; /*ports 12-23, class 1*/
        addrPtr->ledRegs.ledClass[1][2] = 0x05804108; /*ports 12-23, class 2*/
        addrPtr->ledRegs.ledClass[1][3] = 0x05804108; /*ports 12-23, class 3*/
        addrPtr->ledRegs.ledClass[1][4] = 0x0500410C; /*ports 12-23, class 4*/
        addrPtr->ledRegs.ledClass[1][5] = 0x0580410C; /*ports 12-23, class 5*/
        addrPtr->ledRegs.ledClass[1][6] = 0x05804100; /*ports 12-23, class 6*/

        addrPtr->ledRegs.ledGroup[1][0] = 0x05004104; /* ports 12-23, group 0*/
        addrPtr->ledRegs.ledGroup[1][1] = 0x05004104; /* ports 12-23, group 1*/
        addrPtr->ledRegs.ledGroup[1][2] = 0x05804104; /* ports 12-23, group 2*/
        addrPtr->ledRegs.ledGroup[1][3] = 0x05804104; /* ports 12-23, group 3 */

        addrPtr->ledRegs.ledXgClass04Manipulation[0]    = 0x4005100;
        addrPtr->ledRegs.ledXgClass59Manipulation[0]    = 0x4005104;
        addrPtr->ledRegs.ledHyperGStackDebugSelect[0]   = 0x4005110;
        addrPtr->ledRegs.ledHyperGStackDebugSelect1[0]  = 0x4005114;
        addrPtr->ledRegs.ledXgClass1011Manipulation[0]  = 0x4805100;
        addrPtr->ledRegs.ledXgGroup01Configuration[0]   = 0x4805104;

        addrPtr->ledRegs.ledXgClass04Manipulation[1]    = 0x5005100;
        addrPtr->ledRegs.ledXgClass59Manipulation[1]    = 0x5005104;
        addrPtr->ledRegs.ledXgClass1011Manipulation[1]  = 0x5805100;
        addrPtr->ledRegs.ledXgGroup01Configuration[1]   = 0x5805104;
    }
}

/*******************************************************************************
* cheetahSmiRegsAlloc
*
* DESCRIPTION:
*       This function allocates memory for the Smi
*       Configuration registers struct for devices that belong to
*       REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       GT_OK               - on success, or
*       GT_OUT_OF_CPU_MEM   - om malloc failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetahSmiRegsAlloc
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  size;

    /* SIP5 device does not use this old smiRegs DB */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        return GT_OK;
    }

    size = sizeof(GT_U32) * PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    addrPtr->smiRegs.smi10GePhyConfig0 = allocAddressesMemory(size);
    addrPtr->smiRegs.smi10GePhyConfig1 = allocAddressesMemory(size);
    if ( (addrPtr->smiRegs.smi10GePhyConfig0 == NULL) ||
         (addrPtr->smiRegs.smi10GePhyConfig1 == NULL) )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    addrPtr->smiRegs.swSmi10GePhyCfg0Bookmark  = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->smiRegs.swSmi10GePhyCfg0Type      = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->smiRegs.swSmi10GePhyCfg0Size      = size;
    addrPtr->smiRegs.swSmi10GePhyCfg1Bookmark  = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->smiRegs.swSmi10GePhyCfg1Type      = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
    addrPtr->smiRegs.swSmi10GePhyCfg1Size      = size;

    return GT_OK;
}

/*******************************************************************************
* cheetahSmiRegsInit
*
* DESCRIPTION:
*       This function initializes the Serial Management Interface registers
*       struct for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with SMI regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahSmiRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 i;
    GT_U32 offset;

    addrPtr->smiRegs.smiControl     = 0x04004054; /* ports 0-11*/
    /* out of data                 0x05004054     ports 12-23*/
    addrPtr->smiRegs.smiPhyAddr     = 0x04004030; /* ports 0-5*/
    /* out of data                 0x04804030     ports 6-11*/
    /* out of data                 0x05004030     ports 12-17*/
    /* out of data                 0x05804030     ports 18-23*/

   if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
   {
        addrPtr->smiRegs.smi10GeControl = 0x06E40000;
        addrPtr->smiRegs.smi10GeAddr    = 0x06E40008;
        addrPtr->smiRegs.xsmiConfig     = 0x06E4000C;
   }
   else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
            (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
            PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
   {
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            offset = 0x00030000;
        }
        else
        {
            offset = 0x00040000;
        }
        addrPtr->smiRegs.smi10GeControl = offset + 0x0000;
        addrPtr->smiRegs.smi10GeAddr    = offset + 0x0008;
        addrPtr->smiRegs.xsmiConfig     = offset + 0x000C;
   }
   else
   {
        addrPtr->smiRegs.smi10GeControl = 0x01cc0000;
        addrPtr->smiRegs.smi10GeAddr    = 0x01cc0008;
   }


    for(i=0; i<PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[i].portType < PRV_CPSS_PORT_XG_E ||
           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E ||
           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
           (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
        {
            addrPtr->smiRegs.smi10GePhyConfig0[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->smiRegs.smi10GePhyConfig1[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }
        else
        {
            offset = 0x100 * i;
            addrPtr->smiRegs.smi10GePhyConfig0[i] = 0x0a80001c + offset;
            addrPtr->smiRegs.smi10GePhyConfig1[i] = 0x0a800020 + offset;
        }
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
       addrPtr->smiRegs.lms0MiscConfig = 0x04004200;

       if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
          (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
          (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
       {
           addrPtr->smiRegs.lms1MiscConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED ;
       }
       else
       {
           addrPtr->smiRegs.lms1MiscConfig = 0x05004200;
       }
    }

    addrPtr->smiRegs.lms0PhyAutoNeg[0] = 0x4004034;
    addrPtr->smiRegs.lms0PhyAutoNeg[1] = 0x4804034;

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E) &&
        (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE))
    {
    addrPtr->smiRegs.lms1PhyAutoNeg[0] = 0x5004034;
    addrPtr->smiRegs.lms1PhyAutoNeg[1] = 0x5804034;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E) &&
            (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E) &&
            (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE))
    {
        addrPtr->smiRegs.lms0GigPortsModeReg1[0] = 0x4004144;
        addrPtr->smiRegs.lms0GigPortsModeReg1[1] = 0x4804144;
        addrPtr->smiRegs.lms1GigPortsModeReg1[0] = 0x5004144;
        addrPtr->smiRegs.lms1GigPortsModeReg1[1] = 0x5804144;
    }
}
}

/*******************************************************************************
* cheetahIpRegsInit
*
* DESCRIPTION:
*       This function initializes the IP registers struct for
*       devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahIpRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{

    addrPtr->ipRegs.ctrlReg0 = 0x0C000040;

    addrPtr->haRegs.hdrAltGlobalConfig = 0x07800100;
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.routerHdrAltEnMacSaModifyReg,0x07800104);
    addrPtr->haRegs.routerHdrAltMacSaModifyMode[0] = 0x07800120;
    addrPtr->haRegs.routerHdrAltMacSaModifyMode[1] = 0x07800124;
    addrPtr->haRegs.routerMacSaBaseReg[0] = 0x07800108;
    addrPtr->haRegs.routerMacSaBaseReg[1] = 0x0780010C;
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.routerDevIdModifyEnReg,0x07800110);

    addrPtr->ipRegs.ucRoutingEngineConfigurationReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    addrPtr->ipRegs.routerPerPortSipSaEnable0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetahTrafCndRegsInit
*
* DESCRIPTION:
*       This function initializes the Traffic Control block registers
*       struct for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahTrafCndRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /*********************/
    /* policer registers */
    /*********************/
    /* policer global register */
    addrPtr->PLR[0].policerControlReg = 0x0C000000;
}

/*******************************************************************************
* cheetaPclRegsInit
*
* DESCRIPTION:
*       This function initializes the Internal InLif Tables registers
*       struct for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void cheetaPclRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 i;

    /* Cheetah - PCL registers - temporary ommited */
    addrPtr->pclRegs.PclBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->pclRegs.PclType =  PRV_CPSS_SW_TYPE_WRONLY_CNS;
    addrPtr->pclRegs.PclSize = 57;
    addrPtr->pclRegs.control = 0x0B800000;
    for (i = 0; (i < 12); i++)
    {
        addrPtr->pclRegs.tcamWriteData[i] = 0x0B800100 + (4 * i);
    }

    addrPtr->pclRegs.tcamReadMaskBase     = 0x0B810000;
    addrPtr->pclRegs.tcamReadPatternBase  = 0x0B818000;
    addrPtr->pclRegs.tcamOperationTrigger = 0x0B800130;

    addrPtr->pclRegs.udbConfigKey0bytes0_2 = 0x0B800020;
    addrPtr->pclRegs.udbConfigKey0bytes3_5 = 0x0B800024;
    addrPtr->pclRegs.udbConfigKey1bytes0_2 = 0x0B800028;
    addrPtr->pclRegs.udbConfigKey1bytes3_5 = 0x0B80002C;
    addrPtr->pclRegs.udbConfigKey2bytes0_2 = 0x0B800030;
    addrPtr->pclRegs.udbConfigKey3bytes0_2 = 0x0B800034;
    addrPtr->pclRegs.udbConfigKey4bytes0_1 = 0x0B800038;
    addrPtr->pclRegs.udbConfigKey5bytes0_2 = 0x0B80003C;

    addrPtr->pclRegs.ruleMatchCounersBase = 0x0B801000;

    /* to override for next device versions */

    addrPtr->pclRegs.tcamOperationParam1 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.tcamOperationParam2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.tcamReadMaskControlBase = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.tcamReadPatternControlBase = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.actionTableBase = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.epclGlobalConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclCfgTblAccess,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktNonTs,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktTs,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToCpu,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuData,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuControl,PRV_CPSS_SW_PTR_ENTRY_UNUSED);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToAnalyzer,PRV_CPSS_SW_PTR_ENTRY_UNUSED);

    addrPtr->pclRegs.ipclTcpPortComparator0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.ipclUdpPortComparator0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.epclTcpPortComparator0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.epclUdpPortComparator0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.fastStack = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.loopPortReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.cncCountMode = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.cncl2l3IngressVlanCountEnable = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclTcamConfig0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclTcamConfig2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclTcamControl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclUDBConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclEngineConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.ttiUnitConfig   = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.ttiEngineConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.eplrGlobalConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetahSflowRegsInit
*
* DESCRIPTION:
*       This function initializes the Internal
*        sFlow registers struct for devices that belong to
*       REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void cheetahSflowRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 i;
    GT_U32 offset;

    addrPtr->sFlowRegs.sFlowControl            = 0x0B00001C;

    /* Cheetah sampling to cpu registers */
    addrPtr->sFlowRegs.ingStcTblAccessCtrlReg = 0x0B000038;
    addrPtr->sFlowRegs.ingStcTblWord0ValueReg = 0x0B005000;
    addrPtr->sFlowRegs.ingStcTblWord1ValueReg = 0x0B005004; /* read only */
    addrPtr->sFlowRegs.ingStcTblWord2ValueReg = 0x0B005008;
    /* addrPtr->sFlowRegs.ingStcIntCauseReg      = 0x0B000020; */

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Cheetah egress sampling to CPU per port registers */
        for (i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
        {
            offset = i * 0x200;

            addrPtr->sFlowRegs.egrStcTblWord0[i] = 0x01D40000 + offset;
            addrPtr->sFlowRegs.egrStcTblWord1[i] = 0x01D40004 + offset;
            addrPtr->sFlowRegs.egrStcTblWord2[i] = 0x01D40008 + offset;
        }
    }

}

/*******************************************************************************
* cheetaTtiRegsInit
*
* DESCRIPTION:
*       This function initializes the TTI registers
*       struct for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void cheetahTtiRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ttiRegs.pclIdConfig0        = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ttiRegs.pclIdConfig1        = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ttiRegs.ttiIpv4GreEthertype = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ttiRegs.specialEthertypes   = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ttiRegs.mplsEthertypes      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetaHaRegsInit
*
* DESCRIPTION:
*       This function initializes the HA registers struct for
*       devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with HA regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetahHaRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.cscdHeadrInsrtConf,0x07800004);

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.eggrDSATagTypeConf,0x07800020);
}

/*******************************************************************************
* cheetah3CncRegsInit
*
* DESCRIPTION:
*       This function initializes the CNC registers struct for devices that
*       Cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with global regs init
*
* RETURNS:
*       GT_OK               - on success, or
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3CncRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* global configuration           */
    addrPtr->cncRegs.globalCfg                         = 0x08000000;
    addrPtr->cncRegs.blockUploadTriggers               = 0x08000030;
    addrPtr->cncRegs.clearByReadValueWord0             = 0x08000040;
    addrPtr->cncRegs.clearByReadValueWord1             = 0x08000044;
    /* per couner block configuration */
    addrPtr->cncRegs.blockClientCfgAddrBase             = 0x08001080;
    addrPtr->cncRegs.blockWraparoundStatusAddrBase      = 0x080010A0;
    /* counters in blocks */
    addrPtr->cncRegs.blockCountersAddrBase      = 0x08080000;
}

/*******************************************************************************
* prvCpssDxChHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrInit
(
    IN  GT_U32 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;
    GT_U32       *regsAddrPtr32;
    GT_U32        regsAddrPtr32Size;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /*Fill all the words in the struct with initial value*/
    regsAddrPtr32 = (GT_U32*)regsAddrPtr;
    regsAddrPtr32Size = sizeof(PRV_CPSS_DXCH_PP_REGS_ADDR_STC) / sizeof(GT_U32);

    defaultAddressUnusedSet(regsAddrPtr32,regsAddrPtr32Size);

    /*
        Allocation for pointers inside regsAddr.
    */
    if ((cheetahMacRegsAlloc(devNum, regsAddrPtr) != GT_OK)      ||
        (cheetahEgrTxRegsAlloc(devNum, regsAddrPtr) != GT_OK)    ||
        (cheetahBridgeRegsAlloc(devNum, regsAddrPtr) != GT_OK)   ||
        (cheetahBufMngRegsAlloc(devNum, regsAddrPtr) != GT_OK)   ||
        (cheetahSmiRegsAlloc(devNum, regsAddrPtr) != GT_OK) ||
        (cheetahSerdesRegsAlloc(devNum,regsAddrPtr) != GT_OK))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /*
        Initialization of the struct's fields should be added here.
    */
    cheetahMacRegsInit(devNum, regsAddrPtr);

    /* SIP5 device does not use these legacy Regs DB substructures */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
    {
        cheetahEgrTxRegsInit(devNum, regsAddrPtr);
        cheetahBridgeRegsInit(devNum, regsAddrPtr);
        cheetahBufMngRegsInit(devNum, regsAddrPtr);
        cheetahIntrRegsInit(regsAddrPtr);
        cheetahLedRegsInit(devNum, regsAddrPtr);
        cheetahSmiRegsInit(devNum, regsAddrPtr);
        cheetahIpRegsInit(regsAddrPtr);
        cheetahTrafCndRegsInit(regsAddrPtr);
        cheetaPclRegsInit(regsAddrPtr);
        cheetahSflowRegsInit(devNum,regsAddrPtr);
        cheetahTtiRegsInit(regsAddrPtr);
        cheetahHaRegsInit(regsAddrPtr);
    }

    cheetahGlobalRegsInit(devNum,regsAddrPtr);
    cheetahTwsiRegsInit(regsAddrPtr);
    cheetaSdmaRegsInit(devNum, regsAddrPtr);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChHwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* free all the dynamically allocated pointers , and set them to NULL */
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->bridgeRegs.portControl);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->bridgeRegs.portControl1);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->sFlowRegs.egrStcTblWord0);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->sFlowRegs.egrStcTblWord1);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->sFlowRegs.egrStcTblWord2);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->bufferMng.bufMngPerPort);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->egrTxQConf.txPortRegs);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->macRegs.perPortRegs);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->smiRegs.smi10GePhyConfig0);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->smiRegs.smi10GePhyConfig1);
    FREE_PTR_IF_NOT_NULL_MAC(regsAddrPtr->serdesConfig);

    return GT_OK;
}

/*******************************************************************************
* cheetah3BridgeRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3BridgeRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->bridgeRegs.vntReg.vntOamLoopbackConfReg  = 0x01800094;
    addrPtr->bridgeRegs.vntReg.vntCfmEtherTypeConfReg = 0x0B800810;
    addrPtr->bridgeRegs.vntReg.vntGreEtherTypeConfReg = 0x0B800800;

    /* Ingress Log registers */
    addrPtr->bridgeRegs.ingressLog.etherType   = 0x02040700;
    addrPtr->bridgeRegs.ingressLog.daLow       = 0x02040704;
    addrPtr->bridgeRegs.ingressLog.daHigh      = 0x02040708;
    addrPtr->bridgeRegs.ingressLog.daLowMask   = 0x0204070C;
    addrPtr->bridgeRegs.ingressLog.daHighMask  = 0x02040710;
    addrPtr->bridgeRegs.ingressLog.saLow       = 0x02040714;
    addrPtr->bridgeRegs.ingressLog.saHigh      = 0x02040718;
    addrPtr->bridgeRegs.ingressLog.saLowMask   = 0x0204071C;
    addrPtr->bridgeRegs.ingressLog.saHighMask  = 0x02040720;
    addrPtr->bridgeRegs.ingressLog.macDaResult = 0x02040724;
    addrPtr->bridgeRegs.ingressLog.macSaResult = 0x02040728;
}

/*******************************************************************************
* cheetah3EgrTxRegsInit
*
* DESCRIPTION:
*       This function initializes the Egress and Transmit (Tx) Queue
*       Configuration registers struct for devices that belong to
*       REGS_FAMILY_1 registers set. -- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3EgrTxRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i;

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return;
    }

    addrPtr->egrTxQConf.txQueueDpToCfiPerPortEnReg = 0x018000A4;
    addrPtr->egrTxQConf.txQueueDpToCfiReg = 0x018000A8;

    addrPtr->egrTxQConf.stackRemapPortEnReg = 0x01800098;

    addrPtr->egrTxQConf.txQueueSharedPriorityReg[0] = 0x01800028;
    addrPtr->egrTxQConf.txQueueSharedPriorityReg[1] = 0x0180002C;

    /* allocate array for per port registers */
    for(i = 0; i < 64; i++)
    {
        addrPtr->egrTxQConf.secondaryTargetPortMapTbl[i] = 0x01E40000 + i*0x10;
        if(i == 27)
        {
            /* no more needed */
            break;
        }
    }

    /* allocate array for per profile registers */
    for(i = 0; i < 8; i++)
    {
        addrPtr->egrTxQConf.stackTcPerProfileRemapTbl[i] = 0x01E80000 + i*4;
    }

    addrPtr->egrTxQConf.totalBuffersCounter         = 0x01800088;
    addrPtr->egrTxQConf.totalDescCounter            = 0x01800120;
    addrPtr->egrTxQConf.multicastDescriptorsCounter = 0x01800124;
    addrPtr->egrTxQConf.snifferDescriptorsCounter   = 0x01800188;
}

/*******************************************************************************
* cheetah3PclRegsInit
*
* DESCRIPTION:
*       This function initializes the PCL registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3PclRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i;
    GT_U32  offset;

    addrPtr->pclRegs.fastStack = 0x0B800818;
    addrPtr->pclRegs.loopPortReg = 0x0B80081C;
    /* overwrites the CH and CH2 address 0x0B800130 */
    addrPtr->pclRegs.tcamOperationTrigger = 0x0B800138;
    addrPtr->pclRegs.tcamOperationParam1 = 0x0B800130;
    addrPtr->pclRegs.tcamOperationParam2 = 0x0B800134;
    addrPtr->pclRegs.tcamReadMaskBase            = 0x0BA00020;
    addrPtr->pclRegs.tcamReadPatternBase         = 0x0BA00000;
    addrPtr->pclRegs.tcamReadMaskControlBase     = 0x0BA00024;
    addrPtr->pclRegs.tcamReadPatternControlBase  = 0x0BA00004;
    addrPtr->pclRegs.actionTableBase             = 0x0B880000;

    /* CNC related registers - CH3 */
    addrPtr->pclRegs.cncCountMode                  = 0x0B800820;
    addrPtr->pclRegs.cncl2l3IngressVlanCountEnable = 0x0B800824;

    /* TCAM low level HW configuration - CH3 and above */
    addrPtr->pclRegs.pclTcamConfig0       = 0x0B820108;
    addrPtr->pclRegs.pclTcamConfig2       = 0x0B820120;
    addrPtr->pclRegs.pclTcamControl       = 0x0B820104;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        /* Only for DxCh3 devices */
        addrPtr->pclRegs.routedPacket = 0x0B800808;

        /* policy TCAM Test */
        for (i = 0; i < 14; i++)
        {
            offset = i * 0x4;
            addrPtr->pclRegs.policyTcamTest.pointerRelReg[i] = 0x0B830000 + offset;
        }

        addrPtr->pclRegs.policyTcamTest.configStatusReg = 0x0B830050;
        addrPtr->pclRegs.policyTcamTest.opcodeCommandReg = 0x0B830054;
        addrPtr->pclRegs.policyTcamTest.opcodeExpectedHitReg = 0x0B830058;

        for(i = 0; i < 8; i++)
        {
            addrPtr->pclRegs.macToMe[i].macLow = 0xB800700 + i * 0x10;
        }
    }
}

/*******************************************************************************
* xCatPclRegsInit
*
* DESCRIPTION:
*       This function initializes XCAT specific PCL regiters addresses
*       to overlap the Cheetah3 registers addresses
*
* INPUTS:
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void xCatPclRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->pclRegs.epclGlobalConfig        = 0x0F800000;
        /* bit per port registers */
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclCfgTblAccess,0x0F800004);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktNonTs,0x0F800008);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktTs,0x0F80000C);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToCpu,0x0F800010);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuData,0x0F800014);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuControl,0x0F800018);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToAnalyzer,0x0F80001C);
    /* TCP/UDP port comparators (8 register sequences) */
    addrPtr->pclRegs.ipclTcpPortComparator0  = 0x0C0000A0;
    addrPtr->pclRegs.ipclUdpPortComparator0  = 0x0C0000C0;
    addrPtr->pclRegs.epclTcpPortComparator0  = 0x0F800100;
    addrPtr->pclRegs.epclUdpPortComparator0  = 0x0F800140;
    /* xCat new */
    addrPtr->pclRegs.pclUDBConfig            = 0x0B800014;
    addrPtr->pclRegs.pclEngineConfig         = 0x0B80000C;
    addrPtr->pclRegs.ttiUnitConfig           = 0x0C000000;
    addrPtr->pclRegs.ttiEngineConfig         = 0x0C00000C;
    addrPtr->pclRegs.eplrGlobalConfig        = 0x0F800200;

    /* DSA configuration */
    addrPtr->pclRegs.fastStack               = 0x0C000060;
    addrPtr->pclRegs.loopPortReg             = 0x0C000064;
}


/*******************************************************************************
* cheetah3PolicerRegsInit
*
* DESCRIPTION:
*       This function initializes the Policer registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3PolicerRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* IPLR Registers */
    addrPtr->PLR[0].policerControlReg            = 0x0C000000;
    addrPtr->PLR[0].policerPortMeteringEnReg[0]  = 0x0C000004;
    addrPtr->PLR[0].policerMRUReg                = 0x0C000010;
    addrPtr->PLR[0].policerErrorReg              = 0x0C000020;
    addrPtr->PLR[0].policerErrorCntrReg          = 0x0C000024;
    addrPtr->PLR[0].policerTblAccessControlReg   = 0x0C000028;
    addrPtr->PLR[0].policerTblAccessDataReg      = 0x0C000030;
    addrPtr->PLR[0].policerRefreshScanRateReg    = 0x0C000050;
    addrPtr->PLR[0].policerRefreshScanRangeReg   = 0x0C000054;
    addrPtr->PLR[0].policerInitialDPReg          = 0x0C000060;
    addrPtr->PLR[0].policerIntCauseReg           = 0x0C000100;
    addrPtr->PLR[0].policerIntMaskReg            = 0x0C000104;
    addrPtr->PLR[0].policerShadowReg             = 0x0C000108;

    /* IPLR Tables */
    addrPtr->PLR[0].policerMeteringCountingTbl   = 0x0C040000;
    addrPtr->PLR[0].policerQosRemarkingTbl       = 0x0C080000;
    addrPtr->PLR[0].policerManagementCntrsTbl    = 0x0C0C0000;
}

/*******************************************************************************
* xCatPolicerRegsInit
*
* DESCRIPTION:
*       This function initializes the Policer registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatPolicerRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* IPLR #0 Registers */
    addrPtr->PLR[0].policerControlReg            = 0x0C800000;
    addrPtr->PLR[0].policerPortMeteringEnReg[0]  = 0x0C800008;
    addrPtr->PLR[0].policerPortMeteringEnReg[1]  = 0x0C80000C;
    addrPtr->PLR[0].policerMRUReg                = 0x0C800010;
    addrPtr->PLR[0].ipfixControl                 = 0x0C800014;
    addrPtr->PLR[0].ipfixNanoTimerStampUpload    = 0x0C800018;
    addrPtr->PLR[0].ipfixSecLsbTimerStampUpload  = 0x0C80001C;
    addrPtr->PLR[0].ipfixSecMsbTimerStampUpload  = 0x0C800020;
    addrPtr->PLR[0].ipfixDroppedWaThreshold      = 0x0C800030;
    addrPtr->PLR[0].ipfixPacketWaThreshold       = 0x0C800034;
    addrPtr->PLR[0].ipfixByteWaThresholdLsb      = 0x0C800038;
    addrPtr->PLR[0].ipfixByteWaThresholdMsb      = 0x0C80003C;
    addrPtr->PLR[0].ipfixSampleEntriesLog0       = 0x0C800048;
    addrPtr->PLR[0].ipfixSampleEntriesLog1       = 0x0C80004C;
    addrPtr->PLR[0].policerErrorReg              = 0x0C800050;
    addrPtr->PLR[0].policerErrorCntrReg          = 0x0C800054;
    addrPtr->PLR[0].policerTblAccessControlReg   = 0x0C800070;
    addrPtr->PLR[0].policerTblAccessDataReg      = 0x0C800074;
    addrPtr->PLR[0].policerInitialDPReg          = 0x0C8000C0;
    addrPtr->PLR[0].policerIntCauseReg           = 0x0C800100;
    addrPtr->PLR[0].policerIntMaskReg            = 0x0C800104;
    addrPtr->PLR[0].policerShadowReg             = 0x0C800108;
    addrPtr->PLR[0].policerControl1Reg           = 0x0C800004;
    addrPtr->PLR[0].policerControl2Reg           = 0x0C80002C;
    addrPtr->PLR[0].portMeteringPtrIndexReg      = 0x0C801800;

    /* IPLR #0 Tables */

    /* exclude metering and counting tables from dump because
       policer stage may be without tables */
    addrPtr->PLR[0].policerBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->PLR[0].policerType     = PRV_CPSS_SW_TYPE_WRONLY_CNS;
    addrPtr->PLR[0].policerSize     = 3;

    addrPtr->PLR[0].policerMeteringCountingTbl   = 0x0C840000;
    addrPtr->PLR[0].policerQosRemarkingTbl       = 0x0C880000;
    addrPtr->PLR[0].policerManagementCntrsTbl    = 0x0C800500;
    addrPtr->PLR[0].policerCountingTbl           = 0x0C860000;
    addrPtr->PLR[0].policerTimerTbl              = 0x0C800200;
    addrPtr->PLR[0].ipfixWrapAroundAlertTbl      = 0x0C800800;
    addrPtr->PLR[0].ipfixAgingAlertTbl           = 0x0C800900;

    /* IPLR #1 Registers */
    addrPtr->PLR[1].policerControlReg            = 0x0D000000;
    addrPtr->PLR[1].policerPortMeteringEnReg[0]  = 0x0D000008;
    addrPtr->PLR[1].policerPortMeteringEnReg[1]  = 0x0D00000C;
    addrPtr->PLR[1].policerMRUReg                = 0x0D000010;
    addrPtr->PLR[1].ipfixControl                 = 0x0D000014;
    addrPtr->PLR[1].ipfixNanoTimerStampUpload    = 0x0D000018;
    addrPtr->PLR[1].ipfixSecLsbTimerStampUpload  = 0x0D00001C;
    addrPtr->PLR[1].ipfixSecMsbTimerStampUpload  = 0x0D000020;
    addrPtr->PLR[1].ipfixDroppedWaThreshold      = 0x0D000030;
    addrPtr->PLR[1].ipfixPacketWaThreshold       = 0x0D000034;
    addrPtr->PLR[1].ipfixByteWaThresholdLsb      = 0x0D000038;
    addrPtr->PLR[1].ipfixByteWaThresholdMsb      = 0x0D00003C;
    addrPtr->PLR[1].ipfixSampleEntriesLog0       = 0x0D000048;
    addrPtr->PLR[1].ipfixSampleEntriesLog1       = 0x0D00004C;
    addrPtr->PLR[1].policerErrorReg              = 0x0D000050;
    addrPtr->PLR[1].policerErrorCntrReg          = 0x0D000054;
    addrPtr->PLR[1].policerTblAccessControlReg   = 0x0D000070;
    addrPtr->PLR[1].policerTblAccessDataReg      = 0x0D000074;
    addrPtr->PLR[1].policerInitialDPReg          = 0x0D0000C0;
    addrPtr->PLR[1].policerIntCauseReg           = 0x0D000100;
    addrPtr->PLR[1].policerIntMaskReg            = 0x0D000104;
    addrPtr->PLR[1].policerShadowReg             = 0x0D000108;
    addrPtr->PLR[1].policerControl1Reg           = 0x0D000004;
    addrPtr->PLR[1].policerControl2Reg           = 0x0D00002C;
    addrPtr->PLR[1].portMeteringPtrIndexReg      = 0x0D001800;

    /* IPLR #1 Tables */
    /* exclude metering and counting tables from dump because
       policer stage may be without tables */
    addrPtr->PLR[1].policerBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
    addrPtr->PLR[1].policerType     = PRV_CPSS_SW_TYPE_WRONLY_CNS;
    addrPtr->PLR[1].policerSize     = 3;

    addrPtr->PLR[1].policerMeteringCountingTbl   = 0x0D040000;
    addrPtr->PLR[1].policerQosRemarkingTbl       = 0x0D080000;
    addrPtr->PLR[1].policerManagementCntrsTbl    = 0x0D000500;
    addrPtr->PLR[1].policerCountingTbl           = 0x0D060000;
    addrPtr->PLR[1].policerTimerTbl              = 0x0D000200;
    addrPtr->PLR[1].ipfixWrapAroundAlertTbl      = 0x0D000800;
    addrPtr->PLR[1].ipfixAgingAlertTbl           = 0x0D000900;

    /* EPLR Registers */
    addrPtr->PLR[2].policerControlReg            = 0x03800000;
    addrPtr->PLR[2].policerPortMeteringEnReg[0]  = 0x03800008;
    addrPtr->PLR[2].policerPortMeteringEnReg[1]  = 0x0380000C;
    addrPtr->PLR[2].policerMRUReg                = 0x03800010;
    addrPtr->PLR[2].ipfixControl                 = 0x03800014;
    addrPtr->PLR[2].ipfixNanoTimerStampUpload    = 0x03800018;
    addrPtr->PLR[2].ipfixSecLsbTimerStampUpload  = 0x0380001C;
    addrPtr->PLR[2].ipfixSecMsbTimerStampUpload  = 0x03800020;
    addrPtr->PLR[2].ipfixDroppedWaThreshold      = 0x03800030;
    addrPtr->PLR[2].ipfixPacketWaThreshold       = 0x03800034;
    addrPtr->PLR[2].ipfixByteWaThresholdLsb      = 0x03800038;
    addrPtr->PLR[2].ipfixByteWaThresholdMsb      = 0x0380003C;
    addrPtr->PLR[2].ipfixSampleEntriesLog0       = 0x03800048;
    addrPtr->PLR[2].ipfixSampleEntriesLog1       = 0x0380004C;
    addrPtr->PLR[2].policerErrorReg              = 0x03800050;
    addrPtr->PLR[2].policerErrorCntrReg          = 0x03800054;
    addrPtr->PLR[2].policerTblAccessControlReg   = 0x03800070;
    addrPtr->PLR[2].policerTblAccessDataReg      = 0x03800074;
    addrPtr->PLR[2].policerInitialDPReg          = 0x038000C0;
    addrPtr->PLR[2].policerIntCauseReg           = 0x03800100;
    addrPtr->PLR[2].policerIntMaskReg            = 0x03800104;
    addrPtr->PLR[2].policerShadowReg             = 0x03800108;
    addrPtr->PLR[2].policerControl1Reg           = 0x03800004;
    addrPtr->PLR[2].policerControl2Reg           = 0x0380002C;
    addrPtr->PLR[2].portMeteringPtrIndexReg      = 0x03801800;

    /* EPLR Tables */
    addrPtr->PLR[2].policerMeteringCountingTbl   = 0x03840000;
    addrPtr->PLR[2].policerQosRemarkingTbl       = 0x03880000;
    addrPtr->PLR[2].policerManagementCntrsTbl    = 0x03800500;
    addrPtr->PLR[2].policerCountingTbl           = 0x03860000;
    addrPtr->PLR[2].policerTimerTbl              = 0x03800200;
    addrPtr->PLR[2].ipfixWrapAroundAlertTbl      = 0x03800800;
    addrPtr->PLR[2].ipfixAgingAlertTbl           = 0x03800900;
}

/*******************************************************************************
* lion2and3MacRegsInit
*
* DESCRIPTION:
*       This function initializes MAC,SERDESs related registers struct
*       for Lion2 and Bobcat2; Caelum; Bobcat3 devices.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       GT_OK - all ok
*       other - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lion2and3MacRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    PRV_CPSS_PORT_TYPE_ENT  portType; /* type of port accordingly to used MAC Unit */
    GT_PHYSICAL_PORT_NUM    i;        /* global port numbers iterator */
    GT_U32                  j; /* iterator */
    GT_U32                  lane;     /* xpcs lanes loop iterator */
    GT_U32                  baseAddr,   /* MAC Unit base address */
                            baseAddrToUse,
                            xgBaseAddr,
                            hglBaseAddr,
                            mibBaseAddr,
                            xpcsBaseAddr = 0x089C0200; /* XPCS Unit base address */
    GT_U32                  offset, /* step between ports */
                            mibOffset,  /* step between ports in MSM MIB Unit */
                            xpcsLaneOffset, /* step between xpcs lanes */
                            ccfcFcTimerOffset, /* ccfcFcTimer register offset in unit */
                            macCtrl0Offset, /* register offset in unit */
                            macCtrl1Offset,
                            macCtrl2Offset,
                            macCtrl3Offset,
                            macCtrl4Offset,
                            macCtrl5Offset=0,
                            fcDsaTag2Offset,
                            fcDsaTag3Offset,
                            ppfcCtrlOffset;
    GT_U32                  iterNumber;
    GT_U32                  eeeSupported = 0;

    /* clear all previous settings per port registers */
    defaultAddressUnusedSet(addrPtr->macRegs.perPortRegs,
                            addrPtr->macRegs.swPerPortRegsSize / sizeof(GT_U32));

    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        /* Use Lion2 unit address 0x08800000. convertOldRegDbToNewUnitsBaseAddr
           will update it for Bcat2 */
        xpcsBaseAddr = 0x08800000 + 0x180400;

        eeeSupported = 1;
    }


    /* Lion2 - 4 and less active port groups - the numOfPorts should include the CPU port 0x3f */
    iterNumber = ((PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_TRUE)) ?
                  (PRV_CPSS_PP_MAC(devNum)->numOfPorts) :
                  (PRV_CPSS_PP_MAC(devNum)->numOfPorts + 1);

    /* Init MAC registers   */
    for (i = 0; i < iterNumber; i++)
    {
        if((i == PRV_CPSS_PP_MAC(devNum)->numOfPorts) &&
            (i < CPSS_CPU_PORT_NUM_CNS))
        {
            /* jump to CPU port when finished with 'numOfPorts' */
            i = CPSS_CPU_PORT_NUM_CNS;
        }

        baseAddr = 0x08800000; /* convertOldRegDbToNewUnitsBaseAddr will update it for Bcat2 */
        if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            /* right MIB addresses will be set for Bcat2 in prvCpssDxChHwRegAddrVer1Init */
            mibBaseAddr = 0x0;
            mibOffset = 0x0;
            /* all ports are valid */
            if(i > 55)
            {
                baseAddr += 0x200000;
                offset    = (i-56) * 0x1000;
            }
            else
            {
                offset    = i  * 0x1000;
            }
        }
        else
        {
            mibBaseAddr = 0x09000000;
            if(((i & 0xf) > 0xb) && (CPSS_CPU_PORT_NUM_CNS != i))
            {
                continue;
            }

            offset = (CPSS_CPU_PORT_NUM_CNS != i) ? ((i & 0xf) * 0x1000) : 0x3F000;
            mibOffset = (i & 0xf) * 0x20000;
        }

        xgBaseAddr = baseAddr + 0xC0000;/*Lion2 : 0x088C0000;
                                          Bcat2 : 0x100C0000;*/
        hglBaseAddr = baseAddr + 0x1C0000; /*Lion2 : 0x089C0000; */

        for(portType = PRV_CPSS_PORT_GE_E; portType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portType++)
        {
            switch(portType)
            {
                case PRV_CPSS_PORT_GE_E:

                    macCtrl0Offset = 0;
                    macCtrl1Offset = 4;
                    macCtrl2Offset = 8;
                    macCtrl3Offset = 0x48;
                    macCtrl4Offset = 0x90;
                    fcDsaTag2Offset = 0x80;
                    fcDsaTag3Offset = 0x84;
                    ccfcFcTimerOffset = 0x58;
                    ppfcCtrlOffset = 0x94;

                    if(CPSS_CPU_PORT_NUM_CNS == i)
                    {/* set the Ethernet CPU port registers -- port 63 */

CPSS_TBD_BOOKMARK_BOBCAT2 /* this part not needed for Bobcat2, but till right
treatment of CPU port implemented leave it to pass UT's */

                        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl  =
                            baseAddr + macCtrl0Offset + offset;
                        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 =
                            baseAddr + macCtrl1Offset + offset;
                        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 =
                            baseAddr + macCtrl2Offset + offset;
                        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3 =
                            baseAddr + macCtrl3Offset + offset;
                        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl4 =
                            baseAddr + macCtrl4Offset + offset;
                        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].fcDsaTag2 =
                            baseAddr + fcDsaTag2Offset + offset;
                        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].fcDsaTag3 =
                            baseAddr + fcDsaTag3Offset + offset;

                        addrPtr->macRegs.cpuPortRegs.macStatus     = baseAddr + 0x10 + offset;
                        addrPtr->macRegs.cpuPortRegs.autoNegCtrl   = baseAddr + 0xC + offset;
                        addrPtr->macRegs.cpuPortRegs.serialParameters =
                            baseAddr + 0x14 + offset;

                        if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)
                            continue;
                    }

                    addrPtr->macRegs.perPortRegs[i].serialParameters = baseAddr + 0x14 + offset;
                    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                    {
                        addrPtr->macRegs.perPortRegs[i].serialParameters1 = baseAddr + 0x94 + offset;
                        addrPtr->macRegs.perPortRegs[i].xgMibCountersCtrl  = baseAddr + 0x44 + offset;
                        /* if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum)) */
                        {
                            addrPtr->macRegs.perPortRegs[i].serdesCnfg  = baseAddr + 0x28 + offset;
                            addrPtr->macRegs.perPortRegs[i].serdesCnfg3 = baseAddr + 0x34 + offset;
                        }
                    }
                    addrPtr->macRegs.perPortRegs[i].autoNegCtrl      = baseAddr + 0xC + offset;
                    addrPtr->macRegs.perPortRegs[i].prbsCheckStatus  = baseAddr + 0x38 + offset;
                    addrPtr->macRegs.perPortRegs[i].prbsErrorCounter = baseAddr + 0x3C + offset;

                    if(eeeSupported)
                    {
                        addrPtr->macRegs.perPortRegs[i].eeeLpiControl[0] = baseAddr + offset + 0xC0;
                        addrPtr->macRegs.perPortRegs[i].eeeLpiControl[1] = baseAddr + offset + 0xC4;
                        addrPtr->macRegs.perPortRegs[i].eeeLpiControl[2] = baseAddr + offset + 0xC8;
                        addrPtr->macRegs.perPortRegs[i].eeeLpiStatus     = baseAddr + offset + 0xCC;
                        addrPtr->macRegs.perPortRegs[i].eeeLpiCounter    = baseAddr + offset + 0xD0;
                    }

                    /* Actually at this point application didn't define port mode, so
                        just to support old behavior init mac status register to point
                        to GE MAC Unit */
                    addrPtr->macRegs.perPortRegs[i].macStatus = baseAddr + 0x10 + offset;

                    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                    {
                        addrPtr->macRegs.perPortRegs[i].gePortFIFOConfigReg0 =
                                                        baseAddr + 0x18 + offset;
                        addrPtr->macRegs.perPortRegs[i].gePortFIFOConfigReg1 =
                                                        baseAddr + 0x1C + offset;
                    }
                    baseAddrToUse = baseAddr;
                    break;

                case PRV_CPSS_PORT_XLG_E:
                    macCtrl5Offset = 0x88;
                case PRV_CPSS_PORT_ILKN_E:

                    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
                    {
                        /* mask port on '0xf' is relevant to multi port groups only */
                        if((i & 0xf) == 9)
                        {
                            offset = 12 * 0x1000;
                        }
                        else if((i & 0xf) == 11)
                        {
                            offset = 14 * 0x1000;
                        }
                    /* Yep, fall down to XG */
                    }

                case PRV_CPSS_PORT_XG_E:

                    if(((CPSS_CPU_PORT_NUM_CNS == i) && (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE))
                       || (PRV_CPSS_GE_PORT_GE_ONLY_E ==
                                PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, i)))
                    {
                        continue;
                    }

                    macCtrl0Offset = 0;
                    macCtrl1Offset = 4;
                    macCtrl2Offset = 8;
                    macCtrl3Offset = 0x1C;
                    macCtrl4Offset = 0x84;
                    fcDsaTag2Offset = 0x70;
                    fcDsaTag3Offset = 0x74;
                    ccfcFcTimerOffset = 0x38;
                    ppfcCtrlOffset = 0x60;

                    if(PRV_CPSS_PORT_XG_E == portType)
                    {
                        /* MIB MAC counters */
                        addrPtr->macRegs.perPortRegs[i].macCounters = mibBaseAddr + mibOffset;
                        addrPtr->macRegs.perPortRegs[i].xgMibCountersCtrl = xgBaseAddr + 0x30 + offset;
                    }
                    else if (PRV_CPSS_PORT_XLG_E == portType)
                    {
                        /* configure XLG mib counters control only for port 9 and 11 */
                        /* these ports use diffrent mac number in XLG mode*/
                        if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
                        {
                            /* mask port on '0xf' is relevant to multi port groups only */
                            if ( ((i & 0xf) == 9) || ((i & 0xf) == 11))
                            {
                                addrPtr->macRegs.perPortRegs[i].xlgMibCountersCtrl = xgBaseAddr + 0x30 + offset;
                            }
                        }
                        if ((i & 0xf) == 8) /* this register has meaning only on this address */
                        {
                            addrPtr->macRegs.perPortRegs[i].macroControl = xgBaseAddr + 0x94 + offset;
                        }
                        if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                        {
                            addrPtr->macRegs.perPortRegs[i].xlgPortFIFOsThresholdsConfig =
                                                            xgBaseAddr + 0x10 + offset;
                        }
                    }

                    /* not relevant for GE */
                    addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].pcs40GCommonCtrl =
                                                                        xgBaseAddr + 0x414 + offset;
                    addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].pcs40GCommonStatus =
                                                                        xgBaseAddr + 0x430 + offset;
                    addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].pcs40GGearBoxStatus =
                                                                        xgBaseAddr + 0x4D0 + offset;
                    addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].pcs40GFecDecStatus =
                                                                        xgBaseAddr + 0x4E8 + offset;
                    addrPtr->macRegs.perPortRegs[i].xlgExternalControlRegister = xgBaseAddr + 0x90 + offset;

                    baseAddrToUse = xgBaseAddr;
                    break;

                case PRV_CPSS_PORT_HGL_E:

                    if((CPSS_CPU_PORT_NUM_CNS == i)
                       || (PRV_CPSS_GE_PORT_GE_ONLY_E ==
                                PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, i))
                       || (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
                    {
                        continue;
                    }

                    if((i & 0xf) == 9)
                    {
                        offset = 12 * 0x1000;
                    }
                    else if((i & 0xf) == 11)
                    {
                        offset = 14 * 0x1000;
                    }
                    else if((i&0xf)%2 == 0)
                    {
                        /* do nothing */
                    }
                    else
                    {/* not applicable for other odd ports */
                        continue;
                    }

                    addrPtr->macRegs.perPortRegs[i].hglMacDroppedRxCellCntrsReg0 = hglBaseAddr + 0x4C + offset;
                    addrPtr->macRegs.perPortRegs[i].hglMacDroppedRxCellCntrsReg1 = hglBaseAddr + 0x50 + offset;
                    addrPtr->macRegs.perPortRegs[i].hglMacConfigReg0 = hglBaseAddr + 0x18 + offset;
                    addrPtr->macRegs.perPortRegs[i].hglGlobalConfigReg0 = hglBaseAddr + 0x108 + offset;
                    addrPtr->macRegs.perPortRegs[i].hglGlobalConfigReg1 = hglBaseAddr + 0x10C + offset;
                    addrPtr->macRegs.perPortRegs[i].hglMacCellCounterTypeReg = hglBaseAddr + 0x154 + offset;
                    addrPtr->macRegs.perPortRegs[i].hglMacRxCellCounterReg = hglBaseAddr + 0x158 + offset;
                    for(j = 0; j < 3; j++)
                    {
                        addrPtr->macRegs.perPortRegs[i].hglMacSaReg[j] = hglBaseAddr + 0x118 + (0x4*j) + offset;
                    }

                    for(j = 0; j < 5; j++)
                    {
                        addrPtr->macRegs.perPortRegs[i].hglMacE2eConfigReg[j] = hglBaseAddr + 0x138 + (0x4*j) + offset;
                    }

                    for(j = 0; j < 4; j++)
                    {
                        addrPtr->macRegs.perPortRegs[i].hglMacLLFCConfigReg[j] = hglBaseAddr + 0x124 + (0x4*j) + offset;
                    }

                    baseAddrToUse = hglBaseAddr;
                    continue;

                default:
                    continue;
            }

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].macCtrl =
                baseAddrToUse + macCtrl0Offset + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].macCtrl1 =
                baseAddrToUse + macCtrl1Offset + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].macCtrl2 =
                baseAddrToUse + macCtrl2Offset + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].macCtrl3 =
                baseAddrToUse + macCtrl3Offset + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].macCtrl4 =
                baseAddrToUse + macCtrl4Offset + offset;

            if (portType == PRV_CPSS_PORT_XLG_E)
            {
                if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))   /* PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE */
                {
                    addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].macCtrl5 =
                        baseAddrToUse + macCtrl5Offset + offset;
                }
            }

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].fcDsaTag2 =
                baseAddrToUse + fcDsaTag2Offset + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].fcDsaTag3 =
                baseAddrToUse + fcDsaTag3Offset + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].ccfcFcTimer =
                baseAddrToUse + ccfcFcTimerOffset + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].ppfcControl =
                baseAddrToUse + ppfcCtrlOffset + offset;
        } /* end of for(portType = PRV_CPSS_PORT_GE_E; portType < PRV_CPSS_PORT_NOT_APPLICABLE_E; */

        if((CPSS_CPU_PORT_NUM_CNS == i) &&
           (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE))
        {
            continue;
        }

        addrPtr->macRegs.perPortRegs[i].externalUnitsIntmask = xgBaseAddr+0x5C;

        if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            /* all ports are valid */
            addrPtr->macRegs.perPortRegs[i].externalUnitsIntmask += i*0x1000;
        }
        else
        {
            addrPtr->macRegs.perPortRegs[i].externalUnitsIntmask += (i & 0xf)*0x1000;
        }

        if(!PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            /* configure XPCS addresses */
            if((i & 0xf) == 9)
            {
                offset = 6 * 0x2000;
            }
            else if((i & 0xf) == 11)
            {
                offset = 7 * 0x2000;
            }
            else
            {
                if((i & 0xf)%2 != 0)
                    continue;
                else
                    offset = ((i & 0xf)/2) * 0x2000;
            }
        }
        else
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E || 
           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E)
        {
            /* Bobcat3 has XPCS for even MACs >= 48
               Aldrin has only 33 MACs with offset formula like in Bobcat3 */
            if (i % 2)
            {
                continue;
            }

            if (i >= 56)
            {
                offset = 0x200000 + (i - 56) * 0x1000;
            }
            else
            {
                offset = i * 0x1000;
            }
        }
        else
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
        {
            /* Bobcat2 has XPCS for even MACs >= 48*/
            if ((i < 48) || (i % 2))
            {
                continue;
            }

            if (i >= 56)
            {
                offset = 0x200000 + (i - 56) * 0x1000;
            }
            else
            {
                offset = i * 0x1000;
            }
        }
        else
        {
            /* not covered device family ? */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
        }

        if(PRV_CPSS_GE_PORT_GE_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, i))
        {
            /*do nothing - no XPCS */
        }
        else
        {
            addrPtr->macRegs.perPortRegs[i].xgGlobalConfReg0 = xpcsBaseAddr + 0x0 + offset;
            addrPtr->macRegs.perPortRegs[i].xgGlobalConfReg1 = xpcsBaseAddr + 0x4 + offset;
            addrPtr->macRegs.perPortRegs[i].xgXpcsGlobalStatus = xpcsBaseAddr + 0x10 + offset;
            addrPtr->macRegs.perPortRegs[i].xpcsGlobalMaxIdleCounter = xpcsBaseAddr + 0xC + offset;

            for(lane = 0; lane < 6; lane++)
            {
                xpcsLaneOffset = lane * 0x44;

                addrPtr->macRegs.perPortRegs[i].laneConfig0[lane]           =    xpcsBaseAddr + 0x50 + offset + xpcsLaneOffset;
                addrPtr->macRegs.perPortRegs[i].laneConfig1[lane]           =    xpcsBaseAddr + 0x54 + offset + xpcsLaneOffset;
                addrPtr->macRegs.perPortRegs[i].disparityErrorCounter[lane] =    xpcsBaseAddr + 0x6C + offset + xpcsLaneOffset;
                addrPtr->macRegs.perPortRegs[i].prbsErrorCounterLane[lane]  =    xpcsBaseAddr + 0x70 + offset + xpcsLaneOffset;
                addrPtr->macRegs.perPortRegs[i].laneStatus[lane]            =    xpcsBaseAddr + 0x5c + offset + xpcsLaneOffset;
                addrPtr->macRegs.perPortRegs[i].cyclicData[lane]            =    xpcsBaseAddr + 0x84 + offset + xpcsLaneOffset;
            }
        }

    } /* end of for (i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; */

    return GT_OK;
}

/*******************************************************************************
* cheetah3andAboveMacRegsInit
*
* DESCRIPTION:
*       This function initializes MAC,SERDESs related registers struct
*       for devices cheetah3 and upto xCat2 but not Lion2 and above.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3andAboveMacRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  lane;/*(serdes) lanes*/
    GT_U32  offset, offset1, gigBaseAddr, xgBaseAddr, xpcsBaseAddr, macCountersBaseAddr, fcaBaseAddr;
    GT_BOOL usesGigCounters,configureXg,configureGig,configureXlg;

    /* clear all previous settings per port registers */
    defaultAddressUnusedSet(addrPtr->macRegs.perPortRegs,
                            addrPtr->macRegs.swPerPortRegsSize/sizeof(GT_U32));

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        gigBaseAddr = 0x12000000;
        xgBaseAddr = 0x120C0000;
        xpcsBaseAddr = 0x12180400;
        macCountersBaseAddr = 0x11000000;
        fcaBaseAddr = 0x12180600;

    }
    else
    {
        gigBaseAddr = 0x0A800000;
        xgBaseAddr = 0x08800000;
        xpcsBaseAddr = 0x08800200;
        macCountersBaseAddr = 0x09000000;
        fcaBaseAddr = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    /* Init MAC registers   */
    for (i = 0; (i < PRV_CPSS_PP_MAC(devNum)->numOfPorts); i++)
    {
        /* if not existing port */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            if((i & 0xf) > 0xb)
            {
                continue;
            }
        }

        /*should we configure gig mac*/
        if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E &&
             i > 23) ||
             (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E &&
              i > 27))
        {
            configureGig = GT_FALSE;
        }
        else
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
                configureGig = (PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,i)
                                                    < PRV_CPSS_XG_PORT_XG_HX_QX_E) ?
                                                                GT_FALSE : GT_TRUE;
            }
            else
            {
                configureGig = (PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,i)
                                                    < PRV_CPSS_GE_PORT_GE_ONLY_E) ?
                                                                GT_FALSE : GT_TRUE;
            }
        }

        /*should we configure XG mac*/
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
        {
            /* Don't configure XG MAC for NOT_EXIST and GE/FE ports */
            configureXg = (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[i].portType < PRV_CPSS_PORT_XG_E) ?
                                                            GT_FALSE : GT_TRUE;
        }
        else
        {
            /*should we configure XG mac*/
            configureXg = (PRV_CPSS_GE_PORT_GE_ONLY_E ==
                            PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,i)) ?
                                                            GT_FALSE : GT_TRUE;
        }

        /* should we configure XLG mac*/
        if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) &&
            (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,i) == 10))
        {
            configureXlg = GT_TRUE;
        }
        else
        {
            configureXlg = GT_FALSE;
        }

        /* mac counters */
        if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E &&
             PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[i].portType >= PRV_CPSS_PORT_XG_E) ||
            ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E ||
              PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E) && i > 23) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
        {
            usesGigCounters = GT_FALSE;
        }
        else
        {
            usesGigCounters = GT_TRUE;
        }

        /* all gig specific */
        if (configureGig == GT_TRUE)
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                offset = (i & 0xf) * 0x400;
            }
            else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
                offset = i * 0x1000;
            }
            else
            {
                offset = i * 0x400;
            }

            /* Tri-Speed Ports MAC */
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl =
                    gigBaseAddr + 0x0 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl1 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 =
                    gigBaseAddr + 0x4 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl2 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 =
                    gigBaseAddr + 0x8 + offset;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl3 =
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3 =
                    gigBaseAddr + 0x48 + offset;

            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].fcDsaTag2 =
                    addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].fcDsaTag2 =
                        gigBaseAddr + 0x80 + offset;

                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].fcDsaTag3 =
                    addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].fcDsaTag3 =
                        gigBaseAddr + 0x84 + offset;
            }

            if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
                addrPtr->macRegs.perPortRegs[i].ccfcFcTimerBaseGig = gigBaseAddr + 0x58 + offset;
                addrPtr->macRegs.perPortRegs[i].Config100Fx = gigBaseAddr + 0x2C + offset;
            }

            /* Port Status */
            addrPtr->macRegs.perPortRegs[i].macStatus = gigBaseAddr + 0x10 + offset;
            addrPtr->macRegs.perPortRegs[i].macStatus1 = gigBaseAddr + 0x40 + offset;

            /* Port Serial Parameters Configuration */
            addrPtr->macRegs.perPortRegs[i].serialParameters = gigBaseAddr + 0x14 + offset;
            addrPtr->macRegs.perPortRegs[i].serdesCnfg       = gigBaseAddr + 0x28 + offset;

            /* Port Auto-Negotiation Configuration */
            addrPtr->macRegs.perPortRegs[i].autoNegCtrl      = gigBaseAddr + 0xC + offset;

            /* PRBS Check Status */
            addrPtr->macRegs.perPortRegs[i].prbsCheckStatus  = gigBaseAddr + 0x38 + offset;
            addrPtr->macRegs.perPortRegs[i].prbsErrorCounter = gigBaseAddr + 0x3C + offset;

            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_FE_E].macCtrl4 =
                    addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl4 =
                                                                        gigBaseAddr + 0x90 + offset;
                addrPtr->macRegs.perPortRegs[i].serdesCnfg        = gigBaseAddr + 0x28 + offset;
                addrPtr->macRegs.perPortRegs[i].serdesCnfg3       = gigBaseAddr + 0x34 + offset;
                if(PRV_CPSS_PP_MAC(devNum)->revision > 4)
                {
                    if(((i%4) != 0) && (i < 24))
                    {
                        addrPtr->macRegs.perPortRegs[i].serdesCnfg =
                            PRV_CPSS_SW_PTR_ENTRY_UNUSED;  /* txConfigCtrlReg in XCAT2 B1 */
                        addrPtr->macRegs.perPortRegs[i].serdesCnfg3 =
                            PRV_CPSS_SW_PTR_ENTRY_UNUSED;
                    }
                }
                addrPtr->macRegs.perPortRegs[i].serialParameters1 = gigBaseAddr + 0x94 + offset;
            }

            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl4 =
                                                                    gigBaseAddr + 0x90 + offset;
                addrPtr->macRegs.perPortRegs[i].mibCountersCtrl = gigBaseAddr + 0x44 + offset;
            }
        }

        if (usesGigCounters == GT_TRUE)
        {
            addrPtr->macRegs.perPortRegs[i].macCounters = ((i < 24)
             ? (0x04010000 + ((i / 6) * 0x0800000) + ((i % 6) * 0x0080))
             : (0x01C00000 + ((i - 24) * 0x040000)));
            addrPtr->macRegs.perPortRegs[i].macCaptureCounters = ((i < 24)
             ? (0x04010300 + ((i / 6) * 0x0800000))
             : (0x01C00080 + ((i - 24) * 0x040000))) ;
        }

        /* all XG specific */
        if (configureXg == GT_TRUE)
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                offset = (i & 0xf) * 0x400;
            }
            else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
                offset = i * 0x1000;
            }
            else
            {
                offset = i * 0x400;
            }

            /* XG MAC Registers */
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl =
                xgBaseAddr + 0x0 + offset;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl1 =
                xgBaseAddr + 0x4 + offset;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl2 =
                xgBaseAddr + 0x8 + offset;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl3 =
                xgBaseAddr + 0x1C + offset;

            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
                /* XLG MAC Registers */
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl =
                    xgBaseAddr + 0x0 + offset;
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl1 =
                    xgBaseAddr + 0x4 + offset;
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl2 =
                    xgBaseAddr + 0x8 + offset;
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl3 =
                    xgBaseAddr + 0x1C + offset;
            }

            if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].miscConfig =
                    xgBaseAddr + 0x28 + offset;
            }

            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E ||
                PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].fcDsaTag2 =
                    xgBaseAddr + 0x70 + offset;
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].fcDsaTag3 =
                    xgBaseAddr + 0x74 + offset;

                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl4 =
                    xgBaseAddr + 0x84 + offset;
            }

            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl4 =
                    xgBaseAddr + 0x84 + offset;

                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl5 =
                    xgBaseAddr + 0x88 + offset;

                addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl5 =
                    xgBaseAddr + 0x88 + offset;
            }

            /* Mac status registers     */
            addrPtr->macRegs.perPortRegs[i].macStatus = xgBaseAddr + 0x0C + offset;

            addrPtr->macRegs.perPortRegs[i].xgMibCountersCtrl = xgBaseAddr + 0x30 + offset;

            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            {
                addrPtr->macRegs.perPortRegs[i].metalFix = xgBaseAddr + 0x240 + offset;
            }

            if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
                addrPtr->macRegs.perPortRegs[i].ccfcFcTimerBaseXg = xgBaseAddr + 0x38 + offset;
            }

            /* No XPCS for Xcat3 25 & 27 ports */
            if(( i != 25 && i != 27 ) ||
               (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E))
            {
            /* all xpcs */
                addrPtr->macRegs.perPortRegs[i].xgGlobalConfReg0 = xpcsBaseAddr + 0x0 + offset;

                if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
                    || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
                {
                    addrPtr->macRegs.perPortRegs[i].xgXpcsGlobalStatus = xpcsBaseAddr + 0x10 + offset;
                }

                for(lane = 0; lane < 4; lane++)
                {
                    offset1 = lane * 0x044;

                    addrPtr->macRegs.perPortRegs[i].laneConfig0[lane]           = xpcsBaseAddr + 0x50 + offset + offset1;
                    addrPtr->macRegs.perPortRegs[i].laneConfig1[lane]           = xpcsBaseAddr + 0x54 + offset + offset1;
                    addrPtr->macRegs.perPortRegs[i].disparityErrorCounter[lane] = xpcsBaseAddr + 0x6C + offset + offset1;
                    addrPtr->macRegs.perPortRegs[i].prbsErrorCounterLane[lane]  = xpcsBaseAddr + 0x70 + offset + offset1;
                    addrPtr->macRegs.perPortRegs[i].laneStatus[lane]            = xpcsBaseAddr + 0x5c + offset + offset1;
                    addrPtr->macRegs.perPortRegs[i].cyclicData[lane]            = xpcsBaseAddr + 0x84 + offset + offset1;
                }
            }
        }

        /* SERDES configuration for CH3 only*/
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
        {
            /* Only for DxCh3 devices */
            if(i < 24)
            {
                /* group number = i / 4 */
                offset = (i / 4) * 0x1000;
                addrPtr->macRegs.perPortRegs[i].serdesPowerUp1[0] = 0x09800004 + offset;
                addrPtr->macRegs.perPortRegs[i].serdesPowerUp2[0] = 0x09800008 + offset;
                addrPtr->macRegs.perPortRegs[i].serdesReset[0]    = 0x0980000C + offset;
                addrPtr->macRegs.perPortRegs[i].serdesMetalFix    = 0x09800078 + i*0x400;

                if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[i].portType !=
                   PRV_CPSS_PORT_XG_E)
                {
                    offset = i * 0x400;
                    addrPtr->macRegs.perPortRegs[i].serdesCommon[0]        = 0x09800000 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesTxSyncConf[0]    = 0x09800010 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesSpeed1[0]        = 0x09800014 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesSpeed2[0]        = 0x09800018 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesTxConfReg1[0]    = 0x0980001C + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesTxConfReg2[0]    = 0x09800020 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesRxConfReg1[0]    = 0x09800028 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg5[0]   = 0x0980003C + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesIvrefConf1[0]    = 0x09800044 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesIvrefConf2[0]    = 0x09800048 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesMiscConf[0]      = 0x09800054 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesReservConf[0]    = 0x09800058 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesCalibConf1[0]    = 0x09800060 + offset;

                }
                else /* XG port */
                {
                    for( lane = 0 ; lane < 4 ; lane++ )
                    {
                        offset = (i - i%4 + lane) * 0x400;
                        addrPtr->macRegs.perPortRegs[i].serdesCommon[lane]     = 0x09800000 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesTxSyncConf[lane] = 0x09800010 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesSpeed1[lane]     = 0x09800014 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesSpeed2[lane]     = 0x09800018 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesTxConfReg1[lane] = 0x0980001C + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesTxConfReg2[lane] = 0x09800020 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesRxConfReg1[lane] = 0x09800028 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg5[lane]= 0x0980003C + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesIvrefConf1[lane] = 0x09800044 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesIvrefConf2[lane] = 0x09800048 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesMiscConf[lane]   = 0x09800054 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesReservConf[lane] = 0x09800058 + offset;
                        addrPtr->macRegs.perPortRegs[i].serdesCalibConf1[lane] = 0x09800060 + offset;
                    }
                }
            }
            else /* 4 Stack ports 24 - 27 */
            {
                offset = ((i - 24) + 8) * 0x1000;
                addrPtr->macRegs.perPortRegs[i].serdesPowerUp1[0] = 0x09800004 + offset;
                addrPtr->macRegs.perPortRegs[i].serdesPowerUp2[0] = 0x09800008 + offset;
                addrPtr->macRegs.perPortRegs[i].serdesReset[0]    = 0x0980000C + offset;

                for( lane = 0 ; lane < 4 ; lane++ )
                {
                    offset = (32 + (i - 24) * 4 + lane) * 0x0400;
                    addrPtr->macRegs.perPortRegs[i].serdesSpeed1[lane]     = 0x09800014 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesSpeed2[lane]     = 0x09800018 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesSpeed3[lane]     = 0x0980001C + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesTxConfReg1[lane] = 0x09800020 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesTxConfReg2[lane] = 0x09800024 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesRxConfReg1[lane] = 0x09800028 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesRxConfReg2[lane] = 0x0980002C + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg2[lane] = 0x0980004c + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesDtlConfReg3[lane] = 0x09800050 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesIvrefConf1[lane]  = 0x09800064 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesProcessMon[lane] = 0x0980006C + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesMiscConf[lane]   = 0x09800074 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesReservConf[lane] = 0x09800078 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesCalibConf1[lane] = 0x09800080 + offset;
                    addrPtr->macRegs.perPortRegs[i].serdesCalibConf2[lane] = 0x09800084 + offset;
                }
            }
        }

        /* all XLG specific */
        if (configureXlg == GT_TRUE)
        {
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl =
                0x08803000;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl1 =
                0x08803004;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl2 =
                0x08803008;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl3 =
                0x0880301C;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].miscConfig =
                0x08803060;

            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].fcDsaTag2 =
                0x08803070;
            addrPtr->macRegs.perPortRegs[i].macRegsPerType[PRV_CPSS_PORT_XLG_E].fcDsaTag3 =
                0x08803074;

            addrPtr->macRegs.perPortRegs[i].xlgMibCountersCtrl = 0x08803030;
            addrPtr->xlgRegs.pcs40GRegs.commonCtrl = 0x8803214;
            addrPtr->xlgRegs.pcs40GRegs.commonStatus = 0x08803230;
            addrPtr->xlgRegs.pcs40GRegs.gearBoxStatus = 0x088033D0;
        }

        if (usesGigCounters == GT_FALSE)
        {
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ||
               (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
            {
                offset = (i & 0xf) * 0x20000;
#ifdef GM_USED
                if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                {
                    offset = i * 0x400;
                }
#endif
            }
            else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
                offset = i * 0x400;
            }
            else
            {
                offset = i * 0x20000;
            }

            /* MIB MAC counters */
            addrPtr->macRegs.perPortRegs[i].macCounters        = macCountersBaseAddr + offset;
            if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
                (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
                (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
            {
                addrPtr->macRegs.perPortRegs[i].macCaptureCounters = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            }
            else
            {
                addrPtr->macRegs.perPortRegs[i].macCaptureCounters = macCountersBaseAddr + 0x80 + offset;
            }

            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                addrPtr->macRegs.perPortRegs[i].xgMibCountersCtrl  = gigBaseAddr + 0x44 + (i * 0x400);
            }
        }

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            GT_U32  p;

            offset = 0x1000*i;

            { /*0018060c+p*0x4*/
                for (p = 0; p <= 7; p++)
                {
                    addrPtr->macRegs.perPortRegs[i].FCA.LLFCDatabase[p] = fcaBaseAddr + offset +
                        0xc + p * 0x4;
                } /* end of loop p */
            } /*0018060c+i*0x4*/

            { /*0018062c+p*0x4*/
                GT_U32    p;
                for (p = 0; p <= 7; p++)
                {
                    addrPtr->macRegs.perPortRegs[i].FCA.portSpeedTimer[p] = fcaBaseAddr + offset +
                        0x2c + p * 0x4;
                } /* end of loop p */
            } /*0018062c+p*0x4*/
            { /*00180684+0x4*p*/
                GT_U32    p;
                for (p = 0; p <= 7; p++)
                {
                    addrPtr->macRegs.perPortRegs[i].FCA.lastSentTimer[p] = fcaBaseAddr + offset +
                        0x84 + 0x4 * p;
                } /* end of loop p */
            } /*00180684+0x4*p*/
            addrPtr->macRegs.perPortRegs[i].FCA.FCACtrl               = fcaBaseAddr + offset + 0x0;
            addrPtr->macRegs.perPortRegs[i].FCA.DBReg0                = fcaBaseAddr + offset + 0x4;
            addrPtr->macRegs.perPortRegs[i].FCA.DBReg1                = fcaBaseAddr + offset + 0x8;
            addrPtr->macRegs.perPortRegs[i].FCA.LLFCFlowCtrlWindow0   = fcaBaseAddr + offset + 0x4c;
            addrPtr->macRegs.perPortRegs[i].FCA.LLFCFlowCtrlWindow1   = fcaBaseAddr + offset + 0x50;
            addrPtr->macRegs.perPortRegs[i].FCA.MACDA0To15            = fcaBaseAddr + offset + 0x54;
            addrPtr->macRegs.perPortRegs[i].FCA.MACDA16To31           = fcaBaseAddr + offset + 0x58;
            addrPtr->macRegs.perPortRegs[i].FCA.MACDA32To47           = fcaBaseAddr + offset + 0x5c;
            addrPtr->macRegs.perPortRegs[i].FCA.MACSA0To15            = fcaBaseAddr + offset + 0x60;
            addrPtr->macRegs.perPortRegs[i].FCA.MACSA16To31           = fcaBaseAddr + offset + 0x64;
            addrPtr->macRegs.perPortRegs[i].FCA.MACSA32To47           = fcaBaseAddr + offset + 0x68;
            addrPtr->macRegs.perPortRegs[i].FCA.L2CtrlReg0            = fcaBaseAddr + offset + 0x6c;
            addrPtr->macRegs.perPortRegs[i].FCA.L2CtrlReg1            = fcaBaseAddr + offset + 0x70;
            addrPtr->macRegs.perPortRegs[i].FCA.DSATag0To15           = fcaBaseAddr + offset + 0x74;
            addrPtr->macRegs.perPortRegs[i].FCA.DSATag16To31          = fcaBaseAddr + offset + 0x78;
            addrPtr->macRegs.perPortRegs[i].FCA.DSATag32To47          = fcaBaseAddr + offset + 0x7c;
            addrPtr->macRegs.perPortRegs[i].FCA.DSATag48To63          = fcaBaseAddr + offset + 0x80;
            addrPtr->macRegs.perPortRegs[i].FCA.PFCPriorityMask       = fcaBaseAddr + offset + 0x104;
            addrPtr->macRegs.perPortRegs[i].FCA.sentPktsCntrLSb       = fcaBaseAddr + offset + 0x108;
            addrPtr->macRegs.perPortRegs[i].FCA.sentPktsCntrMSb       = fcaBaseAddr + offset + 0x10c;
            addrPtr->macRegs.perPortRegs[i].FCA.periodicCntrLSB       = fcaBaseAddr + offset + 0x110;
            addrPtr->macRegs.perPortRegs[i].FCA.periodicCntrMSB       = fcaBaseAddr + offset + 0x114;
        }
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        /* set the Ethernet CPU port registers -- port 63 */
        offset = 63 * 0x400;
        addrPtr->macRegs.cpuPortRegs.macStatus     = gigBaseAddr + 0x10 + offset;
        addrPtr->macRegs.cpuPortRegs.macStatus1    = gigBaseAddr + 0x40 + offset;

        addrPtr->macRegs.cpuPortRegs.autoNegCtrl   = gigBaseAddr + 0xC + offset;

        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl  =
            gigBaseAddr + 0x0 + offset;
        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 =
            gigBaseAddr + 0x4 + offset;
        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 =
            gigBaseAddr + 0x8 + offset;
        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3 =
            gigBaseAddr + 0x48 + offset;

        addrPtr->macRegs.cpuPortRegs.serialParameters =
            gigBaseAddr + 0x14 + offset;
    }
    else /* PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E */
    {
        GT_U32  p;

        /* set the Ethernet CPU port registers -- port 31 */
        offset = 31 * 0x1000;

        addrPtr->macRegs.cpuPortRegs.macStatus = gigBaseAddr + 0x10 + offset;
        addrPtr->macRegs.cpuPortRegs.macStatus1 = gigBaseAddr + 0x40 + offset;

        addrPtr->macRegs.cpuPortRegs.autoNegCtrl = gigBaseAddr + 0xC + offset;
        addrPtr->macRegs.cpuPortRegs.serialParameters = gigBaseAddr + 0x14 + offset;
        addrPtr->macRegs.cpuPortRegs.serialParameters1 = gigBaseAddr + 0x94 + offset;

        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl  = gigBaseAddr + 0x0 + offset;
        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 = gigBaseAddr + 0x4 + offset;
        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 = gigBaseAddr + 0x8 + offset;
        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3 = gigBaseAddr + 0x48 + offset;
        addrPtr->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl4 = gigBaseAddr + 0x90 + offset;

        addrPtr->macRegs.cpuPortRegs.macCounters    = macCountersBaseAddr + 31 * 0x400;


        /*******************/
        /* FCA unit update */
        /*******************/
        { /*0018060c+p*0x4*/
            for (p = 0; p <= 7; p++)
            {
                addrPtr->macRegs.cpuPortRegs.FCA.LLFCDatabase[p] = fcaBaseAddr + offset +
                    0xc + p * 0x4;
            } /* end of loop p */
        } /*0018060c+i*0x4*/

        { /*0018062c+p*0x4*/
            for (p = 0; p <= 7; p++)
            {
                addrPtr->macRegs.cpuPortRegs.FCA.portSpeedTimer[p] = fcaBaseAddr + offset +
                    0x2c + p * 0x4;
            } /* end of loop p */
        } /*0018062c+p*0x4*/
        { /*00180684+0x4*p*/
            for (p = 0; p <= 7; p++)
            {
                addrPtr->macRegs.cpuPortRegs.FCA.lastSentTimer[p] = fcaBaseAddr + offset +
                    0x84 + 0x4 * p;
            } /* end of loop p */
        } /*00180684+0x4*p*/
        addrPtr->macRegs.cpuPortRegs.FCA.FCACtrl               = fcaBaseAddr + offset + 0x0;
        addrPtr->macRegs.cpuPortRegs.FCA.DBReg0                = fcaBaseAddr + offset + 0x4;
        addrPtr->macRegs.cpuPortRegs.FCA.DBReg1                = fcaBaseAddr + offset + 0x8;
        addrPtr->macRegs.cpuPortRegs.FCA.LLFCFlowCtrlWindow0   = fcaBaseAddr + offset + 0x4c;
        addrPtr->macRegs.cpuPortRegs.FCA.LLFCFlowCtrlWindow1   = fcaBaseAddr + offset + 0x50;
        addrPtr->macRegs.cpuPortRegs.FCA.MACDA0To15            = fcaBaseAddr + offset + 0x54;
        addrPtr->macRegs.cpuPortRegs.FCA.MACDA16To31           = fcaBaseAddr + offset + 0x58;
        addrPtr->macRegs.cpuPortRegs.FCA.MACDA32To47           = fcaBaseAddr + offset + 0x5c;
        addrPtr->macRegs.cpuPortRegs.FCA.MACSA0To15            = fcaBaseAddr + offset + 0x60;
        addrPtr->macRegs.cpuPortRegs.FCA.MACSA16To31           = fcaBaseAddr + offset + 0x64;
        addrPtr->macRegs.cpuPortRegs.FCA.MACSA32To47           = fcaBaseAddr + offset + 0x68;
        addrPtr->macRegs.cpuPortRegs.FCA.L2CtrlReg0            = fcaBaseAddr + offset + 0x6c;
        addrPtr->macRegs.cpuPortRegs.FCA.L2CtrlReg1            = fcaBaseAddr + offset + 0x70;
        addrPtr->macRegs.cpuPortRegs.FCA.DSATag0To15           = fcaBaseAddr + offset + 0x74;
        addrPtr->macRegs.cpuPortRegs.FCA.DSATag16To31          = fcaBaseAddr + offset + 0x78;
        addrPtr->macRegs.cpuPortRegs.FCA.DSATag32To47          = fcaBaseAddr + offset + 0x7c;
        addrPtr->macRegs.cpuPortRegs.FCA.DSATag48To63          = fcaBaseAddr + offset + 0x80;
        addrPtr->macRegs.cpuPortRegs.FCA.PFCPriorityMask       = fcaBaseAddr + offset + 0x104;
        addrPtr->macRegs.cpuPortRegs.FCA.sentPktsCntrLSb       = fcaBaseAddr + offset + 0x108;
        addrPtr->macRegs.cpuPortRegs.FCA.sentPktsCntrMSb       = fcaBaseAddr + offset + 0x10c;
        addrPtr->macRegs.cpuPortRegs.FCA.periodicCntrLSB       = fcaBaseAddr + offset + 0x110;
        addrPtr->macRegs.cpuPortRegs.FCA.periodicCntrMSB       = fcaBaseAddr + offset + 0x114;

    }

    /* QSGMII Control and Status */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E )
    {
        for(i=0 ; i < 6 ; i++)
        {
            addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister0 = 0x0a80004c + 4*i*0x400;
            addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIStatusRegister   = 0x0a800050 + 4*i*0x400;
            addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIPRBSErrorCounter = 0x0a800054 + 4*i*0x400;
            addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister1 = 0x0a80044c + 4*i*0x400;
            addrPtr->macRegs.QSGMIIControlAndStatus[i].QSGMIIControlRegister2 = 0x0a80084c + 4*i*0x400;
        }
    }
}

/*******************************************************************************
* cheetah3IpRegsInit
*
* DESCRIPTION:
*       This function initializes the IP registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IP regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3IpRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i;
    GT_U32  offset;

    /* Router Next Hop Table Age Bits */
    addrPtr->ipRegs.ucMcRouteEntriesAgeBitsBase = 0x02801000;

     /* Tcam bank addresses */
    for (i = 0 ; i <= 3; i++)
        addrPtr->ipRegs.routerTtTcamBankAddr[i] = 0x02A00000 + (i * 0x14000);


    /* IP Hit Log registers */
    for (i = 0; i < 4; i++)
    {
        offset = i * 0x4;
        addrPtr->ipRegs.ipHitLog.dipAddrReg[i]     = 0x02800D00 + offset;
        addrPtr->ipRegs.ipHitLog.dipMaskAddrReg[i] = 0x02800D10 + offset;
        addrPtr->ipRegs.ipHitLog.sipAddrReg[i]     = 0x02800D20 + offset;
        addrPtr->ipRegs.ipHitLog.sipMaskAddrReg[i] = 0x02800D30 + offset;
    }

    addrPtr->ipRegs.ipHitLog.layer4DestPortReg = 0x02800D40;
    addrPtr->ipRegs.ipHitLog.layer4SrcPortReg = 0x02800D44;
    addrPtr->ipRegs.ipHitLog.protocolReg = 0x02800D48;

    for (i = 0; i < 3; i++)
    {
        offset = i * 0x4;
        addrPtr->ipRegs.ipHitLog.dstIpNheEntryReg[i] = 0x02800D50 + offset;
        addrPtr->ipRegs.ipHitLog.srcIpNheEntryReg[i] = 0x02800D5C + offset;
    }

    /* TCAM low level HW configuration */
    addrPtr->ipRegs.ipTcamConfig0       = 0x0280096C;
    addrPtr->ipRegs.ipTcamControl       = 0x02800960;

    /* Router TCAM Test */
    for (i = 0; i < 20; i++)
    {
        offset = i * 0x4;
        addrPtr->ipRegs.routerTcamTest.pointerRelReg[i] = 0x02802000 + offset;
    }

    addrPtr->ipRegs.routerTcamTest.configStatusReg = 0x02802050;
    addrPtr->ipRegs.routerTcamTest.opcodeCommandReg = 0x02802054;
    addrPtr->ipRegs.routerTcamTest.opcodeExpectedHitReg = 0x02802058;

    addrPtr->ipRegs.ucRoutingEngineConfigurationReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ipRegs.routerPerPortSipSaEnable0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetah2BridgeRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2BridgeRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  j;
    addrPtr->bridgeRegs.nstRegs.brgAccessMatrix = 0x02000600;
    addrPtr->bridgeRegs.nstRegs.brgSecureLevelConfReg      = 0x0600003C;
    addrPtr->bridgeRegs.nstRegs.cpuIngrFrwFltConfReg = 0x0B020000;
    addrPtr->bridgeRegs.nstRegs.netIngrFrwFltConfReg = 0x0B020004;
    addrPtr->bridgeRegs.nstRegs.analyzerIngrFrwFltConfReg = 0x0B020008;
    addrPtr->bridgeRegs.nstRegs.cpuPktsFrwFltConfReg = 0x018001D8;
    addrPtr->bridgeRegs.nstRegs.brgPktsFrwFltConfReg = 0x018001DC;
    addrPtr->bridgeRegs.nstRegs.routePktsFrwFltConfReg = 0x018001E0;
    addrPtr->bridgeRegs.nstRegs.ingressFrwDropCounter = 0x0B02000C;

    addrPtr->bridgeRegs.srcIdReg.srcIdAssignedModeConfReg = 0x02040070;
    addrPtr->bridgeRegs.srcIdReg.srcIdUcEgressFilterConfReg = 0x018001D4;


    addrPtr->bridgeRegs.egressFilterUnregBc = 0x018001f4;


    /* UDP BC destination port config. 12 registers */
    for (j = 0; j < 12; j++)
    {
        addrPtr->bridgeRegs.udpBcDestPortConfigReg[j] = 0x02000500 + j*0x4;
    }

    addrPtr->bridgeRegs.trunkTable = 0x0B009000;


    /* TO ANALYZER VLAN Adding Configuration Registers */
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.toAnalyzerVlanAddConfig,0x07800404);

    /* Ingress Analyzer VLAN Tag Configuration Register */
    addrPtr->haRegs.ingAnalyzerVlanTagConfig = 0x07800408;

    /* Egress Analyzer VLAN Tag Configuration Register */
    addrPtr->haRegs.egrAnalyzerVlanTagConfig = 0x0780040C;


    /* Vlan MPU profiles confog reg  */
    addrPtr->bridgeRegs.vlanMruProfilesConfigReg = 0x02000300;
}


/*******************************************************************************
* cheetah2GlobalRegsInit
*
* DESCRIPTION:
*       This function initializes the global registers struct for devices that
*       belong to REGS_FAMILY_1 registers set.-- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with global regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2GlobalRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->globalRegs.fuQBaseAddr = 0xC8;
    addrPtr->globalRegs.fuQControl = 0xCC;
    addrPtr->globalRegs.lastReadTimeStampReg = 0x0000002C;
    addrPtr->globalRegs.sampledAtResetReg = 0x00000028;
}

/*******************************************************************************
* cheetah2BufMngRegsInit
*
* DESCRIPTION:
*       This function initializes the buffer management registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.-- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - device number
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2BufMngRegsInit
(
    IN GT_U32    devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* init CPU Code related registers */
    addrPtr->bufferMng.eqBlkCfgRegs.cpuRateLimiterConfBase = 0x0B500000;
    addrPtr->bufferMng.eqBlkCfgRegs.cpuRateLimiterPktCntrBase = 0x0B080004;
    addrPtr->bufferMng.eqBlkCfgRegs.cpuRateLimiterDropCntrReg = 0x0B000068;
    addrPtr->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal = 0x0B050000;
    addrPtr->bufferMng.eqBlkCfgRegs.dupPktsToCpuConfReg = 0x0B000018;
    addrPtr->bufferMng.eqBlkCfgRegs.ieeeResMcstCpuCodeIndexBase = 0x02000804;
    addrPtr->bufferMng.eqBlkCfgRegs.ipv4McstLinkLocalCpuCodeIndexBase =  0x02030800;
    addrPtr->bufferMng.eqBlkCfgRegs.ipv6McstLinkLocalCpuCodeIndexBase =  0x02040800;
    addrPtr->bufferMng.eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord0Base = 0x0B007000;
    addrPtr->bufferMng.eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord1Base = 0x0B007040;
    addrPtr->bufferMng.eqBlkCfgRegs.ipProtCpuCodeBase = 0x0B008000;
    addrPtr->bufferMng.eqBlkCfgRegs.ipProtCpuCodeValid = 0x0B008010;
    addrPtr->bufferMng.bufMngSharedBufConfigReg = 0x03000014;
    if(PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_LION_E)
    {
        addrPtr->bufferMng.buffMemTxFifoThreshMPPMAccessArb = 0x780006C;
    }
}

/*******************************************************************************
* cheetah2EgrTxRegsInit
*
* DESCRIPTION:
*       This function initializes the Egress and Transmit (Tx) Queue
*       Configuration registers struct for devices that belong to
*       REGS_FAMILY_1 registers set. -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - device number
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2EgrTxRegsInit
(
    IN GT_U32    devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i,j;
    GT_U32  offset;

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return;
    }

    /* init all arrays of per profiles registers */
    /* cheetah 2 has extra profiles */
    for (i = PRV_CPSS_DXCH_PROFILES_NUM_CNS, j = 0;
         i < PRV_CPSS_DXCH2_PROFILES_NUM_CNS;
         i++, j++)
    {
        /* WRR output algorithm configured */
        /* via 4 Transmit scheduler  profiles */
        offset = j * 0x10;
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights0 =   0x01800F00 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].wrrWeights1 =   0x01800F04 + offset;
        addrPtr->egrTxQConf.txPortRegs[i].wrrStrictPrio = 0x01800F08 + offset;
    }

    addrPtr->egrTxQConf.cncCountMode = 0x018000A0;

    /* Tail drop Limits: */
    /* cheetah 2 has extra profiles */
    for (j = PRV_CPSS_DXCH_PROFILES_NUM_CNS;
         j < PRV_CPSS_DXCH2_PROFILES_NUM_CNS;
         j++)
    {
        offset =  (j - PRV_CPSS_DXCH_PROFILES_NUM_CNS)*4;
        addrPtr->egrTxQConf.setsConfigRegs.portLimitsConf[j] = 0x018000C0 + offset;

        for (i = 0; i < 8; i++)
        {
            offset = 0x0400 * (j - PRV_CPSS_DXCH_PROFILES_NUM_CNS) + (i * 0x08);
            addrPtr->egrTxQConf.setsConfigRegs.tcDp0Red[i][j] = 0x01941000 + offset;
            addrPtr->egrTxQConf.setsConfigRegs.tcDp1Red[i][j] = 0x01941004 + offset;
        }
    }

    for (i = 0; i < 2; i++)
    {
        /* egress forwarding dropped packets counter register Cheetah2 only */
        addrPtr->egrTxQConf.txQCountSet[i].egrFrwDropPkts  = 0x01B4015C + i*0x20;
    }

    addrPtr->egrTxQConf.txQueueResSharingAndTunnelEgrFltr = 0x018001F0;
}

/*******************************************************************************
* cheetah2MacRegsInit
*
* DESCRIPTION:
*       This function initializes MAC related registers struct
*       for devices that belong to REGS_FAMILY_1 registers set. - cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2MacRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.invalidCrcModeConfigReg,0x07800400);

    /* the XG port number 27 has different value then the formula of other
       XG ports !!!
    */
    if(PRV_CPSS_PP_MAC(devNum)->numOfPorts >= 28 &&
       (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E) &&
       (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E) &&
       (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE))
    {
        addrPtr->macRegs.perPortRegs[27].macCounters = 0x01DC0000;
    }
}

/*******************************************************************************
* cheetah2Ipv4RegsInit
*
* DESCRIPTION:
*       This function initializes the IPv4 registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2Ipv4RegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ipv4Regs.ctrlReg0 = 0x02800100;
    addrPtr->ipv4Regs.ctrlReg1 = 0x02800104;
    addrPtr->ipv4Regs.ucEnCtrlReg = 0x02800108;
    addrPtr->ipv4Regs.mcEnCtrlReg = 0x0280010C;
}

/*******************************************************************************
* cheetah2Ipv6RegsInit
*
* DESCRIPTION:
*       This function initializes the IPv6 registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv6 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2Ipv6RegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i;
    GT_U32  offset;

    addrPtr->ipv6Regs.ctrlReg0 = 0x02800200;
    addrPtr->ipv6Regs.ctrlReg1 = 0x02800204;
    addrPtr->ipv6Regs.ucEnCtrlReg = 0x02800208;
    addrPtr->ipv6Regs.mcEnCtrlReg = 0x0280020C;

    for (i = 0; i < 4 ; i++)
    {
        offset = i * 0x4;
        addrPtr->ipv6Regs.ucScopeTblRegs[i]    = 0x02800278 + offset;
        addrPtr->ipv6Regs.mcScopeTblRegs[i]    = 0x02800288 + offset;
        addrPtr->ipv6Regs.ipScopeRegs[i]       = 0x02800250 + offset;
        addrPtr->ipv6Regs.ipScopeLevel[i]      = 0x02800260 + offset;
    }
}

/*******************************************************************************
* cheetah2IpRegsInit
*
* DESCRIPTION:
*       This function initializes the IP registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IP regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2IpRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i;
    GT_U32  offset;

    addrPtr->ipRegs.routerGlobalReg = 0x02800000;

    for (i = 0 ; i < 4; i++)
        addrPtr->ipRegs.routerMtuCfgRegs[i] = 0x02800004 + (i * 0x4);

    for (i = 0 ; i <= 4; i++)
        addrPtr->ipRegs.routerTtTcamBankAddr[i] = 0x02A00000 + (i * 0x4000);

    addrPtr->ipRegs.routerTtTcamAccessCtrlReg = 0x0280041C;
    addrPtr->ipRegs.routerTtTcamAccessDataCtrlReg = 0x02800418;

    addrPtr->ipRegs.routerTtTcamAccessDataRegsBase = 0x02800400;

    addrPtr->ipRegs.ucMcRouteEntriesAgeBitsBase = 0x0280100C;

    for (i = 0 ; i < 4; i++)
    {
        offset = i * 0x100;
        addrPtr->ipRegs.routerMngCntSetCfgRegs[i]                  = 0x02800980 + offset;
        addrPtr->ipRegs.routerMngInUcPktCntSet[i]                  = 0x02800900 + offset;
        addrPtr->ipRegs.routerMngMcPktCntSet[i]                    = 0x02800904 + offset;
        addrPtr->ipRegs.routerMngInUcNonRoutedNonExcptPktCntSet[i] = 0x02800908 + offset;
        addrPtr->ipRegs.routerMngInUcNonRoutedExcptPktCntSet[i]    = 0x0280090C + offset;
        addrPtr->ipRegs.routerMngInMcNonRoutedNonExcptPktCntSet[i] = 0x02800910 + offset;
        addrPtr->ipRegs.routerMngInMcNonRoutedExcptCntSet[i]       = 0x02800914 + offset;
        addrPtr->ipRegs.routerMngInUcTrapMrrPktCntSet[i]           = 0x02800918 + offset;
        addrPtr->ipRegs.routerMngInMcTrapMrrPktCntSet[i]           = 0x0280091C + offset;
        addrPtr->ipRegs.routerMngInMcRPFFailCntSet[i]              = 0x02800920 + offset;
        addrPtr->ipRegs.routerMngOutUcPktCntSet[i]                 = 0x02800924 + offset;
    }

    for (i = 0 ; i < MAX_NUM_OF_QOS_PROFILE_CNS ; i++)
    {
        addrPtr->ipRegs.qoSProfile2RouteBlockOffsetMapTable[i] = 0x02800300 + (0x4 * i);
    }

    addrPtr->ipRegs.routerAccessMatrixBase = 0x02800440;

    addrPtr->ipRegs.routerBridgedPktExcptCnt = 0x02800940;
    addrPtr->ipRegs.routerDropCntCfgReg = 0x02800954;
    addrPtr->ipRegs.routerDropCnt = 0x02800950;
    addrPtr->ipRegs.routerInterruptCauseReg = 0x02800D00;
    addrPtr->ipRegs.routerInterruptMaskReg = 0x02800D04;
    addrPtr->ipRegs.routerAdditionalCtrlReg = 0x02800964;

    addrPtr->ipRegs.ucRoutingEngineConfigurationReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ipRegs.routerPerPortSipSaEnable0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* cheetah2IpMtRegsInit
*
* DESCRIPTION:
*       This function initializes the IP Multi Traffic registers struct for
*       devices that belong to REGS_FAMILY_1 registers set . -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah2IpMtRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 i;
    GT_U32 offset;

    addrPtr->ipMtRegs.mllGlobalReg = 0x0C800000;
    addrPtr->ipMtRegs.multiTargetRateShapingReg = 0x0C800210;
    addrPtr->ipMtRegs.mtUcSchedulerModeReg = 0x0C800214;
    addrPtr->ipMtRegs.mtTcQueuesSpEnableReg = 0x0C800200;
    addrPtr->ipMtRegs.mtTcQueuesWeightReg = 0x0C800204;
    addrPtr->ipMtRegs.mtTcQueuesGlobalReg = 0x0C800004;
    addrPtr->ipMtRegs.mllMcFifoFullDropCnt = 0x0C800984;
    for (i = 0 ; i < 8 ; i++)
    {
        addrPtr->ipMtRegs.qoSProfile2MTTCQueuesMapTable[i] = 0x0C800100 + 4 * i;
    }
    addrPtr->ipMtRegs.mllTableBase = 0x0C880000;
    for (i = 0 ; i < 2 ; i++)
    {
        offset = i * 0x100;
        addrPtr->ipMtRegs.mllOutInterfaceCfg[i]    = 0x0C800980 + offset;
        addrPtr->ipMtRegs.mllOutMcPktCnt[i]        = 0x0C800900 + offset;
    }
}

/*******************************************************************************
* cheetah2PclRegsInit
*
* DESCRIPTION:
*       This function initializes the Internal InLif Tables registers
*       struct for devices that belong to REGS_FAMILY_1 registers set. -- cheetah 2
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void cheetah2PclRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->pclRegs.tcamReadMaskBase            = 0x0B880008;
    addrPtr->pclRegs.tcamReadPatternBase         = 0x0B880000;
    addrPtr->pclRegs.tcamReadMaskControlBase     = 0x0B880000;
    addrPtr->pclRegs.tcamReadPatternControlBase  = 0x0B88000C;

    addrPtr->pclRegs.epclGlobalConfig        = 0x07C00000;
    /* bit per port registers */
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclCfgTblAccess,0x07C00004);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktNonTs,0x07C00008);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktTs,0x07C0000C);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToCpu,0x07C00010);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuData,0x07C00014);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktFromCpuControl,0x07C00018);
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->pclRegs.epclEnPktToAnalyzer,0x07C0001C);
    /* TCP/UDP port comparators (8 register sequences) */
    addrPtr->pclRegs.ipclTcpPortComparator0  = 0x0B800600;
    addrPtr->pclRegs.ipclUdpPortComparator0  = 0x0B800640;
    addrPtr->pclRegs.epclTcpPortComparator0  = 0x07C00100;
    addrPtr->pclRegs.epclUdpPortComparator0  = 0x07C00140;
}

/*******************************************************************************
* cheetah3GlobalRegsInit
*
* DESCRIPTION:
*       This function initializes the global registers struct for devices that
*       belong to REGS_FAMILY_1 registers set.-- cheetah 3
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with global regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3GlobalRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->globalRegs.lastReadTimeStampReg = 0x00000040;

    addrPtr->globalRegs.extendedGlobalControl = 0x0000005C;

    addrPtr->globalRegs.mppmPllConfig.mppmPllParams = 0x000000BC;
    addrPtr->globalRegs.mppmPllConfig.mppmPllCtrl   = 0x000000D0;
}

/*******************************************************************************
* cheetah3BuffMemoryRegsInit
*
* DESCRIPTION:
*       This function initializes the buffer memory register for cheetah 3 and above
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum     - device number
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with global regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void cheetah3BuffMemoryRegsInit
(
    IN GT_U32                                devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        addrPtr->addrSpace.buffMemBank0Write = 0x10000000;
        /* no second bank */
        addrPtr->addrSpace.buffMemBank1Write = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->addrSpace.buffMemWriteControl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        addrPtr->addrSpace.buffMemBank0Write = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->addrSpace.buffMemBank1Write = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->addrSpace.buffMemWriteControl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
    else
    {
        addrPtr->addrSpace.buffMemBank1Write   = 0x07100000;
        addrPtr->addrSpace.buffMemWriteControl = 0x07800000;
    }
}


/*******************************************************************************
* prvCpssDxCh3HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah-3 devices.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3HwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    /* first set the "Cheetah" part */
    rc = prvCpssDxCh2HwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the specific "cheetah 3" parts */
    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    cheetah3BridgeRegsInit(regsAddrPtr);
    cheetah3EgrTxRegsInit(devNum,regsAddrPtr);
    cheetah3CncRegsInit(regsAddrPtr);
    cheetah3PclRegsInit(devNum, regsAddrPtr);
    cheetah3PolicerRegsInit(regsAddrPtr);
    if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E)
        && (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE))
    {
        cheetah3andAboveMacRegsInit(devNum,regsAddrPtr);
    }
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        cheetah3IpRegsInit(regsAddrPtr);
    }
    cheetah3GlobalRegsInit(regsAddrPtr);
    cheetah3BuffMemoryRegsInit(devNum, regsAddrPtr);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh2HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah2 devices.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh2HwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    /* first set the "Cheetah" part */
    rc = prvCpssDxChHwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the specific "cheetah 2" parts */
    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    cheetah2BridgeRegsInit(regsAddrPtr);
    cheetah2GlobalRegsInit(regsAddrPtr);
    cheetah2BufMngRegsInit(devNum,regsAddrPtr);
    cheetah2EgrTxRegsInit(devNum,regsAddrPtr);
    cheetah2MacRegsInit(devNum,regsAddrPtr);
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        cheetah2Ipv4RegsInit(regsAddrPtr);
        cheetah2Ipv6RegsInit(regsAddrPtr);
        cheetah2IpRegsInit(regsAddrPtr);
        cheetah2IpMtRegsInit(regsAddrPtr);
    }
    cheetah2PclRegsInit(regsAddrPtr);
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh2HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxCh2HwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh2HwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    /* first free the "Cheetah" part */
    rc = prvCpssDxChHwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "cheetah 2" parts */

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxCh3HwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3HwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    /* first free the "Cheetah2" part */
    rc = prvCpssDxCh2HwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "cheetah 3" parts */

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChHwRegAddrStcInfoGet
*
* DESCRIPTION:
*   This function return the address of the registers struct of cheetah devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       regAddrPtrPtr - (pointer to) address of the registers struct
*       sizePtr - (pointer to) size of registers struct
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrStcInfoGet
(
    IN  GT_U8     devNum,
    OUT GT_U32  **regAddrPtrPtr,
    OUT GT_U32   *sizePtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    *regAddrPtrPtr = (GT_U32 *)PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
    *sizePtr = sizeof(*(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)));

    return GT_OK;
}

/*******************************************************************************
* xCatIpclTccUnitRegsInit
*
* DESCRIPTION:
*       This function initializes the PCL registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatIpclTccUnitRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  ii;

    for (ii = 0; (ii < 12); ii++)
    {
        addrPtr->pclRegs.tcamWriteData[ii] = 0x0d000100 + (4 * ii); /*in ch3 was 0x0B800100*/
    }

    addrPtr->pclRegs.tcamOperationTrigger = 0x0d000138/*in ch3 was 0x0B800138*/;
    addrPtr->pclRegs.tcamOperationParam1  = 0x0d000130/*in ch3 was 0x0B800130*/;
    addrPtr->pclRegs.tcamOperationParam2  = 0x0d000134/*in ch3 was 0x0B800134*/;
    addrPtr->pclRegs.actionTableBase      = 0x0d0b0000/*in ch3 was 0x0B880000*/;
    addrPtr->pclRegs.pclTcamConfig0       = 0x0d000208/*in ch3 was 0x0B820108*/;
    addrPtr->pclRegs.pclTcamConfig2       = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->pclRegs.pclTcamControl       = PRV_CPSS_SW_PTR_ENTRY_UNUSED ;

    /* CNC related registers */
    addrPtr->pclRegs.cncCountMode                  = 0x0B800088;
    addrPtr->pclRegs.cncl2l3IngressVlanCountEnable = 0x0B800080;
    addrPtr->pclRegs.actionTableBase               = 0x0e0b0000;
    addrPtr->pclRegs.tcamOperationTrigger = 0x0E000138/*in ch3 was 0x0B800138*/;
    addrPtr->pclRegs.tcamOperationParam1  = 0x0E000130/*in ch3 was 0x0B800130*/;
    addrPtr->pclRegs.tcamOperationParam2  = 0x0E000134/*in ch3 was 0x0B800134*/;
    addrPtr->pclRegs.tcamReadPatternBase  = 0x0E040000/*in ch3 was 0x0BA00000*/;
    addrPtr->pclRegs.pclTcamConfig0       = 0x0E000208/*in ch3 was 0x0B820108*/;
    for (ii = 0; (ii < 12); ii++)
    {
        addrPtr->pclRegs.tcamWriteData[ii] = 0x0E000100 + (4 * ii); /*in ch3 was 0x0B800100*/
    }

}

/*******************************************************************************
* xCatIpvxTccUnitRegsInit
*
* DESCRIPTION:
*       This function initializes the IPvX registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatIpvxTccUnitRegsInit
(
    IN    GT_U32                            devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32      i;  /* loop iterator */

    addrPtr->ipRegs.routerTtTcamAccessCtrlReg      = 0x0e80041c;/*in ch3 was  0x0280041C*/
    addrPtr->ipRegs.routerTtTcamAccessDataCtrlReg  = 0x0e800418;/*in ch3 was  0x02800418*/
    addrPtr->ipRegs.routerTtTcamAccessDataRegsBase = 0x0e800400;/*in ch3 was  0x02800400*/

    /* only base of all TCAM will be initialized */
    addrPtr->ipRegs.routerTtTcamBankAddr[0] = 0x0e840000;

    addrPtr->ipRegs.ipTcamConfig0 = 0x0e80096c; /*in ch3 was 0x0280096C*/

    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        addrPtr->ipRegs.ipTcamControl = 0x0e800960; /*in ch3 was 0x02800960*/
    }
    else
    {
        addrPtr->ipRegs.ipTcamControl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    /* The following registers are not used in xCat */
    /* Tcam bank addresses, starting from the second one */
    for (i = 1; i <= 4; i++)
    {
        addrPtr->ipRegs.routerTtTcamBankAddr[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
}

/*******************************************************************************
* xCatIpMtRegsInit
*
* DESCRIPTION:
*       This function initializes the IP Multi Traffic registers struct for
*       devices that belong to REGS_FAMILY_1 registers set . -- -- xCat
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatIpMtRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 i;
    GT_U32 offset;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        return;
    }

    addrPtr->ipMtRegs.mllGlobalReg = 0x0d800000;
    addrPtr->ipMtRegs.multiTargetRateShapingReg = 0x0d800210;
    addrPtr->ipMtRegs.mtUcSchedulerModeReg = 0x0d800214;
    addrPtr->ipMtRegs.mtTcQueuesSpEnableReg = 0x0d800200;
    addrPtr->ipMtRegs.mtTcQueuesWeightReg = 0x0d800204;
    addrPtr->ipMtRegs.mtTcQueuesGlobalReg = 0x0d800004;
    addrPtr->ipMtRegs.mllMcFifoFullDropCnt = 0x0d800984;
    for (i = 0 ; i < 8 ; i++)
    {
        addrPtr->ipMtRegs.qoSProfile2MTTCQueuesMapTable[i] = 0x0d800100 + 4 * i;
    }
    addrPtr->ipMtRegs.mllTableBase = 0x0d880000;
    for (i = 0 ; i < 2 ; i++)
    {
        offset = i * 0x100;
        addrPtr->ipMtRegs.mllOutInterfaceCfg[i]    = 0x0d800980 + offset;
        addrPtr->ipMtRegs.mllOutMcPktCnt[i]        = 0x0d800900 + offset;
    }

    if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)&&
       (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E) &&
       (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE))
    {
        addrPtr->ipMtRegs.dft1Reg = 0x0D800014;
        addrPtr->ipMtRegs.mllMetalFix = 0x0D80000C;
    }
}

/*******************************************************************************
* xCatIpRegsInit
*
* DESCRIPTION:
*       This function initializes the IP registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatIpRegsInit
(
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32      i;  /* loop iterator */

    addrPtr->haRegs.hdrAltGlobalConfig = 0x0F000100;
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.routerHdrAltEnMacSaModifyReg,0x0F000104);
    addrPtr->haRegs.routerHdrAltMacSaModifyMode[0] = 0x0F000120;
    addrPtr->haRegs.routerHdrAltMacSaModifyMode[1] = 0x0F000124;
    addrPtr->haRegs.routerMacSaBaseReg[0] = 0x0F000108;
    addrPtr->haRegs.routerMacSaBaseReg[1] = 0x0F00010C;
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.routerDevIdModifyEnReg,0x0F000110);


    /* The following registers are unused for xCat */
    /* Router TCAM Test */
    for (i = 0; i < 20; i++)
    {
        addrPtr->ipRegs.routerTcamTest.pointerRelReg[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    addrPtr->ipRegs.routerTcamTest.configStatusReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ipRegs.routerTcamTest.opcodeCommandReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->ipRegs.routerTcamTest.opcodeExpectedHitReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    addrPtr->ipRegs.ucRoutingEngineConfigurationReg = 0x02800E3C;

    addrPtr->ipRegs.routerPerPortSipSaEnable0 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
}

/*******************************************************************************
* lionIpRegsInit
*
* DESCRIPTION:
*       This function initializes the IP registers struct for
*       devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionIpRegsInit
(
   IN GT_U32          devNum,
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 offset, i;
    GT_U32 baseAddr;

    baseAddr = 0x02800000;

    addrPtr->ipRegs.ctrlReg0 = baseAddr + 0x00000E3C;
    addrPtr->ipRegs.routerPerPortSipSaEnable0 = baseAddr + 0x00000014;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)

    {
        /* Router TCAM Test */
        for (i = 0; i < 20; i++)
        {
            offset = i * 0x4;
            addrPtr->ipRegs.routerTcamTest.pointerRelReg[i] = 0x0D800100 + offset;
        }

        addrPtr->ipRegs.routerTcamTest.configStatusReg = 0x0D800150;
        addrPtr->ipRegs.routerTcamTest.opcodeCommandReg = 0x0D800154;
        addrPtr->ipRegs.routerTcamTest.opcodeExpectedHitReg = 0x0D800158;
    }

    return;
}
/*******************************************************************************
* lionMiscellaneousRegsInit
*
* DESCRIPTION:
*       This function initializes the registers miscellaneous struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- Lion
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionMiscellaneousRegsInit
(
        IN                 GT_U32                                                         devNum,
    INOUT         PRV_CPSS_DXCH_PP_REGS_ADDR_STC         *addrPtr
)
{
    addrPtr->miscellaneousRegs.lms0PeriodicFlowControlInterval0 = 0x04004140;
    if (CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {/* actually in Lion2 there is just single LMS */
        addrPtr->miscellaneousRegs.lms1PeriodicFlowControlInterval1 = 0x04204140;
        addrPtr->miscellaneousRegs.lms1PeriodicFlowControlIntervalSel = 0x04204144;
    }
    else
    {
        addrPtr->miscellaneousRegs.lms1PeriodicFlowControlInterval1 = 0x05004140;
        addrPtr->miscellaneousRegs.lms1PeriodicFlowControlIntervalSel = 0x05004144;
    }
}

/*******************************************************************************
* xCatEgrTxRegsInit
*
* DESCRIPTION:
*       This function initializes the Egress and Transmit (Tx) Queue
*       Configuration registers - L2 and L3 Port Isolation table entries.
*
* INPUTS:
*       devNum     - The PP's device number.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatEgrTxRegsInit
(
    IN GT_U32    devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return;
    }

    addrPtr->egrTxQConf.l2PortIsolationTableBase = 0x01E40004;
    addrPtr->egrTxQConf.l3PortIsolationTableBase = 0x01E40008;

    addrPtr->egrTxQConf.dequeueSWRRWeightReg3 = 0x018000BC;
    addrPtr->egrTxQConf.linkListControlConfig = 0x018000D0;

}

/*******************************************************************************
* xCatBrgRegsInit
*
* DESCRIPTION:
*       This function initializes the Brigde registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* INPUTS:
*       devNum  - device number
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatBrgRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 ii; /* loop iterator */


    for (ii = 0; ii < 4; ii++)
    {
        addrPtr->bridgeRegs.bridgeIngressVlanEtherTypeTable[ii]= 0x0C000300 + ii * 4;
        addrPtr->haRegs.bridgeEgressVlanEtherTypeTable[ii] = 0x0F000430 + ii * 4;
        addrPtr->haRegs.bridgeEgressPortTag0TpidSelect[ii] = 0x0F000440 + ii * 4;
        addrPtr->haRegs.bridgeEgressPortTag1TpidSelect[ii] = 0x0F000460 + ii * 4;
    }

    for (ii = 0; ii < 32; ii++)
    {
        if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)&&
           (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E) &&
           (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE))
        {
            addrPtr->bridgeRegs.bridgeIngressVlanSelect[ii] = 0x0C000310 + ii * 4;
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        for (ii = 0; ii < 4; ii++)
        {
            addrPtr->haRegs.tunnelStartEgressVlanEtherTypeTable[ii] = 0x0F000700 + ii * 4;
            addrPtr->haRegs.tunnelStartEgressPortTagTpidSelect[ii] = 0x0F000710 + ii * 4;
            addrPtr->haRegs.passengerEgressVlanEtherTypeTable[ii] = 0x0F000750 + ii * 4;
        }
        addrPtr->haRegs.passengerEgressPortTagTpidSourceSelect = 0x0F000730;
    }

    /* Protocols Encapsulation Configuration */
    addrPtr->bridgeRegs.vlanRegs.protoEncBased = 0x0C000098;
    addrPtr->bridgeRegs.vlanRegs.protoEncBased1 = 0x0C00009C ;

    /* 6 registers 0x0C000080-0x0C000094 for 12 protocols */
    addrPtr->bridgeRegs.vlanRegs.protoTypeBased = 0x0C000080;

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.haVlanTransEnReg,0x0F000130);

    /* QoS Profile registers */
    addrPtr->haRegs.qosProfile2ExpTable     = 0x0F000300;
    addrPtr->bridgeRegs.qosProfileRegs.dscp2QosProfileMapTable = 0x0c000400;
    addrPtr->haRegs.qosProfile2DpTable      = 0x0F000340;
    /* CFI UP -> Qos */
    addrPtr->bridgeRegs.qosProfileRegs.up2QosProfileMapTable   = 0x0c000440;
    addrPtr->bridgeRegs.qosProfileRegs.exp2QosProfileMapTable  = 0x0c000460;
    addrPtr->bridgeRegs.qosProfileRegs.dscp2DscpMapTable       = 0x0C000900;

    /* TO ANALYZER VLAN Adding Configuration Registers */
    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.toAnalyzerVlanAddConfig,0x0F000404);

    /* Ingress Analyzer VLAN Tag Configuration Register */
    addrPtr->haRegs.ingAnalyzerVlanTagConfig = 0x0F000408;

    /* Egress Analyzer VLAN Tag Configuration Register */
    addrPtr->haRegs.egrAnalyzerVlanTagConfig = 0x0F00040C;

    /* IPv6 Solicited-Node Multicast Address Configuration/Mask Register0 */
    addrPtr->bridgeRegs.ipv6McastSolicitNodeAddrBase = 0x0C000040;
    addrPtr->bridgeRegs.ipv6McastSolicitNodeMaskBase = 0x0C000050;

    addrPtr->bridgeRegs.macRangeFltrBase[1] = 0x02000808;
    addrPtr->bridgeRegs.macRangeFltrBase[2] = 0x02000810;
    addrPtr->bridgeRegs.macRangeFltrBase[3] = 0x02000818;

    addrPtr->bridgeRegs.ieeeReservedMcastProfileSelect0 = 0x0200080C;
    addrPtr->bridgeRegs.ieeeReservedMcastProfileSelect1 = 0x0200081C;
}

/*******************************************************************************
* xCatTtiRegsInit
*
* DESCRIPTION:
*       This function initializes the TTI registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatTtiRegsInit
(
   IN GT_U32 devNum,
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ttiRegs.pclIdConfig0        = 0x0C000010;
    addrPtr->ttiRegs.pclIdConfig1        = 0x0C000014;
    addrPtr->ttiRegs.ttiIpv4GreEthertype = 0x0C000018;
    addrPtr->ttiRegs.ccfcEthertype       = 0x0C000024;
    addrPtr->ttiRegs.specialEthertypes   = 0x0C000028;
    addrPtr->ttiRegs.mplsEthertypes      = 0x0C000030;
    addrPtr->ttiRegs.trunkHashCfg[0]     = 0x0C000070;
    addrPtr->ttiRegs.trunkHashCfg[1]     = 0x0C000074;
    addrPtr->ttiRegs.trunkHashCfg[2]     = 0x0C000078;
    addrPtr->ttiRegs.ttiMetalFix         = 0x0C0002B0;
    addrPtr->ttiRegs.ipMinimumOffset     = 0x0C000020;
    addrPtr->ttiRegs.ttiPwConfig         = 0x0C000034;

    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        FORMULA_SINGLE_REG_MAC(
            addrPtr->ttiRegs.ttIngressTpidSelect,
            0x0C000524,0x4,32);
    }
}

/*******************************************************************************
* xCatHaRegsInit
*
* DESCRIPTION:
*       This function initializes the HA registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatHaRegsInit
(
   IN GT_U32 devNum,
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->haRegs.hdrAltCnmHeaderConfig  = 0x0F000424;
    addrPtr->haRegs.hdrAltMplsEthertypes   = 0x0F000480;
    addrPtr->haRegs.hdrAltIEthertype       = 0x0F000484;

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
            addrPtr->haRegs.cscdHeadrInsrtConf,0x0F000004);

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
            addrPtr->haRegs.eggrDSATagTypeConf,0x0F000020);

    PORTS_BMP_REG_SET___EMULATE_64_PORTS___MAC(
        addrPtr->haRegs.invalidCrcModeConfigReg,0x0F000400);

    if (1 == PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(devNum))
    {
        addrPtr->haRegs.hdrAltCpidReg0     = 0x0F000428;
        addrPtr->haRegs.hdrAltCpidReg1     = 0x0F00042C;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.mplsControlWord,
        0x0F000764,0x4,7);
    }
}

/*******************************************************************************
* xCatBridgeRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatBridgeRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    addrPtr->bridgeRegs.vntReg.vntGreEtherTypeConfReg = 0x0C000028;
    addrPtr->bridgeRegs.vntReg.vntCfmEtherTypeConfReg = 0x0C000028;
}

/*******************************************************************************
* xCatGlobalRegsInit
*
* DESCRIPTION:
*       This function initializes the Global registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* INPUTS:
*       devNum     - The PP's device number.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatGlobalRegsInit
(
   IN      GT_U32                          devNum,
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 ii;

    addrPtr->globalRegs.metalFix               = 0x00000054;

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
    {
        addrPtr->globalRegs.extendedGlobalControl2 = 0x0000008C;
    }

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        addrPtr->globalRegs.analogCfgReg   = 0x0000009C;
        addrPtr->globalRegs.ftdllReg       = 0x0000000C;
        for (ii = 0; ii < 6; ii++)
        {
            addrPtr->globalRegs.addressDecoding.baseAddress[ii] = 0x0000030C + ii * 8;
            addrPtr->globalRegs.addressDecoding.windowSize[ii]        = 0x00000310 + ii * 8;
            if(ii<4)
               addrPtr->globalRegs.addressDecoding.highAddressRemap[ii] = 0x0000033C + ii * 4;
        }
        addrPtr->globalRegs.addressDecoding.baseAddressEnable[0] = 0x0000034C;
        addrPtr->globalRegs.addressDecoding.accessProtect          = 0x00000350;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        addrPtr->globalRegs.auqConfig_generalControl = 0x000000D4;
        addrPtr->globalRegs.auqConfig_hostConfig     = 0x000000D8;
        addrPtr->globalRegs.generalConfigurations    = 0x00000110;

        for (ii = 0; ii < 6; ii++)
        {
            addrPtr->globalRegs.addressDecoding.baseAddress[ii] =       0x0000020C + ii * 8;
            addrPtr->globalRegs.addressDecoding.windowSize[ii]        =       0x00000210 + ii * 8;
            addrPtr->globalRegs.addressDecoding.highAddressRemap[ii] =  0x000023C + ii * 4;
            /* Window Control Register */
            addrPtr->globalRegs.addressDecoding.baseAddressEnable[ii]=  0x0000254 + ii * 4;
        }
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        addrPtr->globalRegs.padsControl.padControl1             = 0x00000048;
    }
}

/*******************************************************************************
* xCatDebugBusRegsInit
*
* DESCRIPTION:
*       This function initializes the Debug Bus registers struct for xCat
*           and xCat2 devices.
*
* INPUTS:
*       devNum     - The PP's device number.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatDebugBusRegsInit
(
    IN      GT_U32                          devNum,
    INOUT   PRV_CPSS_DXCH_PP_REGS_ADDR_STC  *addrPtr
)
{
    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
        case CPSS_PP_FAMILY_DXCH_XCAT3_E:
            addrPtr->debugBusRegs.debugCtrl2 = 0x880008;
            addrPtr->debugBusRegs.debugBus = 0x88000C;
            break;

        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            addrPtr->debugBusRegs.debugCtrl2 = 0xEC;
            break;

        default:
            return;
    }
}


/*******************************************************************************
* xCatMirrorRegsInit
*
* DESCRIPTION:
*       This function initializes the Global registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatMirrorRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;

    addrPtr->bufferMng.eqBlkCfgRegs.sniffPortsCfg = 0x0B00B020;

    for (i = 0; i < 7; i++)
    {
        /* Mirror Interface Parameter Registers */
        addrPtr->bufferMng.eqBlkCfgRegs.mirrorInterfaceParameterReg[i] =
            0x0B00B020 + i * 4;
    }

    for (i = 0; i < 3; i++)
    {
        addrPtr->bufferMng.eqBlkCfgRegs.portRxMirrorIndex[i] = 0x0B00B000 + i * 4;
        addrPtr->bufferMng.eqBlkCfgRegs.portTxMirrorIndex[i] = 0x0B00B010 + i * 4;
    }

    addrPtr->bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig = 0x0B00B040;
}

/*******************************************************************************
* xCatBufMngRegsInit
*
* DESCRIPTION:
*       This function initializes the buffer management registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.-- xCat
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatBufMngRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;

    for (i = 0; i < 7; i++)
    {
        /* Port<4n,4n+1,4n+2,4n+3> TrunkNum Configuration Register<n> (0<=n<7) */
        /* num of registers : (corePpDevs[devNum]->numOfPorts / 4) + 1 */
        /* use CHEETAH_TRUNK_ID_CONIFG_TABLE_OFFSET_CNS for offset */
        addrPtr->haRegs.trunkNumConfigReg[i] = 0x0F00002C + 4*i;
    }

    addrPtr->bufferMng.linkedListTailPtr = 0x03000200;
}

/*******************************************************************************
* lionBufMngRegsInit
*
* DESCRIPTION:
*       This function initializes the buffer management registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionBufMngRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  numOfSteps;

    addrPtr->bufferMng.eqBlkCfgRegs.cscdEgressMonitoringEnableConfReg = 0x0B000074;

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
        (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            for (i = 0; i < 2; i++)
            {
                addrPtr->bufferMng.txdmaThresholdOverrideEn[i] = 0x0F800040 + i*4;
            }
            for (i = 0; i < 5; i++)
            {
                addrPtr->bufferMng.txdmaBufMemFifosThreshold[i] = 0x0F800014 + i*4;
            }

            addrPtr->bufferMng.mcCntrsMemBaseAddr = 0x03810000;
            addrPtr->bufferMng.txdmaBurstLimitGlobalConfig = 0x0F800100;
            FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F800104,4,12, 0);
            FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F800104,4,12,16);
            FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F800104,4,12,32);
            FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F800104,4,12,48);
            addrPtr->bufferMng.txdmaPortBurstLimitThresholds[CPSS_CPU_PORT_NUM_CNS] = 0xF800134;
        }
        else /*lion2*/
        {
            /*Buffer Memory Misc. Thresholds Configuration register*/
            addrPtr->bufferMng.txdmaBufMemMiscTresholdsCfg = 0x0F800040;

            FORMULA_SINGLE_REG_MAC(addrPtr->bufferMng.txdmaBufMemFifosThreshold, 0x0F80002C, 4, 5);
            addrPtr->bufferMng.mcCntrsMemBaseAddr = 0x03800000;
            addrPtr->bufferMng.txdmaBurstLimitGlobalConfig = 0x0F8000C4;
            if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
            {
                /* all ports are valid */
                numOfSteps = PRV_CPSS_PP_MAC(devNum)->numOfPorts;

                FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F8000C8,4,numOfSteps, 0);
            }
            else
            {
                FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F8000C8,4,12, 0);
                FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F8000C8,4,12,16);
                FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F8000C8,4,12,32);
                FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F8000C8,4,12,48);
                FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F8000C8,4,12,64);
                FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F8000C8,4,12,80);
                FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F8000C8,4,12,96);
                FORMULA_SINGLE_REG_WITH_INDEX_MAC(addrPtr->bufferMng.txdmaPortBurstLimitThresholds,0x0F8000C8,4,12,112);
            }

            addrPtr->bufferMng.txdmaPortBurstLimitThresholds[CPSS_CPU_PORT_NUM_CNS] = 0xF800134;

            /* pizza arbiter related registers */
            addrPtr->bufferMng.rxdmaPZCtrlReg = 0xF000000;
            addrPtr->bufferMng.txdmaPZCtrlReg = 0xF800000;
            addrPtr->bufferMng.rxdmaCTUCtrlReg = 0xF000028;
            addrPtr->bufferMng.txdmaCTUCtrlReg = 0xF80009C;
            for(i = 0; i < 8; i++)
            {
                addrPtr->bufferMng.rxdmaPZSliceMapReg[i] = 0xF000004+i*0x4;
                addrPtr->bufferMng.txdmaPZSliceMapReg[i] = 0xF800004+i*0x4;
                if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
                {
                    addrPtr->bufferMng.rxdmaCTUSliceMapReg[i] = 0xF00002C+i*0x4;
                }
                else /*lion3*/
                {
                    CPSS_TBD_BOOKMARK_EARCH/* new count of registers and format*/
                    addrPtr->bufferMng.rxdmaCTUSliceMapReg[i] = 0xF000E08+i*0x4;
                }
                addrPtr->bufferMng.txdmaCTUSliceMapReg[i] = 0xF8000A0+i*0x4;
            }

            addrPtr->bufferMng.bufMngPZGenConfiglReg = 0x3000354;
            addrPtr->bufferMng.bufMngPZSlotEnableReg = 0x3000358;
            for(i = 0; i < 4; i++)
            {
                addrPtr->bufferMng.bufMngPZSlotConfigReg[i] = 0x300035C+i*0x4;
            }
            addrPtr->bufferMng.rxdmaIfConfig[0] = 0xF000090; /* Low DMA I/F Configuration */
            addrPtr->bufferMng.rxdmaIfConfig[1] = 0xF000094; /* High DMA I/F Configuration */

            addrPtr->bufferMng.txdmaExtendedPortsConfig = 0xF800070;

            addrPtr->bufferMng.txdmaCountersReset = 0xF800044;

            addrPtr->bufferMng.thresholdOverrideEnable[0] = 0x0F800060;
            addrPtr->bufferMng.thresholdOverrideEnable[1] = 0x0F800064;

            addrPtr->bufferMng.rxdmaDfxMetalFix     = 0x0F000084;
            addrPtr->bufferMng.txdmaBufMemClearType = 0x0F80006C;

        }
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* xCat2 */
        addrPtr->bufferMng.mcCntrsMemBaseAddr = 0x03040000;
        addrPtr->bufferMng.buffMemMppmConfig = 0x0780000C;
    }
}

/*******************************************************************************
* xCatBcnRegsInit
*
* DESCRIPTION:
*       This function initializes the BCN registers struct
*       for devices that belong to REGS_FAMILY_1 registers set.-- xCat
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatBcnRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        /* those are 'XCAT only' registers */
        addrPtr->bcnRegs.bcnControlReg =               PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bcnRegs.portsBcnAwarenessTbl =        PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->bcnRegs.bcnPauseTriggerEnableReg =    PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bcnRegs.bcnGlobalPauseThresholdsReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bcnRegs.portsBcnProfileReg0 =         PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bcnRegs.portsSpeedIndexesReg0 =       PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
    else
    {
        addrPtr->bcnRegs.bcnControlReg =               0x0A400000;/*xcat only*/
        addrPtr->bcnRegs.portsBcnAwarenessTbl =        0x0A400030;/*xcat only*/
        addrPtr->bcnRegs.bcnPauseTriggerEnableReg =    0x0A40000C;/*xcat only*/
        addrPtr->bcnRegs.bcnGlobalPauseThresholdsReg = 0x0A400014;/*xcat only*/
        addrPtr->bcnRegs.portsBcnProfileReg0 =         0x0A400020;/*xcat only*/
        addrPtr->bcnRegs.portsSpeedIndexesReg0 =       0x0A400034;/*xcat only*/
    }

}

/*******************************************************************************
* xCatSerdesRegsInit
*
* DESCRIPTION:
*       This function initializes the SERDES registers.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2, Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       For devices from Bobcat2 and above registers base address will be replaced to relevant for specific device in later stage.
*
*******************************************************************************/
static void xCatSerdesRegsInit
(
    IN GT_U32 devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    GT_U32 i, offset;
    GT_U32 devType;     /* device type          */
    GT_U32 startSerdesIndx;
    GT_U32 maxSerdesIndx; /* amount of SerDes */
    GT_BOOL haveLpInternalRegisters;
    GT_U32 internalRegistersAdditionalOffset = 0;/* already with the 0x200 offset*/
    GT_U32 xCat3AdditionalOffset = 0;

    maxSerdesIndx = addrPtr->serdesConfigRegsSize / sizeof(struct _lpSerdesConfig);

    /* define type of xCat device either Gig or Fast */
    devType = PRV_CPSS_PP_MAC(devNum)->devType;
    switch(devType)
    {
        case PRV_CPSS_DXCH_XCAT_FE_DEVICES_CASES_MAC:
        case PRV_CPSS_DXCH_XCAT2_FE_DEVICES_CASES_MAC:
            startSerdesIndx = 6;
            break;
        default:
            if(PRV_CPSS_DXCH_CETUS_CHECK_MAC(devNum) == GT_TRUE)
            {
                startSerdesIndx = 20;
            }
            else
            {
                startSerdesIndx = 0;
            }
            break;
    }

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        /* remove the 0x200 offset that was given in legacy devices ,
           and set it on 0x800 based */
        internalRegistersAdditionalOffset = 0x800 - 0x200;
        haveLpInternalRegisters = GT_FALSE; /* patch until issue of those addresses will be solved */
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        xCat3AdditionalOffset = 0x13000000 - 0x09800000;
        haveLpInternalRegisters = GT_FALSE;
    }
    else
    {
        haveLpInternalRegisters = GT_TRUE;
    }

    for (i = startSerdesIndx; (i < maxSerdesIndx); i++)
    {
        if(PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            offset = i * 0x1000;
        }
        else
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            offset = i * 0x400;
        }
        else
        {
            offset = ((i / 2) * 0x1000) + ((i % 2) * 0x400);
        }

        if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum) && PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) == 0)
        {
            if (i == 20  || (i > 23 && i < 36))
            {
                offset += 0x40000;
            }
        }

        if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))/* limited check ... to not upset legacy devices */
        {
            /* skip not valid SERDESes in the loop */
            PRV_CPSS_DXCH_SKIP_NON_VALID_SERDES_IN_LOOP_MAC(((GT_U8)devNum),i);
        }

        /* LpInternalRegisters */
        if(haveLpInternalRegisters == GT_TRUE)
        {
            addrPtr->serdesConfig[i].calibrationReg0 = 0x09800224 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].calibrationReg1 = 0x0980022C + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].calibrationReg2 = 0x09800230 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].calibrationReg3 = 0x09800234 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].calibrationReg5 = 0x0980023C + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].calibrationReg7 = 0x09800228 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].pllIntpReg1     = 0x0980020C + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].pllIntpReg2     = 0x09800210 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].pllIntpReg3     = 0x09800214 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].pllIntpReg4     = 0x09800218 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].pllIntpReg5     = 0x0980021C + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].transmitterReg0 = 0x09800250 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].transmitterReg1 = 0x09800254 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].transmitterReg2 = 0x09800258 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].receiverReg0    = 0x09800278 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].receiverReg1    = 0x0980027C + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].receiverReg2    = 0x09800280 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].ffeReg          = 0x0980028C + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].analogReservReg = 0x098003CC + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].digitalIfReg0   = 0x098003E0 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].powerReg        = 0x09800368 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].resetReg        = 0x0980036C + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].slcReg          = 0x0980035c + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].referenceReg    = 0x09800364 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].standaloneCntrlReg = 0x098003F8 + offset + internalRegistersAdditionalOffset;

            addrPtr->serdesConfig[i].phyTestReg0     = 0x09800378 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].phyTestReg1     = 0x0980037C + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].phyTestDataReg5 = 0x09800394 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].phyTestPrbsCntReg2    = 0x098003A0 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].phyTestPrbsErrCntReg0 = 0x098003A4 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].phyTestPrbsErrCntReg1 = 0x098003A8 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].dfeF0F1CoefCtrl = 0x09800298 + offset + internalRegistersAdditionalOffset;
            addrPtr->serdesConfig[i].testReg0 = 0x098003B8 + offset + internalRegistersAdditionalOffset;
        }

        addrPtr->serdesConfig[i].serdesExternalReg1 = 0x09800000 + offset + xCat3AdditionalOffset;
        addrPtr->serdesConfig[i].serdesExternalReg2 = 0x09800004 + offset + xCat3AdditionalOffset;

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E ||
            PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            addrPtr->serdesConfig[i].serdesExternalReg3 = 0x0980000C + offset;
        }
        else
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
            PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE ||
            PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            addrPtr->serdesConfig[i].serdesExternalReg3 = 0x09800008 + offset + xCat3AdditionalOffset;
        }
    }
}

/*******************************************************************************
* xCatLedsRegsInit
*
* DESCRIPTION:
*       This function initializes the LED interface registers.
*
* INPUTS:
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IP regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatLedRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ledRegs.ledHyperGStackDebugSelect[1]   = 0x5005110;
    addrPtr->ledRegs.ledHyperGStackDebugSelect1[1]  = 0x5005114;
}

/*******************************************************************************
* xCatPreEgressEngineRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatPreEgressEngineRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    addrPtr->bufferMng.eqBlkCfgRegs.vlanEgressFilterCounterReg = 0x0B000074;
    addrPtr->bufferMng.eqBlkCfgRegs.logicalTargetDeviceMappingConfReg = 0x0B050004;
}

/*******************************************************************************
* xCatL2MllRegsInit
*
* DESCRIPTION:
*       This function initializes the L2MLL registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- xCat
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatL2MllRegsInit
(
    IN GT_U32                                devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        addrPtr->l2MllRegs.vplsDropCounter = 0x0D800018;
    }
}

/*******************************************************************************
* xCatDescriptorRegsInit
*
* DESCRIPTION:
*       This function initializes the Descriptor registers.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCatDescriptorRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{

    addrPtr->descriptorsConfig.wrDmaToTti = 0x0C000298;
    addrPtr->descriptorsConfig.ttiToPcl = 0x0C000200;
    addrPtr->descriptorsConfig.pclToBridge = 0x0B800200;
    addrPtr->descriptorsConfig.bridgeToRouterSet0 = 0x02040020;
    addrPtr->descriptorsConfig.bridgeToRouterSet1 = 0x02040A00;
    addrPtr->descriptorsConfig.preEgressToTxq0 = 0x0B000044;
    addrPtr->descriptorsConfig.preEgressToTxq1 = 0x0B00B050;
    addrPtr->descriptorsConfig.ingressPolicer0ToIngressPolicer1Set0 = 0x0C800400;
    addrPtr->descriptorsConfig.ingressPolicer0ToIngressPolicer1Set1 = 0x0C800430;
    addrPtr->descriptorsConfig.ingressPolicer1ToPreEgressSet0 = 0x0D000400;
    addrPtr->descriptorsConfig.ingressPolicer1ToPreEgressSet1 = 0x0D000430;
    addrPtr->descriptorsConfig.egressPolicerEgressToPclSet0 = 0x03800400;
    addrPtr->descriptorsConfig.egressPolicerEgressToPclSet1 = 0x03800430;
    addrPtr->descriptorsConfig.txqToHeaderAlteration = 0x01800194;

}

/*******************************************************************************
* xCat2DescriptorRegsInit
*
* DESCRIPTION:
*       This function initializes the Descriptor registers.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3, xCat; xCat3; Lion.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCat2DescriptorRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    addrPtr->descriptorsConfig.wrDmaToTti = 0x0C001498;
    addrPtr->descriptorsConfig.ttiToPcl = 0x0C001400;
    addrPtr->descriptorsConfig.txqToHeaderAlteration = 0x0F0003E0;

}

/*******************************************************************************
* prvCpssDxChXcatHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh xCat devices.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcatHwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    /* first set the "Cheetah-3" part */
    rc = prvCpssDxCh3HwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the specific "DXCH xcat 3" parts */
    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* for bobcat2 GM the registers that relate to units, will be set as 'not valid by'
       convertOldRegDbToNewUnitsBaseAddr(...) */
    {
        /* IPCL TCC - unit */
        xCatIpclTccUnitRegsInit(regsAddrPtr);
        /* IPvX TCC - unit */
        xCatIpvxTccUnitRegsInit(devNum,regsAddrPtr);
    }

    /* MAC registers configured in cheetah3andAboveMacRegsInit */
    /* no need to call again */

    /* L2 and L3 Port Isolation table settings */
    xCatEgrTxRegsInit(devNum,regsAddrPtr);

    xCatIpRegsInit(regsAddrPtr);

    xCatBrgRegsInit(devNum, regsAddrPtr);

    xCatIpMtRegsInit(devNum, regsAddrPtr);

    xCatPclRegsInit(regsAddrPtr);

    xCatTtiRegsInit(devNum, regsAddrPtr);

    xCatHaRegsInit(devNum, regsAddrPtr);

    xCatBridgeRegsInit(regsAddrPtr);

    xCatGlobalRegsInit(devNum, regsAddrPtr);

    xCatPolicerRegsInit(regsAddrPtr);

    xCatMirrorRegsInit(regsAddrPtr);

    xCatBufMngRegsInit(regsAddrPtr);

    xCatBcnRegsInit(devNum,regsAddrPtr);

    xCatPreEgressEngineRegsInit(regsAddrPtr);

    xCatSerdesRegsInit(devNum,regsAddrPtr);

    /* skip configuration for Lion; Lion2; Bobcat2; Caelum; Bobcat3 */
    if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)&&
       (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E) &&
       (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE))
    {
        xCatLedRegsInit(regsAddrPtr);
    }

    xCatDebugBusRegsInit(devNum, regsAddrPtr);

    xCatDescriptorRegsInit(regsAddrPtr);

    xCatL2MllRegsInit(devNum,regsAddrPtr);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXcat3BaseAddrConvertSet
*
* DESCRIPTION:
*       This function initializes the unit base address conversion table used
*       update the registers addresses based on xCat DB to xCat3 values.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on succes
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat3BaseAddrConvertSet
(
    GT_VOID
)
{
    GT_U32 ii;

    for( ii = 0 ;
         ii < sizeof(prvCpssDxChUnitsBaseAddrXcat3AndXcat)/(sizeof(GT_U32)*2) ;
         ii++)
    {
        prvCpssDxChBaseCovertAddrXcatToXcat3[
            (prvCpssDxChUnitsBaseAddrXcat3AndXcat[ii][1] >> 23)] =
                                 prvCpssDxChUnitsBaseAddrXcat3AndXcat[ii][0];
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXcat3HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh xCat3 devices.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat3HwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;
    GT_U32 *regsAddrAsArrayPtr;
    GT_U32 *addrPtr;
    GT_U32 ii,jj;
    PRV_CPSS_BOOKMARK_STC *bookmark;
    GT_PHYSICAL_PORT_NUM portNum;/* physical port number */
    GT_U32 baseAddr;/* MAC Unit base address */
    GT_U32 xgBaseAddr;
    GT_U32 offset;/* step between ports */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E |
                                          CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    /* first set the "xCat" part */
    rc = prvCpssDxChXcatHwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the specific "DXCH xcat 3" parts */
    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    regsAddrAsArrayPtr = (GT_U32*)regsAddrPtr;

    rc = prvCpssDxChXcat3BaseAddrConvertSet();
    if(rc != GT_OK)
    {
        return rc;
    }

    for( ii = 0 ; ii < sizeof(PRV_CPSS_DXCH_PP_REGS_ADDR_STC)/sizeof(GT_U32) ; ii++ )
    {
        if( PRV_CPSS_SW_PTR_ENTRY_UNUSED == regsAddrAsArrayPtr[ii])
        {
            continue;
        }

        if( PRV_CPSS_SW_PTR_BOOKMARK_CNS == regsAddrAsArrayPtr[ii] )
        {
            bookmark = (PRV_CPSS_BOOKMARK_STC*)(&(regsAddrAsArrayPtr[ii]));

            if( PRV_CPSS_SW_TYPE_PTR_FIELD_CNS == bookmark->type )
            {
                addrPtr = (GT_U32 *)(bookmark->nextPtr);
                for( jj = 0 ; jj < bookmark->size/sizeof(GT_U32) ; jj++ )
                {
                    addrPtr[jj] = (addrPtr[jj] & 0x007FFFFF) |
                                  prvCpssDxChBaseCovertAddrXcatToXcat3[(addrPtr[jj] >> 23)];
                }

                /* jump over the bookmark, -1 due to the loop increment */
                ii += (sizeof(PRV_CPSS_BOOKMARK_STC)/sizeof(GT_U32) -1);
            }
            else if (PRV_CPSS_SW_TYPE_WRONLY_CNS == bookmark->type)
            {
                /* jump over bookmark cns, type and size fields. 2 = 3-1, -1 due to the loop increment */
                ii += 2;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

            continue;
        }

        regsAddrAsArrayPtr[ii] = (regsAddrAsArrayPtr[ii] & 0x007FFFFF) |
                                  prvCpssDxChBaseCovertAddrXcatToXcat3[(regsAddrAsArrayPtr[ii] >> 23)];
    }

    /* Fix BCN related registers */
    regsAddrPtr->bcnRegs.bcnControlReg                  &= 0xFF7FFFFF;
    regsAddrPtr->bcnRegs.bcnPauseTriggerEnableReg       &= 0xFF7FFFFF;
    regsAddrPtr->bcnRegs.bcnGlobalPauseThresholdsReg    &= 0xFF7FFFFF;
    regsAddrPtr->bcnRegs.portsBcnProfileReg0            &= 0xFF7FFFFF;
    regsAddrPtr->bcnRegs.portsBcnAwarenessTbl           &= 0xFF7FFFFF;
    regsAddrPtr->bcnRegs.portsSpeedIndexesReg0          &= 0xFF7FFFFF;

    /* New Designated Trunk Port Table base address */
    regsAddrPtr->bufferMng.trunkTblCfgRegs.baseAddrTrunkDesig = 0x02E4000C;

    /* Selecting between CPU SDMA or Port mode, previously done in dedicated  */
    /* cpu port control register (in MG), is now done in XLG related register */
    /* 0x120C0090 + 29 * 0x1000 */
    regsAddrPtr->globalRegs.cpuPortCtrlReg = 0x120DD090;

    /* Bridge Port Extender (802.1 BR) related registers */
    regsAddrPtr->egrTxQConf.br802_1ConfigRegs.globalConfig = 0x02800F50;
    regsAddrPtr->egrTxQConf.br802_1ConfigRegs.txQueueDpToCfi1PerPortEnReg = 0x02800F54;
    for( ii = 0 ; ii < 32 ; ii++ )
    {
        regsAddrPtr->egrTxQConf.br802_1ConfigRegs.pcidOfTrgPort[ii] =
            0x02800F58 + 0x4 * ii;
    }

    regsAddrPtr->egrTxQConf.ingressEcidTableBase = 0x03A80000;

    regsAddrPtr->egrTxQConf.xgControl = 0x02800180;

    regsAddrPtr->ttiRegs.srcPortBpeEnable = 0x1600001C;
    regsAddrPtr->haRegs.perPortBpeEnable  = 0x1C000600;

    if(PRV_CPSS_DXCH_XCAT3_A1_AND_ABOVE_CHECK_MAC(devNum) == GT_TRUE)
    {
        regsAddrPtr->pclRegs.pclTccRegs.policyTcamControl = 0x1A000148;

        regsAddrPtr->bridgeRegs.bridgeInternal.dft = 0x01040730;

        regsAddrPtr->bridgeRegs.fdbInternal.metalFix = 0x0B00006C;
   }

    /* TXDMA Buffer Memory Tx Fifo Threshold and MPPM access Arbiter Configuration Register */
    regsAddrPtr->bufferMng.txdmaBufMemTxFifoThreshold = 0x0E00006C;

    /* init the DB of 'resetAndInitControllerRegsAddr' */
    rc = prvCpssDxChXcat3HwResetAndInitControllerRegAddrInit((GT_U8)devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    baseAddr = 0x12000000;   /* MAC Unit base address */
    xgBaseAddr = baseAddr + 0xC0000;
    for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if((PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portNum) == PRV_CPSS_PORT_XLG_E) ||
            (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portNum) == PRV_CPSS_PORT_XG_E))
        {
            offset = 0x1000 * portNum;
            regsAddrPtr->macRegs.perPortRegs[portNum].xlgPortFIFOsThresholdsConfig =
                                                            xgBaseAddr + 0x10 + offset;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* lionTtiRegsInit
*
* DESCRIPTION:
*       This function initializes the TTI registers struct for Lion.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionTtiRegsInit
(
    IN  GT_U32   devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 baseAddr;
    GT_U32 i;

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
        (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        /* LionB0 */
        baseAddr = 0x01000000;
    }
    else
    {
        /* XCAT2 */
        baseAddr = 0x0C000000;
    }

    addrPtr->ttiRegs.globalUnitConfig    = baseAddr + 0x0;
    addrPtr->ttiRegs.pclIdConfig0        = baseAddr + 0x10;
    addrPtr->ttiRegs.pclIdConfig1        = baseAddr + 0x14;
    addrPtr->ttiRegs.ttiIpv4GreEthertype = baseAddr + 0x18;
    addrPtr->ttiRegs.ipMinimumOffset     = baseAddr + 0x20;
    addrPtr->ttiRegs.ccfcEthertype       = baseAddr + 0x24;
    addrPtr->ttiRegs.specialEthertypes   = baseAddr + 0x28;
    addrPtr->ttiRegs.udeEthertypesBase   = baseAddr + 0xF0;
    addrPtr->ttiRegs.coreRingPortEnable[0] = baseAddr + 0xF8;
    addrPtr->ttiRegs.coreRingPortEnable[1] = baseAddr + 0xFC;
    addrPtr->ttiRegs.mplsEthertypes      = baseAddr + 0x30;


    addrPtr->ttiRegs.ttiMetalFix         = baseAddr + 0x0200;


    if ((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        addrPtr->ttiRegs.ptpEthertypes       = baseAddr + 0x3000;
        addrPtr->ttiRegs.ptpUdpDstPorts      = baseAddr + 0x3004;
        /*addrPtr->ttiRegs.ptpPacketCmdCfg0Reg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;*/
        /*addrPtr->ttiRegs.ptpPacketCmdCfg1Reg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;*/

        addrPtr->ttiRegs.ptpGlobalConfig     = baseAddr + 0x3030;
        /*addrPtr->ttiRegs.ptpEnableTimeStamp  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;*/

        /*addrPtr->ttiRegs.ptpTimeStampPortEnReg[] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;*/
        /*addrPtr->ttiRegs.ptpGlobalFifoCurEntry[0]    = PRV_CPSS_SW_PTR_ENTRY_UNUSED;*/
        /*addrPtr->ttiRegs.ptpGlobalFifoCurEntry[1]    = PRV_CPSS_SW_PTR_ENTRY_UNUSED;*/
        /*addrPtr->ttiRegs.ptpTodCntrNanoSeconds       = PRV_CPSS_SW_PTR_ENTRY_UNUSED*/
        /*addrPtr->ttiRegs.ptpTodCntrSeconds[0]        = PRV_CPSS_SW_PTR_ENTRY_UNUSED;*/
        /*addrPtr->ttiRegs.ptpTodCntrSeconds[1]        = PRV_CPSS_SW_PTR_ENTRY_UNUSED;*/
        /*addrPtr->ttiRegs.ptpTodCntrShadowNanoSeconds = PRV_CPSS_SW_PTR_ENTRY_UNUSED*/
        /*addrPtr->ttiRegs.ptpTodCntrShadowSeconds[0]  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;*/
        /*addrPtr->ttiRegs.ptpTodCntrShadowSeconds[1]  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;*/

    }
    else
    {
        addrPtr->ttiRegs.ptpEthertypes       = baseAddr + 0x01A4;
        addrPtr->ttiRegs.ptpUdpDstPorts      = baseAddr + 0x01A8;
        addrPtr->ttiRegs.ptpPacketCmdCfg0Reg = baseAddr + 0x01AC;
        addrPtr->ttiRegs.ptpPacketCmdCfg1Reg = baseAddr + 0x01BC;
        addrPtr->ttiRegs.ptpGlobalConfig     = baseAddr + 0x2000;
        addrPtr->ttiRegs.ptpEnableTimeStamp  = baseAddr + 0x2004;

        LOCAL_PORTS_BMP_REG_SET_MAC(addrPtr->ttiRegs.ptpTimeStampPortEnReg,
            (baseAddr + 0x2008));

        addrPtr->ttiRegs.ptpGlobalFifoCurEntry[0]    = baseAddr + 0x2030;
        addrPtr->ttiRegs.ptpGlobalFifoCurEntry[1]    = baseAddr + 0x2034;
        addrPtr->ttiRegs.ptpTodCntrNanoSeconds       = baseAddr + 0x2010;
        addrPtr->ttiRegs.ptpTodCntrSeconds[0]        = baseAddr + 0x2014;
        addrPtr->ttiRegs.ptpTodCntrSeconds[1]        = baseAddr + 0x2018;
        addrPtr->ttiRegs.ptpTodCntrShadowNanoSeconds = baseAddr + 0x201C;
        addrPtr->ttiRegs.ptpTodCntrShadowSeconds[0]  = baseAddr + 0x2020;
        addrPtr->ttiRegs.ptpTodCntrShadowSeconds[1]  = baseAddr + 0x2024;
    }



    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E))
    {
        addrPtr->ttiRegs.ttiMiscConfig  = baseAddr + 0x200;
    }

    if ((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        addrPtr->ttiRegs.ttiMiscConfig  = baseAddr + 0x13F8;

        addrPtr->ttiRegs.globalUnitConfigForCcPorts = baseAddr + 0x64;
        addrPtr->ttiRegs.trunkHashCfg[0]     = baseAddr + 0x84;
        addrPtr->ttiRegs.trunkHashCfg[1]     = baseAddr + 0x88;
        addrPtr->ttiRegs.trunkHashCfg[2]     = baseAddr + 0x8C;

        addrPtr->bridgeRegs.vlanRegs.protoTypeBased = baseAddr + 0x90;
        addrPtr->bridgeRegs.vlanRegs.protoEncBased = baseAddr + 0xA8;
        addrPtr->bridgeRegs.vlanRegs.protoEncBased1 = baseAddr + 0xAC;
        addrPtr->pclRegs.ipclTcpPortComparator0  = baseAddr + 0xB0;
        addrPtr->pclRegs.ipclUdpPortComparator0  = baseAddr + 0xD0;

        /* QoS Profile registers */
        addrPtr->bridgeRegs.qosProfileRegs.dscp2QosProfileMapTable = PRV_CPSS_SW_PTR_ENTRY_UNUSED;/*must use table - baseAddr + 0x0400;*/
        addrPtr->bridgeRegs.qosProfileRegs.up2QosProfileMapTable   = PRV_CPSS_SW_PTR_ENTRY_UNUSED;/*must use table - baseAddr + 0x0C00;*/
        addrPtr->bridgeRegs.qosProfileRegs.exp2QosProfileMapTable  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;/*must use table - baseAddr + 0x0B00;*/
        addrPtr->bridgeRegs.qosProfileRegs.dscp2DscpMapTable       = PRV_CPSS_SW_PTR_ENTRY_UNUSED;/*must use table - baseAddr + 0x0800;*/

        addrPtr->ttiRegs.timestampTagConfig = baseAddr + 0x2C;
    }
    else
    {
        addrPtr->ttiRegs.trunkHashCfg[0]     = baseAddr + 0x70;
        addrPtr->ttiRegs.trunkHashCfg[1]     = baseAddr + 0x74;
        addrPtr->ttiRegs.trunkHashCfg[2]     = baseAddr + 0x78;

        /* 6 registers 0x01000080-0x01000094 for 12 protocols */
        addrPtr->bridgeRegs.vlanRegs.protoTypeBased = baseAddr + 0x80;

        addrPtr->bridgeRegs.vlanRegs.protoEncBased = baseAddr + 0x98;
        addrPtr->bridgeRegs.vlanRegs.protoEncBased1 = baseAddr + 0x9C;

        addrPtr->pclRegs.ipclTcpPortComparator0  = baseAddr + 0xA0;
        addrPtr->pclRegs.ipclUdpPortComparator0  = baseAddr + 0xC0;

        /* QoS Profile registers */
        addrPtr->bridgeRegs.qosProfileRegs.dscp2QosProfileMapTable = baseAddr + 0x0400;
        addrPtr->bridgeRegs.qosProfileRegs.up2QosProfileMapTable   = baseAddr + 0x0440;
        addrPtr->bridgeRegs.qosProfileRegs.exp2QosProfileMapTable  = baseAddr + 0x0460;
        addrPtr->bridgeRegs.qosProfileRegs.dscp2DscpMapTable       = baseAddr + 0x0900;
    }

    /***********************************************/
    /* next registers are actually in the TTI unit */
    /***********************************************/



    addrPtr->pclRegs.ttiUnitConfig           = baseAddr + 0x0;
    addrPtr->pclRegs.ttiEngineConfig         = baseAddr + 0x0C;

    addrPtr->pclRegs.fastStack               = baseAddr + 0x60;
    addrPtr->pclRegs.loopPortReg             = baseAddr + 0x64;



    /* IPv6 Solicited-Node Multicast Address Configuration/Mask Register0 */
    addrPtr->bridgeRegs.ipv6McastSolicitNodeAddrBase = baseAddr + 0x40;
    addrPtr->bridgeRegs.ipv6McastSolicitNodeMaskBase = baseAddr + 0x50;

    addrPtr->bridgeRegs.vntReg.vntCfmEtherTypeConfReg = baseAddr + 0x01C8;

    FORMULA_SINGLE_REG_MAC(
        addrPtr->bridgeRegs.bridgeIngressVlanEtherTypeTable,
        (baseAddr + 0x0300),0x4,4);

    if ((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE))
    {
        FORMULA_SINGLE_REG_MAC(
            addrPtr->bridgeRegs.bridgeIngressVlanSelect,
            (baseAddr + 0x0310),0x4,(LOCAL_PORTS_NUM_CNS / 2));

        FORMULA_SINGLE_REG_MAC(
            addrPtr->ttiRegs.qos.cfiUpToQoSProfileMappingTableSelector,
            (baseAddr + 0x0470),0x4,(LOCAL_PORTS_NUM_CNS / 4));
    }
    else
    {
        FORMULA_SINGLE_REG_MAC(
            addrPtr->bridgeRegs.bridgeIngressVlanSelect,
            (baseAddr + 0x0310),0x4,    4   /* 4 registers : 0x00000310 + (0x8*t)+(0x4*p): where t (0-1) represents Tag, where p (0-1) represents Profile group*/);
    }



    /* next registers are EQ UNIT */
    if(PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups > 1)
    {
        addrPtr->eq.multiCoreLookup.multiCoreLookup0 = 0x0B00007C;

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* multiCoreLookup1 is used as base address to the 16 registers of
               Multi-Core Lookup configuration.  */
            addrPtr->eq.multiCoreLookup.multiCoreLookupInfo[0] = 0x0B0000E0;
        }
        else
        {
            for(i = 0 ; i < 8 ; i++)
            {
                addrPtr->eq.multiCoreLookup.multiCoreLookupInfo[i] = 0x0B000080 + 4 * i;
                if(i == 1 &&
                   PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_LION2_E)
                {
                    /* only 2 registers (support 4 port groups) */
                    break;
                }
            }
        }
    }

}

/*******************************************************************************
* lionHaRegsInit
*
* DESCRIPTION:
*       This function initializes the HA registers struct for Lion
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionHaRegsInit
(
   IN GT_U32 devNum,
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 baseAddr; /* Base Address */

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
        (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        /* Lion */
        baseAddr = 0x0E800000;
    }
    else
    {
        /* xCat2 */
        baseAddr = 0x0F000000;
    }

    addrPtr->haRegs.hdrAltMplsEthertypes = baseAddr + 0x0550;
    addrPtr->haRegs.hdrAltIEthertype     = baseAddr + 0x0554;
    addrPtr->haRegs.hdrAltGlobalConfig    = baseAddr + 0x0100;
    addrPtr->haRegs.hdrAltCnConfig        = baseAddr + 0x0420;
    addrPtr->haRegs.hdrAltCpidReg0        = baseAddr + 0x0430;
    addrPtr->haRegs.hdrAltCpidReg1        = baseAddr + 0x0434;
    addrPtr->haRegs.hdrAltCnTagFlowId     = baseAddr + 0x0428;
    addrPtr->haRegs.hdrAltCnmHeaderConfig = baseAddr + 0x0424;

    LOCAL_PORTS_BMP_REG_SET_MAC(addrPtr->haRegs.cscdHeadrInsrtConf,
        (baseAddr + 0x0004));
    LOCAL_PORTS_BMP_REG_SET_MAC(addrPtr->haRegs.eggrDSATagTypeConf,
        (baseAddr + 0x0020));

    LOCAL_PORTS_BMP_REG_SET_MAC(addrPtr->haRegs.invalidCrcModeConfigReg,
        (baseAddr + 0x0370));

    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.trunkNumConfigReg,
        (baseAddr + 0x002C),0x4,(PORTS_NUM_DEV_MAC(devNum) / 4));

    LOCAL_PORTS_BMP_REG_SET_MAC(addrPtr->haRegs.routerHdrAltEnMacSaModifyReg,
        (baseAddr + 0x0104));

    addrPtr->haRegs.routerMacSaBaseReg[0] = baseAddr + 0x010C;
    addrPtr->haRegs.routerMacSaBaseReg[1] = baseAddr + 0x0110;

    LOCAL_PORTS_BMP_REG_SET_MAC(addrPtr->haRegs.routerDevIdModifyEnReg,
        (baseAddr + 0x0114));

    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.routerHdrAltMacSaModifyMode,
        (baseAddr + 0x0120),0x4,(LOCAL_PORTS_WITH_CPU_BMP_NUM_DEV_CNS * 2));

    LOCAL_PORTS_BMP_REG_SET_MAC(
        addrPtr->haRegs.haVlanTransEnReg,
        (baseAddr + 0x0130));

    addrPtr->haRegs.coreRingConfiguration[0] = baseAddr + 0x140;
    addrPtr->haRegs.coreRingConfiguration[1] = baseAddr + 0x144;

    addrPtr->haRegs.qosProfile2ExpTable     = baseAddr + 0x0300;
    addrPtr->haRegs.qosProfile2DpTable      = baseAddr + 0x0340;

    LOCAL_PORTS_BMP_REG_SET_MAC(/* NOTE: this register not using local port like other registers in the HA */
        addrPtr->haRegs.toAnalyzerVlanAddConfig, (baseAddr + 0x0400));
    addrPtr->haRegs.ingAnalyzerVlanTagConfig = baseAddr + 0x0408;
    addrPtr->haRegs.egrAnalyzerVlanTagConfig = baseAddr + 0x040C;
    LOCAL_PORTS_BMP_REG_SET_MAC(addrPtr->haRegs.mirrorToAnalyzerHeaderConfReg,(baseAddr + 0x0080));

    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.bridgeEgressVlanEtherTypeTable,
        (baseAddr + 0x0500),0x4,TPID_NUM_CNS/2);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.bridgeEgressPortTag0TpidSelect,
        (baseAddr + 0x0510),0x4,(LOCAL_PORTS_NUM_CNS/8));
    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.bridgeEgressPortTag1TpidSelect,
        (baseAddr + 0x0530),0x4,(LOCAL_PORTS_NUM_CNS/8));

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
        (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        addrPtr->haRegs.miscConfig = baseAddr + 0x0710;
    }


    /* PTP */
    addrPtr->haRegs.ptpGlobalConfig = baseAddr + 0x0800;
    addrPtr->haRegs.ptpEnableTimeStamp = baseAddr + 0x0804;
    LOCAL_PORTS_BMP_REG_SET_MAC(addrPtr->haRegs.ptpTimeStampPortEnReg, (baseAddr + 0x0808));

    addrPtr->haRegs.ptpGlobalFifoCurEntry[0] = baseAddr + 0x0830;
    addrPtr->haRegs.ptpGlobalFifoCurEntry[1] = baseAddr + 0x0834;

    addrPtr->haRegs.ptpTodCntrNanoSeconds = baseAddr + 0x0810;
    addrPtr->haRegs.ptpTodCntrSeconds[0] = baseAddr + 0x0814;
    addrPtr->haRegs.ptpTodCntrSeconds[1] = baseAddr + 0x0818;
    addrPtr->haRegs.ptpTodCntrShadowNanoSeconds = baseAddr + 0x081C;
    addrPtr->haRegs.ptpTodCntrShadowSeconds[0] = baseAddr + 0x0820;
    addrPtr->haRegs.ptpTodCntrShadowSeconds[1] = baseAddr + 0x0824;

    FORMULA_SINGLE_REG_MAC(
        addrPtr->haRegs.keepVlan1Reg,
        (baseAddr + 0x0440),0x4,(LOCAL_PORTS_NUM_CNS / 4));

}

/*******************************************************************************
* lionCutThroughRegsInit
*
* DESCRIPTION:
*       This function initializes the Cut Through registers.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionCutThroughRegsInit
(
   IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        addrPtr->cutThroughRegs.ctEnablePerPortReg  = 0x0F000050;
        addrPtr->cutThroughRegs.ctEthertypeReg      = 0x0F000054;
        addrPtr->cutThroughRegs.ctUpEnableReg       = 0x0F000058;
        addrPtr->cutThroughRegs.ctPacketIndentificationReg = 0x0F00005C;
        addrPtr->cutThroughRegs.ctCascadingPortReg  = 0x0F000064;
    }
    else
    {
        addrPtr->cutThroughRegs.ctEnablePerPortReg = 0x0F000010;
        addrPtr->cutThroughRegs.ctEthertypeReg = 0x0F000014;
        addrPtr->cutThroughRegs.ctUpEnableReg = 0x0F000018;
        addrPtr->cutThroughRegs.ctPacketIndentificationReg = 0x0F00001C;
        addrPtr->cutThroughRegs.ctCascadingPortReg = 0x0F000020;
    }

    addrPtr->cutThroughRegs.ctCpuPortMemoryRateLimitThresholdReg = 0x0F800098;
    addrPtr->cutThroughRegs.ctGlobalConfigurationReg = 0x00000390;

    FORMULA_SINGLE_REG_MAC(
        addrPtr->cutThroughRegs.ctPortMemoryRateLimitThresholdReg,0x0F800080,0x4,6);
}

/*******************************************************************************
* lionPolicerRegsInit
*
* DESCRIPTION:
*       This function initializes the Policer registers
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionPolicerRegsInit
(
   IN GT_U32          devNum,
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  baseAddr[3]; /* IPRL0, IPRL1, EPRL*/
    GT_U32  ii;

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
        (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        /* for Bobcat2, Caelum, Bobcat3 those base addresses are aligned by convertOldRegDbToNewUnitsBaseAddr(...) */

        /* for Lion */
        baseAddr[0] = 0x0C000000;
        baseAddr[1] = 0x00800000;
        baseAddr[2] = 0x07800000;
    }
    else
    {
        /* for XCAT2 */
        baseAddr[0] = 0x0C800000;
        baseAddr[1] = 0x0D000000;
        baseAddr[2] = 0x03800000;
    }

    for(ii = 0 ; ii < 3 ; ii++)
    {
        addrPtr->PLR[ii].policerControlReg            = baseAddr[ii] + 0x00000000; /*  0x0C000000  0x00800000 0x07800000 */
        addrPtr->PLR[ii].policerPortMeteringEnReg[0]  = baseAddr[ii] + 0x00000008; /*  0x0C000008  0x00800008 0x07800008 */
        addrPtr->PLR[ii].policerPortMeteringEnReg[1]  = baseAddr[ii] + 0x0000000C; /*  0x0C00000C  0x0080000C 0x0780000C */
        addrPtr->PLR[ii].policerMRUReg                = baseAddr[ii] + 0x00000010; /*  0x0C000010  0x00800010 0x07800010 */
        addrPtr->PLR[ii].policerErrorReg              = baseAddr[ii] + 0x00000050; /*  0x0C000050  0x00800050 0x07800050 */
        addrPtr->PLR[ii].policerErrorCntrReg          = baseAddr[ii] + 0x00000054; /*  0x0C000054  0x00800054 0x07800054 */
        addrPtr->PLR[ii].policerTblAccessControlReg   = baseAddr[ii] + 0x00000070; /*  0x0C000070  0x00800070 0x07800070 */
        addrPtr->PLR[ii].policerTblAccessDataReg      = baseAddr[ii] + 0x00000074; /*  0x0C000074  0x00800074 0x07800074 */
        addrPtr->PLR[ii].policerInitialDPReg          = baseAddr[ii] + 0x000000C0; /*  0x0C0000C0  0x008000C0 0x078000C0 */
        addrPtr->PLR[ii].policerIntCauseReg           = baseAddr[ii] + 0x00000100; /*  0x0C000100  0x00800100 0x07800100 */
        addrPtr->PLR[ii].policerIntMaskReg            = baseAddr[ii] + 0x00000104; /*  0x0C000104  0x00800104 0x07800104 */
        addrPtr->PLR[ii].policerShadowReg             = baseAddr[ii] + 0x00000108; /*  0x0C000108  0x00800108 0x07800108 */
        addrPtr->PLR[ii].policerControl1Reg           = baseAddr[ii] + 0x00000004; /*  0x0C000004  0x00800004 0x07800004 */
        addrPtr->PLR[ii].policerControl2Reg           = baseAddr[ii] + 0x0000002C; /*  0x0C00002C  0x0080002C 0x0780002C */
        addrPtr->PLR[ii].portMeteringPtrIndexReg      = baseAddr[ii] + 0x00001800; /*  0x0C001800  0x00801800 0x07801800 */

        if (((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) && (PRV_CPSS_PP_MAC(devNum)->revision > 3)) ||
            ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) && (PRV_CPSS_PP_MAC(devNum)->revision > 1)))
        {
            addrPtr->PLR[ii].metalFix                 = baseAddr[ii] + 0x00000120; /*  0x0C000120  0x00800120 0x07800120 */
        }

        /* PLR Tables */
        addrPtr->PLR[ii].policerMeteringCountingTbl   = baseAddr[ii] + 0x00040000; /*  0x0C040000  0x00840000 0x07840000 */
        addrPtr->PLR[ii].policerQosRemarkingTbl       = baseAddr[ii] + 0x00080000; /*  0x0C080000  0x00880000 0x07880000 */
        addrPtr->PLR[ii].policerManagementCntrsTbl    = baseAddr[ii] + 0x00000500; /*  0x0C000500  0x00800500 0x07800500 */
        addrPtr->PLR[ii].policerCountingTbl           = baseAddr[ii] + 0x00060000; /*  0x0C060000  0x00860000 0x07860000 */
        addrPtr->PLR[ii].policerTimerTbl              = baseAddr[ii] + 0x00000200; /*  0x0C000200  0x00800200 0x07800200 */
#ifdef GM_USED
        if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            addrPtr->PLR[ii].policerMeteringCountingTbl   = baseAddr[ii] + 0x00100000;
            addrPtr->PLR[ii].policerCountingTbl           = baseAddr[ii] + 0x00140000;
            addrPtr->PLR[ii].policerTimerTbl              = baseAddr[ii] + 0x00000300;
        }
#endif
    }

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
        (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        /* for Lion */
        for(ii = 0 ; ii < 3 ; ii++)
        {
            addrPtr->PLR[ii].ipfixControl                 = baseAddr[ii] + 0x00000014; /*  0x0C000014  0x00800014 0x07800014 */
            addrPtr->PLR[ii].ipfixNanoTimerStampUpload    = baseAddr[ii] + 0x00000018; /*  0x0C000018  0x00800018 0x07800018 */
            addrPtr->PLR[ii].ipfixSecLsbTimerStampUpload  = baseAddr[ii] + 0x0000001C; /*  0x0C00001C  0x0080001C 0x0780001C */
            addrPtr->PLR[ii].ipfixSecMsbTimerStampUpload  = baseAddr[ii] + 0x00000020; /*  0x0C000020  0x00800020 0x07800020 */
            addrPtr->PLR[ii].ipfixDroppedWaThreshold      = baseAddr[ii] + 0x00000030; /*  0x0C000030  0x00800030 0x07800030 */
            addrPtr->PLR[ii].ipfixPacketWaThreshold       = baseAddr[ii] + 0x00000034; /*  0x0C000034  0x00800034 0x07800034 */
            addrPtr->PLR[ii].ipfixByteWaThresholdLsb      = baseAddr[ii] + 0x00000038; /*  0x0C000038  0x00800038 0x07800038 */
            addrPtr->PLR[ii].ipfixByteWaThresholdMsb      = baseAddr[ii] + 0x0000003C; /*  0x0C00003C  0x0080003C 0x0780003C */
            addrPtr->PLR[ii].ipfixSampleEntriesLog0       = baseAddr[ii] + 0x00000048; /*  0x0C000048  0x00800048 0x07800048 */
            addrPtr->PLR[ii].ipfixSampleEntriesLog1       = baseAddr[ii] + 0x0000004C; /*  0x0C00004C  0x0080004C 0x0780004C */
            addrPtr->PLR[ii].ipfixWrapAroundAlertTbl      = baseAddr[ii] + 0x00000800; /*  0x0C000800  0x00800800 0x07800800 */
            addrPtr->PLR[ii].ipfixAgingAlertTbl           = baseAddr[ii] + 0x00000900; /*  0x0C000900  0x00800900 0x07800900 */
        }
    }
    else
    {
        /* for XCAT2 */
        for(ii = 0 ; ii < 3 ; ii++)
        {
            addrPtr->PLR[ii].ipfixControl                 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixNanoTimerStampUpload    = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixSecLsbTimerStampUpload  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixSecMsbTimerStampUpload  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixDroppedWaThreshold      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixPacketWaThreshold       = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixByteWaThresholdLsb      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixByteWaThresholdMsb      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixSampleEntriesLog0       = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixSampleEntriesLog1       = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixWrapAroundAlertTbl      = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->PLR[ii].ipfixAgingAlertTbl           = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }
    }
}

/*******************************************************************************
* lionGlobalRegsInit
*
* DESCRIPTION:
*       This function initializes the Global registers struct
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionGlobalRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    devNum = devNum;

    addrPtr->globalRegs.chipId = 0x000000D4;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE)
    {
        addrPtr->globalRegs.globalControl4         = 0x00000364;
        addrPtr->globalRegs.globalControl3         = 0x00000368;
        addrPtr->globalRegs.tcamPllParams          = 0x0000036c;
        addrPtr->globalRegs.tcamPllCtrl            = 0x00000370;
        /* register exists in BC2_A0 but not used any more.
           in BC2_B0 was removed */
        addrPtr->globalRegs.mediaInterface         = 0x0000038c;

        addrPtr->globalRegs.sdPllParams          = 0x00000374;
        addrPtr->globalRegs.sdPllCtrl            = 0x00000378;
    }
}

/*******************************************************************************
* lionTxqRegsInit
*
* DESCRIPTION:
*       This function initializes the TXQ registers struct
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionTxqRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  *tmpPtr;
    GT_U32  ii;
    GT_U32  size;
    GT_U32  baseAddr;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* not relevant to 'TXQ rev 0'  devices */
        return ;
    }

    /* start by unsetting all the 'TXQ related' registers that MUST not be used
       for this device */
    {
        tmpPtr = (GT_U32*)(&addrPtr->egrTxQConf);
        size = sizeof(addrPtr->egrTxQConf) / sizeof(GT_U32);

        /***************************************/
        /* set 'old' TXQ registers as 'unused' */
        /***************************************/
        defaultAddressUnusedSet(tmpPtr,size);

        addrPtr->egrTxQConf.txPortRegs = NULL;/* avoid FREE error */
        addrPtr->egrTxQConf.swTxPortRegsBookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->egrTxQConf.swTxPortRegsType   = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->egrTxQConf.swTxPortRegsSize   = 0;

        /***************************************/
        /* set 'old' TXQ registers as 'unused' */
        /***************************************/
        addrPtr->bridgeRegs.egressFilter = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.egressFilter2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.mcastLocalEnableConfig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.egressFilterIpMcRouted = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.vntReg.vntOamLoopbackConfReg  = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.nstRegs.cpuPktsFrwFltConfReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.nstRegs.brgPktsFrwFltConfReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.nstRegs.routePktsFrwFltConfReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.srcIdReg.srcIdUcEgressFilterConfReg = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bridgeRegs.egressFilterUnregBc = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        for(ii = 0; ii < 32; ii++)
        {
            addrPtr->bufferMng.devMapTbl[ii]       = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
            addrPtr->bufferMng.srcIdEggFltrTbl[ii] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }

        addrPtr->bufferMng.trunkTblCfgRegs.baseAddrNonTrunk   = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->bufferMng.trunkTblCfgRegs.baseAddrTrunkDesig = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->interrupts.txqIntCause    = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->interrupts.txqIntMask     = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        addrPtr->macRegs.hgXsmiiCountersCtr = PRV_CPSS_SW_PTR_ENTRY_UNUSED;


        /* need to use table PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E
           instead of those registers */
        addrPtr->sFlowRegs.egrStcTblWord0 = NULL;/* avoid FREE error */
        addrPtr->sFlowRegs.swEgrStcTblW0Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW0Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW0Size     = 0;

        addrPtr->sFlowRegs.egrStcTblWord1 = NULL;/* avoid FREE error */
        addrPtr->sFlowRegs.swEgrStcTblW1Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW1Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW1Size     = 0;

        addrPtr->sFlowRegs.egrStcTblWord2 = NULL;/* avoid FREE error */
        addrPtr->sFlowRegs.swEgrStcTblW2Bookmark = PRV_CPSS_SW_PTR_BOOKMARK_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW2Type     = PRV_CPSS_SW_TYPE_PTR_FIELD_CNS;
        addrPtr->sFlowRegs.swEgrStcTblW2Size     = 0;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        baseAddr = 0x01800000;
    }
    else
    {
        baseAddr = PRV_CPSS_DXCH_LION2_TXQ_EGR0_BASE_ADDRESS_CNS;
    }

    addrPtr->txqVer1.egr.global.cpuPortDistribution = baseAddr + 0x0;
    addrPtr->txqVer1.egr.global.egressInterruptMask = baseAddr + 0xB0;

    addrPtr->txqVer1.egr.mcFifoConfig.globalConfig  = baseAddr + 0x1000;
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.egr.mcFifoConfig.distributionMask[0],baseAddr + 0x1010);
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        addrPtr->txqVer1.egr.mcFifoConfig.arbiterWeights = baseAddr + 0x1008;
        for(ii = 1; ii < MC_FIFO_NUM_CNS; ii++)
        {
             prvPortsBmpRegisterSetEmulate128Ports(devNum,
                 addrPtr->txqVer1.egr.mcFifoConfig.distributionMask[ii],
                 baseAddr + 0x1010 + 0x8 * ii);
        }
    }

    addrPtr->txqVer1.egr.devMapTableConfig.addrConstruct  = baseAddr + 0x1100;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.egr.devMapTableConfig.localSrcPortMapOwnDevEn,baseAddr + 0x1110,
        0x4,2);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        /* source port map enable is per global port */
        FORMULA_SINGLE_REG_MAC(
            addrPtr->txqVer1.egr.devMapTableConfig.localSrcPortMapOwnDevEn,baseAddr + 0x1110,
            0x4,4);
    }

    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.egr.devMapTableConfig.localTrgPortMapOwnDevEn,baseAddr + 0x1120);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.egr.devMapTableConfig.secondaryTargetPortTable,baseAddr + 0x1300,
        0x4,LOCAL_PORTS_NUM_CNS);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        FORMULA_SINGLE_REG_WITH_INDEX_MAC(
            addrPtr->txqVer1.egr.devMapTableConfig.secondaryTargetPortTable,
            baseAddr + 0x1300, 0x4, LOCAL_PORTS_NUM_CNS, LOCAL_PORTS_NUM_CNS);
    }

    addrPtr->txqVer1.egr.filterConfig.globalEnables  = baseAddr + 0x10000;
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.egr.filterConfig.unknownUcFilterEn,baseAddr + 0x10010);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.egr.filterConfig.unregisteredMcFilterEn,baseAddr + 0x10020);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.egr.filterConfig.unregisteredBcFilterEn,baseAddr + 0x10030);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.egr.filterConfig.mcLocalEn,baseAddr + 0x10040);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.egr.filterConfig.ipmcRoutedFilterEn,baseAddr + 0x10050);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.egr.filterConfig.ucSrcIdFilterEn,baseAddr + 0x10060);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.egr.filterConfig.oamLoopbackFilterEn,baseAddr + 0x10070);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.egr.filterConfig.egrCnEn,baseAddr + 0x10080);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.egr.filterConfig.egrCnFcEn,baseAddr + 0x10090);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        baseAddr = 0x0A000000;
    }
    else
    {
        baseAddr = PRV_CPSS_DXCH_LION2_TXQ_QUEUE_BASE_ADDRESS_CNS;
    }

    addrPtr->txqVer1.queue.distributor.generalConfig  = baseAddr + 0x91000;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.distributor.tcDpRemapping,baseAddr + 0x91010,0x4,2);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.distributor.stackTcRemapping,baseAddr + 0x91020,0x4,TC_NUM_CNS);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.queue.distributor.stackRemapEn,baseAddr + 0x91050);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.queue.distributor.controlTcRemapEn,baseAddr + 0x91060);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.queue.distributor.dataTcRemapEn,baseAddr + 0x91070);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.queue.distributor.dpToCfiRemapEn,baseAddr + 0x91080);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.queue.distributor.fromCpuForwardRestricted,baseAddr + 0x91090);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.queue.distributor.bridgedForwardRestricted,baseAddr + 0x910a0);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.queue.distributor.routedForwardRestricted,baseAddr + 0x910b0);

    addrPtr->txqVer1.queue.peripheralAccess.misc.misc  = baseAddr + 0x93000;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.config,baseAddr + 0x93200,0x4,2);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingUnicastPacketCounter,baseAddr + 0x93210,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingMulticastPacketCounter,baseAddr + 0x93220,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingBroadcastPacketCounter,baseAddr + 0x93230,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.bridgeEgressFilteredPacketCounter,baseAddr + 0x93240,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.tailDroppedPacketCounter,baseAddr + 0x93250,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.controlPacketCounter,baseAddr + 0x93260,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.egressForwardingRestrictionDroppedPacketsCounter,baseAddr + 0x93270,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.peripheralAccess.egressMibCounterSet.multicastFifoDroppedPacketsCounter,baseAddr + 0x93280,0x4,EGRESS_COUNTERS_SETS_NUM_CNS);

    addrPtr->txqVer1.queue.peripheralAccess.cncModes.modesRegister  = baseAddr + 0x936A0;

    addrPtr->txqVer1.queue.tailDrop.config.config = baseAddr + 0xA0000;
    addrPtr->txqVer1.queue.tailDrop.config.byteCount = baseAddr + 0xA0008;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.config.portEnqueueEnable,baseAddr + 0xA0010,
        0x4,LOCAL_PORTS_NUM_CNS);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        FORMULA_SINGLE_REG_WITH_INDEX_MAC(
            addrPtr->txqVer1.queue.tailDrop.config.portEnqueueEnable,
            baseAddr + 0xA0010, 0x4, LOCAL_PORTS_NUM_CNS, LOCAL_PORTS_NUM_CNS);
    }

    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.config.tailDropCnProfile,baseAddr + 0xA0120,
        0x4,LOCAL_PORTS_NUM_CNS);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        FORMULA_SINGLE_REG_WITH_INDEX_MAC(
            addrPtr->txqVer1.queue.tailDrop.config.tailDropCnProfile,
            baseAddr + 0xA0120, 0x4, LOCAL_PORTS_NUM_CNS, LOCAL_PORTS_NUM_CNS);
    }

    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.config.tcProfileEnableSharedPoolUsage,baseAddr + 0xA0220,0x4,TC_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.config.profileEnableWrtdDp,baseAddr + 0xA0240,0x4,4);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.config.profilePriorityQueueToSharedPoolAssociation,baseAddr + 0xA0250,0x4,PROFILE_NUM_CNS);
    addrPtr->txqVer1.queue.tailDrop.config.wrtdMask0 = baseAddr + 0xA0270;
    addrPtr->txqVer1.queue.tailDrop.config.wrtdMask1 = baseAddr + 0xA0274;

    addrPtr->txqVer1.queue.tailDrop.limits.globalDescriptorsLimit = baseAddr + 0xA0300;
    addrPtr->txqVer1.queue.tailDrop.limits.globalBufferLimit = baseAddr + 0xA0310;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.profilePortLimits,baseAddr + 0xA0320,0x4,PROFILE_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.sharedPoolLimits,baseAddr + 0xA0400,0x4,SHARED_POOLS_NUM_CNS);

    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.maximumQueueLimits,baseAddr + 0xA0500,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.eqQueueLimitsDp0,baseAddr + 0xA0800,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.dqQueueBufLimits,baseAddr + 0xA0900,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.dqQueueDescLimits,baseAddr + 0xA0A00,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.eqQueueLimitsDp1,baseAddr + 0xA0C00,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.limits.eqQueueLimitsDp2,baseAddr + 0xA0D00,
            0x20,PROFILE_NUM_CNS,0x4,TC_NUM_CNS);

    addrPtr->txqVer1.queue.tailDrop.counters.totalDescCounter = baseAddr + 0xA1000;
    addrPtr->txqVer1.queue.tailDrop.counters.totalBuffersCounter = baseAddr + 0xA1004;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.portDescCounter,baseAddr + 0xA1010,0x4,
        LOCAL_PORTS_NUM_CNS);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        FORMULA_SINGLE_REG_WITH_INDEX_MAC(
            addrPtr->txqVer1.queue.tailDrop.counters.portDescCounter,
            baseAddr + 0xA1010, 0x4, LOCAL_PORTS_NUM_CNS, LOCAL_PORTS_NUM_CNS);
    }

    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.portBuffersCounter,baseAddr + 0xA1110,0x4,
        LOCAL_PORTS_NUM_CNS);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        FORMULA_SINGLE_REG_WITH_INDEX_MAC(
            addrPtr->txqVer1.queue.tailDrop.counters.portBuffersCounter,
            baseAddr + 0xA1110, 0x4, LOCAL_PORTS_NUM_CNS, LOCAL_PORTS_NUM_CNS);
    }

    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.tcSharedDescCounter,baseAddr + 0xA1210,0x4,SHARED_POOLS_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.tcSharedBuffersCounter,baseAddr + 0xA1250,0x4,SHARED_POOLS_NUM_CNS);
    /* There are 512 counters (lines) , selected by {port[5:0], prio[2:0]}*/
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.qMainDesc,0x1088C000,
            0x20,LOCAL_PORTS_NUM_CNS ,0x4,TC_NUM_CNS);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        FORMULA_DOUBLE_REG_MAC(
            addrPtr->txqVer1.queue.tailDrop.counters.qMainDesc + LOCAL_PORTS_NUM_CNS ,0x1088C000,
            0x20,LOCAL_PORTS_NUM_CNS ,0x4,TC_NUM_CNS);
    }

    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.counters.qMainBuff,baseAddr + 0xA4000,
            0x20,LOCAL_PORTS_NUM_CNS,0x4,TC_NUM_CNS);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        FORMULA_DOUBLE_REG_MAC(
            addrPtr->txqVer1.queue.tailDrop.counters.qMainBuff + LOCAL_PORTS_NUM_CNS,baseAddr + 0xA4000,
            0x20,LOCAL_PORTS_NUM_CNS ,0x4,TC_NUM_CNS);
    }

    addrPtr->txqVer1.queue.tailDrop.mcFilterLimits.mirroredPacketsToAnalyzerPortDescriptorsLimit = baseAddr + 0xA3300;
    addrPtr->txqVer1.queue.tailDrop.mcFilterLimits.mirroredPacketsToAnalyzerPortBuffersLimit = baseAddr + 0xA3310;
    addrPtr->txqVer1.queue.tailDrop.mcFilterLimits.multicastDescriptorsLimit = baseAddr + 0xA3320;
    addrPtr->txqVer1.queue.tailDrop.mcFilterLimits.multicastBuffersLimit = baseAddr + 0xA3330;

    addrPtr->txqVer1.queue.tailDrop.mcFilterCounters.snifferDescriptorsCounter = baseAddr + 0xA6000;
    addrPtr->txqVer1.queue.tailDrop.mcFilterCounters.snifferBuffersCounter = baseAddr + 0xA6004;
    addrPtr->txqVer1.queue.tailDrop.mcFilterCounters.multicastDescriptorsCounter = baseAddr + 0xA6008;
    addrPtr->txqVer1.queue.tailDrop.mcFilterCounters.multicastBuffersCounter = baseAddr + 0xA600C;

    addrPtr->txqVer1.queue.tailDrop.resourceHistogram.limitRegister1 = baseAddr + 0xA6300;
    addrPtr->txqVer1.queue.tailDrop.resourceHistogram.limitRegister2 = baseAddr + 0xA6304;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.queue.tailDrop.resourceHistogram.counters,baseAddr + 0xA6310,0x4,4);

    addrPtr->txqVer1.queue.congestNotification.cnGlobalConfigReg = baseAddr + 0xB0000;
    addrPtr->txqVer1.queue.congestNotification.cnPrioQueueEnProfile = baseAddr + 0xB0010;
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.queue.congestNotification.cnEnCnFrameTx,baseAddr + 0xB0020);
    prvPortsBmpRegisterSetEmulate128Ports(devNum,
        addrPtr->txqVer1.queue.congestNotification.cnFcEn,baseAddr + 0xB0030);
    addrPtr->txqVer1.queue.congestNotification.fbCalcConfigReg = baseAddr + 0xB0040;
    addrPtr->txqVer1.queue.congestNotification.fbMinReg = baseAddr + 0xB0050;
    addrPtr->txqVer1.queue.congestNotification.fbMaxReg = baseAddr + 0xB0060;
    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.queue.congestNotification.cnProfileThreshold,baseAddr + 0xB0300,
            0x4,TC_NUM_CNS,0x20,PROFILE_NUM_CNS);
    addrPtr->txqVer1.queue.congestNotification.cnSampleTbl = baseAddr + 0xB0090;
    addrPtr->txqVer1.queue.congestNotification.cnBufferFifoParityErrorsCnt = baseAddr + 0xB00E0;

    addrPtr->txqVer1.queue.pfcRegs.pfcGlobalConfigReg = baseAddr + 0xC0000;
    addrPtr->txqVer1.queue.pfcRegs.pfcSrcPortFcMode = baseAddr + 0xC0010;
    addrPtr->txqVer1.queue.pfcRegs.pfcSourcePortProfile = baseAddr + 0xC00B0;
    addrPtr->txqVer1.queue.pfcRegs.pfcGlobaGroupOfPortsThresholds = baseAddr + 0xC0600;
    addrPtr->txqVer1.queue.pfcRegs.pfcProfileXoffThresholds = baseAddr + 0xC0800;
    addrPtr->txqVer1.queue.pfcRegs.pfcProfileXonThresholds = baseAddr + 0xC0A00;

    addrPtr->txqVer1.dq.global.config.config = 0x11000000;
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        LION2_16_LOCAL_PORTS_TO_128_GLOBAL_PORTS_REG_SET_MAC(
            addrPtr->txqVer1.dq.global.flushTrig.portTxqFlushTrigger,0x11000090,0x4);
    }
    else
    {
        LION_16_LOCAL_PORTS_TO_64_GLOBAL_PORTS_REG_SET_MAC(
            addrPtr->txqVer1.dq.global.flushTrig.portTxqFlushTrigger,0x11000090,0x4);
    }

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
        (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        addrPtr->txqVer1.dq.scheduler.portArbiter.pizzaArbiterConfig = 0x11001400;
        FORMULA_SINGLE_REG_MAC(addrPtr->txqVer1.dq.scheduler.portArbiter.pizzaArbiterMap,
                                0x11001404, 0x4,
                                /* (0-41) represents Slice_Group */
                                42);
    }

    addrPtr->txqVer1.dq.global.debug.metalFix = 0x11000180;

    addrPtr->txqVer1.dq.scheduler.config.config = 0x11001000;
    addrPtr->txqVer1.dq.scheduler.config.schedulerByteCountForTailDrop = 0x11001008;
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
    {
        addrPtr->txqVer1.dq.scheduler.config.portRequestMask = 0x11001004;
    LION_16_LOCAL_PORTS_TO_64_GLOBAL_PORTS_REG_SET_MAC(
        addrPtr->txqVer1.dq.scheduler.config.portRequestMaskSelector,0x1100100C,0x4);
    }

    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrWeightsConfig0,
            0x11001140,0x4,PROFILE_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrWeightsConfig1,
            0x11001180,0x4,PROFILE_NUM_CNS);

    {
        FORMULA_SINGLE_REG_MAC(
            addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrGroup,
                0x110011C0,0x10,4);

        FORMULA_SINGLE_REG_WITH_INDEX_MAC(
            addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrGroup,
                0x110011C0 + 0xEC0,
                0x10,4,
                4);/* start index */
    }

    {
        FORMULA_SINGLE_REG_MAC(
            addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrEnable,
                0x11001200,0x10,4);

        FORMULA_SINGLE_REG_WITH_INDEX_MAC(
            addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.profileSdwrrEnable,
                0x11001200 + 0xEC0,
                0x10,4,
                4);/* start index */
    }

    addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.portCpuSchedulerProfile = 0x11001240;
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        LION2_16_LOCAL_PORTS_TO_128_GLOBAL_PORTS_REG_SET_MAC(
            addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.portSchedulerProfile,
                0x11001280,0x4);
    }
    else
    {
        LION_16_LOCAL_PORTS_TO_64_GLOBAL_PORTS_REG_SET_MAC(
            addrPtr->txqVer1.dq.scheduler.priorityArbiterWeights.portSchedulerProfile,
                0x11001280,0x4);
    }


    addrPtr->txqVer1.dq.shaper.tokenBucketUpdateRate = 0x11002000;
    addrPtr->txqVer1.dq.shaper.tokenBucketBaseLine = 0x11002004;
    addrPtr->txqVer1.dq.shaper.cpuTokenBucketMtuConfig = 0x11002008;
    addrPtr->txqVer1.dq.shaper.portsTokenBucketMtuConfig = 0x1100200C;
    addrPtr->txqVer1.dq.shaper.tokenBucketMode = 0x11002010;
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
    (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        LION2_16_LOCAL_PORTS_TO_128_GLOBAL_PORTS_REG_SET_MAC(
            addrPtr->txqVer1.dq.shaper.portDequeueEnable,0x11002014,0x4);
    }
    else
    {
        LION_16_LOCAL_PORTS_TO_64_GLOBAL_PORTS_REG_SET_MAC(
            addrPtr->txqVer1.dq.shaper.portDequeueEnable,0x11002014,0x4);
    }
    addrPtr->txqVer1.dq.flowControl.flowControlConfig = 0x11003000;
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.dq.flowControl.schedulerProfileLlfcMtu,0x11003008,0x4,PROFILE_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.dq.flowControl.pfcTimerToPriorityMapping,0x11003040,0x4,TC_NUM_CNS);
    FORMULA_SINGLE_REG_MAC(
        addrPtr->txqVer1.dq.flowControl.schedulerProfileTcToPriorityMapEnable,0x11003060,0x4,PROFILE_NUM_CNS);

    FORMULA_DOUBLE_REG_MAC(
        addrPtr->txqVer1.dq.flowControl.tcProfilePfcFactor,0x11003080,
            0x20,TC_NUM_CNS,0x4,PROFILE_NUM_CNS);

    addrPtr->txqVer1.dq.statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.
        stcStatisticalTxSniffConfig = 0x11004000;
    addrPtr->txqVer1.dq.statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.
        egressCtmTrigger = 0x11004004;
    addrPtr->txqVer1.dq.statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.
        egressAnalyzerEnable = 0x11004010;

    addrPtr->txqVer1.sht.egressTables.deviceMapTableBaseAddress = 0x11870000;
    addrPtr->txqVer1.sht.global.shtGlobalConfig = 0x118F0000;

    addrPtr->txqVer1.sht.dfx.dfx0 = 0x118F0D80;
    addrPtr->txqVer1.sht.dfx.dfx1 = 0x118F0D84;
    addrPtr->txqVer1.sht.dfx.dfx2 = 0x118F0D88;
    addrPtr->txqVer1.sht.dfx.dfx3 = 0x118F0D8C;

    addrPtr->txqVer1.sht.dfx.dfxStatus0 = 0x118F0D90;
}


/*******************************************************************************
* lionTccUnitRegsInit
*
* DESCRIPTION:
*       This function initializes the IPvX,PCL registers struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- lion B
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionTccUnitRegsInit
(
    IN    GT_U32                            devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32      i;  /* loop iterator */

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* not relevant to 'TXQ rev 0'  devices */
        return ;
    }

    /***************************************************/
    /* those addresses are actually like in the Lion A */
    /* but we set them because xcat A1 has other values*/
    /***************************************************/

    /* IPCL TCC unit */
    addrPtr->pclRegs.actionTableBase      = 0x0d0B0000;
    addrPtr->pclRegs.tcamOperationTrigger = 0x0d000138;
    addrPtr->pclRegs.tcamOperationParam1  = 0x0d000130;
    addrPtr->pclRegs.tcamOperationParam2  = 0x0d000134;
    addrPtr->pclRegs.tcamReadPatternBase  = 0x0d040000;
    addrPtr->pclRegs.pclTcamConfig0       = 0x0d000208;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        addrPtr->pclRegs.tcamReadPatternParityBase  = 0x0D0C0000;
        addrPtr->pclRegs.tcamReadMaskParityBase  = 0x0D0D0000;
        addrPtr->pclRegs.pclTccRegs.policyTcamControl = 0x0D000148;
        addrPtr->pclRegs.pclTccRegs.policyTcamErrorCounter = 0x0D000198;
        addrPtr->pclRegs.pclTccRegs.policyTccErrorInformation = 0x0D000170;
    }

    /* IPvX TCC unit */
    addrPtr->ipRegs.routerTtTcamAccessCtrlReg      = 0x0d80041c;
    addrPtr->ipRegs.routerTtTcamAccessDataCtrlReg  = 0x0d800418;
    addrPtr->ipRegs.routerTtTcamAccessDataRegsBase = 0x0d800400;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        addrPtr->ipRegs.ipTccRegs.ipTcamControl = 0x0D800960;
        addrPtr->ipRegs.ipTccRegs.ipTccErrorInformation = 0x0D800D78;
        addrPtr->ipRegs.ipTccRegs.ipTcamErrorCounter = 0x0D800054;
        addrPtr->ipRegs.ipTccRegs.ipTcamReadPatternParityBase = 0x0D890000;
        addrPtr->ipRegs.ipTccRegs.ipTcamReadMaskParityBase = 0x0D8A0000;
    }

    /* only base of all TCAM will be initialized */
    addrPtr->ipRegs.routerTtTcamBankAddr[0] = 0x0d840000;

    addrPtr->ipRegs.ipTcamConfig0       = 0x0d80096c;

    addrPtr->ipRegs.ipTcamControl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

    /* The following registers are not used in xCat */
    /* Tcam bank addresses, starting from the second one */
    for (i = 1; i <= 4; i++)
    {
        addrPtr->ipRegs.routerTtTcamBankAddr[i] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
}

/*******************************************************************************
* lionEpclRegsInit
*
* DESCRIPTION:
*       This function initializes lion specific EPCL registers addresses
*       to overlap the Cheetah3 registers addresses
*
* INPUTS:
*       devNum     - device number
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void lionEpclRegsInit
(
    IN    GT_U32                            devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 addrBase; /* address base */

    /* EPCL registers */

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
        (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        /* Lion */
        addrBase = 0x0E000000;
    }
    else
    {
        /* xCat2 */
        addrBase = 0x0F800000;
    }

    addrPtr->pclRegs.epclGlobalConfig        = addrBase;

        /* bit per port registers */
    PORTS_BMP_REG_SET_MAC(devNum,
        addrPtr->pclRegs.epclCfgTblAccess, (addrBase + 0x10));
    PORTS_BMP_REG_SET_MAC(devNum,
        addrPtr->pclRegs.epclEnPktNonTs, (addrBase + 0x20));
    PORTS_BMP_REG_SET_MAC(devNum,
        addrPtr->pclRegs.epclEnPktTs, (addrBase + 0x30));
    PORTS_BMP_REG_SET_MAC(devNum,
        addrPtr->pclRegs.epclEnPktToCpu, (addrBase + 0x40));
    PORTS_BMP_REG_SET_MAC(devNum,
        addrPtr->pclRegs.epclEnPktFromCpuData, (addrBase + 0x50));
    PORTS_BMP_REG_SET_MAC(devNum,
        addrPtr->pclRegs.epclEnPktFromCpuControl, (addrBase + 0x60));
    PORTS_BMP_REG_SET_MAC(devNum,
        addrPtr->pclRegs.epclEnPktToAnalyzer, (addrBase + 0x70));

    /* TCP/UDP port EPCL comparators (8 register sequences) */
    addrPtr->pclRegs.epclTcpPortComparator0  = (addrBase + 0x100);
    addrPtr->pclRegs.epclUdpPortComparator0  = (addrBase + 0x140);
    addrPtr->pclRegs.eplrGlobalConfig = (addrBase + 0x0200);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* bit per port registers */
        prvPortsBmpRegisterSetEmulate128Ports(devNum,
            addrPtr->pclRegs.eplrPortBasedMetering,(addrBase + 0x230));
    }
}

/*******************************************************************************
* lionMllRegsInit
*
* DESCRIPTION:
*       This function initializes the MLL registers struct for
*       devices that belong to REGS_FAMILY_1 registers set . -- lion
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionMllRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32 i;
    GT_U32 offset;

    addrPtr->ipMtRegs.mllGlobalReg = 0x0C800000;
    addrPtr->ipMtRegs.multiTargetRateShapingReg = 0x0C800210;
    addrPtr->ipMtRegs.mtUcSchedulerModeReg = 0x0C800214;
    addrPtr->ipMtRegs.mtTcQueuesSpEnableReg = 0x0C800200;
    addrPtr->ipMtRegs.mtTcQueuesWeightReg = 0x0C800204;
    addrPtr->ipMtRegs.mtTcQueuesGlobalReg = 0x0C800004;
    addrPtr->ipMtRegs.mllMcFifoFullDropCnt = 0x0C800984;
    for (i = 0 ; i < 8 ; i++)
    {
        addrPtr->ipMtRegs.qoSProfile2MTTCQueuesMapTable[i] = 0x0C800100 + 4 * i;
    }
    addrPtr->ipMtRegs.mllTableBase = 0x0C880000;
    for (i = 0 ; i < 2 ; i++)
    {
        offset = i * 0x100;
        addrPtr->ipMtRegs.mllOutInterfaceCfg[i]    = 0x0C800980 + offset;
        addrPtr->ipMtRegs.mllOutMcPktCnt[i]        = 0x0C800900 + offset;
    }
}

/*******************************************************************************
* lionCncRegsInit
*
* DESCRIPTION:
*       This function initializes the CNC registers struct for
*       devices that belong to REGS_FAMILY_1 registers set . -- lion
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionCncRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    /* overwrite per couner block configuration */
    addrPtr->cncRegs.blockClientCfgAddrBase   = 0x08001180;

}

/*******************************************************************************
* lionMirrorRegsInit
*
* DESCRIPTION:
*       This function initializes the Global registers struct for
*       devices that belong to REGS_FAMILY_1 registers set. -- lion
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionMirrorRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32   i;

    for (i = 3; i <= 5; i++)
    {
        addrPtr->bufferMng.eqBlkCfgRegs.portTxMirrorIndex[i] =
            0x0B00B044 + (i - 3) * 4;
    }

    addrPtr->bufferMng.eqBlkCfgRegs.portTxMirrorIndex[6] = 0x0B00B070;
}

/*******************************************************************************
* lionPclRegsInit
*
* DESCRIPTION:
*       This function initializes Lion specific PCL registers addresses
*       to overlap the xcat registers addresses , or to add new registers
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static void lionPclRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->pclRegs.ipclEngineInterruptCause = 0x0B800004;
    addrPtr->pclRegs.crcHashConfigurationReg = 0x0B800020;
    addrPtr->pclRegs.pclEngineExtUdbConfig   = 0x0B800024;
    FORMULA_SINGLE_REG_MAC(addrPtr->pclRegs.pearsonHashTableReg,0x0B8005C0 ,0x4,PEARSON_HASH_TABLE_NUM_CNS);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* policy TCAM Test - Lion but not XCAT2 */
        FORMULA_SINGLE_REG_MAC(
            addrPtr->pclRegs.policyTcamTest.pointerRelReg,0x0D000400 ,0x4,14);

        addrPtr->pclRegs.policyTcamTest.configStatusReg = 0x0D000450;
        addrPtr->pclRegs.policyTcamTest.opcodeCommandReg = 0x0D000454;
        addrPtr->pclRegs.policyTcamTest.opcodeExpectedHitReg = 0x0D000458;

        addrPtr->pclRegs.configTblParityErrCtr[0] = 0xB80006C;
        addrPtr->pclRegs.configTblParityErrCtr[1] = 0xB800070;
    }

}

/*******************************************************************************
* lionDescriptorRegsInit
*
* DESCRIPTION:
*       This function initializes the Descriptor registers.
*
* APPLICABLE DEVICES:
*        Lion, Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lionDescriptorRegsInit
(
    IN GT_U32          devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
    {
        addrPtr->descriptorsConfig.wrDmaToTti = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        addrPtr->descriptorsConfig.ttiToPcl = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }
    else
    {
        addrPtr->descriptorsConfig.wrDmaToTti = 0x01001498;
        addrPtr->descriptorsConfig.ttiToPcl = 0x01001400;
    }

    addrPtr->descriptorsConfig.pclToBridge = 0x0B800200;
    addrPtr->descriptorsConfig.bridgeToRouterSet0 = 0x02040020;
    addrPtr->descriptorsConfig.bridgeToRouterSet1 = 0x02040A00;
    addrPtr->descriptorsConfig.preEgressToTxq0 = 0x0B000044;
    addrPtr->descriptorsConfig.preEgressToTxq1 = 0x0B00B050;
    addrPtr->descriptorsConfig.ingressPolicer0ToIngressPolicer1Set0 = 0x0C000400;
    addrPtr->descriptorsConfig.ingressPolicer0ToIngressPolicer1Set1 = 0x0C000430;
    addrPtr->descriptorsConfig.ingressPolicer1ToPreEgressSet0 = 0x00800400;
    addrPtr->descriptorsConfig.ingressPolicer1ToPreEgressSet1 = 0x00800430;
    addrPtr->descriptorsConfig.egressPolicerEgressToPclSet0 = 0x07800400;
    addrPtr->descriptorsConfig.egressPolicerEgressToPclSet1 = 0x07800430;
    addrPtr->descriptorsConfig.txqToHeaderAlteration = 0x0E8003E0;
    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        addrPtr->descriptorsConfig.routerToIngressPolicer = 0x02800CE0;
    }

}

/*******************************************************************************
* prvCpssDxChLionHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh Lion devices.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLionHwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);


    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* first set the "xCat" part */
    rc = prvCpssDxChXcatHwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the specific "DXCH Lion" parts */
    lionTtiRegsInit(devNum,regsAddrPtr);
    lionPclRegsInit(devNum, regsAddrPtr);
    lionHaRegsInit(devNum,regsAddrPtr);
    lionIpRegsInit(devNum, regsAddrPtr);
    lionBufMngRegsInit(devNum, regsAddrPtr);
    lionCutThroughRegsInit(devNum,regsAddrPtr);
    lionPolicerRegsInit(devNum, regsAddrPtr);
    lionGlobalRegsInit(devNum, regsAddrPtr);
    lionTccUnitRegsInit(devNum, regsAddrPtr);
    lionMllRegsInit(regsAddrPtr);
    lionTxqRegsInit(devNum, regsAddrPtr);
    lionEpclRegsInit(devNum, regsAddrPtr);
    lionCncRegsInit(regsAddrPtr);
    lionMirrorRegsInit(regsAddrPtr);
    lionDescriptorRegsInit(devNum,regsAddrPtr);
    lionMiscellaneousRegsInit(devNum,regsAddrPtr);

    return GT_OK;
}
/*******************************************************************************
* xCat2BridgeRegsInit
*
* DESCRIPTION:
*       This function initializes the registers bridge struct for devices that
*       belong to REGS_FAMILY_1 registers set. -- xCat2
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCat2BridgeRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    addrPtr->bridgeRegs.miiSpeedGranularity   = 0x02040740;
    addrPtr->bridgeRegs.gmiiSpeedGranularity  = 0x02040744;
    addrPtr->bridgeRegs.stackSpeedGranularity = 0x02040748;
}


/*******************************************************************************
* xCat2DfxUnitsRegsInit
*
* DESCRIPTION:
*       This function initializes the registers DFX Units struct for xCat2
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCat2DfxUnitsRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    GT_U8 i; /* iterator */
    GT_U32  dfxClientBaseAddr = 0x820000; /* base address of DFX clients memory */

    addrPtr->dfxUnits.server.pipeSelect              = 0x008F8004;
    addrPtr->dfxUnits.server.temperatureSensorStatus = 0x008F8050;
    addrPtr->dfxUnits.server.resetControl            = 0x008F8014;
    addrPtr->dfxUnits.server.serverSkipInitMatrix    = 0x008F8024;
    addrPtr->dfxUnits.server.genSkipInitMatrix0      = 0x008F8028;
    addrPtr->dfxUnits.server.genSkipInitMatrix1      = 0x008F802C;
    addrPtr->dfxUnits.server.ledPadControl           = 0x008F8300;
    addrPtr->dfxUnits.server.gppPadControl           = 0x008F8304;
    addrPtr->dfxUnits.server.rgmiiPadControl         = 0x008F8310;
    addrPtr->dfxUnits.server.ssmiiPadControl         = 0x008F8314;

    addrPtr->dfxUnits.ingressLower.tcBist.tcamBistConfigStatus = 0x00804050;

    addrPtr->dfxUnits.ingressUpper.tcBist.tcamBistConfigStatus = 0x0080C050;

    for(i = 0; i < 2; i++)
    {/* actually these are cliens 4 and 10, but because used just they two I don't
        implement exact formula */
        addrPtr->dfxUnits.client[i].dataCtrl = dfxClientBaseAddr + (i*0x30000) + 0x2008;
        addrPtr->dfxUnits.client[i].dataSamplingCtrl = dfxClientBaseAddr + (i*0x30000) + 0x200C;
        addrPtr->dfxUnits.client[i].debugInfo = dfxClientBaseAddr + (i*0x30000) + 0x2014;
    }

}

/*******************************************************************************
* xCat2TxqRegsInit
*
* DESCRIPTION:
*       This function initializes the TXQ registers -- xCat2
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with TXQ regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void xCat2TxqRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC *addrPtr
)
{
    GT_U8   i;

    addrPtr->egrTxQConf.queueWRTDMask = 0x01800230;
    addrPtr->egrTxQConf.portWRTDMask = 0x01800234;
    addrPtr->egrTxQConf.sharedPoolWRTDMask = 0x01800238;

    for (i = 0; i < PRV_CPSS_DXCH2_PROFILES_NUM_CNS; i++)
    {
        addrPtr->egrTxQConf.setsConfigRegs.portWRTDEn[i]       = 0x01800260 + i * 4;
        addrPtr->egrTxQConf.setsConfigRegs.queueWRTDEn[i]      = 0x018002A0 + i * 4;
        addrPtr->egrTxQConf.setsConfigRegs.sharedPoolWRTDEn[i] = 0x018002C0 + i * 4;
    }
}

/*******************************************************************************
* prvCpssDxChXcat2HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh xCat2 devices.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat2HwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                          CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* first set the "xCat" part */
    rc = prvCpssDxChXcatHwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set common Lion and xCat2 registers */
    lionTtiRegsInit(devNum,regsAddrPtr);
    lionPclRegsInit(devNum, regsAddrPtr);
    lionEpclRegsInit(devNum, regsAddrPtr);
    lionHaRegsInit(devNum,regsAddrPtr);
    lionBufMngRegsInit(devNum, regsAddrPtr);
    lionPolicerRegsInit(devNum, regsAddrPtr);
    lionCncRegsInit(regsAddrPtr);
    lionMirrorRegsInit(regsAddrPtr);

    /* set xCat2 specific registers */
    xCat2BridgeRegsInit(regsAddrPtr);
    xCat2DfxUnitsRegsInit(regsAddrPtr);
    xCat2TxqRegsInit(regsAddrPtr);
    xCat2DescriptorRegsInit(regsAddrPtr);

    return GT_OK;
}

/*******************************************************************************
* lion2MirrorRegsInit
*
* DESCRIPTION:
*       This function initializes the Mirror registers struct for Lion2 devices
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr     - registers struct with bridge regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion2MirrorRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;

    devNum = devNum;
    for (i = 7; i <= 13; i++)
    {
        addrPtr->bufferMng.eqBlkCfgRegs.portTxMirrorIndex[i] =
            0x0B00B074 + (i - 7) * 4;
    }
}

/*******************************************************************************
* lion2TxqRegsInit
*
* DESCRIPTION:
*       This function initializes the TXQ registers struct
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion2TxqRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    devNum = devNum;
    /* override Lion registers addresses */

    /* In Lion2 PFC triggering configurations moved to CPFC Unit */
    addrPtr->txqVer1.queue.pfcRegs.pfcGlobalConfigReg = 0x13800000;
    addrPtr->txqVer1.queue.pfcRegs.pfcSrcPortFcMode = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    addrPtr->txqVer1.queue.pfcRegs.pfcSourcePortProfile = 0x138000F0;
    addrPtr->txqVer1.queue.pfcRegs.pfcGlobaGroupOfPortsThresholds = 0x13800600;
    addrPtr->txqVer1.queue.pfcRegs.pfcProfileXoffThresholds = 0x13800800;
    addrPtr->txqVer1.queue.pfcRegs.pfcProfileXonThresholds = 0x13800A00;

    /* Lion2 LLFC register inside the TXQ unit*/
    addrPtr->txqVer1.queue.llfcRegs.pfcGlobalConfigReg = 0x100C0000;
    addrPtr->txqVer1.queue.llfcRegs.pfcProfileXoffThresholds = 0x100c1800;
    addrPtr->txqVer1.queue.llfcRegs.pfcGlobalGroupOfPortsThresholds = 0x100C1000;
    FORMULA_SINGLE_REG_MAC(
    addrPtr->txqVer1.queue.llfcRegs.pfcPipeCountersParityErrorsCounter, 0x100C00D0,
    0x4,8);

    /* Lion2 Unicast or Multicast Arbiter unit inside the TXQ unit */
    addrPtr->txqVer1.queue.ucMcArb.mcShaperControl = 0x1000980C;


    addrPtr->txqVer1.dist.deviceMapTable.addrConstruct = 0x13000200;
    FORMULA_SINGLE_REG_MAC(
    addrPtr->txqVer1.dist.deviceMapTable.localSrcPortMapOwnDevEn,0x13000210,
    0x4,4);
    FORMULA_SINGLE_REG_MAC(
    addrPtr->txqVer1.dist.deviceMapTable.localTrgPortMapOwnDevEn,0x13000220,
    0x4,4);
    addrPtr->txqVer1.dist.deviceMapTable.distEntryDevMapTableBaseAddress = 0x13000230;
    addrPtr->txqVer1.dist.global.distGlobalControlReg = 0x13000000;

    /* Lion2 TxQ SHT global registers */
    addrPtr->txqVer1.sht.global.vlanParityErrorLastEntries = 0x118F0010;
    addrPtr->txqVer1.sht.global.egrVlanParityErrorCounter = 0x118F0020;
    addrPtr->txqVer1.sht.global.ingVlanParityErrorCounter = 0x118F0030;
    addrPtr->txqVer1.sht.global.portIsolationParityErrorLastEntries = 0x118F0040;
    addrPtr->txqVer1.sht.global.vidxParityErrorLastEntry = 0x118F0044;
    addrPtr->txqVer1.sht.global.l2PortIsolationParityErrorCounter = 0x118F0050;
    addrPtr->txqVer1.sht.global.l3PortIsolationParityErrorCounter = 0x118F0060;
    addrPtr->txqVer1.sht.global.vidxParityErrorCounter = 0x118F0070;

    /* Lion2 TxQ DQ registers */
    addrPtr->txqVer1.dq.global.memoryParityError.tokenBucketPriorityParityErrorCounter = 0x11000150;
    addrPtr->txqVer1.dq.global.memoryParityError.stateVariablesParityErrorCounter = 0x11000154;
    addrPtr->txqVer1.dq.global.memoryParityError.parityErrorBadAddress = 0x11000158;

    /* Lion2 TxQ LL registers */
    addrPtr->txqVer1.ll.linkList.linkListEccControl.fbufRamEccErrorCounter = 0x1088A034;
    addrPtr->txqVer1.ll.linkList.linkListEccControl.fbufRamEccStatus = 0x1088A024;
}

/*******************************************************************************
* lion2HaRegsInit
*
* DESCRIPTION:
*       This function initializes the HA registers struct for Lion
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion2HaRegsInit
(
   IN GT_U32 devNum,
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
        /* support 128 ports instead of 64 (global ports) */
        FORMULA_SINGLE_REG_MAC(addrPtr->haRegs.trunkNumConfigReg,
                               0x0E800160,0x4,(PORTS_NUM_DEV_MAC(devNum) / 4 * 2));

        FORMULA_SINGLE_REG_MAC(addrPtr->haRegs.erspanEntryWord1, 0x0E8005D4, 0x18, 8);

}

/*******************************************************************************
* lion2DfxUnitsRegsInit
*
* DESCRIPTION:
*       This function initializes the DFX registers struct for Lion2.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion2DfxUnitsRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    devNum = devNum;
    addrPtr->dfxUnits.server.pipeSelect                   = 0x018F8000;
    addrPtr->dfxUnits.server.resetControl                 = 0x018f800C;
    addrPtr->dfxUnits.server.initDoneStatus               = 0x018F8014;
    addrPtr->dfxUnits.server.temperatureSensorControlMsb  = 0x018F8074;
    addrPtr->dfxUnits.server.temperatureSensorStatus      = 0x018F8078;

    addrPtr->dfxUnits.server.functionalSarStatusLsb       = 0x018F8200;

}

/*******************************************************************************
* lion2PackGenRegsInit
*
* DESCRIPTION:
*       This function initializes the Packet Generator registers struct for Lion2.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion2PackGenRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U32  i, offset;
    GT_U32  gensNum; /* number of supported packet generators */

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        /* there is no packet Gen */
        return;
    }

    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        gensNum = 1;
    }
    else
    {
        gensNum = 4;
    }

    for (i=0;i < gensNum; i++)
    {
        offset = i * 0x4000;
        if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            offset += 0xC00;
        }
        addrPtr->packGenConfig[i].macDa[0]       = 0x08980000 + offset;
        addrPtr->packGenConfig[i].macDa[1]       = 0x08980004 + offset;
        addrPtr->packGenConfig[i].macDa[2]       = 0x08980008 + offset;
        addrPtr->packGenConfig[i].macSa[0]       = 0x0898000C + offset;
        addrPtr->packGenConfig[i].macSa[1]       = 0x08980010 + offset;
        addrPtr->packGenConfig[i].macSa[2]       = 0x08980014 + offset;
        addrPtr->packGenConfig[i].etherType      = 0x08980018 + offset;
        addrPtr->packGenConfig[i].vlanTag        = 0x0898001C + offset;
        addrPtr->packGenConfig[i].dataPattern[0] = 0x08980020 + offset;
        addrPtr->packGenConfig[i].dataPattern[1] = 0x08980024 + offset;
        addrPtr->packGenConfig[i].dataPattern[2] = 0x08980028 + offset;
        addrPtr->packGenConfig[i].dataPattern[3] = 0x0898002C + offset;
        addrPtr->packGenConfig[i].packetLength   = 0x08980040 + offset;
        addrPtr->packGenConfig[i].packetCount    = 0x08980044 + offset;
        addrPtr->packGenConfig[i].ifg            = 0x08980048 + offset;
        addrPtr->packGenConfig[i].controlReg0    = 0x08980050 + offset;
    }

}

/*******************************************************************************
* lion2MppmUnitRegsInit
*
* DESCRIPTION:
*       This function initializes the MPPM registers struct for Lion2.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion2MppmUnitRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->mppm[0].xlgMode = 0x06800000;
    addrPtr->mppm[1].xlgMode = 0x0A000000;

    addrPtr->mppm[0].ecc.eccLastFailedAddress = 0x06800018;
    addrPtr->mppm[1].ecc.eccLastFailedAddress = 0x0A000018;
}

/*******************************************************************************
* lion3SdmaRegsInit
*
* DESCRIPTION:
*       This function initializes the SDMA registers struct
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with sdma regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion3SdmaRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    GT_U8   i;
    GT_U32  offset;

    for (i = 0; i < 8; i++)
    {
        offset = i * 4;
        addrPtr->sdmaRegs.txSdmaPacketGeneratorConfigQueue[i] =
                                                            0x28B0 + offset;
        addrPtr->sdmaRegs.txSdmaPacketCountConfigQueue[i]     =
                                                            0x28D0 + offset;
    }
}

/*******************************************************************************
* lion2andEqUnitRegsInit
*
* DESCRIPTION:
*       This function initializes the EQ registers struct for Lion2 and Bobcat2; Caelum; Bobcat3.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion2andEqUnitRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    devNum = devNum;
    addrPtr->eq.globalConfig.dualDevIdAndLossyDropConfig = 0x0B0000AC;
}

/*******************************************************************************
* lion2CoprocessorUnitRegsInit
*
* DESCRIPTION:
*       This function initializes the Z80 related registers struct for Lion2
*       and Bobcat2; Caelum; Bobcat3.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion2CoprocessorUnitRegsInit
(
    IN GT_U32                               devNum,
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    devNum = devNum;
    (GT_VOID)addrPtr;
#ifndef ASIC_SIMULATION_NO_HW
    addrPtr->globalRegs.coprocessor.hostCpuDoorBell = 0x00000518;
#endif
}

/*******************************************************************************
* lion2MllRegsInit
*
* DESCRIPTION:
*       This function initializes the MLL registers struct for
*       devices that belong to REGS_FAMILY_1 registers set . -- lion2
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with IPv4 regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion2MllRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ipMtRegs.mllMetalFix  = 0x0C80000C;
}


/*******************************************************************************
* lion2MgRegsInit
*
* DESCRIPTION:
*       This function initializes the MG registers struct for
*       devices that belong to Lion2 registers set.
*
* INPUTS:
*       addrPtr    - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with MG regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion2MgRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->globalRegs.globalInterrupt.dfxInterruptCause  = 0x000000AC;
    addrPtr->globalRegs.globalInterrupt.dfxInterruptMask  = 0x000000B0;
}


/*******************************************************************************
* lion2IpRegsInit
*
* DESCRIPTION:
*       This function initializes the IP registers struct for
*       devices that belong to Lion2 registers set.
*
* INPUTS:
*       addrPtr - pointer to registers struct
*
* OUTPUTS:
*       addrPtr - registers struct with regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion2IpRegsInit
(
   INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->ipRegs.routerDataError = 0x0280096C;

    addrPtr->bma.bmaCore[0].mcCntParityErrorCounter = 0x0381000C;
    addrPtr->bma.bmaCore[1].mcCntParityErrorCounter = 0x0383000C;
    addrPtr->bma.bmaCore[2].mcCntParityErrorCounter = 0x0385000C;
    addrPtr->bma.bmaCore[3].mcCntParityErrorCounter = 0x0387000C;

    return;
}


/*******************************************************************************
* prvCpssDxChLion2HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh Lion2 devices.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLion2HwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E| CPSS_XCAT2_E);

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* first set the "Lion" part */
    rc = prvCpssDxChLionHwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Add Xcat 2 sections */
    xCat2BridgeRegsInit(regsAddrPtr);


    /* set common Lion2 registers */
    lion2MgRegsInit(regsAddrPtr);
    lion2MirrorRegsInit(devNum, regsAddrPtr);
    lion2TxqRegsInit(devNum, regsAddrPtr);
    lion2HaRegsInit(devNum,regsAddrPtr);
    lion2DfxUnitsRegsInit(devNum,regsAddrPtr);
    lion2PackGenRegsInit(devNum,regsAddrPtr);

    rc = lion2and3MacRegsInit(devNum,regsAddrPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
    lion2MppmUnitRegsInit(regsAddrPtr);
    lion2andEqUnitRegsInit(devNum,regsAddrPtr);
    lion2CoprocessorUnitRegsInit(devNum,regsAddrPtr);
    lion2MllRegsInit(regsAddrPtr);
    lion2IpRegsInit(regsAddrPtr);

    return GT_OK;
}

/*******************************************************************************
* lion3GlobalRegsInit
*
* DESCRIPTION:
*       This function initializes the Global registers struct
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       addrPtr     - pointer to registers struct
*
* OUTPUTS:
*       addrPtr    - registers struct with buffer management regs init
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void lion3GlobalRegsInit
(
    INOUT PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr
)
{
    addrPtr->globalRegs.generalConfigurations  = 0x0000001C;
    addrPtr->globalRegs.auqConfig_generalControl = 0x000000D4;
    addrPtr->globalRegs.auqConfig_hostConfig     = 0x000000D8;
    addrPtr->globalRegs.genxsRateConfig = 0x00000060;
}

/*******************************************************************************
* prvCpssDxChLion3HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLion3HwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E| CPSS_XCAT2_E | CPSS_LION2_E);

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* first set the "Lion" part */
    rc = prvCpssDxChLionHwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set common Lion3 registers */
    lion3GlobalRegsInit(regsAddrPtr);
    lion2PackGenRegsInit(devNum,regsAddrPtr);
    rc = lion2and3MacRegsInit(devNum,regsAddrPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
    lion3SdmaRegsInit(regsAddrPtr);

    /* init the DB of 'regsAddrVer1' */
    rc = prvCpssDxChHwRegAddrVer1Init((GT_U8)devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* init the DB of 'resetAndInitControllerRegsAddr' */
    rc = prvCpssDxChHwResetAndInitControllerRegAddrInit((GT_U8)devNum);
    return rc;
}

/*******************************************************************************
* prvCpssDxChBobcat2HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh Bobcat2 devices.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2HwRegAddrInit
(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E| CPSS_XCAT2_E | CPSS_LION2_E );

    /* first set the "Lion" part */
    rc = prvCpssDxChLion3HwRegAddrInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXcatHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChXcatHwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcatHwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    /* first free the "Cheetah3" part */
    rc = prvCpssDxCh3HwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "xCat" parts */

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXcat3HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChXcat3HwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat3HwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E |
                                          CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    /* first free the "xCat" part */
    rc = prvCpssDxChXcatHwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "xCat3" parts */

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXcat2HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChXcat2HwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat2HwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                          CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    /* first free the "xCat" part */
    rc = prvCpssDxChXcatHwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "xCat2" parts */

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLionHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChLionHwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLionHwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    /* first free the "xCat" part */
    rc = prvCpssDxChXcatHwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "Lion" parts */

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLion2HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChLionHwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLion2HwRegAddrRemove
(
    IN  GT_U32 devNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    /* first free the "Lion" part */
    rc = prvCpssDxChLionHwRegAddrRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free the specific "Lion2" parts */

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChHwRegAddrPortMacUpdate
*
* DESCRIPTION:
*       This function updates mac registers addresses for given port accordingly
*           to currently used MAC Unit
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       ifMode   - new interface mode used with this MAC
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrPortMacUpdate
(
    IN  GT_U32 devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_U32 offset, mibOffset;   /* register offset */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsPtr;
    GT_U32 baseAddr,
            mibMacCntrsBaseAddr = 0x9000000; /* base address of MIB MAC counters */
    GT_U32  localPort;/* local port - support multi-port-groups device */
    GT_U32  gigBaseAddr, xgBaseAddr;

    GT_BOOL extendedMode; /* port uses extended MAC */
    GT_STATUS rc; /* return code */
    GT_U32          portMacNum;      /* MAC number */
    GT_BOOL supportSpecialPort_9_11 = GT_FALSE;
    CPSS_DXCH_PORT_MAC_PARAMS_STC   activeMac;
    CPSS_DXCH_PORT_COMBO_PARAMS_STC *comboParamsPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC((GT_U8)(devNum), portNum, portMacNum);

    /* convert the 'Physical port' to local port -- supporting multi-port-groups device */
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portMacNum);


    mibOffset = localPort * 0x20000;

    regsPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
            /* call specific bc2 function */
            rc = prvCpssDxChHwBcat2RegAddrPortMacUpdate((GT_U8)devNum,portMacNum);
            /* no more to do */
            return rc;
        case CPSS_PP_FAMILY_DXCH_LION2_E:
        case CPSS_PP_FAMILY_DXCH_XCAT3_E:

            gigBaseAddr = prvCpssDxChHwUnitBaseAddrGet((GT_U8)devNum,PRV_CPSS_DXCH_UNIT_GOP_E,NULL);/*Lion2 : 0x08800000*/
            xgBaseAddr = gigBaseAddr + 0x000C0000;/*Lion2 : 0x088C0000;*/
            offset = localPort * 0x1000;

            mibMacCntrsBaseAddr = prvCpssDxChHwUnitBaseAddrGet((GT_U8)devNum,PRV_CPSS_DXCH_UNIT_MIB_E,NULL);/*Lion2 : 0x09000000*/

            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
            {
                supportSpecialPort_9_11 = GT_TRUE;
            }


            break;
        default:
            gigBaseAddr = 0x0A800000;
            xgBaseAddr = 0x08800000;
            offset = localPort * 0x400;
            break;
    }

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) < PRV_CPSS_PORT_XG_E)
    {
        if(PRV_CPSS_PP_MAC(devNum)->revision == 0 &&
           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E &&
           portNum >= 24)
        {
            baseAddr = 0x08800000;
        }
        else
        {
            baseAddr = gigBaseAddr;/* xcat A1 and above */
        }

        if(supportSpecialPort_9_11 == GT_TRUE)
        {/* In Lion 2,3 mac counters are common for all MAC Units, only ports 9,11
            which could use alien MAC need redirection */
            if((9 == localPort) || (11 == localPort))
            {
                extendedMode = GT_FALSE;
                comboParamsPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->port.comboPortsInfoArray[portNum];
                if(comboParamsPtr->macArray[0].macNum != CPSS_DXCH_PORT_COMBO_NA_MAC_CNS)
                {
                    rc = cpssDxChPortComboPortActiveMacGet((GT_U8)devNum, portNum,
                                                            &activeMac);
                    if(rc != GT_OK)
                        return rc;

                    if (activeMac.macNum != localPort)
                    {
                        extendedMode = GT_TRUE;
                    }
                }
                else
                {
                    rc = cpssDxChPortExtendedModeEnableGet((GT_U8)devNum, portNum,
                                                                &extendedMode);
                    if(rc != GT_OK)
                        return rc;
                }

                if(extendedMode)
                {
                    mibOffset += (3 * 0x20000);
                    offset += (3 * 0x1000);
                }

                regsPtr->macRegs.perPortRegs[portMacNum].macCounters =
                                    mibMacCntrsBaseAddr + mibOffset;

                regsPtr->macRegs.perPortRegs[portMacNum].serialParameters = baseAddr + 0x14 + offset;
                regsPtr->macRegs.perPortRegs[portMacNum].autoNegCtrl      = baseAddr + 0xC + offset;
                regsPtr->macRegs.perPortRegs[portMacNum].prbsCheckStatus  = baseAddr + 0x38 + offset;
                regsPtr->macRegs.perPortRegs[portMacNum].prbsErrorCounter = baseAddr + 0x3C + offset;

                regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl =
                    baseAddr + 0 + offset;
                regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 =
                    baseAddr + 4 + offset;
                regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 =
                    baseAddr + 8 + offset;
                regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3 =
                    baseAddr + 0x48 + offset;
                regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl4 =
                    baseAddr + 0x90 + offset;
                regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_GE_E].fcDsaTag2 =
                    baseAddr + 0x80 + offset;
                regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_GE_E].fcDsaTag3 =
                    baseAddr + 0x84 + offset;
                regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_GE_E].ccfcFcTimer =
                    baseAddr + 0x58 + offset;
            }
        }
        regsPtr->macRegs.perPortRegs[portMacNum].macStatus = baseAddr + 0x10 + offset;
        regsPtr->macRegs.perPortRegs[portMacNum].macStatus1 = baseAddr + 0x40 + offset;
    }
    /* XG ports */
    else
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_LION2_E)
        {
            if((CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily) &&
                (ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E))
            {
                offset = 0x3000;
            }
            regsPtr->macRegs.perPortRegs[portMacNum].macStatus = xgBaseAddr + 0x0C + offset;
        }
        else  /* Lion2,3 */
        {
            if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) != PRV_CPSS_PORT_XG_E)
            {
                if((9 == localPort) || (11 == localPort))
                {
                    mibOffset += (3 * 0x20000);
                    offset += (3 * 0x1000);
                }

                regsPtr->macRegs.perPortRegs[portMacNum].macCounters =
                                        mibMacCntrsBaseAddr + mibOffset;
            }
            else
            {
                if((9 == localPort) || (11 == localPort))
                {
                    extendedMode = GT_FALSE;
                    comboParamsPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->port.comboPortsInfoArray[portNum];
                    if(comboParamsPtr->macArray[0].macNum != CPSS_DXCH_PORT_COMBO_NA_MAC_CNS)
                    {
                        rc = cpssDxChPortComboPortActiveMacGet((GT_U8)devNum, portNum,
                                                                &activeMac);
                        if(rc != GT_OK)
                            return rc;

                        if (activeMac.macNum != localPort)
                        {
                            extendedMode = GT_TRUE;
                        }
                    }
                    else
                    {
                        rc = cpssDxChPortExtendedModeEnableGet((GT_U8)devNum,portNum,&extendedMode);
                        if(rc != GT_OK)
                            return rc;
                    }

                    if(extendedMode)
                    {
                        mibOffset += (3 * 0x20000);
                        offset += (3 * 0x1000);
                    }

                    regsPtr->macRegs.perPortRegs[portMacNum].macCounters =
                                        mibMacCntrsBaseAddr + mibOffset;

                    regsPtr->macRegs.perPortRegs[portMacNum].xgMibCountersCtrl = xgBaseAddr + 0x30 + offset;
                    regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_XG_E].pcs40GCommonCtrl =
                                                                        xgBaseAddr + 0x414 + offset;
                    regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_XG_E].pcs40GCommonStatus =
                                                                        xgBaseAddr + 0x430 + offset;
                    regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_XG_E].pcs40GGearBoxStatus =
                                                                        xgBaseAddr + 0x4D0 + offset;

                    regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl =
                        xgBaseAddr + 0 + offset;
                    regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl1 =
                        xgBaseAddr + 4 + offset;
                    regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl2 =
                        xgBaseAddr + 8 + offset;
                    regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl3 =
                        xgBaseAddr + 0x1C + offset;
                    regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl4 =
                        xgBaseAddr + 0x84 + offset;
                    regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_XG_E].fcDsaTag2 =
                        xgBaseAddr + 0x70 + offset;
                    regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_XG_E].fcDsaTag3 =
                        xgBaseAddr + 0x74 + offset;
                    regsPtr->macRegs.perPortRegs[portMacNum].macRegsPerType[PRV_CPSS_PORT_XG_E].ccfcFcTimer =
                        xgBaseAddr + 0x38 + offset;
                }
            }
            regsPtr->macRegs.perPortRegs[portMacNum].macStatus = xgBaseAddr + 0x0C + offset;
            regsPtr->macRegs.perPortRegs[portMacNum].externalUnitsIntmask = xgBaseAddr + 0x5C + offset;
        }
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* MIB MAC counters */
        if (ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E)
        {
            regsPtr->macRegs.perPortRegs[portMacNum].macCounters        =
                0x09180000;
        }
        else
        {
            offset = localPort * 0x20000;
            regsPtr->macRegs.perPortRegs[portMacNum].macCounters        =
                mibMacCntrsBaseAddr + offset;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChHwBcat2RegAddrPortMacUpdate
*
* DESCRIPTION:
*       This function updates mac registers addresses for given port accordingly
*       to currently used MAC Unit.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - physical device number
*       macNum  - port MAC number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChHwBcat2RegAddrPortMacUpdate
(
    IN  GT_U8   devNum,
    IN  GT_U32  macNum
)
{
    GT_U32 baseAddr;    /* base address of sub-unit */
    GT_U32 offset;   /* register offset */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsPtr; /* ptr to register address DB */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                          | CPSS_LION_E | CPSS_XCAT2_E
                                          | CPSS_LION2_E );

    regsPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    baseAddr = 0x10000000;

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E)
    {
        offset = (macNum < 72) ? (0x1000*(macNum%36)) : (0x1000*(36));

        if((macNum >=36) && (macNum != 72))
        {
            /* pipe 1 indication */
            baseAddr |= PRV_CPSS_DXCH_BOBCAT3_PIPE1_OFFSET_CNS;
        }
    }
    else
    {
        offset = (macNum < 56) ? (0x1000*macNum) : (0x200000+0x1000*(macNum-56));
    }


    if(PRV_CPSS_PORT_GE_E == PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, macNum))
    {
        regsPtr->macRegs.perPortRegs[macNum].macStatus = baseAddr + 0x10 + offset;
        regsPtr->macRegs.perPortRegs[macNum].serdesCnfg = baseAddr + 0x28 + offset;
        regsPtr->macRegs.perPortRegs[macNum].serdesCnfg2 = baseAddr + 0x30 + offset;
    }
    else
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E ||
           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E)
        {
            /* allow all ports to be XG */
        }
        else if(macNum < 48)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        baseAddr += 0x000C0000;
        regsPtr->macRegs.perPortRegs[macNum].macStatus = baseAddr + 0xC + offset;
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChRegDbSerdesPrint
*
* DESCRIPTION:
*       debug function to print the addresses of all the 'SERDES' registers in
*       the 'regDb'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS   prvCpssDxChRegDbSerdesPrint
(
    IN GT_U8 devNum
)
{
    GT_U32 maxSerdesIndx; /* amount of SerDes */
    GT_U32 numRegisters = sizeof(struct _lpSerdesConfig) / sizeof(GT_U32);
    GT_U32 ii,jj;
    GT_U32       *regsAddrPtr32;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    if(regsAddrPtr->serdesConfig == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    maxSerdesIndx = regsAddrPtr->serdesConfigRegsSize / sizeof(struct _lpSerdesConfig);
    regsAddrPtr32 = (GT_U32*)regsAddrPtr->serdesConfig;

    cpssOsPrintf("Serdeses info : numOfSerdeses[%d] with numRegisters[%d] per serdes \n",
        maxSerdesIndx,numRegisters);
    cpssOsPrintf("\n\n\n");

    for (ii = 0; (ii < maxSerdesIndx); ii++)
    {
        cpssOsPrintf("start Serdes [%d] register Addresses \n" ,
            ii);
        for(jj = 0 ; jj < numRegisters; jj++ , regsAddrPtr32++)
        {
            cpssOsPrintf("id[%d] [0x%8.8x] \n" ,
                jj,
                (*regsAddrPtr32));
        }
        cpssOsPrintf("Ended Serdes [%d] \n" ,
            ii);
        cpssOsPrintf("\n\n\n");
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXcat3PortMacRegAddrSwap
*
* DESCRIPTION:
*       MAC address DB update, in order to support extended MAC feature.
*
* APPLICABLE DEVICES:
*       xCat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portsBmp  - physical port number
*       enable    - extended MAC mode enabled
*                       GT_TRUE - update registers to work in extended mode
*                       GT_FALSE - update registers to work in regular mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat3PortMacRegAddrSwap
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_PHYSICAL_PORT_NUM localPort; /* local port number */
    GT_U32 offset;                  /* register offset */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr; /* pointer to register DB */
    GT_U32  p;                      /* loop iterator */
    GT_U32  xgBaseAddr;             /* XG/XLG base address */
    GT_U32  gigBaseAddr;            /* GIG base address */
    GT_U32  fcaBaseAddr;            /* FCA unit base address */
    GT_U32  macBaseAddr;            /* base address of sub-unit */

    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    if(portNum == 25)
    {
        localPort = (enable == GT_TRUE) ? 28 : 25;
    }
    else if(portNum == 27)
    {
        localPort = (enable == GT_TRUE) ? 29 : 27;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    xgBaseAddr = 0x120C0000;
    gigBaseAddr = 0x12000000;
    fcaBaseAddr = 0x12180600;
    macBaseAddr = 0x11000000;
    offset = localPort * 0x1000;

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    regsAddrPtr->macRegs.perPortRegs[portNum].serialParameters = gigBaseAddr + 0x14 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].serialParameters1 = gigBaseAddr + 0x94 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].autoNegCtrl      = gigBaseAddr + 0xC + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].prbsCheckStatus  = gigBaseAddr + 0x38 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].prbsErrorCounter = gigBaseAddr + 0x3C + offset;

    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl =
                    gigBaseAddr + 0 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1 =
                    gigBaseAddr + 4 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2 =
                    gigBaseAddr + 8 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3 =
                    gigBaseAddr + 0x48 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl4 =
                    gigBaseAddr + 0x90 + offset;


    /* XG MAC Registers */
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl =
                xgBaseAddr + 0x0 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl1 =
                xgBaseAddr + 0x4 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl2 =
                xgBaseAddr + 0x8 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl3 =
                xgBaseAddr + 0x1C + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl4 =
                xgBaseAddr + 0x84 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XG_E].macCtrl5 =
                xgBaseAddr + 0x88 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XG_E].fcDsaTag2 =
                xgBaseAddr + 0x70 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XG_E].fcDsaTag3 =
                xgBaseAddr + 0x74 + offset;
    /* XLG MAC Registers */
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl =
                xgBaseAddr + 0x0 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl1 =
                xgBaseAddr + 0x4 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl2 =
                xgBaseAddr + 0x8 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl3 =
                xgBaseAddr + 0x1C + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl4 =
                xgBaseAddr + 0x84 + offset;
    regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XLG_E].macCtrl5 =
                xgBaseAddr + 0x88 + offset;

    regsAddrPtr->macRegs.perPortRegs[portNum].xgMibCountersCtrl =
                xgBaseAddr + 0x30 + offset;

    regsAddrPtr->macRegs.perPortRegs[portNum].macCounters = macBaseAddr + 0x400 * localPort;


    /*******************/
    /* FCA unit update */
    /*******************/

    { /*0018060c+p*0x4*/
        for (p = 0; p <= 7; p++)
        {
            regsAddrPtr->macRegs.perPortRegs[portNum].FCA.LLFCDatabase[p] = fcaBaseAddr + offset +
                0xc + p * 0x4;
        } /* end of loop p */
    } /*0018060c+i*0x4*/

    { /*0018062c+p*0x4*/
        for (p = 0; p <= 7; p++)
        {
            regsAddrPtr->macRegs.perPortRegs[portNum].FCA.portSpeedTimer[p] = fcaBaseAddr + offset +
                0x2c + p * 0x4;
        } /* end of loop p */
    } /*0018062c+p*0x4*/
    { /*00180684+0x4*p*/
        for (p = 0; p <= 7; p++)
        {
            regsAddrPtr->macRegs.perPortRegs[portNum].FCA.lastSentTimer[p] = fcaBaseAddr + offset +
                0x84 + 0x4 * p;
        } /* end of loop p */
    } /*00180684+0x4*p*/
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.FCACtrl               = fcaBaseAddr + offset + 0x0;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.DBReg0                = fcaBaseAddr + offset + 0x4;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.DBReg1                = fcaBaseAddr + offset + 0x8;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.LLFCFlowCtrlWindow0   = fcaBaseAddr + offset + 0x4c;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.LLFCFlowCtrlWindow1   = fcaBaseAddr + offset + 0x50;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.MACDA0To15            = fcaBaseAddr + offset + 0x54;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.MACDA16To31           = fcaBaseAddr + offset + 0x58;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.MACDA32To47           = fcaBaseAddr + offset + 0x5c;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.MACSA0To15            = fcaBaseAddr + offset + 0x60;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.MACSA16To31           = fcaBaseAddr + offset + 0x64;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.MACSA32To47           = fcaBaseAddr + offset + 0x68;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.L2CtrlReg0            = fcaBaseAddr + offset + 0x6c;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.L2CtrlReg1            = fcaBaseAddr + offset + 0x70;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.DSATag0To15           = fcaBaseAddr + offset + 0x74;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.DSATag16To31          = fcaBaseAddr + offset + 0x78;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.DSATag32To47          = fcaBaseAddr + offset + 0x7c;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.DSATag48To63          = fcaBaseAddr + offset + 0x80;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.PFCPriorityMask       = fcaBaseAddr + offset + 0x104;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.sentPktsCntrLSb       = fcaBaseAddr + offset + 0x108;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.sentPktsCntrMSb       = fcaBaseAddr + offset + 0x10c;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.periodicCntrLSB       = fcaBaseAddr + offset + 0x110;
    regsAddrPtr->macRegs.perPortRegs[portNum].FCA.periodicCntrMSB       = fcaBaseAddr + offset + 0x114;


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChHwXcat3RegAddrPortMacUpdate
*
* DESCRIPTION:
*       This function updates mac registers addresses for given port accordingly
*       to currently used MAC Unit.
*
* APPLICABLE DEVICES:
*       xCat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwXcat3RegAddrPortMacUpdate
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_STATUS rc;   /* return code */
    GT_U32 baseAddr;    /* base address of sub-unit */
    GT_U32 offset;   /* register offset */
    GT_U32 macRegOffset;   /* register offset */
    GT_U32 localPort;   /* local port number */
    GT_BOOL enable; /* extended port status */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsPtr; /* ptr to register address DB */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                          | CPSS_LION_E | CPSS_XCAT2_E
                                          | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    regsPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    offset = (0x1000*portNum);

    if(PRV_CPSS_PORT_GE_E == PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portNum))
    {
        baseAddr = 0x12000000;
        macRegOffset = 0x10;
    }
    else
    {
        baseAddr = 0x120C0000;
        macRegOffset = 0xC;
    }

    if((portNum == 25) || (portNum == 27))
    {
        rc = cpssDxChPortExtendedModeEnableGet(devNum, portNum, &enable);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(portNum == 25)
        {
            localPort = (enable == GT_TRUE) ? 28 : 25;
        }
        else
        {
            localPort = (enable == GT_TRUE) ? 29 : 27;
        }

        offset = (0x1000*localPort);
    }

    regsPtr->macRegs.perPortRegs[portNum].macStatus = baseAddr + macRegOffset + offset;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChHwIsSerdesExists
*
* DESCRIPTION:
*       the function check if the SERDES exists in the device.
*
* APPLICABLE DEVICES:
*       xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; xCat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - physical device number
*       serdesId - the SERDES id.
*               Bobcat2     : 0..35
*               Bobk-Caelum : 0..11,20,24..35
*               Bobk-Cetus  :       20,24..35
*
* OUTPUTS:
*       isExistsPtr - (pointer to) is SERDES exits
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwIsSerdesExists(
    IN GT_U8    devNum,
    IN GT_U32   serdesId,
    OUT GT_BOOL *isExistsPtr
)
{
    GT_U32 devType;     /* device type          */
    GT_U32 startSerdesIndx;
    GT_U32 maxSerdesIndx; /* amount of SerDes */
    GT_BOOL isExists = GT_FALSE;

    *isExistsPtr = GT_FALSE;

    maxSerdesIndx = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfigRegsSize / sizeof(struct _lpSerdesConfig);

    /* define type of xCat device either Gig or Fast */
    devType = PRV_CPSS_PP_MAC(devNum)->devType;
    switch(devType)
    {
        case PRV_CPSS_DXCH_XCAT_FE_DEVICES_CASES_MAC:
        case PRV_CPSS_DXCH_XCAT2_FE_DEVICES_CASES_MAC:
            startSerdesIndx = 6;
            break;
        default:
            if(PRV_CPSS_DXCH_CETUS_CHECK_MAC(devNum) == GT_TRUE)
            {
                startSerdesIndx = 20;
            }
            else
            {
                startSerdesIndx = 0;
            }
            break;
    }

    if((serdesId >= maxSerdesIndx) ||
       (serdesId < startSerdesIndx))
    {
        *isExistsPtr = GT_FALSE;
        return GT_OK;
    }

    isExists = GT_TRUE;


    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
       &&
       (PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E))
    {
        if (serdesId == 20  || (serdesId > 23 && serdesId < 36))
        {
            /* valid SERDES */
            isExists = GT_TRUE;
        }
        else
        {
            if (serdesId > 11)
            {
                isExists = GT_FALSE;
            }
            else
            {
                /* valid SERDES */
                isExists = GT_TRUE;
            }
        }
    }

    *isExistsPtr = isExists;

    return GT_OK;
}

