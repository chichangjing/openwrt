/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwRegAddrVer1.c
*
* DESCRIPTION:
*       This file includes definitions of all different 'eArch' registers
*       addresses to be used by all functions accessing PP registers.
*       a variable of type PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC should be allocated
*       for each PP type.
*
* FILE REVISION NUMBER:
*       $Revision: 101 $
*
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChModuleConfig.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwRegAddrVer1_ppDb.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>
#include <cpss/generic/port/private/prvCpssPortTypes.h>
#include <cpss/dxCh/dxChxGen/phy/prvCpssDxChSmiUnitDrv.h>

#include <cpssDriver/pp/prvCpssDrvPpDefs.h>

/*calc number of words form number of bits */
#define NUM_WORDS_FROM_BITS_MAC(x)   (((x) + 31)>>5)

/* SDMA id of the CPU */
#define CPU_SDMA_ID 72
/* NOTE : the 0xFE as start of address is to bypass arithmetic issue in
    cpssDrvPpHwPciCntlSim.c /cpssDrvPpHwPciCntl.c that do 'next unit' calculation
    (if done on 0xFF --> (0xFF + 1) get 0 on GT_U8) */
#define UNIT_MASK_9_MSB_CNS   0xFF800000

extern GT_U32 prvDxChBobcat2UnitsBaseAddr[PRV_CPSS_DXCH_UNIT_LAST_E + 1];  /* Bobcat2 unit base address array */
extern GT_U32 prvDxChBobkUnitsBaseAddr   [PRV_CPSS_DXCH_UNIT_LAST_E + 1];  /* Bobk unit base address array */
extern GT_U32 prvDxChBobcat3UnitsBaseAddr[PRV_CPSS_DXCH_UNIT_LAST_E + 1];  /* Bobcat3 unit base address array */
extern GT_U32 prvDxChAldrinUnitsBaseAddr[PRV_CPSS_DXCH_UNIT_LAST_E + 1];   /* Aldrin unit base address array */

#ifdef ASIC_SIMULATION /* needed for GM*/
/* checks existence of unit according to address */
typedef GT_BOOL (* SMEM_GM_HW_IS_UNIT_USED_PROC_FUN ) (
                   IN GT_U32       cpssDevNum,
                   IN GT_U32       portGroupId,
                   IN GT_U32       regAddr
);

/* init unit memory use checking function */
extern void smemGmUnitUsedFuncInit
(
    IN GT_U32       simDeviceId,
    IN GT_U32       cpssDevNum,
    IN SMEM_GM_HW_IS_UNIT_USED_PROC_FUN unitUsedFuncPtr
);

/* convert CPSS devNum, coreId to simulation deviceId */
extern GT_STATUS prvCpssDrvDevNumToSimDevIdConvert
(
    IN  GT_U8       devNum,
    IN  GT_U32      portGroupId,
    OUT GT_U32      *simDeviceIdPtr
);
#endif  /*ASIC_SIMULATION*/


static void txqDqInit
(
    IN  GT_U8 devNum,
    IN  GT_U32  index
);
static void txqDqUpdateUnit0AsGlobalPortsAddr
(
    IN  GT_U8   devNum,
    IN  GT_U32  currentUnitIndex,
    IN  GT_U32  portIn_unit_0,
    IN  GT_U32  portInCurrentUnit,
    IN  GT_U32  numOfPorts
);
static void txqDqLegacyDestroy
(
    IN  GT_U8 devNum
);


#define  NUM_ENTRIES_FOR_8_MSBITS_BASE_ADDR_CNS         BIT_8/*256*/

/* filled in runtime */
static PRV_CPSS_DXCH_UNIT_ENT prvDxChBobcat2UnitIdPer8MSBitsBaseAddrArr[NUM_ENTRIES_FOR_8_MSBITS_BASE_ADDR_CNS] =
{
    0,
};
static PRV_CPSS_DXCH_UNIT_ENT prvDxChBobcat3UnitIdPer8MSBitsBaseAddrArr[NUM_ENTRIES_FOR_8_MSBITS_BASE_ADDR_CNS] =
{
    0,
};
static PRV_CPSS_DXCH_UNIT_ENT prvDxChAldrinUnitIdPer8MSBitsBaseAddrArr[NUM_ENTRIES_FOR_8_MSBITS_BASE_ADDR_CNS] =
{
    0,
};
/*array for large units (Bobcat2; Caelum; Bobcat3)*/
struct{
    PRV_CPSS_DXCH_UNIT_ENT unitId;
    GT_U32                 numOfSubUnits;/* in 8 MSBits units*/
}prvDxChLion3LargeUnitsArr[]=
{
     {PRV_CPSS_DXCH_UNIT_EGF_SHT_E,8}

    /* mast be last */
    ,{END_OF_TABLE_INDICATOR_CNS , 0}
};

/* Lion3 unit base address array */
static GT_U32 prvDxChLion2UnitsBaseAddr[PRV_CPSS_DXCH_UNIT_LAST_E + 1] =
{
/*UNIT_TTI_E,            */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TTI_CNS    ,
/*UNIT_PCL_E,            */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_PCL_CNS    ,
/*UNIT_L2I_E,            */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_L2I_CNS    ,
/*UNIT_FDB_E,            */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_FDB_CNS    ,
/*UNIT_EQ_E,             */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_EQ_CNS     ,
/*UNIT_LPM_E,            */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LPM_CNS    ,
/*UNIT_EGF_EFT_E,        */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_EGF_EFT_CNS,
/*UNIT_EGF_QAG_E,        */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_EGF_QAG_CNS,
/*UNIT_EGF_SHT_E,        */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_EGF_SHT_CNS,
/*UNIT_HA_E,             */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_HA_CNS     ,
/*UNIT_ETS_E,            */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_ETS_CNS    ,
/*UNIT_MLL_E,            */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_MLL_CNS    ,
/*UNIT_IPLR_E,           */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_IPLR_CNS   ,
/*UNIT_EPLR_E,           */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_EPLR_CNS   ,
/*UNIT_IPVX_E,           */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_IPVX_CNS   ,
/*UNIT_IOAM_E,           */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_IOAM_CNS   ,
/*UNIT_EOAM_E,           */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_EOAM_CNS   ,
/*UNIT_TCAM_E,           */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TCAM_CNS   ,
/*UNIT_RXDMA_E,          */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_RXDMA_CNS  ,
/*UNIT_EPCL_E,           */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_EPCL_CNS   ,
/*UNIT_TM_FCU_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TM_FCU_CNS                     ,
/*UNIT_TM_DROP_E,        */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TM_DROP_CNS                    ,
/*UNIT_TM_QMAP_E,        */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TM_QMAP_CNS                    ,
/*UNIT_TM_E,             */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TM_CNS                         ,
/*UNIT_GOP_E,            */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_GOP_CNS                        ,
/*UNIT_MIB_E,            */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_MIB_CNS                        ,
/*UNIT_SERDES_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_SERDES_CNS                     ,
/*UNIT_ERMRK_E,          */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_ERMRK_CNS                      ,
/*UNIT_BM_E,             */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_BM_CNS                         ,
/*UNIT_TM_INGRESS_GLUE_E,*/    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TM_INGRESS_GLUE_CNS            ,
/*UNIT_TM_EGRESS_GLUE_E, */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TM_EGRESS_GLUE_CNS             ,
/*UNIT_ETH_TXFIFO_E,     */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_ETH_TXFIFO_CNS                 ,
/*UNIT_ILKN_TXFIFO_E,    */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_ILKN_TXFIFO_CNS                ,
/*UNIT_ILKN_E,           */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_ILKN_CNS                       ,
/*UNIT_LMS0_0_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS0_0_CNS                     ,
/*UNIT_LMS0_1_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS0_1_CNS                     ,
/*UNIT_LMS0_2_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS0_2_CNS                     ,
/*UNIT_LMS0_3_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS0_3_CNS                     ,
/*UNIT_TX_FIFO_E,        */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TX_FIFO_CNS                    ,
/*UNIT_BMA_E,            */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_BMA_CNS                        ,
/*UNIT_CNC_0_E,          */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_CNC_0_CNS                      ,
/*UNIT_CNC_1_E,          */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_CNC_1_CNS                      ,
/*UNIT_TXQ_QUEUE_E,      */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TXQ_QUEUE_CNS                  ,
/*UNIT_TXQ_LL_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TXQ_LL_CNS                     ,
/*UNIT_TXQ_PFC_E,        */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TXQ_PFC_CNS                    ,
/*UNIT_TXQ_QCN_E,        */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TXQ_QCN_CNS                    ,
/*UNIT_TXQ_DQ_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TXQ_DQ_CNS                     ,
/*UNIT_DFX_SERVER_E,     */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_DFX_SERVER_CNS                 ,
/*UNIT_MPPM_E,           */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_MPPM_CNS                       ,
/*UNIT_LMS1_0_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS1_0_CNS                     ,
/*UNIT_LMS1_1_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS1_1_CNS                     ,
/*UNIT_LMS1_2_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS1_2_CNS                     ,
/*UNIT_LMS1_3_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS1_3_CNS                     ,
/*UNIT_LMS2_0_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS2_0_CNS                     ,
/*UNIT_LMS2_1_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS2_1_CNS                     ,
/*UNIT_LMS2_2_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS2_2_CNS                     ,
/*UNIT_LMS2_3_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_LMS2_3_CNS                     ,
/*UNIT_MPPM_1_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_MPPM_1_CNS                     ,
/*UNIT_CTU_0_E,          */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_CTU_0_CNS                      ,
/*UNIT_CTU_1_E,          */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_CTU_1_CNS                      ,
/*UNIT_TXQ_SHT_E ,       */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TXQ_SHT_CNS                    ,
/*UNIT_TXQ_EGR0_E,       */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TXQ_EGR0_CNS                   ,
/*UNIT_TXQ_EGR1_E,       */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TXQ_EGR1_CNS                   ,
/*UNIT_TXQ_DIST_E,       */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TXQ_DIST_CNS                   ,
/*UNIT_IPLR_1_E,         */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_IPLR_1_CNS                     ,
/*UNIT_TXDMA_E,          */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TXDMA_CNS                      ,
/*UNIT_MG_E,             */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_MG_CNS                         ,
/*UNIT_TCC_PCL_E,        */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TCC_PCL_CNS                    ,
/*UNIT_TCC_IPVX_E,       */    PRV_CPSS_DXCH_LION2_BASE_ADDRESS_TCC_IPVX_CNS                   ,

    END_OF_TABLE_INDICATOR_CNS/*last 'non valid' */
};

/* number of elements in unit */
#define NUM_ELEMENTS_IN_UNIT_MAC(unit,element) (sizeof(unit) / sizeof(element))

/* macro to set parameters for function regUnitBaseAddrSet(...) */
#define REG_UNIT_INFO_MAC(devNum, unitBase, unitName)                   \
        (void*)(&PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->unitName),    \
        NUM_ELEMENTS_IN_UNIT_MAC(PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->unitName , GT_U32) , \
        unitBase

/* macro to set parameters for function regUnitPerPortAddrUpdate(...) */
#define REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, unitName1,unitName2 , regName ,startIndexUnit1,startIndexUnit2,numOfPorts, \
        saveLastNumOfPorts,saveToIndex,localPortMapArr) \
        &PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->unitName1.regName[0],   \
        &PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->unitName2.regName[0],   \
        startIndexUnit1,                                                  \
        startIndexUnit2,                                                  \
        numOfPorts,                                                       \
        saveLastNumOfPorts,                                               \
        saveToIndex,                                                      \
        localPortMapArr

/*******************************************************************************
* regUnitPerPortAddrUpdate
*
* DESCRIPTION:
*   This function replaces registers from unit2 to unit1.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       unitStart1Ptr - pointer to start of the unit1
*       unitStart2Ptr - pointer to start of the unit2
*       startIndexUnit1 - the number of ports to skip in unitStart1Ptr
*       startIndexUnit2 - the number of ports to skip in unitStart2Ptr
*       numOfPorts    - number of ports to copy
*       saveOrMoveLastNumOfPorts - number of ports from the end of the unitStart1Ptr[]
*                           that need to be either
*                            - saved before overridden by unitStart2Ptr[] when localPortMapArr == NULL
*                            OR
*                            - moved when localPortMapArr != NULL
*       saveToOrMoveIndex - the index in unitStart1Ptr[] to either save or move the 'last ports'
*                       valid when saveOrMoveLastNumOfPorts != 0
*
*       meaning : copy unitStart2Ptr from index = startIndexUnit2 to index = (startIndexUnit2 + numOfPorts - 1)
*                 into unitStart2Ptr from index = startIndexUnit1 to index = (startIndexUnit1 + numOfPorts - 1)
* 
*       localPortMapArr  - array with index is relative port in Unit2 and value is local port in Unit2
*                          may be NULL in 1:1 map
*
* OUTPUTS:
*       unitStartPtr - unit register are updated
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
static void regUnitPerPortAddrUpdate
(
    IN GT_U32    *unitStart1Ptr,
    IN GT_U32    *unitStart2Ptr,
    IN GT_U32    startIndexUnit1,
    IN GT_U32    startIndexUnit2,
    IN GT_U32    numOfPorts,
    IN GT_U32    saveOrMoveLastNumOfPorts,
    IN GT_U32    saveOrMoveToIndex,
    IN const GT_U32 *localPortMapArr
)
{
    GT_U32  ii,jj,kk;
    GT_U32  mm;

    ii = startIndexUnit1;
    jj = startIndexUnit2;

    if(localPortMapArr)
    {
        numOfPorts -= saveOrMoveLastNumOfPorts;
        for(kk = 0 ; kk < numOfPorts ; kk++)
        {
            unitStart1Ptr[ii] = unitStart2Ptr[localPortMapArr[jj]];
            ii++;
            jj++;
        }

        if(saveOrMoveLastNumOfPorts > 0)
        {
            ii = saveOrMoveToIndex;
            /* jj - continue from previous loop */
            /* move last indexes to other place - used for aldrin CPU port moving to position 72 */
            for(mm = 0; mm < saveOrMoveLastNumOfPorts; mm++)
            {
                unitStart1Ptr[ii] = unitStart2Ptr[localPortMapArr[jj]];
                ii++;
                jj++;
            }
        }
    }
    else
    {
        if(saveOrMoveLastNumOfPorts > 0)
            /* save last indexes to other place to will not be totally deleted */
            for(mm = 0; mm < saveOrMoveLastNumOfPorts; mm++)
            {
                unitStart1Ptr[saveOrMoveToIndex + mm] = unitStart1Ptr[numOfPorts - saveOrMoveLastNumOfPorts + mm];
            }

        for(kk = 0 ; kk < numOfPorts ; kk++)
        {
            unitStart1Ptr[ii] = unitStart2Ptr[jj];
            ii++;
            jj++;
        }
    }
}


extern void defaultAddressUnusedSet
(
    INOUT void   *startPtr,
    IN GT_U32   numOfRegs
);

#define STC_AND_SIZE_MAC(stcName) \
    (GT_U32*)&(stcName) , (sizeof(stcName) / 4)


#define DESTROY_STC_MAC(stcName) \
    regDestroyArr(STC_AND_SIZE_MAC(stcName))


#define ARRAY_AND_SIZE_MAC(arrRegName) \
    (arrRegName) , (sizeof(arrRegName) / sizeof((arrRegName)[0]))

/* destroy compilation array of registers */
#define DESTROY_COMPILATION_ARRAY_REG_MAC(arrRegName) \
    regDestroyArr(ARRAY_AND_SIZE_MAC(arrRegName))

/* destroy dynamic allocated array of registers */
#define DESTROY_DYNAMIC_ALLOC_ARRAY_REG_MAC(arrRegName)     \
    if(arrRegName)                                          \
    {                                                       \
        /* can free the allocation , and set as NULL */     \
        cpssOsFree(arrRegName);                             \
        arrRegName = NULL;                                  \
    }

/* destroy single register */
#define DESTROY_SINGLE_REG_MAC(regName) \
    regDestroy(&(regName))



/*******************************************************************************
* prvCpssDxChHwUnitBaseAddrGet
*
* DESCRIPTION:
*       This function retrieves base address of unit.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       unitId  - the ID of the address space unit.
* OUTPUTS:
*       errorPtr - (pointer to) indication that function did error.
*
* RETURNS:
*       on success - return the base address
*       on error   - return address that ((address % 4) != 0)
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvCpssDxChHwUnitBaseAddrGet
(
    IN GT_U8                    devNum,
    IN PRV_CPSS_DXCH_UNIT_ENT   unitId,
    OUT GT_BOOL                 *errorPtr
)
{
    GT_U32  baseAddr = 1;/* default to cause fatal error on simulation */

    if(errorPtr)
    {
        *errorPtr = GT_TRUE;
    }

    if(unitId >= PRV_CPSS_DXCH_UNIT_LAST_E)
    {
        /* error */
        return baseAddr;
    }

    if(devNum == LION2_DEVICE_INDICATION_CNS)
    {
        /* indication the we need Lion2 DB regardless to the actual devNum */
        goto forceLion2_lbl;
    }
    else
    if (devNum == BOBK_DEVICE_INDICATION_CNS)
    {
        /* indication the we need Bobk DB regardless to the actual devNum */
        goto forceBobk_lbl;
    }

    /* need to use prvCpssDrvPpConfig[] because the 'DXCH DB' may not exists yet !
       (during phase1 init of the driver itself) */
    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS ||
       NULL == prvCpssDrvPpConfig[devNum])
    {
        /* error */
        return baseAddr;
    }

    switch(prvCpssDrvPpConfig[devNum]->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_LION2_E:
            forceLion2_lbl:
            baseAddr = prvDxChLion2UnitsBaseAddr[unitId];
            break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
            if(prvCpssDrvPpConfig[devNum]->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
            {
                forceBobk_lbl:
                baseAddr = prvDxChBobkUnitsBaseAddr[unitId];
            }
            else
            {
                baseAddr = prvDxChBobcat2UnitsBaseAddr[unitId];
            }
            break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
            baseAddr = prvDxChBobcat3UnitsBaseAddr[unitId];
            break;
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
            baseAddr = prvDxChAldrinUnitsBaseAddr[unitId];
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT3_E:
            if( PRV_CPSS_DXCH_UNIT_GOP_E == unitId )
            {
                baseAddr = 0x12000000;
            }
            else if( PRV_CPSS_DXCH_UNIT_SERDES_E == unitId )
            {
                baseAddr = 0x13000000;
            }
            else if( PRV_CPSS_DXCH_UNIT_MIB_E == unitId )
            {
                baseAddr = 0x11000000;
            }
            break;
        default:
            /* error */
            return baseAddr;
    }

    if(errorPtr && (baseAddr != NON_VALID_BASE_ADDR_CNS))
    {
        *errorPtr = GT_FALSE;
    }

    if(baseAddr == NON_VALID_BASE_ADDR_CNS)
    {
        baseAddr |= 1;/* indication of not valid unit */
    }

    return baseAddr;
}

static PRV_CPSS_DXCH_UNIT_ENT gopPortUnits[MAX_PIPES_CNS] =
{
     PRV_CPSS_DXCH_UNIT_GOP_E
    ,PRV_CPSS_DXCH_UNIT_GOP1_E
};

/*******************************************************************************
* getGopPortBaseAddress
*
* DESCRIPTION:
*       get base address of port in the GOP
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
*       GT_OK - on success
*       GT_BAD_STATE - on bad state of logic
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32  getGopPortBaseAddress(
    IN GT_U8    devNum,
    IN GT_U32   portNum
)
{
    GT_STATUS   rc;
    static GT_U32  prevPipeIndex = 0xFFFFFFFF;/* previous pipe index (in case of multi-pipe) */
    GT_U32         portIndex;/* 'local port' index (in case of multi-pipe) */
    GT_U32         pipeIndex;/* pipe index (in case of multi-pipe) */
    static GT_U32  gopBaseAddr = 0xFFFFFFFF; /* gop memory base address */

    if(! PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiPipe.numOfPipes)
    {
        return prvCpssDxChHwUnitBaseAddrGet(devNum,gopPortUnits[0],NULL);
    }

    rc = prvCpssDxChHwPpGopGlobalMacPortNumToLocalMacPortInPipeConvert(devNum,
        portNum/*global port*/,
        &pipeIndex,
        &portIndex/* local port */);
    if(rc != GT_OK)
    {
        return 0xFFFFFFFF;
    }

    /* optimization , calculate base addr only when unitIndex is changed */
    if(pipeIndex != prevPipeIndex)
    {
        /* get base address of the unit according to the pipeIndex */
        gopBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,gopPortUnits[pipeIndex],NULL);
    }

    /* update the previous pipeId */
    prevPipeIndex = pipeIndex;

    return gopBaseAddr;
}

/*******************************************************************************
* offsetToAddressAdd
*
* DESCRIPTION:
*       This function add offset to all the register addresses .
*
* INPUTS:
*       startPtr    - pointer to start of register addresses
*       numOfRegs   - number of registers to update
* OUTPUTS:
*       startPtr    - pointer to start of register addresses that where initialized
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void offsetToAddressAdd
(
    INOUT void   *startPtr,
    IN GT_U32   numOfRegs,
    IN GT_U32   offset
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
        if(regsAddrPtr32[ii] == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            continue;
        }

        regsAddrPtr32[ii] += offset;
    }

    return;
}

/*******************************************************************************
* addressCopy
*
* DESCRIPTION:
*       This function copy register addresses , from src to dest .
*
* INPUTS:
*       startPtr    - pointer to start of register addresses
*       numOfRegs   - number of registers to update
* OUTPUTS:
*       startPtr    - pointer to start of register addresses that where initialized
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void addressCopy
(
    INOUT void   *trgPtr,
    INOUT void   *srcPtr,
    IN GT_U32   numOfRegs
)
{
    GT_U32        ii;
    GT_U32       *regsAddrPtr32;
    GT_U32        regsAddrPtr32Size;
    GT_U32       *regsAddrPtr32_src = srcPtr;

    /*Fill the all words in the pointer with initial value*/
    regsAddrPtr32       = trgPtr;
    regsAddrPtr32Size   = numOfRegs;

    for( ii = 0; ii < regsAddrPtr32Size; ii++ )
    {
        regsAddrPtr32[ii] = regsAddrPtr32_src[ii];
    }

    return;
}

/*******************************************************************************
* updateGopOldRegDbWithMultiPipe
*
* DESCRIPTION:
*       update the 'old DB' of per port with values on 'multi-pipe'
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
*       none
*
* COMMENTS:
*
*******************************************************************************/
static void  updateGopOldRegDbWithMultiPipe(
    IN GT_U8    devNum
)
{
    GT_U32        regsAddrPtr32Size;
    struct dxch_perPortRegs *portRegsPtr;
    GT_U32 portMacNum;
    GT_U32 portOffset = 36;
    GT_U32 portMax = 36;
    GT_U32  gop0Base = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_GOP_E,NULL);
    GT_U32  gop1Base = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_GOP1_E,NULL);
    GT_U32  pipe1Offset = gop1Base - gop0Base;
    GT_U32       *regsAddrPtr32;

    /*Fill all the words in the struct with initial value*/
    regsAddrPtr32Size = sizeof(*portRegsPtr) / 4;

    for(portMacNum = 0 ; portMacNum < portMax ; portMacNum++)
    {
        /* support macs that not exists */
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portMacNum);

        portRegsPtr = &PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum + portOffset];

        /* copy addresses from pipe 0 */
        addressCopy(portRegsPtr,
            &PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum],
            regsAddrPtr32Size);

        /* add offsets of pipe 1 */
        offsetToAddressAdd(portRegsPtr,regsAddrPtr32Size,pipe1Offset);
    }


    /* MAC TG register addresses */
    regsAddrPtr32Size = sizeof(PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->packGenConfig[0]) / 4;
    for(portMacNum = 0 ; portMacNum < portMax ; portMacNum+=4)
    {
        /* support macs that not exists */
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portMacNum);

        regsAddrPtr32 = (GT_U32*)&PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->packGenConfig[(portOffset + portMacNum)/4];

        /* copy addresses from pipe 0 */
        addressCopy(regsAddrPtr32,
            &PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->packGenConfig[ (portMacNum/4)],
            regsAddrPtr32Size);

        /* add offsets of pipe 1 */
        offsetToAddressAdd(regsAddrPtr32,regsAddrPtr32Size,pipe1Offset);
    }


    /* FCA register addresses */
    regsAddrPtr32Size = sizeof(PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.FCA[0]) / 4;
    for(portMacNum = 0 ; portMacNum < portMax ; portMacNum++)
    {
        /* support macs that not exists */
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portMacNum);

        regsAddrPtr32 = (GT_U32*)&PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.FCA[portOffset + portMacNum];

        /* copy addresses from pipe 0 */
        addressCopy(regsAddrPtr32,
            &PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.FCA[portMacNum],
            regsAddrPtr32Size);

        /* add offsets of pipe 1 */
        offsetToAddressAdd(regsAddrPtr32,regsAddrPtr32Size,pipe1Offset);
    }




}


/*******************************************************************************
* prvCpssDxChHwSubunitBaseAddrGet
*
* DESCRIPTION:
*       This function retrieves base address of subunit.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - The PP's device number to init the struct for.
*       subunitId  - the ID of the address space unit.
*       indexArr   - (pointer to) array of indexes that identify the
*                    subunit instance.
* OUTPUTS:
*       None.
*
* RETURNS:
*       on success - return the base address
*       on error   - return address that ((address % 4) != 0)
*
* COMMENTS:
*       For multi-port-group devices expected local port.
*
*******************************************************************************/
GT_U32 prvCpssDxChHwSubunitBaseAddrGet
(
    IN GT_U8                       devNum,
    IN PRV_CPSS_DXCH_SUBUNIT_ENT   subunitId,
    IN GT_U32                      *indexArr
)
{
    GT_U32 gopBase = 0;
    GT_U32 subUnitOffset;
    PRV_CPSS_DXCH_UNIT_ENT  unitId;
    GT_U32  portNum;

    if ((devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS) ||
       (NULL == prvCpssDrvPpConfig[devNum]))
    {
        return 1;/* error */
    }

    if(prvCpssDrvPpConfig[devNum]->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E)
    {
        switch (subunitId)
        {
            case PRV_CPSS_DXCH_SUBUNIT_GOP_TAI_E:
                /* single TAI unit */
                /* indexArr[0] is pipe index */
                /* indexArr[1] is ignored    */
                if(indexArr[0] == 0)
                {
                    unitId = PRV_CPSS_DXCH_UNIT_TAI_E;
                }
                else
                if(indexArr[0] == 1)
                {
                    unitId = PRV_CPSS_DXCH_UNIT_TAI1_E;
                }
                else
                {
                    return 1; /* error */
                }

                return prvCpssDxChHwUnitBaseAddrGet(devNum,unitId,NULL);

            case PRV_CPSS_DXCH_SUBUNIT_GOP_PTP_E:
            case PRV_CPSS_DXCH_SUBUNIT_GOP_FCA_E:
                if(PRV_CPSS_DXCH_SUBUNIT_GOP_FCA_E == subunitId)
                {
                    subUnitOffset = 0x00180600;
                }
                else
                {
                    subUnitOffset = 0x00180800;
                }

                portNum = indexArr[0];
                gopBase = getGopPortBaseAddress(devNum,portNum);

                return gopBase + subUnitOffset;
            default:
                break;
        }

        return 1;/* error */
    }

    if(prvCpssDrvPpConfig[devNum]->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E)
    {
        switch (subunitId)
        {
            case PRV_CPSS_DXCH_SUBUNIT_GOP_TAI_E:
                /* single TAI unit */
                unitId = PRV_CPSS_DXCH_UNIT_TAI_E;
                return prvCpssDxChHwUnitBaseAddrGet(devNum,unitId,NULL);

            case PRV_CPSS_DXCH_SUBUNIT_GOP_PTP_E:
            case PRV_CPSS_DXCH_SUBUNIT_GOP_FCA_E:
                if(PRV_CPSS_DXCH_SUBUNIT_GOP_FCA_E == subunitId)
                {
                    subUnitOffset = 0x00180600;
                }
                else
                {
                    subUnitOffset = 0x00180800;
                }

                portNum = indexArr[0];
                gopBase = getGopPortBaseAddress(devNum,portNum);

                return gopBase + subUnitOffset;
            default:
                break;
        }

        return 1;/* error */
    }

    switch (prvCpssDrvPpConfig[devNum]->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
            gopBase = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_GOP_E,NULL);
            break;
        default:
            return 1;/* error */
    }

    switch (prvCpssDrvPpConfig[devNum]->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
            switch (subunitId)
            {
                case PRV_CPSS_DXCH_SUBUNIT_GOP_TAI_E:

                    /* pairs of subunits per GOP   */
                    /* indexArr[0] is GOP index 0-8, ILKN index 9*/
                    /* indexArr[1] is TAI index 0-1*/

                    if (indexArr[1] > 1)
                    {
                        return 1; /* error */
                    }

                    if (indexArr[0] < 3)
                    {
                        /* GOPs 0-2 for ports 0-15, 16-31, 32-48 */
                        return
                            (gopBase + 0x00180A00 +
                             (0x1000 * (indexArr[1] + (indexArr[0] * 16))));
                    }
                    else if (indexArr[0] < 5)
                    {
                        /* GOPs 3-4 for ports 48-51, 52-55 */
                        return
                            (gopBase + 0x00180A00 +
                             (0x1000 * (indexArr[1] + 48 + ((indexArr[0] - 3) * 4))));
                    }
                    else if (indexArr[0] < 9)
                    {
                        /* GOPs 5-8 for ports 56-59, 60-63, 64-67, 68-71 */
                        return
                            (gopBase + 0x00180A00 + 0x200000 +
                             (0x1000 * (indexArr[1] + ((indexArr[0] - 5) * 4))));
                    }
                    else if (indexArr[0] < 10)
                    {
                        /* ILKN - only for adress calculation                   */
                        /* correct even for devices that have no ILKN interface */
                        /* It is address of Global TAI                          */
                        gopBase =
                            prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_ILKN_E,NULL);

                        return
                            (gopBase + 0x00002800 + (0x800 * (indexArr[1])));
                    }
                    else
                    {
                        return 1; /* error */
                    }

                case PRV_CPSS_DXCH_SUBUNIT_GOP_PTP_E:
                case PRV_CPSS_DXCH_SUBUNIT_GOP_FCA_E:
                    if(PRV_CPSS_DXCH_SUBUNIT_GOP_FCA_E == subunitId)
                    {
                        subUnitOffset = 0x00180600;
                    }
                    else
                    {
                        subUnitOffset = 0x00180800;
                    }

                    /* subunit per port 0-71      */
                    /* indexArr[0] is port number */
                    if (indexArr[0] < 56)
                    {
                        /* Bobcat2 network ports */
                        return (gopBase + subUnitOffset + (0x1000 * indexArr[0]));
                    }
                    else if (indexArr[0] < 72)
                    {
                        /* Bobcat2 fabric ports */
                        return
                            (gopBase + subUnitOffset + 0x200000 +
                                (0x1000 * (indexArr[0] - 56)));
                    }
                    else if ((PRV_CPSS_DXCH_SUBUNIT_GOP_PTP_E == subunitId) &&
                             (indexArr[0] < 73 ))
                    {
                        /* ILKN */
                        if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_ilkn.supported == GT_FALSE)
                        {
                            return 0xFFFFFFFF;/* need to ignore */
                        }
                        /* WA due to devcie stuck problam while accessing this */
                        /* registers range - meanwhile accessing the PTP       */
                        /* registers range of the "previous" PTP.              */
                        return
                            (gopBase + subUnitOffset + 0x200000 +
                                (0x1000 * ((indexArr[0]-1) - 56)));
                        /*
                        gopBase =
                            prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_ILKN_E,NULL);
                        return
                            (gopBase + 0x2000);
                        */
                    }
                    else
                    {
                        return 1; /* error */
                    }
                case PRV_CPSS_DXCH_SUBUNIT_GOP_PR_E:
                    /* actually ILKN unit memory space : 0x1F000000 !!! */
                    subUnitOffset = 0x0F000000;
                    return (gopBase + subUnitOffset);

                default:
                    return 1; /* error */
            }
            break;
        default:
            return 1;/* error */
    }
}

/*******************************************************************************
* regDestroy
*
* DESCRIPTION:
*       destroy register info --> the register not relevant to the device
*
* INPUTS:
*       regPtr  - pointer to the register info.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void regDestroy(
    IN GT_U32 *regPtr
)
{
    *regPtr = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    return;
}
/*******************************************************************************
* regDestroy
*
* DESCRIPTION:
*       destroy array of registers info --> the registers not relevant to the device
*
* INPUTS:
*       regArrPtr  - pointer to the registers array info.
*       numOfElements - number of elements
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void regDestroyArr(
    IN GT_U32 *regArrPtr,
    IN GT_U32 numOfElements
)
{
    defaultAddressUnusedSet(regArrPtr,numOfElements);
}

/*******************************************************************************
* dbReset
*
* DESCRIPTION:
*       reset the DB
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void dbReset
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32       *regsAddrPtr32;
    GT_U32        regsAddrPtr32Size;

    /*Fill all the words in the struct with initial value*/
    regsAddrPtr32 = (GT_U32*)regsAddrPtr;
    regsAddrPtr32Size = sizeof(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC) / 4;

    defaultAddressUnusedSet(regsAddrPtr32,regsAddrPtr32Size);
}

/*******************************************************************************
* resetAndInitControllerDbReset
*
* DESCRIPTION:
*       reset the Reset and Init Controller DB
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void resetAndInitControllerDbReset
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_RESET_AND_INIT_CTRL_REGS_ADDR_STC *regsAddrPtr;
    GT_U32       *regsAddrPtr32;
    GT_U32        regsAddrPtr32Size;

    regsAddrPtr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum);

    /*Fill all the words in the struct with initial value*/
    regsAddrPtr32 = (GT_U32*)regsAddrPtr;
    regsAddrPtr32Size =
                    sizeof(PRV_CPSS_DXCH_RESET_AND_INIT_CTRL_REGS_ADDR_STC)/4;

    defaultAddressUnusedSet(regsAddrPtr32,regsAddrPtr32Size);
}

/*******************************************************************************
* regUnitBaseAddrSet
*
* DESCRIPTION:
*   This function replaces base address of all registers in this unit.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       unitStartPtr - pointer to start of the unit
*       numOfElements - number of elements in the unit (num of registers)
*       newBaseAddr - new base address for the unit
*
* OUTPUTS:
*       unitStartPtr - unit register are updated
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
static void regUnitBaseAddrSet(
    IN GT_U32    *unitStartPtr,
    IN GT_U32    numOfElements,
    IN GT_U32    newBaseAddr
)
{
    GT_U32  ii;

    for(ii = 0 ; ii < numOfElements ; ii++)
    {
        if(unitStartPtr[ii] == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            /* skip registers that are not used in the 'legacy device' */
            continue;
        }

        if(newBaseAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            /* the whole unit is not valid ! --> set all addresses to 'not used' */
            unitStartPtr[ii] = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }
        else
        {
            /* addresses must be '0' based -- to support 'huge units' */
            unitStartPtr[ii] = unitStartPtr[ii] + newBaseAddr;
        }
    }
}

/*******************************************************************************
* ttiInit
*
* DESCRIPTION:
*       init the DB - TTI
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void ttiInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);

    {/*start of unit TTI */
        {/*start of unit VLANAssignment */
            {/*300+i*4*/
                GT_U32    i;
                for(i = 0 ; i <= 3 ; i++) {
                    regsAddrPtr->TTI.VLANAssignment.ingrTPIDConfig[i] =
                        0x300+i*4;
                }/* end of loop i */
            }/*300+i*4*/
            {/*310+(0x8*t)+(0x4*p)*/
                GT_U32    t,p;
                for(t = 0 ; t <= 1 ; t++) {
                    for(p = 0 ; p <= 1 ; p++) {
                        regsAddrPtr->TTI.VLANAssignment.ingrTagProfileTPIDSelect[t][p] =
                            0x310+(0x8*t)+(0x4*p);
                    }/* end of loop p */
                }/* end of loop t */
            }/*310+(0x8*t)+(0x4*p)*/
            regsAddrPtr->TTI.VLANAssignment.ingrTPIDTagType = 0x00000320;

        }/*end of unit VLANAssignment */


        {/*start of unit userDefinedBytes */
            {/*b0+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->TTI.userDefinedBytes.IPCLTCPPortRangeComparatorConfig[n] =
                        0xb0+n*0x4;
                }/* end of loop n */
            }/*b0+n*0x4*/
            {/*d0+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->TTI.userDefinedBytes.IPCLUDPPortRangeComparatorConfig[n] =
                        0xd0+n*0x4;
                }/* end of loop n */
            }/*d0+n*0x4*/

        }/*end of unit userDefinedBytes */


        {/*start of unit TTIUnitGlobalConfigs */
            regsAddrPtr->TTI.TTIUnitGlobalConfigs.TTIUnitGlobalConfig = 0x00000000;
            regsAddrPtr->TTI.TTIUnitGlobalConfigs.TTIUnitGlobalConfigForCCPorts = 0x00000064;
            regsAddrPtr->TTI.TTIUnitGlobalConfigs.TTIUnitGlobalConfigExt = 0x00000140;

        }/*end of unit TTIUnitGlobalConfigs */


        {/*start of unit TTIEngine */
            regsAddrPtr->TTI.TTIEngine.TTIEngineInterruptCause = 0x00000004;
            regsAddrPtr->TTI.TTIEngine.TTIEngineInterruptMask = 0x00000008;
            regsAddrPtr->TTI.TTIEngine.TTIEngineConfig = 0x0000000c;
            regsAddrPtr->TTI.TTIEngine.TTIPCLIDConfig0 = 0x00000010;
            regsAddrPtr->TTI.TTIEngine.TTIPCLIDConfig1 = 0x00000014;
            regsAddrPtr->TTI.TTIEngine.TTILookupIPExceptionCommands = 0x00000068;
            {/*120+0x4*p*/
                GT_U32    p;
                for(p = 0 ; p <= 7 ; p++) {
                    regsAddrPtr->TTI.TTIEngine.TTIPktTypeKeys[p] =
                        0x120+0x4*p;
                }/* end of loop p */
            }/*120+0x4*p*/
            regsAddrPtr->TTI.TTIEngine.TTILookupIPExceptionCodes0 = 0x00000110;
            regsAddrPtr->TTI.TTIEngine.TTIKeySegmentMode0 = 0x00000114;
            regsAddrPtr->TTI.TTIEngine.TTIKeyType = 0x00000118;
            regsAddrPtr->TTI.TTIEngine.TTILookupMPLSExceptionCommands = 0x0000011c;
            regsAddrPtr->TTI.TTIEngine.TTILookupIPExceptionCodes1 = 0x00000144;

        }/*end of unit TTIEngine */


        {/*start of unit trunkHashSimpleHash */
            regsAddrPtr->TTI.trunkHashSimpleHash.trunkHashConfigReg0 = 0x00000084;
            regsAddrPtr->TTI.trunkHashSimpleHash.trunkHashConfigReg1 = 0x00000088;
            regsAddrPtr->TTI.trunkHashSimpleHash.trunkHashConfigReg2 = 0x0000008c;

        }/*end of unit trunkHashSimpleHash */


        {/*start of unit TRILLConfigs */
            regsAddrPtr->TTI.TRILLConfigs.TRILLGlobalConfigs0 = 0x00000200;
            regsAddrPtr->TTI.TRILLConfigs.TRILLGlobalConfigs1 = 0x00000204;
            regsAddrPtr->TTI.TRILLConfigs.TRILLExceptionCommands0 = 0x00000208;
            regsAddrPtr->TTI.TRILLConfigs.TRILLExceptionCommands1 = 0x0000020c;
            regsAddrPtr->TTI.TRILLConfigs.TRILLGlobalConfigs2 = 0x00000210;

        }/*end of unit TRILLConfigs */


        {/*start of unit TRILLAdjacency */
            {/*6008+e*0xc*/
                GT_U32    e;
                for(e = 0 ; e <= 127 ; e++) {
                    regsAddrPtr->TTI.TRILLAdjacency.adjacencyEntryReg2[e] =
                        0x6008+e*0xc;
                }/* end of loop e */
            }/*6008+e*0xc*/
            {/*6004+0xc*e*/
                GT_U32    e;
                for(e = 0 ; e <= 127 ; e++) {
                    regsAddrPtr->TTI.TRILLAdjacency.adjacencyEntryReg1[e] =
                        0x6004+0xc*e;
                }/* end of loop e */
            }/*6004+0xc*e*/
            {/*6000+e*0xc*/
                GT_U32    e;
                for(e = 0 ; e <= 127 ; e++) {
                    regsAddrPtr->TTI.TRILLAdjacency.adjacencyEntryReg0[e] =
                        0x6000+e*0xc;
                }/* end of loop e */
            }/*6000+e*0xc*/

        }/*end of unit TRILLAdjacency */


        {/*start of unit PTP */
            {/*3040+d*0x10+i*0x4*/
                GT_U32    i,d;
                for(i = 0 ; i <= 3 ; i++) {
                    for(d = 0 ; d <= 3 ; d++) {
                        regsAddrPtr->TTI.PTP._1588V1DomainDomainNumber[i][d] =
                            0x3040+d*0x10+i*0x4;
                    }/* end of loop d */
                }/* end of loop i */
            }/*3040+d*0x10+i*0x4*/
            {/*309c+0x4*d*/
                GT_U32    d;
                for(d = 0 ; d <= 4 ; d++) {
                    regsAddrPtr->TTI.PTP.PTPExceptionCheckingModeDomain[d] =
                        0x309c+0x4*d;
                }/* end of loop d */
            }/*309c+0x4*d*/
            if(!isSip5_20)
            {/*3010+0x4*n*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->TTI.PTP.piggyBackTSConfigs[n] =
                        0x3010+0x4*n;
                }/* end of loop n */
            }/*3010+0x4*n*/
            regsAddrPtr->TTI.PTP.PTPConfigs = 0x00003030;
            regsAddrPtr->TTI.PTP.PTPOverUDPDestinationPorts = 0x00003004;
            regsAddrPtr->TTI.PTP.PTPExceptionsAndCPUCodeConfig = 0x0000300c;
            regsAddrPtr->TTI.PTP.PTPEtherTypes = 0x00003000;
            regsAddrPtr->TTI.PTP.PTPExceptionsCntr = 0x00003034;
            regsAddrPtr->TTI.PTP._1588V2DomainDomainNumber = 0x00003090;
            regsAddrPtr->TTI.PTP.timestampEtherTypes = 0x00003094;
            regsAddrPtr->TTI.PTP.timestampConfigs = 0x00003098;

        }/*end of unit PTP */


        {/*start of unit pseudoWire */
            {/*1504+0x4*n*/
                GT_U32    n;
                for(n = 0 ; n <= 14 ; n++) {
                    regsAddrPtr->TTI.pseudoWire.MPLSOAMChannelTypeProfile[n] =
                        0x1504+0x4*n;
                }/* end of loop n */
            }/*1504+0x4*n*/
            regsAddrPtr->TTI.pseudoWire.PWE3Config = 0x00001500;
            regsAddrPtr->TTI.pseudoWire.PWTagModeExceptionConfig = 0x00001540;

        }/*end of unit pseudoWire */


        {/*start of unit protocolMatch */
            {/*90+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 5 ; n++) {
                    regsAddrPtr->TTI.protocolMatch.protocolsConfig[n] =
                        0x90+n*0x4;
                }/* end of loop n */
            }/*90+n*0x4*/
            regsAddrPtr->TTI.protocolMatch.protocolsEncapsulationConfig0 = 0x000000a8;
            regsAddrPtr->TTI.protocolMatch.protocolsEncapsulationConfig1 = 0x000000ac;

        }/*end of unit protocolMatch */

        if(!isSip5_20)
        {/*start of unit portMap */
            {/*10b0+4*n*/
                GT_U32    n;
                for(n = 0 ; n <= 31 ; n++) {
                    regsAddrPtr->TTI.portMap.portGroupMap[n] =
                        0x10b0+4*n;
                }/* end of loop n */
            }/*10b0+4*n*/
            {/*1000+n*4*/
                GT_U32    n;
                for(n = 0 ; n <= 42 ; n++) {
                    regsAddrPtr->TTI.portMap.portListMap[n] =
                        0x1000+n*4;
                }/* end of loop n */
            }/*1000+n*4*/

        }/*end of unit portMap */

        {/*start of unit phyPortAndEPortAttributes */
            {/*110004+p*4*/
                GT_U32    p;
                for(p = 0 ; p <= 255 ; p++) {
                    regsAddrPtr->TTI.phyPortAndEPortAttributes.myPhysicalPortAttributes[p] =
                        0x110004+p*4;
                }/* end of loop p */
            }/*110004+p*4*/
            regsAddrPtr->TTI.phyPortAndEPortAttributes.remotePhysicalPortAssignmentGlobalConfig = 0x00110000;

        }/*end of unit phyPortAndEPortAttributes */


        {/*start of unit MRUException */
            {/*170+n*4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->TTI.MRUException.MRUSize[n] =
                        0x170+n*4;
                }/* end of loop n */
            }/*170+n*4*/
            regsAddrPtr->TTI.MRUException.MRUCommandAndCode = 0x00000180;

        }/*end of unit MRUException */


        {/*start of unit MAC2ME */
            {/*160c+n*0x20*/
                GT_U32    n;
                for(n = 0 ; n <= 127 ; n++) {
                    regsAddrPtr->TTI.MAC2ME.MAC2MEHighMask[n] =
                        0x160c+n*0x20;
                }/* end of loop n */
            }/*160c+n*0x20*/
            {/*1600+n*0x20*/
                GT_U32    n;
                for(n = 0 ; n <= 127 ; n++) {
                    regsAddrPtr->TTI.MAC2ME.MAC2MELow[n] =
                        0x1600+n*0x20;
                }/* end of loop n */
            }/*1600+n*0x20*/
            {/*1604+n*0x20*/
                GT_U32    n;
                for(n = 0 ; n <= 127 ; n++) {
                    regsAddrPtr->TTI.MAC2ME.MAC2MEHigh[n] =
                        0x1604+n*0x20;
                }/* end of loop n */
            }/*1604+n*0x20*/
            {/*1608+n*0x20*/
                GT_U32    n;
                for(n = 0 ; n <= 127 ; n++) {
                    regsAddrPtr->TTI.MAC2ME.MAC2MELowMask[n] =
                        0x1608+n*0x20;
                }/* end of loop n */
            }/*1608+n*0x20*/
            {/*1610+0x20*n*/
                GT_U32    n;
                for(n = 0 ; n <= 127 ; n++) {
                    regsAddrPtr->TTI.MAC2ME.MAC2MESrcInterface[n] =
                        0x1610+0x20*n;
                }/* end of loop n */
            }/*1610+0x20*n*/
            {/*1614+0x20*n*/
                GT_U32    n;
                for(n = 0 ; n <= 127 ; n++) {
                    regsAddrPtr->TTI.MAC2ME.MAC2MESrcInterfaceSel[n] =
                        0x1614+0x20*n;
                }/* end of loop n */
            }/*1614+0x20*n*/

        }/*end of unit MAC2ME */


        {/*start of unit IPv6SolicitedNode */
            regsAddrPtr->TTI.IPv6SolicitedNode.IPv6SolicitedNodeMcAddrConfigReg0 = 0x00000040;
            regsAddrPtr->TTI.IPv6SolicitedNode.IPv6SolicitedNodeMcAddrConfigReg1 = 0x00000044;
            regsAddrPtr->TTI.IPv6SolicitedNode.IPv6SolicitedNodeMcAddrConfigReg2 = 0x00000048;
            regsAddrPtr->TTI.IPv6SolicitedNode.IPv6SolicitedNodeMcAddrConfigReg3 = 0x0000004c;
            regsAddrPtr->TTI.IPv6SolicitedNode.IPv6SolicitedNodeMcAddrMaskReg0 = 0x00000050;
            regsAddrPtr->TTI.IPv6SolicitedNode.IPv6SolicitedNodeMcAddrMaskReg1 = 0x00000054;
            regsAddrPtr->TTI.IPv6SolicitedNode.IPv6SolicitedNodeMcAddrMaskReg2 = 0x00000058;
            regsAddrPtr->TTI.IPv6SolicitedNode.IPv6SolicitedNodeMcAddrMaskReg3 = 0x0000005c;

        }/*end of unit IPv6SolicitedNode */


        {/*start of unit FCOE */
            regsAddrPtr->TTI.FCOE.FCoEGlobalConfig = 0x00000070;
            regsAddrPtr->TTI.FCOE.FCoEExceptionConfigs = 0x00000074;
            regsAddrPtr->TTI.FCOE.FCoEExceptionCntrs0 = 0x00000078;
            regsAddrPtr->TTI.FCOE.FCoEExceptionCntrs1 = 0x0000007c;
            regsAddrPtr->TTI.FCOE.FCoEExceptionCntrs2 = 0x00000080;

        }/*end of unit FCOE */


        {/*start of unit duplicationConfigs */
            regsAddrPtr->TTI.duplicationConfigs.duplicationConfigs0 = 0x00000150;
            regsAddrPtr->TTI.duplicationConfigs.duplicationConfigs1 = 0x00000154;
            regsAddrPtr->TTI.duplicationConfigs.MPLSMCDALow = 0x00000158;
            regsAddrPtr->TTI.duplicationConfigs.MPLSMCDAHigh = 0x0000015c;
            regsAddrPtr->TTI.duplicationConfigs.MPLSMCDALowMask = 0x00000160;
            regsAddrPtr->TTI.duplicationConfigs.MPLSMCDAHighMask = 0x00000164;

        }/*end of unit duplicationConfigs */


        {/*1a4+n*4*/
            GT_U32    n;
            for(n = 0 ; n <= 2 ; n++) {
                regsAddrPtr->TTI.TTIKRuleMatchCntr[n] =
                    0x1a4+n*4;
            }/* end of loop n */
        }/*1a4+n*4*/
        {/*100+4*i*/
            GT_U32    i;
            for(i = 0 ; i <= 3 ; i++) {
                regsAddrPtr->TTI.TTIUserDefinedEtherTypes[i] =
                    0x100+4*i;
            }/* end of loop i */
        }/*100+4*i*/
        regsAddrPtr->TTI.TTIIPv6GREEtherType = 0x00000034;
        regsAddrPtr->TTI.TTIIPv4GREEthertype = 0x00000018;
        regsAddrPtr->TTI.specialEtherTypes = 0x00000028;
        regsAddrPtr->TTI.sourceIDAssignment0 = 0x0000014c;
        regsAddrPtr->TTI.receivedFlowCtrlPktsCntr = 0x000001d0;
        regsAddrPtr->TTI.preRouteExceptionMasks = 0x0000002c;
        {/*f0+4*i*/
            GT_U32    i;
            for(i = 0 ; i <= 3 ; i++) {
                regsAddrPtr->TTI.PCLUserDefinedEtherTypes[i] =
                    0xf0+4*i;
            }/* end of loop i */
        }/*f0+4*i*/
        regsAddrPtr->TTI.MPLSEtherTypes = 0x00000030;
        regsAddrPtr->TTI.IPv6ExtensionValue = 0x00000038;
        regsAddrPtr->TTI.IPMinimumOffset = 0x00000020;
        regsAddrPtr->TTI.IPLengthChecks = 0x000001cc;
        regsAddrPtr->TTI.ECCTwoErrorsCntr = 0x000001dc;
        regsAddrPtr->TTI.ECCOneErrorCntr = 0x000001d8;
        regsAddrPtr->TTI.DSAConfig = 0x00000060;
        regsAddrPtr->TTI.droppedFlowCtrlPktsCntr = 0x000001d4;
        regsAddrPtr->TTI.CNAndFCConfig = 0x00000024;
        regsAddrPtr->TTI.CFMEtherType = 0x000001c8;
        regsAddrPtr->TTI.TTIMetalFix = 0x000013f8;

    }/*end of unit TTI */

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        regsAddrPtr->TTI.oamLabelGlobalConfiguration0 = 0x0000001c;
        regsAddrPtr->TTI.oamLabelGlobalConfiguration1 = 0x0000003c;
        regsAddrPtr->TTI.TTIEngine.TTILookupIPExceptionCodes1 = 0x00000114;
        regsAddrPtr->TTI.TTIEngine.TTIKeySegmentMode0 = 0x00000144;
        regsAddrPtr->TTI.TTIEngine.TTIKeySegmentMode1 = 0x00000148;
        regsAddrPtr->TTI.sourceIDAssignment0 = 0x0000014c;
        regsAddrPtr->TTI.sourceIDAssignment1 = 0x00000150;
        regsAddrPtr->TTI.duplicationConfigs.duplicationConfigs0 = 0x00000154;
        regsAddrPtr->TTI.duplicationConfigs.duplicationConfigs1 = 0x00000158;
        regsAddrPtr->TTI.duplicationConfigs.MPLSMCDALow = 0x0000015c;
        regsAddrPtr->TTI.duplicationConfigs.MPLSMCDAHigh = 0x00000160;
        regsAddrPtr->TTI.duplicationConfigs.MPLSMCDALowMask = 0x00000164;
        regsAddrPtr->TTI.duplicationConfigs.MPLSMCDAHighMask = 0x00000168;
        {/*190+n*4*/
            GT_U32    n;
            for(n = 0 ; n <= 11 ; n++) {
                regsAddrPtr->TTI.TTIKRuleMatchCntr[n] =
                    0x190+n*4;
            }/* end of loop n */
        }/*190+n*4*/

        DESTROY_SINGLE_REG_MAC(regsAddrPtr->TTI.CFMEtherType);
        DESTROY_SINGLE_REG_MAC(regsAddrPtr->TTI.ECCOneErrorCntr);
        DESTROY_SINGLE_REG_MAC(regsAddrPtr->TTI.ECCTwoErrorsCntr);
    }
}

/*******************************************************************************
* l2iInit
*
* DESCRIPTION:
*       init the DB - L2I
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void l2iInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);


    {/*start of unit L2I */
        {/*start of unit VLANRangesConfig */
            regsAddrPtr->L2I.VLANRangesConfig.ingrVLANRangeConfig = 0x00000130;

        }/*end of unit VLANRangesConfig */


        {/*start of unit VLANMRUProfilesConfig */
            {/*140+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->L2I.VLANMRUProfilesConfig.VLANMRUProfileConfig[n] =
                        0x140+n*0x4;
                }/* end of loop n */
            }/*140+n*0x4*/

        }/*end of unit VLANMRUProfilesConfig */


        {/*start of unit UDPBcMirrorTrapUDPRelayConfig */
            {/*1100+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 11 ; n++) {
                    regsAddrPtr->L2I.UDPBcMirrorTrapUDPRelayConfig.UDPBcDestinationPortConfigTable[n] =
                        0x1100+n*0x4;
                }/* end of loop n */
            }/*1100+n*0x4*/

        }/*end of unit UDPBcMirrorTrapUDPRelayConfig */


        {/*start of unit securityBreachStatus */
            regsAddrPtr->L2I.securityBreachStatus.securityBreachStatus0 = 0x00001400;
            regsAddrPtr->L2I.securityBreachStatus.securityBreachStatus1 = 0x00001404;
            regsAddrPtr->L2I.securityBreachStatus.securityBreachStatus2 = 0x00001408;
            regsAddrPtr->L2I.securityBreachStatus.securityBreachStatus3 = 0x0000140c;

        }/*end of unit securityBreachStatus */


        {/*start of unit MACBasedQoSTable */
            {/*1000+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 6 ; n++) {
                    regsAddrPtr->L2I.MACBasedQoSTable.MACQoSTableEntry[n] =
                        0x1000+n*0x4;
                }/* end of loop n */
            }/*1000+n*0x4*/

        }/*end of unit MACBasedQoSTable */


        {/*start of unit layer2BridgeMIBCntrs */
            {/*153c+n*0x20*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->L2I.layer2BridgeMIBCntrs.setVLANIngrFilteredPktCount[n] =
                        0x153c+n*0x20;
                }/* end of loop n */
            }/*153c+n*0x20*/
            {/*1530+0x20*n*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->L2I.layer2BridgeMIBCntrs.cntrsSetConfig0[n] =
                        0x1530+0x20*n;
                }/* end of loop n */
            }/*1530+0x20*n*/
            {/*1534+0x20*n*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->L2I.layer2BridgeMIBCntrs.cntrsSetConfig1[n] =
                        0x1534+0x20*n;
                }/* end of loop n */
            }/*1534+0x20*n*/
            {/*1538+n*0x20*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->L2I.layer2BridgeMIBCntrs.setIncomingPktCount[n] =
                        0x1538+n*0x20;
                }/* end of loop n */
            }/*1538+n*0x20*/
            {/*1540+n*0x20*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->L2I.layer2BridgeMIBCntrs.setSecurityFilteredPktCount[n] =
                        0x1540+n*0x20;
                }/* end of loop n */
            }/*1540+n*0x20*/
            {/*1544+n*0x20*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->L2I.layer2BridgeMIBCntrs.setBridgeFilteredPktCount[n] =
                        0x1544+n*0x20;
                }/* end of loop n */
            }/*1544+n*0x20*/
            regsAddrPtr->L2I.layer2BridgeMIBCntrs.MACAddrCount0 = 0x00001500;
            regsAddrPtr->L2I.layer2BridgeMIBCntrs.MACAddrCount1 = 0x00001504;
            regsAddrPtr->L2I.layer2BridgeMIBCntrs.MACAddrCount2 = 0x00001508;
            regsAddrPtr->L2I.layer2BridgeMIBCntrs.hostOutgoingMcPktCount = 0x00001520;
            regsAddrPtr->L2I.layer2BridgeMIBCntrs.hostOutgoingPktsCount = 0x00001514;
            regsAddrPtr->L2I.layer2BridgeMIBCntrs.hostIncomingPktsCount = 0x00001510;
            regsAddrPtr->L2I.layer2BridgeMIBCntrs.hostOutgoingBcPktCount = 0x00001524;
            regsAddrPtr->L2I.layer2BridgeMIBCntrs.matrixSourceDestinationPktCount = 0x00001528;

        }/*end of unit layer2BridgeMIBCntrs */


        {/*start of unit IPv6McBridgingBytesSelection */
            regsAddrPtr->L2I.IPv6McBridgingBytesSelection.IPv6MCBridgingBytesSelectionConfig = 0x00000120;

        }/*end of unit IPv6McBridgingBytesSelection */


        {/*start of unit ingrPortsRateLimitConfig */
            {/*12a0+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.ingrPortsRateLimitConfig.portSpeedOverrideEnable[n] =
                        0x12a0+n*0x4;
                }/* end of loop n */
            }/*12a0+n*0x4*/
            {/*1240+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.ingrPortsRateLimitConfig.MIISpeedGranularity[n] =
                        0x1240+n*0x4;
                }/* end of loop n */
            }/*1240+n*0x4*/
            {/*1260+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.ingrPortsRateLimitConfig.GMIISpeedGranularity[n] =
                        0x1260+n*0x4;
                }/* end of loop n */
            }/*1260+n*0x4*/
            {/*1280+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.ingrPortsRateLimitConfig.XGSpeedGranularity[n] =
                        0x1280+n*0x4;
                }/* end of loop n */
            }/*1280+n*0x4*/
            regsAddrPtr->L2I.ingrPortsRateLimitConfig.ingrRateLimitConfig0 = 0x00001200;
            regsAddrPtr->L2I.ingrPortsRateLimitConfig.ingrRateLimitConfig3 = 0x0000120c;
            regsAddrPtr->L2I.ingrPortsRateLimitConfig.ingrRateLimitConfig2 = 0x00001208;
            regsAddrPtr->L2I.ingrPortsRateLimitConfig.ingrRateLimitConfig1 = 0x00001204;
            regsAddrPtr->L2I.ingrPortsRateLimitConfig.ingrRateLimitDropCntr310 = 0x00001220;
            regsAddrPtr->L2I.ingrPortsRateLimitConfig.ingrRateLimitDropCntr3932 = 0x00001224;

        }/*end of unit ingrPortsRateLimitConfig */


        {/*start of unit globalEportConifguration */
            regsAddrPtr->L2I.globalEportConifguration.globalEPortValue = 0x00000160;
            regsAddrPtr->L2I.globalEportConifguration.globalEPortMask = 0x00000164;

        }/*end of unit globalEportConifguration */


        {/*start of unit ctrlTrafficToCPUConfig */
            {/*c00+n*0x8 +t*0x4*/
                GT_U32    n,t;
                for(n = 0 ; n <= 7 ; n++) {
                    for(t = 0 ; t <= 1 ; t++) {
                        regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IPv6McLinkLocalCPUCodeIndex[n][t] =
                            0xc00+n*0x8 +t*0x4;
                    }/* end of loop t */
                }/* end of loop n */
            }/*c00+n*0x8 +t*0x4*/
            {/*d00+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IPv6McLinkLocalReged[n] =
                        0xd00+n*0x4;
                }/* end of loop n */
            }/*d00+n*0x4*/
            {/*b00+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IPv6McLinkLocalConfig[n] =
                        0xb00+n*0x4;
                }/* end of loop n */
            }/*b00+n*0x4*/
            {/*e04+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IPv6ICMPMsgTypeConfig[n] =
                        0xe04+n*0x4;
                }/* end of loop n */
            }/*e04+n*0x4*/
            regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IPv6ICMPCommand = 0x00000e00;
            {/*a00+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IPv4McLinkLocalReged[n] =
                        0xa00+n*0x4;
                }/* end of loop n */
            }/*a00+n*0x4*/
            {/*900+n*0x8+ t*0x4*/
                GT_U32    n,t;
                for(n = 0 ; n <= 7 ; n++) {
                    for(t = 0 ; t <= 1 ; t++) {
                        regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IPv4McLinkLocalCPUCodeIndex[n][t] =
                            0x900+n*0x8+ t*0x4;
                    }/* end of loop t */
                }/* end of loop n */
            }/*900+n*0x8+ t*0x4*/
            {/*800+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IPv4McLinkLocalConfig[n] =
                        0x800+n*0x4;
                }/* end of loop n */
            }/*800+n*0x4*/
            {/*700+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IEEEReservedMcReged[n] =
                        0x700+n*0x4;
                }/* end of loop n */
            }/*700+n*0x4*/
            {/*600+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IEEEReservedMcCPUIndex[n] =
                        0x600+n*0x4;
                }/* end of loop n */
            }/*600+n*0x4*/
            {/*580+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IEEEReservedMcConfig7[n] =
                        0x580+n*0x4;
                }/* end of loop n */
            }/*580+n*0x4*/
            {/*500+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IEEEReservedMcConfig6[n] =
                        0x500+n*0x4;
                }/* end of loop n */
            }/*500+n*0x4*/
            {/*480+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IEEEReservedMcConfig5[n] =
                        0x480+n*0x4;
                }/* end of loop n */
            }/*480+n*0x4*/
            {/*400+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IEEEReservedMcConfig4[n] =
                        0x400+n*0x4;
                }/* end of loop n */
            }/*400+n*0x4*/
            {/*380+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IEEEReservedMcConfig3[n] =
                        0x380+n*0x4;
                }/* end of loop n */
            }/*380+n*0x4*/
            {/*300+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IEEEReservedMcConfig2[n] =
                        0x300+n*0x4;
                }/* end of loop n */
            }/*300+n*0x4*/
            {/*280+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IEEEReservedMcConfig1[n] =
                        0x280+n*0x4;
                }/* end of loop n */
            }/*280+n*0x4*/
            {/*200+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IEEEReservedMcConfig0[n] =
                        0x200+n*0x4;
                }/* end of loop n */
            }/*200+n*0x4*/

        }/*end of unit ctrlTrafficToCPUConfig */


        {/*start of unit bridgeEngineInterrupts */
            regsAddrPtr->L2I.bridgeEngineInterrupts.bridgeInterruptCause = 0x00002100;
            regsAddrPtr->L2I.bridgeEngineInterrupts.bridgeInterruptMask = 0x00002104;

        }/*end of unit bridgeEngineInterrupts */


        {/*start of unit bridgeEngineConfig */
            regsAddrPtr->L2I.bridgeEngineConfig.bridgeGlobalConfig0 = 0x00000000;
            regsAddrPtr->L2I.bridgeEngineConfig.bridgeGlobalConfig1 = 0x00000004;
            regsAddrPtr->L2I.bridgeEngineConfig.bridgeCommandConfig0 = 0x00000020;
            regsAddrPtr->L2I.bridgeEngineConfig.bridgeCommandConfig1 = 0x00000024;
            regsAddrPtr->L2I.bridgeEngineConfig.bridgeCommandConfig2 = 0x00000028;
            regsAddrPtr->L2I.bridgeEngineConfig.bridgeCommandConfig3 = 0x0000002c;
            regsAddrPtr->L2I.bridgeEngineConfig.bridgeSecurityBreachDropCntrCfg0 = 0x00000040;
            regsAddrPtr->L2I.bridgeEngineConfig.bridgeSecurityBreachDropCntrCfg1 = 0x00000044;
            regsAddrPtr->L2I.bridgeEngineConfig.bridgecpuPortConfig = 0x00000048;

        }/*end of unit bridgeEngineConfig */


        {/*start of unit bridgeDropCntrAndSecurityBreachDropCntrs */
            regsAddrPtr->L2I.bridgeDropCntrAndSecurityBreachDropCntrs.globalSecurityBreachFilterCntr = 0x00001300;
            regsAddrPtr->L2I.bridgeDropCntrAndSecurityBreachDropCntrs.portVLANSecurityBreachDropCntr = 0x00001304;
            regsAddrPtr->L2I.bridgeDropCntrAndSecurityBreachDropCntrs.bridgeFilterCntr = 0x00001308;

        }/*end of unit bridgeDropCntrAndSecurityBreachDropCntrs */


        {/*start of unit bridgeAccessMatrix */
            {/*180+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.bridgeAccessMatrix.bridgeAccessMatrixLine[n] =
                        0x180+n*0x4;
                }/* end of loop n */
            }/*180+n*0x4*/
            regsAddrPtr->L2I.bridgeAccessMatrix.bridgeAccessMatrixDefault = 0x00000170;

        }/*end of unit bridgeAccessMatrix */

        {/*start of unit eVlanMirroringToAnalyser */
            {/*1600+n*0x4*/
                GT_U32    n;
                GT_U32    nMax = 7;
                for(n = 0 ; n <= nMax ; n++) {
                    regsAddrPtr->L2I.eVlanMirroringToAnalyser.eVlanMirroringToAnalyserEnable[n] =
                        0x1600+n*0x4;
                }/* end of loop n */
            }/*1600+n*0x4*/

        }/*end of unit eVlanMirroringToAnalyser */

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            {/*12a0+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.ingrPortsRateLimitConfig.CGSpeedGranularity[n] =
                        0x12a0+n*0x4;
                }/* end of loop n */
            }/*12a0+n*0x4*/
            {/*12c0+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->L2I.ingrPortsRateLimitConfig.portSpeedOverrideEnable[n] =
                        0x12c0+n*0x4;
                }/* end of loop n */
            }/*12c0+n*0x4*/

            regsAddrPtr->L2I.l2iBadAddressLatchReg = 0x00002120;

            {/*800+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 31 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IPv4McLinkLocalConfig[n] =
                        0x800+n*0x4;
                }/* end of loop n */
            }/*800+n*0x4*/
            {/*b00+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 31 ; n++) {
                    regsAddrPtr->L2I.ctrlTrafficToCPUConfig.IPv6McLinkLocalConfig[n] =
                        0xb00+n*0x4;
                }/* end of loop n */
            }/*b00+n*0x4*/
        }
    }/*end of unit L2I */
}


/*******************************************************************************
* fdbInit
*
* DESCRIPTION:
*       init the DB - FDB
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void fdbInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*start of unit FDB[16] */
        {/*start of unit FDBCore */
            {/*start of unit sourceAddrNotLearntCntr */
                regsAddrPtr->FDB.FDBCore.sourceAddrNotLearntCntr.learnedEntryDiscardsCount = 0x00000060;

            }/*end of unit sourceAddrNotLearntCntr */


            {/*start of unit FDB */
                regsAddrPtr->FDB.FDBCore.FDB.MTCPipeDispatcher = 0x00000400;

            }/*end of unit FDB */


            {/*start of unit FDBInterrupt */
                regsAddrPtr->FDB.FDBCore.FDBInterrupt.FDBInterruptCauseReg = 0x000001b0;
                regsAddrPtr->FDB.FDBCore.FDBInterrupt.FDBInterruptMaskReg = 0x000001b4;

            }/*end of unit FDBInterrupt */


            {/*start of unit FDBInternal */
                regsAddrPtr->FDB.FDBCore.FDBInternal.metalFix = 0x00000170;
                regsAddrPtr->FDB.FDBCore.FDBInternal.FDBIdleState = 0x00000174;

            }/*end of unit FDBInternal */


            {/*start of unit FDBIndirectAccess */
                {/*134+n*4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 4 ; n++) {
                        regsAddrPtr->FDB.FDBCore.FDBIndirectAccess.FDBIndirectAccessData[n] =
                            0x134+n*4;
                    }/* end of loop n */
                }/*134+n*4*/
                regsAddrPtr->FDB.FDBCore.FDBIndirectAccess.FDBIndirectAccessCtrl = 0x00000130;
                regsAddrPtr->FDB.FDBCore.FDBIndirectAccess.FDBIndirectAccessAccess = 0x00000150;

            }/*end of unit FDBIndirectAccess */


            {/*start of unit FDBHashResults */
                {/*300+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 7 ; n++) {
                        regsAddrPtr->FDB.FDBCore.FDBHashResults.FDBMultiHashCRCResultReg[n] =
                            0x300+0x4*n;
                    }/* end of loop n */
                }/*300+0x4*n*/
                regsAddrPtr->FDB.FDBCore.FDBHashResults.FDBNonMulti_hash_crc_result = 0x00000320;
                regsAddrPtr->FDB.FDBCore.FDBHashResults.FDBXorHash = 0x00000324;

            }/*end of unit FDBHashResults */


            {/*start of unit FDBGlobalConfig */
                regsAddrPtr->FDB.FDBCore.FDBGlobalConfig.FDBGlobalConfig = 0x00000000;
                regsAddrPtr->FDB.FDBCore.FDBGlobalConfig.FDBGlobalConfig1 = 0x00000004;
                regsAddrPtr->FDB.FDBCore.FDBGlobalConfig.FDBCRCHashInitConfig = 0x0000000c;
                regsAddrPtr->FDB.FDBCore.FDBGlobalConfig.bridgeAccessLevelConfig = 0x00000010;
                regsAddrPtr->FDB.FDBCore.FDBGlobalConfig.FDBAgingWindowSize = 0x00000014;

            }/*end of unit FDBGlobalConfig */


            {/*start of unit FDBCntrsUpdate */
                regsAddrPtr->FDB.FDBCore.FDBCntrsUpdate.FDBCntrsUpdateCtrl = 0x00000340;

            }/*end of unit FDBCntrsUpdate */


            {/*start of unit FDBBankCntrs */
                {/*200+n * 0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 15 ; n++) {
                        regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankCntr[n] =
                            0x200+n * 0x4;
                    }/* end of loop n */
                }/*200+n * 0x4*/
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank0 = 0x00000240;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank1 = 0x00000244;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank2 = 0x00000248;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank3 = 0x0000024c;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank4 = 0x00000250;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank5 = 0x00000254;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank6 = 0x00000258;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank7 = 0x0000025c;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank8 = 0x00000260;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank9 = 0x00000264;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank13 = 0x00000274;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank12 = 0x00000270;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank11 = 0x0000026c;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank10 = 0x00000268;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank14 = 0x00000278;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankRank15 = 0x0000027c;
                regsAddrPtr->FDB.FDBCore.FDBBankCntrs.FDBBankCntrsCtrl = 0x00000280;

            }/*end of unit FDBBankCntrs */


            {/*start of unit FDBAddrUpdateMsgs */
                {/*start of unit AUMsgToCPU */
                    regsAddrPtr->FDB.FDBCore.FDBAddrUpdateMsgs.AUMsgToCPU.AUMsgToCPU = 0x00000090;
                    regsAddrPtr->FDB.FDBCore.FDBAddrUpdateMsgs.AUMsgToCPU.AUFIFOToCPUConfig = 0x00000094;

                }/*end of unit AUMsgToCPU */


                {/*start of unit AUMsgFromCPU */
                    {/*d0+n*4*/
                        GT_U32    n;
                        for(n = 0 ; n <= 5 ; n++) {
                            regsAddrPtr->FDB.FDBCore.FDBAddrUpdateMsgs.AUMsgFromCPU.AUMsgFromCPUWord[n] =
                                0xd0+n*4;
                        }/* end of loop n */
                    }/*d0+n*4*/
                    regsAddrPtr->FDB.FDBCore.FDBAddrUpdateMsgs.AUMsgFromCPU.AUMsgFromCPUCtrl = 0x000000c0;

                }/*end of unit AUMsgFromCPU */


            }/*end of unit FDBAddrUpdateMsgs */


            {/*start of unit FDBAction */
                regsAddrPtr->FDB.FDBCore.FDBAction.FDBActionGeneral = 0x00000020;
                regsAddrPtr->FDB.FDBCore.FDBAction.FDBAction0 = 0x00000024;
                regsAddrPtr->FDB.FDBCore.FDBAction.FDBAction1 = 0x00000028;
                regsAddrPtr->FDB.FDBCore.FDBAction.FDBAction2 = 0x0000002c;
                regsAddrPtr->FDB.FDBCore.FDBAction.FDBAction3 = 0x00000030;
                regsAddrPtr->FDB.FDBCore.FDBAction.FDBAction4 = 0x00000034;
                regsAddrPtr->FDB.FDBCore.FDBAction.FDBActionOldParameters = 0x00000038;
                regsAddrPtr->FDB.FDBCore.FDBAction.FDBActionNewParameters = 0x0000003c;

            }/*end of unit FDBAction */

            { /*start of unit AUMsgRateLimitingCounter*/
                regsAddrPtr->FDB.FDBCore.AUMsgRateLimitingCounter.AUMsgRateLimCnt = 0x00000070;
            } /*end of unit AUMsgRateLimitingCounter*/

            { /*start of unit FDBScedulerConfiguration*/
                regsAddrPtr->FDB.FDBCore.FDBScedulerConfiguration.FDBScedulerSWRRControl = 0x00000380;
                regsAddrPtr->FDB.FDBCore.FDBScedulerConfiguration.FDBScedulerSWRRConfig  = 0x00000384;
            } /*end of unit FDBScedulerConfiguration*/

        }/*end of unit FDBCore */


    }/*end of unit FDB */
}

/*******************************************************************************
* eqInit
*
* DESCRIPTION:
*       init the DB - EQ
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void eqInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);

    {/*start of unit EQ */
        {/*start of unit toCpuPktRateLimiters */
            regsAddrPtr->EQ.toCpuPktRateLimiters.CPUCodeRateLimiterDropCntr = 0x00000068;
            {/*80000+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 254 ; n++) {
                    regsAddrPtr->EQ.toCpuPktRateLimiters.toCpuRateLimiterPktCntr[n] =
                        0x80000+n*0x4;
                }/* end of loop n */
            }/*80000+n*0x4*/

        }/*end of unit toCpuPktRateLimiters */


        {/*start of unit SCTRateLimitersInterrupt */
            {/*10020+(n-1)*0x10*/
                GT_U32    n;
                GT_U32    nMax = isSip5_20 ? 32 : 16;
                for(n = 1 ; n <= nMax ; n++) {
                    regsAddrPtr->EQ.SCTRateLimitersInterrupt.CPUCodeRateLimitersInterruptCause[n-1] =
                        0x10020+(n-1)*0x10;
                }/* end of loop n */
            }/*10020+(n-1)*0x10*/
            {/*10024+(n-1)*0x10*/
                GT_U32    n;
                GT_U32    nMax = isSip5_20 ? 32 : 16;
                for(n = 1 ; n <= nMax ; n++) {
                    regsAddrPtr->EQ.SCTRateLimitersInterrupt.CPUCodeRateLimitersInterruptMask[n-1] =
                        0x10024+(n-1)*0x10;
                }/* end of loop n */
            }/*10024+(n-1)*0x10*/
            regsAddrPtr->EQ.SCTRateLimitersInterrupt.CPUCodeRateLimitersInterruptSummary = 0x00010000;
            regsAddrPtr->EQ.SCTRateLimitersInterrupt.CPUCodeRateLimitersInterruptSummaryMask = 0x00010004;

        }/*end of unit SCTRateLimitersInterrupt */


        {/*start of unit preEgrInterrupt */
            regsAddrPtr->EQ.preEgrInterrupt.preEgrInterruptSummary = 0x00000058;
            regsAddrPtr->EQ.preEgrInterrupt.preEgrInterruptSummaryMask = 0x0000005c;

        }/*end of unit preEgrInterrupt */


        {/*start of unit preEgrEngineGlobalConfig */
            {/*a100+r*0x4*/
                GT_U32    r;
                GT_U32    rMax = isSip5_20 ? 15 : 7;
                for(r = 0 ; r <= rMax ; r++) {
                    regsAddrPtr->EQ.preEgrEngineGlobalConfig.duplicationOfPktsToCPUConfig[r] =
                        0xa100+r*0x4;
                }/* end of loop r */
            }/*a100+r*0x4*/
            {/*a028+r*0x4*/
                GT_U32    r;
                for(r = 0 ; r <= 3 ; r++) {
                    regsAddrPtr->EQ.preEgrEngineGlobalConfig.MTUTableEntry[r] =
                        0xa028+r*0x4;
                }/* end of loop r */
            }/*a028+r*0x4*/
            regsAddrPtr->EQ.preEgrEngineGlobalConfig.preEgrEngineGlobalConfig = 0x0000a000;
            regsAddrPtr->EQ.preEgrEngineGlobalConfig.MTUGlobalConfig = 0x0000a020;
            regsAddrPtr->EQ.preEgrEngineGlobalConfig.lossyDropConfig = 0x0000a008;
            regsAddrPtr->EQ.preEgrEngineGlobalConfig.cscdTrunkHashBitSelectionConfig = 0x0000a010;
            regsAddrPtr->EQ.preEgrEngineGlobalConfig.targetEPortMTUExceededCntr = 0x0000a024;

        }/*end of unit preEgrEngineGlobalConfig */


        {/*start of unit mirrToAnalyzerPortConfigs */
            {/*b200+i*4*/
                GT_U32    i;
                for(i = 0 ; i <= 6 ; i++) {
                    regsAddrPtr->EQ.mirrToAnalyzerPortConfigs.mirrorInterfaceParameterReg[i] =
                        0xb200+i*4;
                }/* end of loop i */
            }/*b200+i*4*/
            regsAddrPtr->EQ.mirrToAnalyzerPortConfigs.analyzerPortGlobalConfig = 0x0000b000;
            regsAddrPtr->EQ.mirrToAnalyzerPortConfigs.ingrStatisticMirrToAnalyzerPortConfig = 0x0000b004;
            regsAddrPtr->EQ.mirrToAnalyzerPortConfigs.ingrAndEgrMonitoringToAnalyzerQoSConfig = 0x0000b008;

        }/*end of unit mirrToAnalyzerPortConfigs */


        {/*start of unit L2ECMP */
            regsAddrPtr->EQ.L2ECMP.ePortECMPEPortValue = 0x00005010;
            regsAddrPtr->EQ.L2ECMP.ePortECMPEPortMask = 0x00005014;
            regsAddrPtr->EQ.L2ECMP.ePortECMPLTTIndexBaseEPort = 0x00005018;
            regsAddrPtr->EQ.L2ECMP.ePortECMPLFSRConfig = 0x00005020;
            regsAddrPtr->EQ.L2ECMP.ePortECMPLFSRSeed = 0x00005024;
            regsAddrPtr->EQ.L2ECMP.ePortECMPHashBitSelectionConfig = 0x00005030;
            regsAddrPtr->EQ.L2ECMP.trunkLFSRConfig = 0x0000d000;
            regsAddrPtr->EQ.L2ECMP.trunkLFSRSeed = 0x0000d004;
            regsAddrPtr->EQ.L2ECMP.trunkHashBitSelectionConfig = 0x0000d010;

        }/*end of unit L2ECMP */


        {/*start of unit ingrSTCInterrupt */
            {/*620+(n-1)*16*/
                GT_U32    n;
                GT_U32    nMax = isSip5_20 ? 32 : 16;
                for(n = 1 ; n <= nMax ; n++) {
                    regsAddrPtr->EQ.ingrSTCInterrupt.ingrSTCInterruptCause[n-1] =
                        0x620+(n-1)*16;
                }/* end of loop n */
            }/*620+(n-1)*16*/
            {/*624+(n-1)*16*/
                GT_U32    n;
                GT_U32    nMax = isSip5_20 ? 32 : 16;
                for(n = 1 ; n <= nMax ; n++) {
                    regsAddrPtr->EQ.ingrSTCInterrupt.ingrSTCInterruptMask[n-1] =
                        0x624+(n-1)*16;
                }/* end of loop n */
            }/*624+(n-1)*16*/
            regsAddrPtr->EQ.ingrSTCInterrupt.ingrSTCInterruptSummary = 0x00000600;
            regsAddrPtr->EQ.ingrSTCInterrupt.ingrSTCInterruptSummaryMask = 0x00000604;

            if(isSip5_20)
            {
                regsAddrPtr->EQ.ingrSTCInterrupt.ingrSTCInterruptSummary1 = 0x00000608;
                regsAddrPtr->EQ.ingrSTCInterrupt.ingrSTCInterruptSummary1Mask = 0x0000060c;
            }
        }/*end of unit ingrSTCInterrupt */


        {/*start of unit ingrSTCConfig */
            regsAddrPtr->EQ.ingrSTCConfig.ingrSTCConfig = 0x0000001c;

        }/*end of unit ingrSTCConfig */


        {/*start of unit ingrForwardingRestrictions */
            regsAddrPtr->EQ.ingrForwardingRestrictions.ingrForwardingRestrictionsDroppedPktsCntr = 0x0002000c;
            regsAddrPtr->EQ.ingrForwardingRestrictions.ingrForwardingRestrictionsDropCode = 0x00020010;

        }/*end of unit ingrForwardingRestrictions */


        {/*start of unit ingrDropCntr */
            regsAddrPtr->EQ.ingrDropCntr.ingrDropCntrConfig = 0x0000003c;
            regsAddrPtr->EQ.ingrDropCntr.ingrDropCntr = 0x00000040;

        }/*end of unit ingrDropCntr */


        {/*start of unit ingrDropCodesCntr */
            regsAddrPtr->EQ.ingrDropCodesCntr.dropCodesCntrConfig = 0x0000006c;
            regsAddrPtr->EQ.ingrDropCodesCntr.dropCodesCntr = 0x00000070;

        }/*end of unit ingrDropCodesCntr */


        {/*start of unit ePort */
            regsAddrPtr->EQ.ePort.ePortGlobalConfig = 0x00005000;
            regsAddrPtr->EQ.ePort.protectionLOCWriteMask = 0x00005050;
            regsAddrPtr->EQ.ePort.protectionSwitchingRXExceptionConfig = 0x00005054;

        }/*end of unit ePort */


        {/*start of unit criticalECCCntrs */
            regsAddrPtr->EQ.criticalECCCntrs.criticalECCConfig = 0x00000074;
            regsAddrPtr->EQ.criticalECCCntrs.BMClearCriticalECCErrorCntr = 0x00000078;
            regsAddrPtr->EQ.criticalECCCntrs.BMClearCriticalECCErrorParams = 0x0000007c;
            regsAddrPtr->EQ.criticalECCCntrs.BMIncCriticalECCErrorCntr = 0x00000088;
            regsAddrPtr->EQ.criticalECCCntrs.BMIncCriticalECCErrorParams = 0x0000008c;
            regsAddrPtr->EQ.criticalECCCntrs.BMCriticalECCInterruptCause = 0x000000a0;
            regsAddrPtr->EQ.criticalECCCntrs.BMCriticalECCInterruptMask = 0x000000a4;

        }/*end of unit criticalECCCntrs */


        {/*start of unit CPUTargetDeviceConfig */
            regsAddrPtr->EQ.CPUTargetDeviceConfig.CPUTargetDeviceConfigReg0 = 0x00000110;
            regsAddrPtr->EQ.CPUTargetDeviceConfig.CPUTargetDeviceConfigReg1 = 0x00000114;
            regsAddrPtr->EQ.CPUTargetDeviceConfig.CPUTargetDeviceConfigReg2 = 0x00000118;
            regsAddrPtr->EQ.CPUTargetDeviceConfig.CPUTargetDeviceConfigReg3 = 0x0000011c;

        }/*end of unit CPUTargetDeviceConfig */


        {/*start of unit applicationSpecificCPUCodes */
            {/*7c00+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->EQ.applicationSpecificCPUCodes.IPProtocolCPUCodeEntry[n] =
                        0x7c00+n*0x4;
                }/* end of loop n */
            }/*7c00+n*0x4*/
            {/*7000+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->EQ.applicationSpecificCPUCodes.TCPUDPDestPortRangeCPUCodeEntryWord0[n] =
                        0x7000+n*0x4;
                }/* end of loop n */
            }/*7000+n*0x4*/
            {/*7800+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->EQ.applicationSpecificCPUCodes.TCPUDPDestPortRangeCPUCodeEntryWord1[n] =
                        0x7800+n*0x4;
                }/* end of loop n */
            }/*7800+n*0x4*/
            regsAddrPtr->EQ.applicationSpecificCPUCodes.IPProtocolCPUCodeValidConfig = 0x00007c10;

        }/*end of unit applicationSpecificCPUCodes */


    }/*end of unit EQ */
}

/*******************************************************************************
* lpmInit
*
* DESCRIPTION:
*       init the DB - LPM
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void lpmInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*start of unit LPM */
        regsAddrPtr->LPM.globalConfig       = 0x00D00000;
        regsAddrPtr->LPM.directAccessMode   = 0x00D00010;
        regsAddrPtr->LPM.exceptionStatus    = 0x00D00020;
        regsAddrPtr->LPM.LPMInterruptsCause = 0x00D00100;
        regsAddrPtr->LPM.LPMInterruptsMask  = 0x00D00110;
        regsAddrPtr->LPM.LPMGeneralIntCause = 0x00D00120;
        regsAddrPtr->LPM.LPMGeneralIntMask  = 0x00D00130;
    }/*end of unit LPM */
}

/*******************************************************************************
* tcamInit
*
* DESCRIPTION:
*       init the DB - TCAM
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void tcamInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  maxFloors = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.tcam.maxFloors;

    {/*start of unit TCAM */
        {/*start of unit tcamInterrupts */
            regsAddrPtr->TCAM.tcamInterrupts.TCAMInterruptsSummaryMask = 0x00501000;
            regsAddrPtr->TCAM.tcamInterrupts.TCAMInterruptsSummaryCause = 0x00501004;
            regsAddrPtr->TCAM.tcamInterrupts.TCAMInterruptMask = 0x00501008;
            regsAddrPtr->TCAM.tcamInterrupts.TCAMInterruptCause = 0x0050100c;
            regsAddrPtr->TCAM.tcamInterrupts.tcamParityErrorAddr = 0x00501010;

        }/*end of unit tcamInterrupts */


        {/*start of unit tcamArrayConf */
            regsAddrPtr->TCAM.tcamArrayConf.tcamActiveFloors = 0x00504010;

        }/*end of unit tcamArrayConf */


        {/*start of unit parityDaemonRegs */
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonCtrl = 0x00503000;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonWriteAdressStart = 0x00503004;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonWriteAdressEnd = 0x00503008;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonWriteAdressJump = 0x0050300c;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonReadAdressStart = 0x00503010;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonReadAdressEnd = 0x00503014;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonReadAdressJump = 0x00503018;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonDefaultWriteDataX0 = 0x0050301c;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonDefaultWriteDataX1 = 0x00503020;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonDefaultWriteDataY0 = 0x00503028;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonDefaultWriteDataX2 = 0x00503024;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonDefaultWriteDataY1 = 0x0050302c;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonDefaultWriteDataY2 = 0x00503030;
            regsAddrPtr->TCAM.parityDaemonRegs.parityDaemonStatus = 0x0050303c;

        }/*end of unit parityDaemonRegs */


        {/*start of unit mgAccess */
            {/*5021a8+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 4 ; n++) {
                    regsAddrPtr->TCAM.mgAccess.mgKeySizeAndTypeGroup[n] =
                        0x5021a8+n*0x4;
                }/* end of loop n */
            }/*5021a8+n*0x4*/
            {/*5021bc+i*0x4+n*0x14*/
                GT_U32    n,i;
                for(n = 0 ; n <= 4 ; n++) {
                    for(i = 0 ; i <= 3 ; i++) {
                        regsAddrPtr->TCAM.mgAccess.mgHitGroupHitNum[n][i] =
                            0x5021bc+i*0x4+n*0x14;
                    }/* end of loop i */
                }/* end of loop n */
            }/*5021bc+i*0x4+n*0x14*/
            {/*502004+i*0x4+n*0x54*/
                GT_U32    n,i;
                for(n = 0 ; n <= 4 ; n++) {
                    for(i = 0 ; i <= 20 ; i++) {
                        regsAddrPtr->TCAM.mgAccess.mgKeyGroupChunk[n][i] =
                            0x502004+i*0x4+n*0x54;
                    }/* end of loop i */
                }/* end of loop n */
            }/*502004+i*0x4+n*0x54*/
            regsAddrPtr->TCAM.mgAccess.mgCompRequest = 0x00502000;
            regsAddrPtr->TCAM.mgAccess.mgKeyTypeUpdateReq = 0x00502300;
            regsAddrPtr->TCAM.mgAccess.keyTypeData = 0x00502304;

        }/*end of unit mgAccess */


        {/*500000+n*0x4*/
            GT_U32    n;
            for(n = 0 ; n <= 4 ; n++) {
                regsAddrPtr->TCAM.groupClientEnable[n] =
                    0x500000+n*0x4;
            }/* end of loop n */
        }/*500000+n*0x4*/
        {/*500100+n*0x8*/
            GT_U32    n;
            for(n = 0 ; n < maxFloors ; n++) {
                regsAddrPtr->TCAM.tcamHitNumAndGroupSelFloor[n] =
                    0x500100+n*0x8;
            }/* end of loop n */
        }/*500100+n*0x8*/

        regsAddrPtr->TCAM.tcamGlobalConfig = 0x507008;

    }/*end of unit TCAM */
}

/*******************************************************************************
* l2LegacyDestroy
*
* DESCRIPTION:
*       destroy L2i registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void l2LegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* destroy legacy DB register's address data */
    DESTROY_STC_MAC(addrPtr->bridgeRegs);

    DESTROY_SINGLE_REG_MAC(addrPtr->descriptorsConfig.bridgeToRouterSet0);
    DESTROY_SINGLE_REG_MAC(addrPtr->descriptorsConfig.bridgeToRouterSet1);

    /*
        next are destroyed by the EQ : DESTROY_STC_MAC(addrPtr->bufferMng.eqBlkCfgRegs);

        even though those are actually 'L2I' registers.

        DESTROY_SINGLE_REG_MAC(addrPtr->bufferMng.eqBlkCfgRegs.ipv4McstLinkLocalCpuCodeIndexBase);
        DESTROY_SINGLE_REG_MAC(addrPtr->bufferMng.eqBlkCfgRegs.ipv6McstLinkLocalCpuCodeIndexBase);
        DESTROY_SINGLE_REG_MAC(addrPtr->bufferMng.eqBlkCfgRegs.ieeeResMcstCpuCodeIndexBase);
    */


}

/*******************************************************************************
* fdbLegacyDestroy
*
* DESCRIPTION:
*       destroy FDB registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void fdbLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    DESTROY_SINGLE_REG_MAC(addrPtr->globalRegs.addrUpdateControlReg);
    DESTROY_COMPILATION_ARRAY_REG_MAC(addrPtr->globalRegs.addrUpdate);

    DESTROY_SINGLE_REG_MAC(addrPtr->interrupts.ethBrgCause);
    DESTROY_SINGLE_REG_MAC(addrPtr->interrupts.ethBrgMask);

}

/*******************************************************************************
* eqLegacyDestroy
*
* DESCRIPTION:
*       destroy EQ registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void eqLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    DESTROY_STC_MAC(addrPtr->eq);

    DESTROY_SINGLE_REG_MAC(addrPtr->sFlowRegs.sFlowControl);

    DESTROY_SINGLE_REG_MAC(addrPtr->sFlowRegs.ingStcTblAccessCtrlReg);
    DESTROY_SINGLE_REG_MAC(addrPtr->sFlowRegs.ingStcTblWord0ValueReg);
    DESTROY_SINGLE_REG_MAC(addrPtr->sFlowRegs.ingStcTblWord1ValueReg);
    DESTROY_SINGLE_REG_MAC(addrPtr->sFlowRegs.ingStcTblWord2ValueReg);

    DESTROY_SINGLE_REG_MAC(addrPtr->descriptorsConfig.preEgressToTxq0);
    DESTROY_SINGLE_REG_MAC(addrPtr->descriptorsConfig.preEgressToTxq1);
}

/*******************************************************************************
* ttiLegacyDestroy
*
* DESCRIPTION:
*       destroy TTI registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void ttiLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;
    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    DESTROY_STC_MAC(addrPtr->ttiRegs);
}


/*******************************************************************************
* ttiInit
*
* DESCRIPTION:
*       init the DB - EGF (EFT,SHT,QAG)
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void egfInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  numPhyPorts = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.maxPhysicalPorts;
    GT_U32  numWordsForBitPerPort = NUM_WORDS_FROM_BITS_MAC(numPhyPorts);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);


    {/*start of unit EGF_eft */
        {/*start of unit MCFIFO */
            {/*start of unit MCFIFOConfigs */
                if(isSip5_20)
                {/*2200+4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 31 ; n++) {
                        regsAddrPtr->EGF_eft.MCFIFO.MCFIFOConfigs.portToMcFIFOAttributionReg[n] =
                            0x2200+4*n;
                    }/* end of loop n */
                }/*2200+4*n*/
                else
                {/*21f0+4*n*/
                    GT_U32    n;
                    for(n = 0 ; n < (2*numWordsForBitPerPort) ; n++) {
                        regsAddrPtr->EGF_eft.MCFIFO.MCFIFOConfigs.portToMcFIFOAttributionReg[n] =
                            0x21f0+4*n;
                    }/* end of loop n */
                }/*21f0+4*n*/
                if(isSip5_20)
                {/*2280+4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 15 ; n++) {
                        regsAddrPtr->EGF_eft.MCFIFO.MCFIFOConfigs.portToHemisphereMapReg[n] =
                            0x2280+4*n;
                    }/* end of loop n */
                }/*2280+4*n*/
                else
                {/*2240+4*n*/
                    GT_U32    n;
                    for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                        regsAddrPtr->EGF_eft.MCFIFO.MCFIFOConfigs.portToHemisphereMapReg[n] =
                            0x2240+4*n;
                    }/* end of loop n */
                }/*2240+4*n*/
                regsAddrPtr->EGF_eft.MCFIFO.MCFIFOConfigs.MCFIFOGlobalConfigs = 0x00002100;
                regsAddrPtr->EGF_eft.MCFIFO.MCFIFOConfigs.MCFIFOArbiterWeights0 = 0x00002108;
                regsAddrPtr->EGF_eft.MCFIFO.MCFIFOConfigs.lossyDropAndPfcUcThreshold = 0x00002300;

            }/*end of unit MCFIFOConfigs */


        }/*end of unit MCFIFO */


        {/*start of unit global */
            {/*1150+4*n*/
                GT_U32    n;
                GT_U32    nMax;

                nMax = isSip5_20 ? 128 : 64;
                for(n = 0 ; n < nMax ; n++) {
                    regsAddrPtr->EGF_eft.global.cpuPortMapOfReg[n] =
                        0x1150+4*n;
                }/* end of loop n */
            }/*1150+4*n*/
            regsAddrPtr->EGF_eft.global.cpuPortDist = 0x00001000;
            regsAddrPtr->EGF_eft.global.portIsolationLookup0 = 0x00001020;
            regsAddrPtr->EGF_eft.global.portIsolationLookup1 = 0x00001030;
            regsAddrPtr->EGF_eft.global.EFTInterruptsCause = 0x000010a0;
            regsAddrPtr->EGF_eft.global.EFTInterruptsMask = 0x000010b0;
            regsAddrPtr->EGF_eft.global.ECCConf = 0x00001130;
            regsAddrPtr->EGF_eft.global.ECCStatus = 0x00001140;

        }/*end of unit global */


        {/*start of unit egrFilterConfigs */
            {/*100f0+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x10180 : 0x100f0;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_eft.egrFilterConfigs.enFCTriggerByCNFrameOnPort[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*100f0+0x4*n*/
            {/*100d0+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x10140 : 0x100d0;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_eft.egrFilterConfigs.enCNFrameTxOnPort[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*100d0+0x4*n*/
            {/*10050+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x100c0 : 0x10050;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_eft.egrFilterConfigs.unregedBCFilterEn[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*10050+0x4*n*/
            {/*10010+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x10040 : 0x10010;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_eft.egrFilterConfigs.unknownUCFilterEn[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*10010+0x4*n*/
            {/*10030+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x10080 : 0x10030;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_eft.egrFilterConfigs.unregedMCFilterEn[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*10030+0x4*n*/
            {/*100b0+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x10100 : 0x100b0;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_eft.egrFilterConfigs.OAMLoopbackFilterEn[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*100b0+0x4*n*/
            if(!isSip5_20)
            {/*10120+4*n*/
                GT_U32    n;
                GT_U32    offset = 0x10120;
                GT_U32    numRegs = (8*numWordsForBitPerPort);
                for(n = 0 ; n < numRegs ; n++) {
                    regsAddrPtr->EGF_eft.egrFilterConfigs.virtual2PhysicalPortRemap[n] =
                        offset+4*n;
                }/* end of loop n */
            }/*10120+4*n*/
            if(!isSip5_20)
            {/*10220+4*n*/
                GT_U32    n;
                GT_U32    offset = 0x10220;
                for(n = 0 ; n < (2*numWordsForBitPerPort) ; n++) {
                    regsAddrPtr->EGF_eft.egrFilterConfigs.physicalPortLinkStatusMask[n] =
                        offset+4*n;
                }/* end of loop n */
            }/*10220+4*n*/
            else
            {/*101c0+4*n*/
                GT_U32    n;
                GT_U32    offset = 0x101c0;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_eft.egrFilterConfigs.physicalPortLinkStatusMask[n] =
                        offset+4*n;
                }/* end of loop n */
            }/*101c0+4*n*/
            regsAddrPtr->EGF_eft.egrFilterConfigs.egrFiltersGlobal = 0x00010000;
            regsAddrPtr->EGF_eft.egrFilterConfigs.egrFiltersEnable = 0x00010004;

        }/*end of unit egrFilterConfigs */


        {/*start of unit deviceMapConfigs */
            {/*3000+4*p*/
                GT_U32    p;
                for(p = 0 ; p <= 7 ; p++) {
                    regsAddrPtr->EGF_eft.deviceMapConfigs.devMapTableAddrConstructProfile[p] =
                        0x3000+4*p;
                }/* end of loop p */
            }/*3000+4*p*/
            {/*3020+0x4*n*/
                GT_U32    n;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_eft.deviceMapConfigs.localSrcPortMapOwnDevEn[n] =
                        0x3020+0x4*n;
                }/* end of loop n */
            }/*3020+0x4*n*/
            {/*3040+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x3060 : 0x3040;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_eft.deviceMapConfigs.localTrgPortMapOwnDevEn[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*3040+0x4*n*/
            {/*3060+4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x3100 : 0x3060;
                GT_U32    numRegs = isSip5_20 ? numPhyPorts/8 : numPhyPorts;
                for(n = 0 ; n < numRegs ; n++) {
                    regsAddrPtr->EGF_eft.deviceMapConfigs.portAddrConstructMode[n] =
                        offset+4*n;
                }/* end of loop n */
            }/*3060+4*n*/

        }/*end of unit deviceMapConfigs */

        {/*start of unit counters */
            regsAddrPtr->EGF_eft.counters.eftDescEcc1ErrorCntr = 0x00001120;
        }

    }/*end of unit EGF_eft */


    {/*start of unit EGF_qag */
        {/*start of unit uniMultiArb */
            regsAddrPtr->EGF_qag.uniMultiArb.uniMultiCtrl = 0x00f00100;
            regsAddrPtr->EGF_qag.uniMultiArb.uniWeights = 0x00f00104;
            regsAddrPtr->EGF_qag.uniMultiArb.multiWeights = 0x00f00108;
            regsAddrPtr->EGF_qag.uniMultiArb.multiShaperCtrl = 0x00f0010c;

        }/*end of unit uniMultiArb */


        {/*start of unit global */
            regsAddrPtr->EGF_qag.global.QAGInterruptsCause = 0x00f00010;
            regsAddrPtr->EGF_qag.global.QAGInterruptsMask = 0x00f00020;

        }/*end of unit global */


        {/*start of unit distributor */
            if(! isSip5_20)
            {/*e00a04+4*n*/
                GT_U32    n;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_qag.distributor.useVlanTag1ForTagStateReg[n] =
                        0xe00a04+4*n;
                }/* end of loop n */
            }/*e00a04+4*n*/
            if(! isSip5_20)
            {/*e00180+0x4*n*/
                GT_U32    n;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_qag.distributor.toAnalyzerLoopback[n] =
                        0xe00180+0x4*n;
                }/* end of loop n */
            }/*e00180+0x4*n*/
            if(! isSip5_20)
            {/*e001e0+4*n*/
                GT_U32    n;
                for(n = 0 ; n <= 31 ; n++) {
                    regsAddrPtr->EGF_qag.distributor.TCRemap[n] =
                        0xe001e0+4*n;
                }/* end of loop n */
            }/*e001e0+4*n*/
            if(! isSip5_20)
            {/*e00050+0x4*n*/
                GT_U32    n;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_qag.distributor.stackRemapEn[n] =
                        0xe00050+0x4*n;
                }/* end of loop n */
            }/*e00050+0x4*n*/
            if(! isSip5_20)
            {/*e00130+0x4*n*/
                GT_U32    n;
                for(n = 0 ; n < (2*numWordsForBitPerPort) ; n++) {
                    regsAddrPtr->EGF_qag.distributor.perTrgPhyPortLbProfile[n] =
                        0xe00130+0x4*n;
                }/* end of loop n */
            }/*e00130+0x4*n*/
            if(! isSip5_20)
            {/*e000f0+0x4*n*/
                GT_U32    n;
                for(n = 0 ; n < (2*numWordsForBitPerPort) ; n++) {
                    regsAddrPtr->EGF_qag.distributor.perSrcPhyPortLbProfile[n] =
                        0xe000f0+0x4*n;
                }/* end of loop n */
            }/*e000f0+0x4*n*/
            regsAddrPtr->EGF_qag.distributor.loopbackForwardingGlobalConfig = 0x00e00a00;
            regsAddrPtr->EGF_qag.distributor.loopbackEnableConfig[0] = 0x00e00170;
            if(! isSip5_20)
            {/*e001c0+0x4*n*/
                GT_U32    n;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_qag.distributor.evlanMirrEnable[n] =
                        0xe001c0+0x4*n;
                }/* end of loop n */
            }/*e001c0+0x4*n*/
            if(! isSip5_20)
            {/*e005fc+4*n*/
                GT_U32    n;
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->EGF_qag.distributor.DPRemap[n] =
                        0xe005fc+4*n;
                }/* end of loop n */
            }/*e005fc+4*n*/
            regsAddrPtr->EGF_qag.distributor.distributorGeneralConfigs = 0x00e00000;

        }/*end of unit distributor */


    }/*end of unit EGF_qag */



    {/*start of unit EGF_sht */
        {/*start of unit global */
            {/*60200f0+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x60201c0 : 0x60200f0;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_sht.global.eportStpState[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*60200f0+0x4*n*/
            {/*60200b0+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x6020140 : 0x60200b0;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_sht.global.eportEvlanFilteringEnable[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*60200b0+0x4*n*/
            regsAddrPtr->EGF_sht.global.SHTInterruptsCause = 0x06020010;
            {/*6020030+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x6020040 : 0x6020030;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_sht.global.UCSrcIDFilterEn[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*6020030+0x4*n*/
            regsAddrPtr->EGF_sht.global.SHTEgrFiltersEnable = 0x06020008;
            {/*6020270+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x60208c0 : 0x6020270;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_sht.global.MCLocalEn[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*6020270+0x4*n*/
            {/*6020240+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x6020880 : 0x6020240;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_sht.global.ignorePhySrcMcFilterEn[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*6020240+0x4*n*/
            regsAddrPtr->EGF_sht.global.eportVlanEgrFiltering =
                                isSip5_20 ? 0x0602000c : 0x06020310;

            {/*60200d0+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x6020180 : 0x60200d0;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_sht.global.eportStpStateMode[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*60200d0+0x4*n*/
            {/*6020190+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x60202c0 : 0x6020190;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_sht.global.eportOddOnlySrcIdFilterEnable[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*6020190+0x4*n*/
            {/*6020090+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x6020100 : 0x6020090;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_sht.global.eportRoutedForwardRestricted[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*6020090+0x4*n*/
            {/*6020130+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x6020200 : 0x6020130;
                for(n = 0 ; n < (2*numWordsForBitPerPort) ; n++) {
                    regsAddrPtr->EGF_sht.global.eportPortIsolationMode[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*6020130+0x4*n*/
            regsAddrPtr->EGF_sht.global.SHTGlobalConfigs = 0x06020000;
            {/*6020170+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x6020280 : 0x6020170;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_sht.global.eportIPMCRoutedFilterEn[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*6020170+0x4*n*/
            {/*6020050+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x6020080 : 0x6020050;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_sht.global.eportFromCPUForwardRestricted[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*6020050+0x4*n*/
            regsAddrPtr->EGF_sht.global.SHTInterruptsMask = 0x06020020;
            {/*6020070+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x60200c0 : 0x6020070;
                for(n = 0 ; n < numWordsForBitPerPort ; n++) {
                    regsAddrPtr->EGF_sht.global.eportBridgedForwardRestricted[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*6020070+0x4*n*/

            regsAddrPtr->EGF_sht.global.meshIdConfigs = 0x06020004;
            {/*6020320+0x4*n*/
                GT_U32    n;
                GT_U32    offset = isSip5_20 ? 0x6020300 : 0x6020320;
                for(n = 0 ; n < (4*numWordsForBitPerPort) ; n++) {
                    regsAddrPtr->EGF_sht.global.ePortMeshId[n] =
                        offset+0x4*n;
                }/* end of loop n */
            }/*6020320+0x4*n*/
            if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
            {
                {/*60203A0+0x4*n*/
                    GT_U32    n;
                    GT_U32    offset = isSip5_20 ? 0x6020800 : 0x60203a0;
                    for (n = 0; n < numWordsForBitPerPort ; n++) {
                        regsAddrPtr->EGF_sht.global.dropOnEportVid1MismatchEn[n] =
                            offset+0x4*n;
                    }/* end of loop n */
                }/*60203A0+0x4*n*/
                {/*6020400+0x4*n*/
                    GT_U32    n;
                    for (n = 0; n < (16*numWordsForBitPerPort) ; n++) {
                        regsAddrPtr->EGF_sht.global.eportVid1[n] =
                            0x6020400+0x4*n;
                    }/* end of loop n */
                }/*6020400+0x4*n*/
            }
        }/*end of unit global */


    }/*end of unit EGF_sht */

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        regsAddrPtr->EGF_eft.counters.eftDescEcc1ErrorCntr = 0x00004008;

        DESTROY_SINGLE_REG_MAC(regsAddrPtr->EGF_eft.MCFIFO.MCFIFOConfigs.lossyDropAndPfcUcThreshold);
        {/*e00170+0x4*n*/
            GT_U32    n;
            for(n = 0; n <= 3 ; n++) {
                regsAddrPtr->EGF_qag.distributor.loopbackEnableConfig[n] =
                    0xe00170+0x4*n;
            }/* end of loop n */
        }/*e00170+0x4*n*/
    }
}

/*******************************************************************************
* egfLegacyDestroy
*
* DESCRIPTION:
*       destroy EGF registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void egfLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    DESTROY_STC_MAC(addrPtr->txqVer1.egr);/*--> EGF_eft*/ /*-->EGF_qag*/
    DESTROY_STC_MAC(addrPtr->txqVer1.sht);/*-->EGF_sht*/  /*-->EGF_qag*/
    DESTROY_STC_MAC(addrPtr->txqVer1.dist);/* removed */

    return;
}

/*******************************************************************************
* haInit
*
* DESCRIPTION:
*       init the DB - HA (header alteration) unit
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void haInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*start of unit HA */
        regsAddrPtr->HA.invalidPTPHeaderCntr = 0x00000014;
        regsAddrPtr->HA.TRILLEtherType = 0x00000558;
        {/*500+n*4*/
            GT_U32    n;
            for(n = 0 ; n <= 7 ; n++) {
                regsAddrPtr->HA.TPIDConfigReg[n] =
                    0x500+n*4;
            }/* end of loop n */
        }/*500+n*4*/
        regsAddrPtr->HA.routerMACSABaseReg1 = 0x00000144;
        {/*20+(w-1)*4*/
            GT_U32    w;
            for(w = 1 ; w <= 15 ; w++) {
                regsAddrPtr->HA.PWCtrlWord[w-1] =
                    0x20+(w-1)*4;
            }/* end of loop w */
        }/*20+(w-1)*4*/
        regsAddrPtr->HA.PWETreeEnable = 0x00000060;
        regsAddrPtr->HA.invalidPTPOutgoingPiggybackCntr = 0x00000018;
        regsAddrPtr->HA.PTPTimestampTagEthertype = 0x0000000c;
        regsAddrPtr->HA.PTPAndTimestampingExceptionConfig = 0x00000010;
        regsAddrPtr->HA.MPLSEtherType = 0x00000550;
        regsAddrPtr->HA.MPLSChannelTypeProfileReg7 = 0x0000016c;
        {/*150+4*r*/
            GT_U32    r;
            for(r = 0 ; r <= 6 ; r++) {
                regsAddrPtr->HA.MPLSChannelTypeProfileReg[r] =
                    0x150+4*r;
            }/* end of loop r */
        }/*150+4*r*/
        regsAddrPtr->HA.IPLengthOffset = 0x00000560;
        {/*120+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 7 ; n++) {
                regsAddrPtr->HA.egrUDPPortRangeComparatorConfig[n] =
                    0x120+4*n;
            }/* end of loop n */
        }/*120+4*n*/
        regsAddrPtr->HA.invalidPTPIncomingPiggybackCntr = 0x0000001c;
        regsAddrPtr->HA.egrPolicyDIPSolicitationData1 = 0x00000084;
        regsAddrPtr->HA.ingrAnalyzerVLANTagConfig = 0x00000408;
        regsAddrPtr->HA.IEtherType = 0x00000554;
        regsAddrPtr->HA.haGlobalConfig1 = 0x00000004;
        regsAddrPtr->HA.haGlobalConfig = 0x00000000;
        regsAddrPtr->HA.HAInterruptMask = 0x00000304;
        regsAddrPtr->HA.HAInterruptCause = 0x00000300;
        regsAddrPtr->HA.ethernetOverGREProtocolType = 0x0000055c;
        regsAddrPtr->HA.routerMACSABaseReg0 = 0x00000140;
        {/*100+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 7 ; n++) {
                regsAddrPtr->HA.egrTCPPortRangeComparatorConfig[n] =
                    0x100+4*n;
            }/* end of loop n */
        }/*100+4*n*/
        regsAddrPtr->HA.egrPolicyDIPSolicitationMask3 = 0x0000009c;
        regsAddrPtr->HA.egrPolicyDIPSolicitationMask2 = 0x00000098;
        regsAddrPtr->HA.egrPolicyDIPSolicitationMask1 = 0x00000094;
        regsAddrPtr->HA.egrPolicyDIPSolicitationMask0 = 0x00000090;
        regsAddrPtr->HA.egrPolicyDIPSolicitationData3 = 0x0000008c;
        regsAddrPtr->HA.egrPolicyDIPSolicitationData2 = 0x00000088;
        regsAddrPtr->HA.tunnelStartFragmentIDConfig = 0x00000410;
        regsAddrPtr->HA.egrPolicyDIPSolicitationData0 = 0x00000080;
        regsAddrPtr->HA.egrAnalyzerVLANTagConfig = 0x0000040c;
        regsAddrPtr->HA.dataECCStatus = 0x00000064;
        regsAddrPtr->HA.CPIDReg1 = 0x00000434;
        regsAddrPtr->HA.CPIDReg0 = 0x00000430;
        regsAddrPtr->HA.congestionNotificationConfig = 0x00000420;
        regsAddrPtr->HA.CNMHeaderConfig = 0x00000424;
        regsAddrPtr->HA.BPEConfigReg2 = 0x0000006c;
        regsAddrPtr->HA.BPEConfigReg1 = 0x00000068;

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            regsAddrPtr->HA.haBadAddrLatchReg = 0x00000308;
            regsAddrPtr->HA.NATConfig = 0x00000070;
            regsAddrPtr->HA.NATExceptionDropCounter = 0x000000314;
        }

    }/*end of unit HA */
}

/*******************************************************************************
* haLegacyDestroy
*
* DESCRIPTION:
*       destroy HA registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void haLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    DESTROY_STC_MAC(addrPtr->haRegs);
    DESTROY_SINGLE_REG_MAC(addrPtr->descriptorsConfig.txqToHeaderAlteration);

    return;
}


/*******************************************************************************
* mllInit
*
* DESCRIPTION:
*       init the DB - MLL (IP_MLL and L2_MLL) unit
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void mllInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*start of unit MLL */
        {/*start of unit multiTargetRateShape */
            regsAddrPtr->MLL.multiTargetRateShape.multiTargetRateShapeConfig = 0x00000210;
            regsAddrPtr->MLL.multiTargetRateShape.multiTargetRateShapeConfig2 = 0x00000218;
            regsAddrPtr->MLL.multiTargetRateShape.multiTargetMTUReg = 0x0000021c;
            regsAddrPtr->MLL.multiTargetRateShape.tokenBucketBaseLine = 0x00000220;

        }/*end of unit multiTargetRateShape */


        {/*start of unit mcLinkedListMLLTables */
            {/*100+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 63 ; n++) {
                    regsAddrPtr->MLL.mcLinkedListMLLTables.qoSProfileToMultiTargetTCQueuesReg[n] =
                        0x100+n*0x4;
                }/* end of loop n */
            }/*100+n*0x4*/
            {/*400+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->MLL.mcLinkedListMLLTables.extQoSModeMCQueueSelectionReg[n] =
                        0x400+n*0x4;
                }/* end of loop n */
            }/*400+n*0x4*/

        }/*end of unit mcLinkedListMLLTables */


        {/*start of unit multiTargetVsUcSDWRRAndStrictPriorityScheduler */
            regsAddrPtr->MLL.multiTargetVsUcSDWRRAndStrictPriorityScheduler.mcUcSDWRRAndStrictPriorityConfig = 0x00000214;

        }/*end of unit multiTargetVsUcSDWRRAndStrictPriorityScheduler */


        {/*start of unit multiTargetTCQueuesAndArbitrationConfig */
            regsAddrPtr->MLL.multiTargetTCQueuesAndArbitrationConfig.multitargetTCQueuesGlobalConfig = 0x00000004;
            regsAddrPtr->MLL.multiTargetTCQueuesAndArbitrationConfig.multitargetTCQueuesStrictPriorityEnableConfig = 0x00000200;
            regsAddrPtr->MLL.multiTargetTCQueuesAndArbitrationConfig.multitargetTCQueuesWeightConfig = 0x00000204;

        }/*end of unit multiTargetTCQueuesAndArbitrationConfig */


        {/*start of unit multiTargetEPortMap */
            regsAddrPtr->MLL.multiTargetEPortMap.multiTargetEPortValue = 0x00000300;
            regsAddrPtr->MLL.multiTargetEPortMap.multiTargetEPortMask = 0x00000304;
            regsAddrPtr->MLL.multiTargetEPortMap.multiTargetEPortBase = 0x00000308;
            regsAddrPtr->MLL.multiTargetEPortMap.ePortToEVIDXBase = 0x0000030c;

        }/*end of unit multiTargetEPortMap */


        {/*start of unit MLLOutInterfaceCntrs */
            {/*c84+n*0x100*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->MLL.MLLOutInterfaceCntrs.L2MLLOutInterfaceCntrConfig1[n] =
                        0xc84+n*0x100;
                }/* end of loop n */
            }/*c84+n*0x100*/
            {/*c80+n*0x100*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->MLL.MLLOutInterfaceCntrs.L2MLLOutInterfaceCntrConfig[n] =
                        0xc80+n*0x100;
                }/* end of loop n */
            }/*c80+n*0x100*/
            {/*e20+(n-1)*0x4*/
                GT_U32    n;
                for(n = 1 ; n <= 3 ; n++) {
                    regsAddrPtr->MLL.MLLOutInterfaceCntrs.L2MLLValidProcessedEntriesCntr[n-1] =
                        0xe20+(n-1)*0x4;
                }/* end of loop n */
            }/*e20+(n-1)*0x4*/
            regsAddrPtr->MLL.MLLOutInterfaceCntrs.L2MLLTTLExceptionCntr = 0x00000e08;
            regsAddrPtr->MLL.MLLOutInterfaceCntrs.L2MLLSkippedEntriesCntr = 0x00000e04;
            regsAddrPtr->MLL.MLLOutInterfaceCntrs.L2MLLSilentDropCntr = 0x00000e00;
            {/*c00+n*0x100*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->MLL.MLLOutInterfaceCntrs.L2MLLOutMcPktsCntr[n] =
                        0xc00+n*0x100;
                }/* end of loop n */
            }/*c00+n*0x100*/
            regsAddrPtr->MLL.MLLOutInterfaceCntrs.MLLSilentDropCntr = 0x00000800;
            regsAddrPtr->MLL.MLLOutInterfaceCntrs.MLLMCFIFODropCntr = 0x00000804;
            regsAddrPtr->MLL.MLLOutInterfaceCntrs.IPMLLSkippedEntriesCntr = 0x00000b04;
            {/*900+n*0x100*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->MLL.MLLOutInterfaceCntrs.IPMLLOutMcPktsCntr[n] =
                        0x900+n*0x100;
                }/* end of loop n */
            }/*900+n*0x100*/
            {/*984+n*0x100*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->MLL.MLLOutInterfaceCntrs.IPMLLOutInterfaceCntrConfig1[n] =
                        0x984+n*0x100;
                }/* end of loop n */
            }/*984+n*0x100*/
            {/*980+n*0x100*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->MLL.MLLOutInterfaceCntrs.IPMLLOutInterfaceCntrConfig[n] =
                        0x980+n*0x100;
                }/* end of loop n */
            }/*980+n*0x100*/

        }/*end of unit MLLOutInterfaceCntrs */


        {/*start of unit MLLGlobalCtrl */
            regsAddrPtr->MLL.MLLGlobalCtrl.MLLGlobalCtrl = 0x00000000;
            regsAddrPtr->MLL.MLLGlobalCtrl.MLLLookupTriggerConfig = 0x0000000c;
            regsAddrPtr->MLL.MLLGlobalCtrl.globalTTLExceptionConfig = 0x00000010;
            regsAddrPtr->MLL.MLLGlobalCtrl.globalEPortRangeConfig = 0x00000014;
            regsAddrPtr->MLL.MLLGlobalCtrl.globalEPortRangeMask = 0x00000018;
            regsAddrPtr->MLL.MLLGlobalCtrl.IPMLLTableResourceSharingConfig = 0x00000020;
            regsAddrPtr->MLL.MLLGlobalCtrl.L2MLLTableResourceSharingConfig = 0x00000024;
            regsAddrPtr->MLL.MLLGlobalCtrl.MLLInterruptCauseReg = 0x00000030;
            regsAddrPtr->MLL.MLLGlobalCtrl.MLLInterruptMaskReg = 0x00000034;
            regsAddrPtr->MLL.MLLGlobalCtrl.MLLReplicatedTrafficCtrl = 0x00000040;
            regsAddrPtr->MLL.MLLGlobalCtrl.sourceBasedL2MLLFiltering = 0x00000054;
            regsAddrPtr->MLL.MLLGlobalCtrl.MLLEccStatus = 0x00000058;
            regsAddrPtr->MLL.MLLGlobalCtrl.MLLMCSourceID = 0x00000060;

        }/*end of unit MLLGlobalCtrl */


    }/*end of unit MLL */
}

/*******************************************************************************
* mllLegacyDestroy
*
* DESCRIPTION:
*       destroy MLL registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void mllLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    DESTROY_STC_MAC(addrPtr->ipMtRegs);

    return;
}

/*******************************************************************************
* pclInit
*
* DESCRIPTION:
*       init the DB - PCL unit (IPCL)
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void pclInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);

    {/*start of unit PCL */
        {/*start of unit OAMConfigs */
            {/*740+4*n*/
                GT_U32    n;
                for(n = 1 ; n <= 15 ; n++) {
                    regsAddrPtr->PCL.OAMConfigs.channelTypeOAMOpcode[n-1] =
                        0x740+4*n;
                }/* end of loop n */
            }/*740+4*n*/
            regsAddrPtr->PCL.OAMConfigs.OAMRDIConfigs = 0x00000730;
            regsAddrPtr->PCL.OAMConfigs.OAMMEGLevelModeConfigs = 0x00000734;
            regsAddrPtr->PCL.OAMConfigs.OAMRFlagConfigs = 0x00000738;
            regsAddrPtr->PCL.OAMConfigs.RBitAssignmentEnable = 0x0000073c;

        }/*end of unit OAMConfigs */


        {/*start of unit HASH */
            {/*5c0+4*i*/
                GT_U32    i;
                for(i = 0 ; i <= 15 ; i++) {
                    regsAddrPtr->PCL.HASH.pearsonHashTable[i] =
                        0x5c0+4*i;
                }/* end of loop i */
            }/*5c0+4*i*/
            regsAddrPtr->PCL.HASH.CRCHashConfig = 0x00000020;
            {/*804+0x4*n*/
                GT_U32    n;
                for(n = 0 ; n <= 19 ; n++) {
                    regsAddrPtr->PCL.HASH.CRC32Salt[n] =
                        0x804+0x4*n;
                }/* end of loop n */
            }/*804+0x4*n*/
            regsAddrPtr->PCL.HASH.CRC32Seed = 0x00000800;

        }/*end of unit HASH */


        regsAddrPtr->PCL.ingrPolicyGlobalConfig = 0x00000000;
        regsAddrPtr->PCL.PCLUnitInterruptCause = 0x00000004;
        regsAddrPtr->PCL.PCLUnitInterruptMask = 0x00000008;
        regsAddrPtr->PCL.policyEngineConfig = 0x0000000c;
        regsAddrPtr->PCL.policyEngineUserDefinedBytesConfig = 0x00000014;
        {/*34+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 2 ; n++) {
                regsAddrPtr->PCL.lookupSourceIDMask[n] =
                    0x34+4*n;
            }/* end of loop n */
        }/*34+4*n*/
        regsAddrPtr->PCL.policyEngineUserDefinedBytesExtConfig = 0x00000024;
        {/*74+0x4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 1 ; n++) {
                regsAddrPtr->PCL.TCAMKeyTypeMap[n] =
                    0x74+0x4*n;
            }/* end of loop n */
        }/*74+0x4*n*/
        {/*80+4*i*/
            GT_U32    i;
            GT_U32    nMax = isSip5_20 ? 15 : 7;
            for(i = 0 ; i <= nMax ; i++) {
                regsAddrPtr->PCL.L2L3IngrVLANCountingEnable[i] =
                    0x80+4*i;
            }/* end of loop i */
        }/*80+4*i*/
        regsAddrPtr->PCL.countingModeConfig = 0x00000070;

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            regsAddrPtr->PCL.pclBadAddressLatchReg = 0x00000010;
            regsAddrPtr->PCL.CFMEtherType = 0x00000018;
        }
    }/*end of unit PCL */
}


/*******************************************************************************
* pclLegacyDestroy
*
* DESCRIPTION:
*       destroy PCL registers from the legacy DB. (IPCL)
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void pclLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;
    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    DESTROY_STC_MAC(addrPtr->pclRegs);

    DESTROY_SINGLE_REG_MAC(addrPtr->descriptorsConfig.pclToBridge);
}

/*******************************************************************************
* epclInit
*
* DESCRIPTION:
*       init the DB - EPCL unit (EPCL)
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void epclInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*start of unit EPCL */
        regsAddrPtr->EPCL.egrPolicyGlobalConfig = 0x00000000;
        regsAddrPtr->EPCL.EPCLOAMPktDetection = 0x00000004;
        regsAddrPtr->EPCL.EPCLOAMMEGLevelMode = 0x00000008;
        regsAddrPtr->EPCL.EPCLOAMRDI = 0x0000000c;
        regsAddrPtr->EPCL.EPCLInterruptCause = 0x00000010;
        regsAddrPtr->EPCL.EPCLInterruptMask = 0x00000014;
        {/*30+4*s*/
            GT_U32    s;
            for(s = 0 ; s <= 4 ; s++) {
                regsAddrPtr->EPCL.EPCLChannelTypeToOAMOpcodeMap[s] =
                    0x30+4*s;
            }/* end of loop s */
        }/*30+4*s*/
        regsAddrPtr->EPCL.EPCLReservedBitMask = 0x00000020;
        regsAddrPtr->EPCL.TCAMKeyTypeMap = 0x00000074;

    }/*end of unit EPCL */

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        regsAddrPtr->EPCL.EPCLBadAddrLatchReg = 0x00000018;
        regsAddrPtr->EPCL.oamRBbitConfigReg = 0x0000001c;
    }

}

#define  CPU_PORT_DMA_CNS   72

/* copy from index to index */
#define COPY_REG_ADDR_MAC(regArr,srcIndex,trgIndex) \
    regArr[trgIndex] = regArr[srcIndex]

/*******************************************************************************
* rxdmaInit
*
* DESCRIPTION:
*       init the DB - RXDMA unit (per unit index)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       index   - unit index
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void rxdmaInit
(
    IN  GT_U8 devNum,
    IN  GT_U32  index
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  lastRxDmaIndex = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E) ? 12 : CPU_PORT_DMA_CNS;

    {/*start of unit rxDMA */
        {/*start of unit singleChannelDMAConfigs */
            {/*304+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                    regsAddrPtr->rxDMA[index].singleChannelDMAConfigs.SCDMAConfig0[n] =
                        0x304+n*0x4;
                }/* end of loop n */
            }/*304+n*0x4*/
            {/*450+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                    regsAddrPtr->rxDMA[index].singleChannelDMAConfigs.SCDMAConfig1[n] =
                        0x450+n*0x4;
                }/* end of loop n */
            }/*450+n*0x4*/
            {/*600+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                    regsAddrPtr->rxDMA[index].singleChannelDMAConfigs.SCDMACTPktIndentification[n] =
                        0x600+n*0x4;
                }/* end of loop n */
            }/*600+n*0x4*/

        }/*end of unit singleChannelDMAConfigs */


        {/*start of unit globalRxDMAConfigs */
            {/*start of unit rejectConfigs */
                regsAddrPtr->rxDMA[index].globalRxDMAConfigs.rejectConfigs.rejectPktCommand = 0x000000a0;
                regsAddrPtr->rxDMA[index].globalRxDMAConfigs.rejectConfigs.rejectCPUCode = 0x000000a4;

            }/*end of unit rejectConfigs */


            {/*start of unit globalConfigCutThrough */
                regsAddrPtr->rxDMA[index].globalRxDMAConfigs.globalConfigCutThrough.CTEtherTypeIdentification = 0x00000084;
                regsAddrPtr->rxDMA[index].globalRxDMAConfigs.globalConfigCutThrough.CTUPEnable = 0x00000088;

            }/*end of unit globalConfigCutThrough */


            {/*start of unit buffersLimitProfiles */
                GT_U32    n;
                regsAddrPtr->rxDMA[index].globalRxDMAConfigs.buffersLimitProfiles.portsBuffersLimitProfileXONXOFFConfig[0] = 0;
                regsAddrPtr->rxDMA[index].globalRxDMAConfigs.buffersLimitProfiles.portsBuffersLimitProfileRxlimitConfig[0] = 0x00000004;
                for(n = 1 ; n <= 7 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.buffersLimitProfiles.portsBuffersLimitProfileXONXOFFConfig[n] =
                        regsAddrPtr->rxDMA[index].globalRxDMAConfigs.buffersLimitProfiles.portsBuffersLimitProfileXONXOFFConfig[n - 1] + 8;
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.buffersLimitProfiles.portsBuffersLimitProfileRxlimitConfig[n] =
                        regsAddrPtr->rxDMA[index].globalRxDMAConfigs.buffersLimitProfiles.portsBuffersLimitProfileRxlimitConfig[n - 1] + 8;
                }/* end of loop n */
            }/*end of unit buffersLimitProfiles */

        }/*end of unit globalRxDMAConfigs */

        { /* start of unit SCDMA debug */
            GT_U32    n;
            for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                regsAddrPtr->rxDMA[index].scdmaDebug.portBuffAllocCnt[n] = 0x1900+n*0x4;
            }
        }
        {/*start of unit SIP_COMMON_MODULES */
            {/*start of unit pizzaArbiter */
                {/*2508+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 84 ; n++) {
                        regsAddrPtr->rxDMA[index].SIP_COMMON_MODULES.pizzaArbiter.pizzaArbiterConfigReg[n] =
                            0x2508+n*0x4;
                    }/* end of loop n */
                }/*2508+n*0x4*/
                regsAddrPtr->rxDMA[index].SIP_COMMON_MODULES.pizzaArbiter.pizzaArbiterCtrlReg = 0x00002500;
                regsAddrPtr->rxDMA[index].SIP_COMMON_MODULES.pizzaArbiter.pizzaArbiterStatusReg = 0x00002504;

            }/*end of unit pizzaArbiter */


        }/*end of unit SIP_COMMON_MODULES */

        {/*start of unit interrupts */
            {/*d0c+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                    regsAddrPtr->rxDMA[index].interrupts.rxDMASCDMAInterruptMask[n] =
                        0xd0c+n*0x4;
                }/* end of loop n */
            }/*d0c+n*0x4*/
            {/*908+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                    regsAddrPtr->rxDMA[index].interrupts.rxDMASCDMAInterruptCause[n] =
                        0x908+n*0x4;
                }/* end of loop n */
            }/*908+n*0x4*/
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterrupt0Cause = 0x00000900;
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterrupt0Mask = 0x00000904;
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary0Cause = 0x00001028;
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary0Mask = 0x0000102c;
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary1Cause = 0x00001030;
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary1Mask = 0x00001034;
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary2Cause = 0x00001038;
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary2Mask = 0x0000103c;
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary3Cause = 0x00001040;
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary3Mask = 0x00001044;
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummaryCause = 0x00001048;
            regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummaryMask = 0x0000104c;

        }/*end of unit interrupts */

        { /* start of unit debug */

            regsAddrPtr->rxDMA[index].debug.countersAndCountersStatuses.currentNumOfPacketsStoredInMppm = 0x0000123C;

        }/*end of unit debug */

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            {/*start of unit SIP_COMMON_MODULES */
                {/*start of unit pizzaArbiter */
                    {/*2a08+n*0x4*/
                        GT_U32    n;
                        for(n = 0 ; n <= 84 ; n++) {
                            regsAddrPtr->rxDMA[index].SIP_COMMON_MODULES.pizzaArbiter.pizzaArbiterConfigReg[n] =
                                0x2a08+n*0x4;
                        }/* end of loop n */
                    }/*2a08+n*0x4*/
                    regsAddrPtr->rxDMA[index].SIP_COMMON_MODULES.pizzaArbiter.pizzaArbiterCtrlReg = 0x00002a00;
                    regsAddrPtr->rxDMA[index].SIP_COMMON_MODULES.pizzaArbiter.pizzaArbiterStatusReg = 0x00002a04;

                }/*end of unit pizzaArbiter */


            }/*end of unit SIP_COMMON_MODULES */

            {/*start of unit singleChannelDMAConfigs */
                {/*804+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                        regsAddrPtr->rxDMA[index].singleChannelDMAConfigs.SCDMAConfig0[n] =
                            0x804+n*0x4;
                    }/* end of loop n */
                }/*804+n*0x4*/
                {/*950+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                        regsAddrPtr->rxDMA[index].singleChannelDMAConfigs.SCDMAConfig1[n] =
                            0x950+n*0x4;
                    }/* end of loop n */
                }/*950+n*0x4*/
                {/*b00+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                        regsAddrPtr->rxDMA[index].singleChannelDMAConfigs.SCDMACTPktIndentification[n] =
                            0xb00+n*0x4;
                    }/* end of loop n */
                }/*b00+n*0x4*/

            }/*end of unit singleChannelDMAConfigs */
            { /* start of unit SCDMA debug */
                GT_U32    n;
                for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                    regsAddrPtr->rxDMA[index].scdmaDebug.portBuffAllocCnt[n] = 0x1e00+n*0x4;
                }
            }
            {/*start of unit interrupts */
                {/*120c+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                        regsAddrPtr->rxDMA[index].interrupts.rxDMASCDMAInterruptMask[n] =
                            0x120c+n*0x4;
                    }/* end of loop n */
                }/*120c+n*0x4*/
                {/*e08+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                        regsAddrPtr->rxDMA[index].interrupts.rxDMASCDMAInterruptCause[n] =
                            0xe08+n*0x4;
                    }/* end of loop n */
                }/*e08+n*0x4*/
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterrupt0Cause = 0x00000e00;
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterrupt0Mask = 0x00000e04;
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary0Cause = 0x00001528;
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary0Mask = 0x0000152c;
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary1Cause = 0x00001530;
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary1Mask = 0x00001534;
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary2Cause = 0x00001538;
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary2Mask = 0x0000153c;
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary3Cause = 0x00001540;
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummary3Mask = 0x00001544;
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummaryCause = 0x00001548;
                regsAddrPtr->rxDMA[index].interrupts.rxDMAInterruptSummaryMask = 0x0000154c;

            }/*end of unit interrupts */
            { /* start of unit debug */

                regsAddrPtr->rxDMA[index].debug.countersAndCountersStatuses.currentNumOfPacketsStoredInMppm = 0x0000173C;

            }/*end of unit debug */
        }

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {/*start of unit preIngrPrioritizationConfStatus */
            {/*000000c4+n * 0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.VLANUPPriorityArrayProfile[n] =
                        0x000000c4+n * 0x4;
                }/* end of loop n */
            }/*000000c4+n * 0x4*/
            {/*000003a8+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 1 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.MPLSEtherTypeConf[n] =
                        0x000003a8+n*0x4;
                }/* end of loop n */
            }/*000003a8+n*0x4*/
            {/*00000450+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.VIDConf[n] =
                        0x00000450+n*0x4;
                }/* end of loop n */
            }/*00000450+n*0x4*/
            {/*000003c8+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.genericEtherType[n] =
                        0x000003c8+n*0x4;
                }/* end of loop n */
            }/*000003c8+n*0x4*/
            {/*000000e4+m*0x4 + n*0x20*/
                GT_U32    n,m;
                for(n = 0 ; n <= 3 ; n++) {
                    for(m = 0 ; m <= 7 ; m++) {
                        regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.qoSDSAPriorityArrayProfileSegment[n][m] =
                            0x000000e4+m*0x4 + n*0x20;
                    }/* end of loop m */
                }/* end of loop n */
            }/*000000e4+m*0x4 + n*0x20*/
            regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.preingressPrioritizationEnable = 0x000000b4;
            regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.PIPPriorityDropGlobalCntrsClear = 0x00000740;
            regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.PIPPrioThresholds = 0x000000b0;
            regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.PIPMedPriorityDropGlobalCntrMsb = 0x00000724;
            regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.PIPMedPriorityDropGlobalCntrLsb = 0x00000720;
            regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.PIPLowPriorityDropGlobalCntrMsb = 0x00000714;
            {/*00000398+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.VLANEtherTypeConf[n] =
                        0x00000398+n*0x4;
                }/* end of loop n */
            }/*00000398+n*0x4*/
            {/*00000408+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.MACDA2MsbBytesConf[n] =
                        0x00000408+n*0x4;
                }/* end of loop n */
            }/*00000408+n*0x4*/
            regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.PIPHighPriorityDropGlobalCntrLsb = 0x00000730;
            {/*00000174+n * 0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.MPLSEXPPriorityArrayProfile[n] =
                        0x00000174+n * 0x4;
                }/* end of loop n */
            }/*00000174+n * 0x4*/
            {/*00000630+n * 0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.UPDSATagArrayProfile[n] =
                        0x00000630+n * 0x4;
                }/* end of loop n */
            }/*00000630+n * 0x4*/
            {/*000002f0+n * 0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.MACDAPriorityArrayProfile[n] =
                        0x000002f0+n * 0x4;
                }/* end of loop n */
            }/*000002f0+n * 0x4*/
            {/*00000428+n *0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.MACDAMask4LsbConf[n] =
                        0x00000428+n *0x4;
                }/* end of loop n */
            }/*00000428+n *0x4*/
            {/*0000043c+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.MACDAMask2MsbConf[n] =
                        0x0000043c+n*0x4;
                }/* end of loop n */
            }/*0000043c+n*0x4*/
            {/*000003e8+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.MACDA4LsbBytesConf[n] =
                        0x000003e8+n*0x4;
                }/* end of loop n */
            }/*000003e8+n*0x4*/
            regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.PIPLowPriorityDropGlobalCntrLSb = 0x00000710;
            {/*000004d0+m*0x4 + n*0x40*/
                GT_U32    n,m;
                for(n = 0 ; n <= 3 ; n++) {
                    for(m = 0 ; m <= 15 ; m++) {
                        regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.IPv6TCPriorityArrayProfileSegment[n][m] =
                            0x000004d0+m*0x4 + n*0x40;
                    }/* end of loop m */
                }/* end of loop n */
            }/*000004d0+m*0x4 + n*0x40*/
            regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.IPv6EtherTypeConf = 0x00000374;
            {/*00000198+m*0x4 + n*0x40*/
                GT_U32    n,m;
                for(n = 0 ; n <= 3 ; n++) {
                    for(m = 0 ; m <= 15 ; m++) {
                        regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.IPv4ToSPriorityArrayProfileSegment[n][m] =
                            0x00000198+m*0x4 + n*0x40;
                    }/* end of loop m */
                }/* end of loop n */
            }/*00000198+m*0x4 + n*0x40*/
            regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.IPv4EtherTypeConf = 0x00000350;
            regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.PIPHighPriorityDropGlobalCntrMsb = 0x00000734;
            if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
            {
                regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.PIPDropGlobalCounterTrigger  = 0x00000738;
            }
            {/*00000320+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->rxDMA[index].globalRxDMAConfigs.preIngrPrioritizationConfStatus.etherTypePriorityArrayProfile[n] =
                        0x00000320+n*0x4;
                }/* end of loop n */
            }/*00000320+n*0x4*/

        }/*end of unit preIngrPrioritizationConfStatus */

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            /* manually added : rxDMA.singleChannelDMAPip */
            {/*3A00+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                    regsAddrPtr->rxDMA[index].singleChannelDMAPip.SCDMA_n_PIP_Config[n] =
                        0x3A00+n*0x4;
                }/* end of loop n */
            }/*3A00+n*0x4*/

            {/*3500+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= lastRxDmaIndex ; n++) {
                    regsAddrPtr->rxDMA[index].singleChannelDMAPip.SCDMA_n_Drop_Pkt_Counter[n] =
                        0x3500+n*0x4;
                }/* end of loop n */
            }/*3500+n*0x4*/
        }

    }/*end of unit rxDMA */


    if(index == 0 && (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E))
    {
        /* copy last RxDma to cpu port position */
        COPY_REG_ADDR_MAC(regsAddrPtr->rxDMA[0].singleChannelDMAConfigs.SCDMAConfig0              , lastRxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->rxDMA[0].singleChannelDMAConfigs.SCDMAConfig1              , lastRxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->rxDMA[0].singleChannelDMAConfigs.SCDMACTPktIndentification , lastRxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->rxDMA[0].scdmaDebug.portBuffAllocCnt                       , lastRxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->rxDMA[0].interrupts.rxDMASCDMAInterruptMask                , lastRxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->rxDMA[0].interrupts.rxDMASCDMAInterruptCause               , lastRxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->rxDMA[0].singleChannelDMAPip.SCDMA_n_PIP_Config            , lastRxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->rxDMA[0].singleChannelDMAPip.SCDMA_n_Drop_Pkt_Counter      , lastRxDmaIndex, CPU_PORT_DMA_CNS);
    }
}

/*******************************************************************************
* rxdmaUpdateUnit0AsGlobalPortsAddr
*
* DESCRIPTION:
*       update the DB - RXDMA unit - after multi units set 'per port' addresses.
*       the DB of RXDMA[0] updated for the ports with 'global port index'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       currentUnitIndex - the index of current RXDMA[] unit
*       portIn_unit_0     - the 'global port' index (in unit 0)
*       portInCurrentUnit - the 'local port'  index (in current unit)
*       numOfPorts - number of ports to update.
*       isFirstUpdate - indication of first update
*       isLastUpdate - indication of last update
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void rxdmaUpdateUnit0AsGlobalPortsAddr
(
    IN  GT_U8   devNum,
    IN  GT_U32  currentUnitIndex,
    IN  GT_U32  portIn_unit_0,
    IN  GT_U32  portInCurrentUnit,
    IN  GT_U32  numOfPorts,
    IN  GT_BOOL isFirstUpdate,
    IN  GT_BOOL isLastUpdate
)
{
    GT_U32    saveLastNumOfPorts;/*number of ports from the end of the unitStart1Ptr[]
                                  that need to be saved before overridden by unitStart2Ptr[]*/
    GT_U32    saveToIndex;/*the index in unitStart1Ptr[] to save the 'last ports'
                                  valid when saveLastNumOfPorts != 0*/
    const GT_U32 *localDmaMapArr; /* local DMAs in specific DP, NULL in cases 1:1 mapping */

    localDmaMapArr = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[currentUnitIndex].localDmaMapArr;

    if(isLastUpdate == GT_TRUE &&
       PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.supportRelativePortNum)
    {
        /* do not let the DB be overridden with the CPU port of last DP */
        /* because we saved the CPU port of the first DP */
        numOfPorts -=
            (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[0].cpuPortDmaNum ? 1 : 0);
    }

    if(isFirstUpdate == GT_TRUE &&
       PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.supportRelativePortNum)
    {
        /* update the DB with the CPU port address on the lowest existing DP */
        /* save CPU port */
        saveLastNumOfPorts =
            (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[0].cpuPortDmaNum ? 1 : 0);
        saveToIndex = CPU_PORT_DMA_CNS;
    }
    else
    {
        if((localDmaMapArr != NULL) && (isLastUpdate == GT_TRUE))
        {
            /* move CPU port registers addresses to position 72 */
            saveLastNumOfPorts = 1;
            saveToIndex = CPU_PORT_DMA_CNS;
        }
        else
        {
            saveLastNumOfPorts = 0;
            saveToIndex = 0;
        }
    }
    /************************************/
    /*   update the per port addresses  */
    /*   of the rxdma unit 0 with those */
    /*   of unit 1                      */
    /************************************/
    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, rxDMA[0] , rxDMA[currentUnitIndex] , singleChannelDMAConfigs.SCDMAConfig0 ,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );
    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, rxDMA[0] , rxDMA[currentUnitIndex] , singleChannelDMAConfigs.SCDMAConfig1,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );
    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, rxDMA[0] , rxDMA[currentUnitIndex] , singleChannelDMAConfigs.SCDMACTPktIndentification,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, rxDMA[0] , rxDMA[currentUnitIndex] , scdmaDebug.portBuffAllocCnt,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, rxDMA[0] , rxDMA[currentUnitIndex] , interrupts.rxDMASCDMAInterruptMask,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, rxDMA[0] , rxDMA[currentUnitIndex] , interrupts.rxDMASCDMAInterruptCause,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, rxDMA[0] , rxDMA[currentUnitIndex] , singleChannelDMAPip.SCDMA_n_PIP_Config,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, rxDMA[0] , rxDMA[currentUnitIndex] , singleChannelDMAPip.SCDMA_n_Drop_Pkt_Counter,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

}

/*******************************************************************************
* rxdmaLegacyDestroy
*
* DESCRIPTION:
*       destroy RXDMA registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void rxdmaLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* invalidate previous registers */
    DESTROY_STC_MAC(addrPtr->cutThroughRegs);

}

/*******************************************************************************
* txdmaInit
*
* DESCRIPTION:
*       init the DB - TXDMA unit (per unit index)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       index   - unit index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void txdmaInit
(
    IN  GT_U8 devNum,
    IN  GT_U32  index
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  lastTxDmaIndex = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E) ? 12 : 73;

    {/*start of unit txDMA */
        {/*start of unit txDMAThresholdsConfigs */
            {/*00001020+r*0x4*/
                GT_U32    r;
                for(r = 0 ; r <= 0 ; r++) {
                    regsAddrPtr->txDMA[index].txDMAThresholdsConfigs.RXDMAUpdatesFIFOsThresholds[r] =
                        0x00001020+r*0x4;
                }/* end of loop r */
            }/*00001020+r*0x4*/
            regsAddrPtr->txDMA[index].txDMAThresholdsConfigs.clearFIFOThreshold = 0x00001000;
            regsAddrPtr->txDMA[index].txDMAThresholdsConfigs.headerReorderFifoThreshold = 0x0000100C;
            regsAddrPtr->txDMA[index].txDMAThresholdsConfigs.payloadReorderFifoThreshold = 0x00001010;

        }/*end of unit txDMAThresholdsConfigs */


        {/*start of unit txDMAPerSCDMAConfigs */
            {/*0000300c+0x20*p*/
                GT_U32    p;
                for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                    regsAddrPtr->txDMA[index].txDMAPerSCDMAConfigs.burstLimiterSCDMA[p] =
                        0x0000300c+0x20*p;
                }/* end of loop p */
            }/*0000300c+0x20*p*/
            {/*00003000+p*0x20*/
                GT_U32    p;
                for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                    regsAddrPtr->txDMA[index].txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1[p] =
                        0x00003000+p*0x20;
                }/* end of loop p */
            }/*00003000+p*0x20*/
            {/*00003008+0x20*p*/
                GT_U32    p;
                for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                    regsAddrPtr->txDMA[index].txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA[p] =
                        0x00003008+0x20*p;
                }/* end of loop p */
            }/*00003008+0x20*p*/
            {/*00003010+p*0x20*/
                GT_U32    p;
                for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                    regsAddrPtr->txDMA[index].txDMAPerSCDMAConfigs.SCDMAConfigs[p] =
                        0x00003010+p*0x20;
                }/* end of loop p */
            }/*00003010+p*0x20*/
            {/*00003014+p*0x20*/
                GT_U32    p;
                for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                    regsAddrPtr->txDMA[index].txDMAPerSCDMAConfigs.SCDMAMetalFixReg[p] =
                        0x00003014+p*0x20;
                }/* end of loop p */
            }/*00003014+p*0x20*/

        }/*end of unit txDMAPerSCDMAConfigs */


        {/*start of unit txDMAInterrupts */
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAIDFIFOOverrunInterruptCauseReg1 = 0x00002000;
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAIDFIFOOverrunInterruptMaskReg1 = 0x00002004;
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAIDFIFOOverrunInterruptCauseReg2 = 0x00002008;
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAIDFIFOOverrunInterruptMaskReg2 = 0x0000200c;
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAWaitingForNextReplyInterruptMaskReg3 = 0x00002034;
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAIDFIFOOverrunInterruptMaskReg3 = 0x00002014;
            regsAddrPtr->txDMA[index].txDMAInterrupts.RXDMAUpdatesFIFOsOverrunInterruptCauseReg1 = 0x00002018;
            regsAddrPtr->txDMA[index].txDMAInterrupts.RXDMAUpdatesFIFOsOverrunInterruptMaskReg1 = 0x0000201c;
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAWaitingForNextReplyInterruptCauseReg1 = 0x00002020;
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAWaitingForNextReplyInterruptMaskReg1 = 0x00002024;
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAWaitingForNextReplyInterruptCauseReg2 = 0x00002028;
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAWaitingForNextReplyInterruptMaskReg2 = 0x0000202c;
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAWaitingForNextReplyInterruptCauseReg3 = 0x00002030;
            regsAddrPtr->txDMA[index].txDMAInterrupts.SCDMAIDFIFOOverrunInterruptCauseReg3 = 0x00002010;
            regsAddrPtr->txDMA[index].txDMAInterrupts.txDMAGeneralCauseReg1 = 0x00002038;
            regsAddrPtr->txDMA[index].txDMAInterrupts.txDMAGeneralMaskReg1 = 0x0000203c;
            regsAddrPtr->txDMA[index].txDMAInterrupts.txDMASummaryCauseReg = 0x00002040;
            regsAddrPtr->txDMA[index].txDMAInterrupts.txDMASummaryMaskReg = 0x00002044;

        }/*end of unit txDMAInterrupts */


        {/*start of unit txDMAGlobalConfigs */
            regsAddrPtr->txDMA[index].txDMAGlobalConfigs.txDMAGlobalConfig1 = 0x00000000;
            regsAddrPtr->txDMA[index].txDMAGlobalConfigs.txDMAGlobalConfig3 = 0x00000008;
            regsAddrPtr->txDMA[index].txDMAGlobalConfigs.ingrCoreIDToMPPMMap = 0x0000000c;
            regsAddrPtr->txDMA[index].txDMAGlobalConfigs.TXDMAMetalFixReg = 0x00000010;

        }/*end of unit txDMAGlobalConfigs */


        {/*start of unit txDMADescCriticalFieldsECCConfigs */
            regsAddrPtr->txDMA[index].txDMADescCriticalFieldsECCConfigs.dataECCConfig = 0x00000500;
            regsAddrPtr->txDMA[index].txDMADescCriticalFieldsECCConfigs.dataECCStatus = 0x00000504;

        }/*end of unit txDMADescCriticalFieldsECCConfigs */


        {/*start of unit txDMADebug */
            {/*start of unit informativeDebug */
                {/*00005100+c*0x4*/
                    GT_U32    c;
                    for(c = 0 ; c <= 0 ; c++) {
                        regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.nextBCFIFOFillLevel[c] =
                            0x00005100+c*0x4;
                    }/* end of loop c */
                }/*00005100+c*0x4*/
                {/*00005120+c*0x4*/
                    GT_U32    c;
                    for(c = 0 ; c <= 0 ; c++) {
                        regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.nextBCFIFOMaxFillLevel[c] =
                            0x00005120+c*0x4;
                    }/* end of loop c */
                }/*00005120+c*0x4*/
                {/*00005200+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                        regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.SCDMAStatusReg1[p] =
                            0x00005200+p*0x4;
                    }/* end of loop p */
                }/*00005200+p*0x4*/
                {/*00005500+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                        regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.SCDMAStatusReg2[p] =
                            0x00005500+p*0x4;
                    }/* end of loop p */
                }/*00005500+p*0x4*/
                {/*00005700+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                        regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.SCDMAPrefFIFOFillLevel[p] =
                            0x00005700+p*0x4;
                    }/* end of loop p */
                }/*00005700+p*0x4*/
                {/*00005900+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                        regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.SCDMAPrefFIFOMaxFillLevel[p] =
                            0x00005900+p*0x4;
                    }/* end of loop p */
                }/*00005900+p*0x4*/
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.descOverrunReg1 = 0x00005000;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.descOverrunReg2 = 0x00005004;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.descOverrunReg3 = 0x00005008;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.clearFIFOFillLevel = 0x0000500c;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.clearFIFOMaxFillLevel = 0x00005010;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.headerReorderFIFOFillLevel = 0x00005014;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.headerReorderFIFOMaxFillLevel = 0x0000501c;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.payloadReorderFIFOFillLevel = 0x00005020;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.payloadReorderFIFOMaxFillLevel = 0x00005024;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.descIDFIFOFillLevel = 0x00005028;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.descIDFIFOMaxFillLevel = 0x0000502c;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.debugBusCtrlReg = 0x00005030;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.txDMADebugBus = 0x00005034;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.incomingDescsCntrLsb = 0x00005038;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingDescsCntrLsb = 0x0000503c;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingClearRequestsCntrLsb = 0x00005040;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.MPPMReadRequestsCntr = 0x00005044;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.MPPMReadAnswersCntrLsb = 0x00005048;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingPayloadsCntrLsb = 0x0000504c;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingPayloadsEOPCntrLsb = 0x00005050;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingHeadersEOPCntrLsb = 0x00005054;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingCTBCUpdatesCntr = 0x00005058;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.HADescIDDBFreeIDFIFOFillLevel = 0x0000505c;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.internalDescIDDBFreeIDFIFOFillLevel = 0x00005060;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.headerReorderMemIDDBFreeIDFIFOFillLevel = 0x00005070;
                regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.payloadReorderMemIDDBFreeIDFIFOFillLevel = 0x00005074;

            }/*end of unit informativeDebug */


        }/*end of unit txDMADebug */


        {/*start of unit txDMAPizzaArb */
            {/*start of unit pizzaArbiter */
                {/*00004008+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 84 ; n++) {
                        regsAddrPtr->txDMA[index].txDMAPizzaArb.pizzaArbiter.pizzaArbiterConfigReg[n] =
                            0x00004008+n*0x4;
                    }/* end of loop n */
                }/*00004008+n*0x4*/
                regsAddrPtr->txDMA[index].txDMAPizzaArb.pizzaArbiter.pizzaArbiterCtrlReg = 0x00004000;
                regsAddrPtr->txDMA[index].txDMAPizzaArb.pizzaArbiter.pizzaArbiterStatusReg = 0x00004004;

            }/*end of unit pizzaArbiter */


        }/*end of unit txDMAPizzaArb */

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.incomingDescsCntrLsb = 0x00005038;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.incomingDescsCntrMsb = 0x0000503c;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingDescsCntrLsb = 0x00005040;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingDescsCntrMsb = 0x00005044;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingClearRequestsCntrLsb = 0x00005048;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingClearRequestsCntrMsb = 0x0000504c;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.MPPMReadRequestsCntr = 0x00005050;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.MPPMReadAnswersCntrLsb = 0x00005054;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.MPPMReadAnswersCntrMsb = 0x00005058;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingPayloadsCntrLsb = 0x0000505c;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingPayloadsCntrMsb = 0x00005060;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingPayloadsEOPCntrLsb = 0x00005064;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingPayloadsEOPCntrMsb = 0x00005068;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingHeadersEOPCntrLsb = 0x0000506c;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingHeadersEOPCntrMsb = 0x00005070;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.outgoingCTBCUpdatesCntr = 0x00005074;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.internalDescIDDBFreeIDFIFOFillLevel = 0x00005080;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.headerReorderMemIDDBFreeIDFIFOFillLevel = 0x00005084;
            regsAddrPtr->txDMA[index].txDMADebug.informativeDebug.payloadReorderMemIDDBFreeIDFIFOFillLevel = 0x00005088;
            {/*0000300c+0x20*p*/
                GT_U32    p;
                for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                    regsAddrPtr->txDMA[index].txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA[p] =
                        0x0000300c+0x20*p;
                }/* end of loop p */
            }/*0000300c+0x20*p*/

            {/*00003018+0x20*p*/
                GT_U32    p;
                for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                    regsAddrPtr->txDMA[index].txDMAPerSCDMAConfigs.burstLimiterSCDMA[p] =
                        0x00003018+0x20*p;
                }/* end of loop p */
            }/*00003018+0x20*p*/

        }

        if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
        {
            {/*0000301C+p*0x20*/
                GT_U32    p;
                for(p = 0 ; p <= lastTxDmaIndex ; p++) {
                    regsAddrPtr->txDMA[index].txDMAPerSCDMAConfigs.SCDMAConfigs2[p] =
                        0x0000301C+p*0x20;
                }/* end of loop p */
            }/*0000301C+p*0x20*/

        }

    }/*end of unit txDMA */


    if(index == 0 && (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E))
    {
        /* copy last RxDma to cpu port position */
        COPY_REG_ADDR_MAC(regsAddrPtr->txDMA[0].txDMAPerSCDMAConfigs.burstLimiterSCDMA                , lastTxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txDMA[0].txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1  , lastTxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txDMA[0].txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA          , lastTxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txDMA[0].txDMAPerSCDMAConfigs.SCDMAConfigs                     , lastTxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txDMA[0].txDMAPerSCDMAConfigs.SCDMAMetalFixReg                 , lastTxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txDMA[0].txDMADebug.informativeDebug.SCDMAStatusReg1           , lastTxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txDMA[0].txDMADebug.informativeDebug.SCDMAStatusReg2           , lastTxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txDMA[0].txDMADebug.informativeDebug.SCDMAPrefFIFOFillLevel    , lastTxDmaIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txDMA[0].txDMADebug.informativeDebug.SCDMAPrefFIFOMaxFillLevel , lastTxDmaIndex, CPU_PORT_DMA_CNS);
    }

}
/*******************************************************************************
* txdmaUpdateUnit0AsGlobalPortsAddr
*
* DESCRIPTION:
*       update the DB - TXDMA unit - after multi units set 'per port' addresses.
*       the DB of TXDMA[0] updated for the ports with 'global port index'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       currentUnitIndex - the index of current TXDMA[] unit
*       portIn_unit_0     - the 'global port' index (in unit 0)
*       portInCurrentUnit - the 'local port'  index (in current unit)
*       numOfPorts - number of ports to update.
*       isFirstUpdate - indication of first update
*       isLastUpdate - indication of last update
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void txdmaUpdateUnit0AsGlobalPortsAddr
(
    IN  GT_U8 devNum,
    IN  GT_U32  currentUnitIndex,
    IN  GT_U32  portIn_unit_0,
    IN  GT_U32  portInCurrentUnit,
    IN  GT_U32  numOfPorts,
    IN  GT_BOOL isFirstUpdate,
    IN  GT_BOOL isLastUpdate
)
{
    GT_U32    saveLastNumOfPorts;/*number of ports from the end of the unitStart1Ptr[]
                                 that need to be saved before overridden by unitStart2Ptr[]*/
    GT_U32    saveToIndex;/*the index in unitStart1Ptr[] to save the 'last ports'
                                  valid when saveLastNumOfPorts != 0*/
    const GT_U32 *localDmaMapArr; /* local DMAs in specific DP, NULL in cases 1:1 mapping */

    localDmaMapArr = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[currentUnitIndex].localDmaMapArr;


    if(isLastUpdate == GT_TRUE &&
       PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.supportRelativePortNum)
    {
        /* do not let the DB be overridden with the CPU port of last DP */
        /* because we saved the CPU port of the first DP */
        numOfPorts -=
            (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[0].cpuPortDmaNum ? 1 : 0)+
            (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[0].tmDmaNum ? 1 : 0);
    }

    if(isFirstUpdate == GT_TRUE &&
       PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.supportRelativePortNum)
    {
        /* update the DB with the CPU port address on the lowest existing DP */
        /* save CPU port */
        saveLastNumOfPorts =
            (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[0].cpuPortDmaNum ? 1 : 0)+
            (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[0].tmDmaNum ? 1 : 0);

        saveToIndex = CPU_PORT_DMA_CNS;
    }
    else
    {
        if((localDmaMapArr != NULL) && (isLastUpdate == GT_TRUE))
        {
            /* move CPU port registers addresses to position 72 */
            saveLastNumOfPorts = 1;
            saveToIndex = CPU_PORT_DMA_CNS;
        }
        else
        {
            saveLastNumOfPorts = 0;
            saveToIndex = 0;
        }
    }
    /************************************/
    /*   update the per port addresses  */
    /*   of the txdma unit 0 with those */
    /*   of unit 1                      */
    /************************************/
    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txDMA[0] , txDMA[currentUnitIndex] , txDMAPerSCDMAConfigs.burstLimiterSCDMA,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txDMA[0] , txDMA[currentUnitIndex] , txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txDMA[0] , txDMA[currentUnitIndex] , txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txDMA[0] , txDMA[currentUnitIndex] , txDMAPerSCDMAConfigs.SCDMAConfigs,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txDMA[0] , txDMA[currentUnitIndex] , txDMAPerSCDMAConfigs.SCDMAMetalFixReg,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txDMA[0] , txDMA[currentUnitIndex] , txDMADebug.informativeDebug.SCDMAStatusReg1,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txDMA[0] , txDMA[currentUnitIndex] , txDMADebug.informativeDebug.SCDMAStatusReg2,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txDMA[0] , txDMA[currentUnitIndex] , txDMADebug.informativeDebug.SCDMAPrefFIFOFillLevel,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txDMA[0] , txDMA[currentUnitIndex] , txDMADebug.informativeDebug.SCDMAPrefFIFOMaxFillLevel,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

}

/*******************************************************************************
* txfifoInit
*
* DESCRIPTION:
*       init the DB - TXFIFO unit (per unit index)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       index   - unit index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void txfifoInit
(
    IN  GT_U8 devNum,
    IN  GT_U32  index
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  lastTxFifoIndex = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E) ? 12 : 73;

    {/*start of unit txFIFO */
        {/*start of unit txFIFOShiftersConfig */
            {/*00000600+0x4*p*/
                GT_U32    p;
                for(p = 0 ; p <= lastTxFifoIndex ; p++) {
                    regsAddrPtr->txFIFO[index].txFIFOShiftersConfig.SCDMAShiftersConf[p] =
                        0x00000600+0x4*p;
                }/* end of loop p */
            }/*00000600+0x4*p*/

        }/*end of unit txFIFOShiftersConfig */


        {/*start of unit txFIFOLinkedLists */
            {/*start of unit payloadLinkedList */
                {/*start of unit payloadLinkListRAM */
                    regsAddrPtr->txFIFO[index].txFIFOLinkedLists.payloadLinkedList.payloadLinkListRAM.payloadLinkListAddr = 0x00024000;

                }/*end of unit payloadLinkListRAM */


            }/*end of unit payloadLinkedList */


            {/*start of unit headerLinkedList */
                {/*start of unit headerLinkListRAM */
                    regsAddrPtr->txFIFO[index].txFIFOLinkedLists.headerLinkedList.headerLinkListRAM.headerLinkListAddr = 0x00014000;

                }/*end of unit headerLinkListRAM */


            }/*end of unit headerLinkedList */


        }/*end of unit txFIFOLinkedLists */


        {/*start of unit txFIFOInterrupts */
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.txFIFOGeneralCauseReg1 = 0x00000400;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.txFIFOGeneralMaskReg1 = 0x00000404;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptCauseReg1 = 0x00000408;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptMaskReg1 = 0x0000040c;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptCauseReg2 = 0x00000410;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptMaskReg2 = 0x00000414;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptCauseReg3 = 0x00000418;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptMaskReg3 = 0x0000041c;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.CTByteCountArrivedLateInterruptCauseReg1 = 0x00000420;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.CTByteCountArrivedLateInterruptMaskReg1 = 0x00000424;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.CTByteCountArrivedLateInterruptCauseReg2 = 0x00000428;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.CTByteCountArrivedLateInterruptMaskReg2 = 0x0000042c;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.CTByteCountArrivedLateInterruptCauseReg3 = 0x00000430;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.CTByteCountArrivedLateInterruptMaskReg3 = 0x00000434;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.txFIFOSummaryCauseReg = 0x00000440;
            regsAddrPtr->txFIFO[index].txFIFOInterrupts.txFIFOSummaryMaskReg = 0x00000444;

        }/*end of unit txFIFOInterrupts */


        {/*start of unit txFIFOGlobalConfig */
            {/*00000008+p*0x4*/
                GT_U32    p;
                for(p = 0 ; p <= lastTxFifoIndex ; p++) {
                    regsAddrPtr->txFIFO[index].txFIFOGlobalConfig.SCDMAPayloadThreshold[p] =
                        0x00000008+p*0x4;
                }/* end of loop p */
            }/*00000008+p*0x4*/
            regsAddrPtr->txFIFO[index].txFIFOGlobalConfig.txFIFOMetalFixReg = 0x00000004;

        }/*end of unit txFIFOGlobalConfig */


        {/*start of unit txFIFOEnd2EndECCConfigs */
            {/*00000508+0x4*i*/
                GT_U32    i;
                for(i = 0 ; i <= 1 ; i++) {
                    regsAddrPtr->txFIFO[index].txFIFOEnd2EndECCConfigs.dataECCConfig[i] =
                        0x00000508+0x4*i;
                }/* end of loop i */
            }/*00000508+0x4*i*/
            {/*00000518+0x4*i*/
                GT_U32    i;
                for(i = 0 ; i <= 1 ; i++) {
                    regsAddrPtr->txFIFO[index].txFIFOEnd2EndECCConfigs.dataECCStatus[i] =
                        0x00000518+0x4*i;
                }/* end of loop i */
            }/*00000518+0x4*i*/

        }/*end of unit txFIFOEnd2EndECCConfigs */


        {/*start of unit txFIFODebug */
            {/*start of unit informativeDebug */
                {/*00001b00+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= lastTxFifoIndex ; p++) {
                        regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.SCDMAStatusReg6[p] =
                            0x00001b00+p*0x4;
                    }/* end of loop p */
                }/*00001b00+p*0x4*/
                {/*00001100+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= lastTxFifoIndex ; p++) {
                        regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.SCDMAStatusReg1[p] =
                            0x00001100+p*0x4;
                    }/* end of loop p */
                }/*00001100+p*0x4*/
                {/*00001300+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= lastTxFifoIndex ; p++) {
                        regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.SCDMAStatusReg2[p] =
                            0x00001300+p*0x4;
                    }/* end of loop p */
                }/*00001300+p*0x4*/
                {/*00001500+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= lastTxFifoIndex ; p++) {
                        regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.SCDMAStatusReg3[p] =
                            0x00001500+p*0x4;
                    }/* end of loop p */
                }/*00001500+p*0x4*/
                {/*00001700+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= lastTxFifoIndex ; p++) {
                        regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.SCDMAStatusReg4[p] =
                            0x00001700+p*0x4;
                    }/* end of loop p */
                }/*00001700+p*0x4*/
                {/*00001900+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= lastTxFifoIndex ; p++) {
                        regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.SCDMAStatusReg5[p] =
                            0x00001900+p*0x4;
                    }/* end of loop p */
                }/*00001900+p*0x4*/
                regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.debugBusCtrlReg = 0x00001000;
                regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.txFIFODebugBus = 0x00001004;
                regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.LLEOPCntrLsb = 0x0000100c;
                regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.incomingHeadersCntrLsb = 0x00001008;
                regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.headerIDDBFreeIDFIFOFillLevel = 0x00001010;
                regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.payloadIDDBFreeIDFIFOFillLevel = 0x00001014;
                regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.CTBCIDDBFreeIDFIFOFillLevel = 0x00001018;

            }/*end of unit informativeDebug */


        }/*end of unit txFIFODebug */


        {/*start of unit txFIFOPizzaArb */
            {/*start of unit pizzaArbiter */
                {/*00000808+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 84 ; n++) {
                        regsAddrPtr->txFIFO[index].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterConfigReg[n] =
                            0x00000808+n*0x4;
                    }/* end of loop n */
                }/*00000808+n*0x4*/
                regsAddrPtr->txFIFO[index].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterCtrlReg = 0x00000800;
                regsAddrPtr->txFIFO[index].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterStatusReg = 0x00000804;

            }/*end of unit pizzaArbiter */


        }/*end of unit txFIFOPizzaArb */

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.LLEOPCntrLsb = 0x00001010;
            regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.LLEOPCntrMsb = 0x00001014;
            regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.headerIDDBFreeIDFIFOFillLevel = 0x00001018;
            regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.incomingHeadersCntrMsb = 0x0000100c;
            regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.payloadIDDBFreeIDFIFOFillLevel = 0x0000101c;
            regsAddrPtr->txFIFO[index].txFIFODebug.informativeDebug.CTBCIDDBFreeIDFIFOFillLevel = 0x00001020;
        }

    }/*end of unit txFIFO */

    if(index == 0 && (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E))
    {
        /* copy last RxDma to cpu port position */
        COPY_REG_ADDR_MAC(regsAddrPtr->txFIFO[0].txFIFOShiftersConfig.SCDMAShiftersConf       , lastTxFifoIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txFIFO[0].txFIFOGlobalConfig.SCDMAPayloadThreshold     , lastTxFifoIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txFIFO[0].txFIFODebug.informativeDebug.SCDMAStatusReg1 , lastTxFifoIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txFIFO[0].txFIFODebug.informativeDebug.SCDMAStatusReg2 , lastTxFifoIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txFIFO[0].txFIFODebug.informativeDebug.SCDMAStatusReg3 , lastTxFifoIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txFIFO[0].txFIFODebug.informativeDebug.SCDMAStatusReg4 , lastTxFifoIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txFIFO[0].txFIFODebug.informativeDebug.SCDMAStatusReg5 , lastTxFifoIndex, CPU_PORT_DMA_CNS);
        COPY_REG_ADDR_MAC(regsAddrPtr->txFIFO[0].txFIFODebug.informativeDebug.SCDMAStatusReg6 , lastTxFifoIndex, CPU_PORT_DMA_CNS);
    }

}

/*******************************************************************************
* txfifoUpdateUnit0AsGlobalPortsAddr
*
* DESCRIPTION:
*       update the DB - txFIFO unit - after multi units set 'per port' addresses.
*       the DB of txFIFO[0] updated for the ports with 'global port index'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       currentUnitIndex - the index of current txFIFO[] unit
*       portIn_unit_0     - the 'global port' index (in unit 0)
*       portInCurrentUnit - the 'local port'  index (in current unit)
*       numOfPorts - number of ports to update.
*       isFirstUpdate - indication of first update
*       isLastUpdate - indication of last update
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void txfifoUpdateUnit0AsGlobalPortsAddr
(
    IN  GT_U8 devNum,
    IN  GT_U32  currentUnitIndex,
    IN  GT_U32  portIn_unit_0,
    IN  GT_U32  portInCurrentUnit,
    IN  GT_U32  numOfPorts,
    IN  GT_BOOL isFirstUpdate,
    IN  GT_BOOL isLastUpdate
)
{
    IN GT_U32    saveLastNumOfPorts;/*number of ports from the end of the unitStart1Ptr[]
                                  that need to be saved before overridden by unitStart2Ptr[]*/
    IN GT_U32    saveToIndex;/*the index in unitStart1Ptr[] to save the 'last ports'
                                  valid when saveLastNumOfPorts != 0*/
    const GT_U32 *localDmaMapArr; /* local DMAs in specific DP, NULL in cases 1:1 mapping */

    localDmaMapArr = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[currentUnitIndex].localDmaMapArr;

    if(isLastUpdate == GT_TRUE &&
       PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.supportRelativePortNum)
    {
        /* do not let the DB be overridden with the CPU port of last DP */
        /* because we saved the CPU port of the first DP */
        numOfPorts -=
            (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[0].cpuPortDmaNum ? 1 : 0)+
            (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[0].tmDmaNum ? 1 : 0);
    }

    if(isFirstUpdate == GT_TRUE &&
       PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.supportRelativePortNum)
    {
        /* update the DB with the CPU port address on the lowest existing DP */
        /* save CPU port */
        saveLastNumOfPorts =
            (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[0].cpuPortDmaNum ? 1 : 0)+
            (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[0].tmDmaNum ? 1 : 0);

        saveToIndex = CPU_PORT_DMA_CNS;
    }
    else
    {
        if((localDmaMapArr != NULL) && (isLastUpdate == GT_TRUE))
        {
            /* move CPU port registers addresses to position 72 */
            saveLastNumOfPorts = 1;
            saveToIndex = CPU_PORT_DMA_CNS;
        }
        else
        {
            saveLastNumOfPorts = 0;
            saveToIndex = 0;
        }
    }
    /************************************/
    /*   update the per port addresses  */
    /*   of the txFIFO unit 0 with those */
    /*   of unit 1                      */
    /************************************/
    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txFIFO[0] , txFIFO[currentUnitIndex] , txFIFOShiftersConfig.SCDMAShiftersConf,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );


    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txFIFO[0] , txFIFO[currentUnitIndex] , txFIFOGlobalConfig.SCDMAPayloadThreshold,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txFIFO[0] , txFIFO[currentUnitIndex] , txFIFODebug.informativeDebug.SCDMAStatusReg1,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txFIFO[0] , txFIFO[currentUnitIndex] , txFIFODebug.informativeDebug.SCDMAStatusReg2,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txFIFO[0] , txFIFO[currentUnitIndex] , txFIFODebug.informativeDebug.SCDMAStatusReg3,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txFIFO[0] , txFIFO[currentUnitIndex] , txFIFODebug.informativeDebug.SCDMAStatusReg4,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txFIFO[0] , txFIFO[currentUnitIndex] , txFIFODebug.informativeDebug.SCDMAStatusReg5,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, txFIFO[0] , txFIFO[currentUnitIndex] , txFIFODebug.informativeDebug.SCDMAStatusReg6,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex,localDmaMapArr)
        );

}

static PRV_CPSS_DXCH_UNIT_ENT rxDmaUnits[MAX_DP_CNS] =
{
     PRV_CPSS_DXCH_UNIT_RXDMA_E
    ,PRV_CPSS_DXCH_UNIT_RXDMA1_E
    ,PRV_CPSS_DXCH_UNIT_RXDMA2_E
    ,PRV_CPSS_DXCH_UNIT_RXDMA3_E
    ,PRV_CPSS_DXCH_UNIT_RXDMA4_E
    ,PRV_CPSS_DXCH_UNIT_RXDMA5_E
};

static PRV_CPSS_DXCH_UNIT_ENT txDmaUnits[MAX_DP_CNS] =
{
     PRV_CPSS_DXCH_UNIT_TXDMA_E
    ,PRV_CPSS_DXCH_UNIT_TXDMA1_E
    ,PRV_CPSS_DXCH_UNIT_TXDMA2_E
    ,PRV_CPSS_DXCH_UNIT_TXDMA3_E
    ,PRV_CPSS_DXCH_UNIT_TXDMA4_E
    ,PRV_CPSS_DXCH_UNIT_TXDMA5_E
};

static PRV_CPSS_DXCH_UNIT_ENT txFifoUnits[MAX_DP_CNS] =
{
     PRV_CPSS_DXCH_UNIT_TX_FIFO_E
    ,PRV_CPSS_DXCH_UNIT_TX_FIFO1_E
    ,PRV_CPSS_DXCH_UNIT_TX_FIFO2_E
    ,PRV_CPSS_DXCH_UNIT_TX_FIFO3_E
    ,PRV_CPSS_DXCH_UNIT_TX_FIFO4_E
    ,PRV_CPSS_DXCH_UNIT_TX_FIFO5_E
};
static PRV_CPSS_DXCH_UNIT_ENT txqDqUnits[MAX_DP_CNS] =
{
     PRV_CPSS_DXCH_UNIT_TXQ_DQ_E
    ,PRV_CPSS_DXCH_UNIT_TXQ_DQ1_E
    ,PRV_CPSS_DXCH_UNIT_TXQ_DQ2_E
    ,PRV_CPSS_DXCH_UNIT_TXQ_DQ3_E
    ,PRV_CPSS_DXCH_UNIT_TXQ_DQ4_E
    ,PRV_CPSS_DXCH_UNIT_TXQ_DQ5_E
};




/*******************************************************************************
* dataPathInit
*
* DESCRIPTION:
*       init for DATA PATH . init for RxDma,txDma,txFifo,txq-DQ.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       index   - unit index
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void dataPathInit
(
    IN  GT_U8 devNum,
    IN  GT_U32 * unitBasePtr
)
{
    GT_U32  ii,jj;
    GT_U32  supportMultiDataPath = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.supportMultiDataPath;
    GT_U32  validDataPathBmp;/* bmp of existing DPs (data path)*/
    GT_U32  portIn_unit_0;/* current port in unit 0 (global port) */
    GT_U32  port_inUnit_ii;/* current port in unit ii (local port) */
    GT_U32  numOfPorts;/* number of ports in the unit ii*/
    GT_U32  numOfRxSdmaPorts;/* number of RX SDMA ports in the unit ii*/
    GT_BOOL isFirstUpdate; /* indication of first update */
    GT_BOOL isLastUpdate;  /* indication of last update  */

    if(supportMultiDataPath)
    {
        validDataPathBmp = 0;
        for(ii = 0 ; ii < PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.maxDp; ii++)
        {
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[ii].dataPathNumOfPorts == 0)
            {
                /* not valid DP (data path) */
                continue;
            }

            validDataPathBmp |= (BIT_0 << ii);
        }
    }
    else
    {
        validDataPathBmp = BIT_0;
    }

    isFirstUpdate = GT_TRUE;
    isLastUpdate = GT_FALSE;
    ii = 0;
    for(jj = 0 ; jj < MAX_DP_CNS ; jj++)
    {
        if (0 == (validDataPathBmp & (1<<jj)))
        {
            continue;
        }

        if(validDataPathBmp < (GT_U32)(1 << (jj+1)))
        {
            isLastUpdate = GT_TRUE;
        }

        /* NOTE: the first time we get here ii is 0 ,
           but jj can be more than 0 !!!

           we initialize the rxDMA[0] with the register values , because we want
           the CPSS to use rxDMA[0] and not be aware to multi-units.
        */

        /****************/
        /* init unit ii */
        /****************/
        rxdmaInit(devNum,ii);
        /* set the addresses of the rxdma unit registers according to the needed base address */
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[rxDmaUnits[jj]], rxDMA[ii]));

        txdmaInit(devNum,ii);
        /* set the addresses of the txdma unit registers according to the needed base address */
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[txDmaUnits[jj]], txDMA[ii]));

        txfifoInit(devNum,ii);
        /* set the addresses of the txfifo unit registers according to the needed base address */
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[txFifoUnits[jj]], txFIFO[ii]));

        if((ii != 0) ||
           (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[ii].localDmaMapArr != NULL))/* have multi data path , set 'global' ports */
        {
            portIn_unit_0 = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[ii].dataPathFirstPort;
            numOfPorts    = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.info[ii].dataPathNumOfPorts;

            numOfRxSdmaPorts = numOfPorts;
            if(isLastUpdate &&
               (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.rxNumPortsPerDp < PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.txNumPortsPerDp))
            {
                /* RX SDMA does not have TM port that is counted in dataPathNumOfPorts.
                   Number of ports of last RX SDMA unit must be corrected. */
                --numOfRxSdmaPorts;
            }

            if(0 == PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.supportRelativePortNum)
            {
                /* all the Data path units hold 'global ports' */
                port_inUnit_ii = portIn_unit_0; /* global port */
            }
            else
            {
                /* all the Data path units hold 'local ports' */
                port_inUnit_ii = 0;/* local port */
            }

            /* update the DB - RXDMA unit - set 'global' ports 'per port' addresses. */
            rxdmaUpdateUnit0AsGlobalPortsAddr(devNum,ii,portIn_unit_0,port_inUnit_ii,numOfRxSdmaPorts,
                isFirstUpdate,isLastUpdate);

            /* update the DB - TXDMA unit - set 'global' ports 'per port' addresses. */
            txdmaUpdateUnit0AsGlobalPortsAddr(devNum,ii,portIn_unit_0,port_inUnit_ii,numOfPorts,
                isFirstUpdate,isLastUpdate);

            /* update the DB - txfifo unit - set 'global' ports 'per port' addresses. */
            txfifoUpdateUnit0AsGlobalPortsAddr(devNum,ii,portIn_unit_0,port_inUnit_ii,numOfPorts,
                isFirstUpdate,isLastUpdate);

            if(isFirstUpdate == GT_TRUE)
            {
                isFirstUpdate = GT_FALSE;
            }
        }


        ii = jj + 1;/* after the first time ii set to be like jj
                      (the jj will get ++ in the next operation) */
    }

    if (PRV_CPSS_DXCH_CETUS_CHECK_MAC(devNum))
    {
                PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->rxDMA[1] = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->rxDMA[0];
                PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->txDMA[1] = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->txDMA[0];
                PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->txFIFO[1] = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->txFIFO[0];
    }

    /* destroy the rxdma legacy registers */
    rxdmaLegacyDestroy(devNum);

    if(supportMultiDataPath)
    {
        validDataPathBmp = (BIT_0 << PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.numTxqDq) - 1;
    }
    else
    {
        validDataPathBmp = BIT_0;
    }

    isFirstUpdate = GT_TRUE;
    isLastUpdate = GT_FALSE;
    for(ii = 0 ; ii < MAX_DP_CNS ; ii++)
    {
        if (0 == (validDataPathBmp & (1<<ii)))
        {
            continue;
        }

        /* init the TXQ_DQ unit*/
        txqDqInit(devNum,ii);
        /* set the addresses of the TXQ_DQ unit registers according to the needed base address */
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[txqDqUnits[ii]], TXQ.dq[ii]));

        if(validDataPathBmp == BIT_0)
        {
            break;
        }

        /* update the DB - txq-DQ unit - set 'global' ports 'per port' addresses. */
        numOfPorts    = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.txqDqNumPortsPerDp;
        portIn_unit_0 = ii*numOfPorts;
        port_inUnit_ii = 0;
        txqDqUpdateUnit0AsGlobalPortsAddr(devNum,ii,portIn_unit_0,port_inUnit_ii,numOfPorts);
    }

    /* destroy the TXQ_DQ legacy registers */
    txqDqLegacyDestroy(devNum);

}


/*******************************************************************************
* ethTxFifoInit
*
* DESCRIPTION:
*       init the DB - ethTxFifo unit
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void ethTxFifoInit
(
    IN  GT_U8 devNum,
    IN  GT_U32  idx
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);


    {/*start of unit SIP_ETH_TXFIFO */
        {/*start of unit txFIFOShiftersConfig */
            {/*00000600+0x4*p*/
                GT_U32    p;
                for(p = 0 ; p <= 72 ; p++) {
                    regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOShiftersConfig.SCDMAShiftersConf[p] =
                        0x00000600+0x4*p;
                }/* end of loop p */
            }/*00000600+0x4*p*/

        }/*end of unit txFIFOShiftersConfig */


        {/*start of unit txFIFOPizzaArb */
            {/*start of unit pizzaArbiter */
                {/*00000808+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 84 ; n++) {
                        regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterConfigReg[n] =
                            0x00000808+n*0x4;
                    }/* end of loop n */
                }/*00000808+n*0x4*/
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterCtrlReg = 0x00000800;
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterStatusReg = 0x00000804;

            }/*end of unit pizzaArbiter */


        }/*end of unit txFIFOPizzaArb */


        {/*start of unit txFIFOLinkedLists */
            {/*start of unit payloadLinkedList */
                {/*start of unit payloadLinkListRAM */
                    regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOLinkedLists.payloadLinkedList.payloadLinkListRAM.payloadLinkListAddr = 0x00054000;

                }/*end of unit payloadLinkListRAM */


            }/*end of unit payloadLinkedList */


            {/*start of unit headerLinkedList */
                {/*start of unit headerLinkListRAM */
                    regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOLinkedLists.headerLinkedList.headerLinkListRAM.headerLinkListAddr = 0x00022000;

                }/*end of unit headerLinkListRAM */


            }/*end of unit headerLinkedList */


        }/*end of unit txFIFOLinkedLists */


        {/*start of unit txFIFOInterrupts */
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.txFIFOGeneralCauseReg1 = 0x00000400;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.txFIFOGeneralMaskReg1 = 0x00000404;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptCauseReg1 = 0x00000408;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptMaskReg1 = 0x0000040c;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptCauseReg2 = 0x00000410;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptMaskReg2 = 0x00000414;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptCauseReg3 = 0x00000418;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.shifterSOPEOPFIFONotReadyInterruptMaskReg3 = 0x0000041c;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.CTByteCountArrivedLateInterruptCauseReg1 = 0x00000420;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.CTByteCountArrivedLateInterruptMaskReg1 = 0x00000424;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.CTByteCountArrivedLateInterruptCauseReg2 = 0x00000428;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.CTByteCountArrivedLateInterruptMaskReg2 = 0x0000042c;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.CTByteCountArrivedLateInterruptCauseReg3 = 0x00000430;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.CTByteCountArrivedLateInterruptMaskReg3 = 0x00000434;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.txFIFOSummaryCauseReg = 0x00000440;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOInterrupts.txFIFOSummaryMaskReg = 0x00000444;

        }/*end of unit txFIFOInterrupts */


        {/*start of unit txFIFOGlobalConfig */
            {/*00000008+p*0x4*/
                GT_U32    p;
                for(p = 0 ; p <= 72 ; p++) {
                    regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOGlobalConfig.SCDMAPayloadThreshold[p] =
                        0x00000008+p*0x4;
                }/* end of loop p */
            }/*00000008+p*0x4*/
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOGlobalConfig.txFIFOGlobalConfigs = 0x00000000;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOGlobalConfig.TXFIFOMetalFixReg = 0x00000004;

        }/*end of unit txFIFOGlobalConfig */


        {/*start of unit txFIFOEnd2EndECCConfigs */
            {/*00000518+0x4*i*/
                GT_U32    i;
                for(i = 0 ; i <= 0 ; i++) {
                    regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOEnd2EndECCConfigs.dataECCStatus[i] =
                        0x00000518+0x4*i;
                }/* end of loop i */
            }/*00000518+0x4*i*/
            {/*00000508+0x4*i*/
                GT_U32    i;
                for(i = 0 ; i <= 0 ; i++) {
                    regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOEnd2EndECCConfigs.dataECCConfig[i] =
                        0x00000508+0x4*i;
                }/* end of loop i */
            }/*00000508+0x4*i*/
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOEnd2EndECCConfigs.eccErrorCntr = 0x00000528;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOEnd2EndECCConfigs.ECCFailedAddr = 0x00000538;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOEnd2EndECCConfigs.eccErrorType = 0x00000548;
            regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFOEnd2EndECCConfigs.lockErronousEvent = PRV_CPSS_SW_PTR_ENTRY_UNUSED/*0x00000552 --> Errata for bad address */;

        }/*end of unit txFIFOEnd2EndECCConfigs */


        {/*start of unit txFIFODebug */
            {/*start of unit informativeDebug */
                {/*00001b00+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= 72 ; p++) {
                        regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.SCDMAStatusReg6[p] =
                            0x00001b00+p*0x4;
                    }/* end of loop p */
                }/*00001b00+p*0x4*/
                {/*00001100+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= 72 ; p++) {
                        regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.SCDMAStatusReg1[p] =
                            0x00001100+p*0x4;
                    }/* end of loop p */
                }/*00001100+p*0x4*/
                {/*00001900+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= 72 ; p++) {
                        regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.SCDMAStatusReg5[p] =
                            0x00001900+p*0x4;
                    }/* end of loop p */
                }/*00001900+p*0x4*/
                {/*00001700+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= 72 ; p++) {
                        regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.SCDMAStatusReg4[p] =
                            0x00001700+p*0x4;
                    }/* end of loop p */
                }/*00001700+p*0x4*/
                {/*00001500+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= 72 ; p++) {
                        regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.SCDMAStatusReg3[p] =
                            0x00001500+p*0x4;
                    }/* end of loop p */
                }/*00001500+p*0x4*/
                {/*00001300+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= 72 ; p++) {
                        regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.SCDMAStatusReg2[p] =
                            0x00001300+p*0x4;
                    }/* end of loop p */
                }/*00001300+p*0x4*/
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.debugBusCtrlReg = 0x00001000;
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.txFIFODebugBus = 0x00001004;
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.LLEOPCntr = 0x0000100c;
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.incomingHeadersCntr = 0x00001008;
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.headerIDDBFreeIDFIFOFillLevel = 0x00001010;
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.payloadIDDBFreeIDFIFOFillLevel = 0x00001014;
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.informativeDebug.CTBCIDDBFreeIDFIFOFillLevel = 0x00001018;

            }/*end of unit informativeDebug */


            {/*start of unit activeDebug */
                {/*00003100+0x4*p*/
                    GT_U32    p;
                    for(p = 0 ; p <= 72 ; p++) {
                        regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.activeDebug.debugSCDMA[p] =
                            0x00003100+0x4*p;
                    }/* end of loop p */
                }/*00003100+0x4*p*/
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.activeDebug.globalActiveDebugReg = 0x00003000;
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.activeDebug.globalActiveDebugConfReg = 0x00003004;
                regsAddrPtr->SIP_ETH_TXFIFO[idx].txFIFODebug.activeDebug.debugSCDMAConfReg = 0x00003300;

            }/*end of unit activeDebug */


        }/*end of unit txFIFODebug */


    }/*end of unit SIP_ETH_TXFIFO */
}


/*******************************************************************************
* ilknTxFifoInit
*
* DESCRIPTION:
*       init the DB - ilkn Tx Fifo unit
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void ilknTxFifoInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*start of unit SIP_ILKN_TXFIFO */
        {/*start of unit ilknTxFIFOInterrupts */
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.ilknTxfifoGeneralInterruptCause = 0x00000020;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.ilknTxfifoGeneralInterruptMask = 0x00000030;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDataMemFullIlknTxfifoInterruptCauseReg1 = 0x00000040;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDataMemFullIlknTxfifoInterruptMaskReg1 = 0x00000044;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDescMemFullIlknTxfifoInterruptCauseReg1 = 0x00000048;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDescMemFullIlknTxfifoInterruptMaskReg1 = 0x0000004c;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDataMemFullIlknTxfifoInterruptCauseReg2 = 0x00000050;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDataMemFullIlknTxfifoInterruptMaskReg2 = 0x00000054;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDescMemFullIlknTxfifoInterruptCauseReg2 = 0x00000058;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDescMemFullIlknTxfifoInterruptMaskReg2 = 0x0000005c;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDataMemFullIlknTxfifoInterruptCauseReg3 = 0x00000060;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDataMemFullIlknTxfifoInterruptMaskReg3 = 0x00000064;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDescMemFullIlknTxfifoInterruptCauseReg3 = 0x00000068;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.streamIDDescMemFullIlknTxfifoInterruptMaskReg3 = 0x0000006c;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.ilknTxfifoUnderrunInterruptCauseReg1 = 0x00000070;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.ilknTxfifoUnderrunInterruptMaskReg1 = 0x00000074;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.ilknTxfifoUnderrunInterruptCauseReg2 = 0x00000078;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.ilknTxfifoUnderrunInterruptMaskReg2 = 0x0000007c;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.ilknTxfifoUnderrunInterruptCauseReg3 = 0x00000080;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.ilknTxfifoUnderrunInterruptMaskReg3 = 0x00000084;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.ilknTxfifoSummaryCauseReg = 0x000000a0;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOInterrupts.ilknTxfifoSummaryMaskReg = 0x000000a4;

        }/*end of unit ilknTxFIFOInterrupts */


        {/*start of unit ilknTxFIFOGlobalConfig */
            {/*00000500+p*0x4*/
                GT_U32    p;
                for(p = 0 ; p <= 63 ; p++) {
                    regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOGlobalConfig.portDynamicMemoryAlocation[p] =
                        0x00000500+p*0x4;
                }/* end of loop p */
            }/*00000500+p*0x4*/
            {/*00000200+p*0x4*/
                GT_U32    p;
                for(p = 0 ; p <= 63 ; p++) {
                    regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOGlobalConfig.portTransmittingThreshold[p] =
                        0x00000200+p*0x4;
                }/* end of loop p */
            }/*00000200+p*0x4*/
            {/*00000360+p*0x4*/
                GT_U32    p;
                for(p = 0 ; p <= 63 ; p++) {
                    regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOGlobalConfig.portChannelEnable[p] =
                        0x00000360+p*0x4;
                }/* end of loop p */
            }/*00000360+p*0x4*/
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOGlobalConfig.globalConfig = 0x00000350;

        }/*end of unit ilknTxFIFOGlobalConfig */


        {/*start of unit ilknTxFIFODebug */
            {/*00006d00+p*0x4*/
                GT_U32    p;
                for(p = 0 ; p <= 63 ; p++) {
                    regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFODebug.dataFifoFillLevelForPort[p] =
                        0x00006d00+p*0x4;
                }/* end of loop p */
            }/*00006d00+p*0x4*/

        }/*end of unit ilknTxFIFODebug */


        {/*start of unit ilknTxFIFOArbiter */
            {/*00001510+s*0x4*/
                GT_U32    s;
                for(s = 0 ; s <= 1063 ; s++) {
                    regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOArbiter.pizzaSlicePortMap[s] =
                        0x00001510+s*0x4;
                }/* end of loop s */
            }/*00001510+s*0x4*/
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOArbiter.pizzaArbiterCtrlReg = 0x00001500;
            regsAddrPtr->SIP_ILKN_TXFIFO.ilknTxFIFOArbiter.pizzaArbiterStatusReg = 0x00001504;

        }/*end of unit ilknTxFIFOArbiter */


    }/*end of unit SIP_ILKN_TXFIFO */

}




/*******************************************************************************
* oamInit
*
* DESCRIPTION:
*       init the DB - OAM unit - ingress or egress
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       oamUnitIndex - PRV_CPSS_DXCH_UNIT_IOAM_E / PRV_CPSS_DXCH_UNIT_EOAM_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void oamInit
(
    IN  GT_U8 devNum,
    IN PRV_CPSS_DXCH_UNIT_ENT oamUnitIndex

)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  index =  (oamUnitIndex == PRV_CPSS_DXCH_UNIT_IOAM_E) ? 0 : 1;
    GT_U32  isSip5_15 = PRV_CPSS_SIP_5_15_CHECK_MAC(devNum);

    {/*start of unit OAMUnit */
        regsAddrPtr->OAMUnit[index].OAMGlobalCtrl = 0x00000000;
        {/*e0+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 1 ; n++) {
                regsAddrPtr->OAMUnit[index].agingEntryGroupStatus[n] = isSip5_15 ? 0x900+4*n :
                    0xe0+4*n;
            }/* end of loop n */
        }/*e0+4*n*/
        {/*2c0+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 1 ; n++) {
                regsAddrPtr->OAMUnit[index].summaryExceptionGroupStatus[n] = isSip5_15 ? 0x800+4*n :
                    0x2c0+4*n;
            }/* end of loop n */
        }/*2c0+4*n*/
        {/*2a0+n*4*/
            GT_U32    n;
            for(n = 0 ; n <= 1 ; n++) {
                regsAddrPtr->OAMUnit[index].txPeriodExceptionGroupStatus[n] = isSip5_15 ? 0x700+4*n :
                    0x2a0+n*4;
            }/* end of loop n */
        }/*2a0+n*4*/
        regsAddrPtr->OAMUnit[index].sourceInterfaceMismatchCntr = isSip5_15 ? 0x00000a04 : 0x00000304;
        regsAddrPtr->OAMUnit[index].OAMGlobalCtrl1 = 0x00000004;
        {/*280+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 1 ; n++) {
                regsAddrPtr->OAMUnit[index].RDIStatusChangeExceptionGroupStatus[n] = isSip5_15 ? 0x600+4*n :
                    0x280+4*n;
            }/* end of loop n */
        }/*280+4*n*/
        regsAddrPtr->OAMUnit[index].RDIStatusChangeCntr = isSip5_15 ? 0x00000a14 : 0x00000314;
        regsAddrPtr->OAMUnit[index].OAMUnitInterruptMask = 0x000000f4;
        regsAddrPtr->OAMUnit[index].OAMUnitInterruptCause = 0x000000f0;
        {/*d4+n*4*/
            GT_U32    n;
            for(n = 0 ; n <= 2 ; n++) {
                regsAddrPtr->OAMUnit[index].OAMTableAccessData[n] =
                    0xd4+n*4;
            }/* end of loop n */
        }/*d4+n*4*/
        regsAddrPtr->OAMUnit[index].OAMTableAccessCtrl = 0x000000d0;
        {/*30+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 3 ; n++) {
                regsAddrPtr->OAMUnit[index].OAMLossMeasurementOpcodes[n] =
                    0x30+4*n;
            }/* end of loop n */
        }/*30+4*n*/
        {/*28+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 1 ; n++) {
                regsAddrPtr->OAMUnit[index].OAMLMCountedOpcodes[n] =
                    0x28+4*n;
            }/* end of loop n */
        }/*28+4*n*/
        {/*70+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 3 ; n++) {
                regsAddrPtr->OAMUnit[index].OAMKeepAliveOpcodes[n] =
                    0x70+4*n;
            }/* end of loop n */
        }/*70+4*n*/
        regsAddrPtr->OAMUnit[index].OAMInvalidKeepaliveExceptionConfigs = 0x00000020;
        {/*94+8*n*/
            GT_U32    n;
            for(n = 0 ; n <= 7 ; n++) {
                regsAddrPtr->OAMUnit[index].agingPeriodEntryHigh[n] =
                    0x94+8*n;
            }/* end of loop n */
        }/*94+8*n*/
        regsAddrPtr->OAMUnit[index].dualEndedLM = 0x00000084;
        regsAddrPtr->OAMUnit[index].OAMExceptionConfigs2 = 0x0000001c;
        regsAddrPtr->OAMUnit[index].OAMExceptionConfigs1 = 0x00000018;
        regsAddrPtr->OAMUnit[index].OAMExceptionConfigs = 0x00000014;
        {/*50+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 3 ; n++) {
                regsAddrPtr->OAMUnit[index].OAMDelayMeasurementOpcodes[n] =
                    0x50+4*n;
            }/* end of loop n */
        }/*50+4*n*/
        {/*240+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 1 ; n++) {
                regsAddrPtr->OAMUnit[index].MEGLevelGroupStatus[n] = isSip5_15 ? 0x400+4*n :
                    0x240+4*n;
            }/* end of loop n */
        }/*240+4*n*/
        regsAddrPtr->OAMUnit[index].MEGLevelExceptionCntr = isSip5_15 ? 0x00000a00 : 0x00000300;
        {/*260+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 1 ; n++) {
                regsAddrPtr->OAMUnit[index].sourceInterfaceGroupStatus[n] = isSip5_15 ? 0x500+4*n :
                    0x260+4*n;
            }/* end of loop n */
        }/*260+4*n*/
        regsAddrPtr->OAMUnit[index].invalidKeepaliveHashCntr = isSip5_15 ? 0x00000a08 : 0x00000308;
        {/*220+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 1 ; n++) {
                regsAddrPtr->OAMUnit[index].invalidKeepaliveGroupStatus[n] = isSip5_15 ? 0x300+4*n :
                    0x220+4*n;
            }/* end of loop n */
        }/*220+4*n*/
        regsAddrPtr->OAMUnit[index].hashBitsSelection = 0x00000024;
        {/*200+4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 1 ; n++) {
                regsAddrPtr->OAMUnit[index].excessKeepaliveGroupStatus[n] =
                    0x200+4*n;
            }/* end of loop n */
        }/*200+4*n*/
        regsAddrPtr->OAMUnit[index].excessKeepaliveCntr = isSip5_15 ? 0x00000a0c : 0x0000030c;
        regsAddrPtr->OAMUnit[index].keepaliveAgingCntr = isSip5_15 ? 0x00000a10 : 0x00000310;
        {/*90+8*n*/
            GT_U32    n;
            for(n = 0 ; n <= 7 ; n++) {
                regsAddrPtr->OAMUnit[index].agingPeriodEntryLow[n] =
                    0x90+8*n;
            }/* end of loop n */
        }/*90+8*n*/
        regsAddrPtr->OAMUnit[index].txPeriodExceptionCntr = isSip5_15 ? 0x00000a18 : 0x00000318;
        regsAddrPtr->OAMUnit[index].summaryExceptionCntr = isSip5_15 ? 0x00000a1c : 0x0000031c;

    }/*end of unit OAMUnit */
}

/*******************************************************************************
* ermrkInit
*
* DESCRIPTION:
*       init the DB - ermrk unit
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void ermrkInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*start of unit ERMRK */
        regsAddrPtr->ERMRK.ERMRKGlobalConfig = 0x00000000;
        regsAddrPtr->ERMRK.ERMRKInterruptCause = 0x00000004;
        regsAddrPtr->ERMRK.ERMRKInterruptMask = 0x00000008;
        regsAddrPtr->ERMRK.ERMRKPTPConfig = 0x00000010;
        regsAddrPtr->ERMRK.timestampingUDPChecksumMode = 0x00000014;
        regsAddrPtr->ERMRK.timestampQueueMsgType = 0x00000018;
        regsAddrPtr->ERMRK.timestampQueueEntryID = 0x0000001c;
        regsAddrPtr->ERMRK.PTPNTPOffset = 0x0000010c;
        {/*40+s*4*/
            GT_U32    s;
            for(s = 0 ; s <= 11 ; s++) {
                regsAddrPtr->ERMRK.ERMRKUP2UPMapTable[s] =
                    0x40+s*4;
            }/* end of loop s */
        }/*40+s*4*/
        regsAddrPtr->ERMRK.ERMRKPTPInvalidTimestampCntr = 0x00000020;
        regsAddrPtr->ERMRK.ERMRKDP2CFITable = 0x00000038;
        {/*70+s*4*/
            GT_U32    s;
            for(s = 0 ; s <= 11 ; s++) {
                regsAddrPtr->ERMRK.ERMRKEXP2EXPMapTable[s] =
                    0x70+s*4;
            }/* end of loop s */
        }/*70+s*4*/
        {/*110+4*q*/
            GT_U32    q;
            for(q = 0 ; q <= 3 ; q++) {
                regsAddrPtr->ERMRK.timestampQueueEntryWord0[q] =
                    0x110+4*q;
            }/* end of loop q */
        }/*110+4*q*/
        {/*120+4*q*/
            GT_U32    q;
            for(q = 0 ; q <= 3 ; q++) {
                regsAddrPtr->ERMRK.timestampQueueEntryWord1[q] =
                    0x120+4*q;
            }/* end of loop q */
        }/*120+4*q*/
        {/*130+4*q*/
            GT_U32    q;
            for(q = 0 ; q <= 1 ; q++) {
                regsAddrPtr->ERMRK.timestampQueueEntryWord2[q] =
                    0x130+4*q;
            }/* end of loop q */
        }/*130+4*q*/
        regsAddrPtr->ERMRK.timestampQueueEntryIDClear = 0x000000e4;
        regsAddrPtr->ERMRK.ECCErrorInjection = 0x00000024;

    }/*end of unit ERMRK */

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        regsAddrPtr->ERMRK.ERMRKBadAddrLatch = 0x00000028;
    }
}
/*******************************************************************************
* bmInit
*
* DESCRIPTION:
*       init the DB - bm unit
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void bmInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  num_BMBufferTermination;
    GT_U32  num_rxDMANextUpdatesFIFOMaxFillLevel;
    GT_U32  num_rxDMANextUpdatesFIFOFillLevel;
    GT_U32  num_SCDMAInProgRegs;


    if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum)) /* bobk*/
    {
        num_BMBufferTermination                 = 4     +1;/*same as bc2*/
        num_rxDMANextUpdatesFIFOMaxFillLevel    = 3     +1;/*2 less than bc2*/
        num_rxDMANextUpdatesFIFOFillLevel       = 3     +1;/*2 less than bc2*/
        num_SCDMAInProgRegs                     = 147   +1;/*much more than bc2*/
    }
    else /* bobcat2 */
    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum)) /* bobcat2 B0*/
    {
        num_BMBufferTermination                 = 4     +1;/*same as bc2*/
        num_rxDMANextUpdatesFIFOMaxFillLevel    = 3     +1;/*2 less than bc2 A0*/
        num_rxDMANextUpdatesFIFOFillLevel       = 3     +1;/*2 less than bc2 A0*/
        num_SCDMAInProgRegs                     = 113   +1;
    }
    else /* bobcat2 */
    {
        num_BMBufferTermination                 = 4     +1;
        num_rxDMANextUpdatesFIFOMaxFillLevel    = 5     +1;
        num_rxDMANextUpdatesFIFOFillLevel       = 5     +1;
        num_SCDMAInProgRegs                     = 113   +1;
    }


    {/*start of unit BM */
        {/*start of unit BMInterrupts */
            regsAddrPtr->BM.BMInterrupts.BMGeneralCauseReg1 = 0x00000300;
            regsAddrPtr->BM.BMInterrupts.BMGeneralMaskReg1 = 0x00000304;
            regsAddrPtr->BM.BMInterrupts.BMGeneralCauseReg2 = 0x00000308;
            regsAddrPtr->BM.BMInterrupts.BMGeneralMaskReg2 = 0x0000030c;
            regsAddrPtr->BM.BMInterrupts.BMSummaryCauseReg = 0x00000310;
            regsAddrPtr->BM.BMInterrupts.BMSummaryMaskReg = 0x00000314;

        }/*end of unit BMInterrupts */


        {/*start of unit BMGlobalConfigs */
            {/*00000050+0x4*i*/
                GT_U32    i;
                for(i = 0 ; i < num_BMBufferTermination ; i++) {
                    regsAddrPtr->BM.BMGlobalConfigs.BMBufferTermination[i] =
                        0x00000050+0x4*i;
                }/* end of loop i */
            }/*00000050+0x4*i*/
            regsAddrPtr->BM.BMGlobalConfigs.BMGlobalConfig1 = 0x00000000;
            regsAddrPtr->BM.BMGlobalConfigs.BMGlobalConfig2 = 0x00000004;
            regsAddrPtr->BM.BMGlobalConfigs.BMPauseConfig = 0x00000010;
            regsAddrPtr->BM.BMGlobalConfigs.BMBufferLimitConfig1 = 0x00000014;
            regsAddrPtr->BM.BMGlobalConfigs.BMBufferLimitConfig2 = 0x00000018;
            regsAddrPtr->BM.BMGlobalConfigs.BMMetalFixReg = 0x00000020;

        }/*end of unit BMGlobalConfigs */


        {/*start of unit BMDebug */
            {/*00000480+c*0x4*/
                GT_U32    c;
                for(c = 0 ; c < num_rxDMANextUpdatesFIFOMaxFillLevel ; c++) {
                    regsAddrPtr->BM.BMDebug.rxDMANextUpdatesFIFOMaxFillLevel[c] =
                        0x00000480+c*0x4;
                }/* end of loop c */
            }/*00000480+c*0x4*/
            {/*00000460+c*0x4*/
                GT_U32    c;
                for(c = 0 ; c < num_rxDMANextUpdatesFIFOFillLevel ; c++) {
                    regsAddrPtr->BM.BMDebug.rxDMANextUpdatesFIFOFillLevel[c] =
                        0x00000460+c*0x4;
                }/* end of loop c */
            }/*00000460+c*0x4*/
            {/*00000500+s*0x4*/
                GT_U32    s;
                for(s = 0 ; s < num_SCDMAInProgRegs ; s++) {
                    regsAddrPtr->BM.BMDebug.SCDMAInProgRegs[s] =
                        0x00000500+s*0x4;
                }/* end of loop s */
            }/*00000500+s*0x4*/

        }/*end of unit BMDebug */


        {/*start of unit BMCntrs */
            regsAddrPtr->BM.BMCntrs.BMGlobalBufferCntr = 0x00000200;
            regsAddrPtr->BM.BMCntrs.rxDMAsAllocationsCntr = 0x00000204;
            regsAddrPtr->BM.BMCntrs.BMFreeBuffersInsideBMCntr = 0x00000208;
            regsAddrPtr->BM.BMCntrs.BMRxDMAPendingAllocationCntr = 0x0000020c;
            regsAddrPtr->BM.BMCntrs.BMCTClearsInsideBMCntr = 0x00000210;
            regsAddrPtr->BM.BMCntrs.incomingCTClearsCntr = 0x00000214;
            regsAddrPtr->BM.BMCntrs.incomingRegularClearsCntr = 0x00000218;
            regsAddrPtr->BM.BMCntrs.nextTableAccessCntr = 0x0000021c;
            regsAddrPtr->BM.BMCntrs.nextTableWriteAccessCntr = 0x00000220;
            regsAddrPtr->BM.BMCntrs.BMTerminatedBuffersCntr = 0x00000224;
            regsAddrPtr->BM.BMCntrs.BMLostClearRequestsCntr = 0x00000228;

        }/*end of unit BMCntrs */


    }/*end of unit BM */

}


/*******************************************************************************
* gopTaiInit
*
* DESCRIPTION:
*       init the DB - TAI unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - The PP's device number to init the struct for.
*       gopIndex - index of GOP
*       taiIndex - index of TAI inside of GOP
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void gopTaiInit
(
    IN  GT_U8  devNum,
    IN  GT_U32 gopIndex,
    IN  GT_U32 taiIndex
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    /*start of unit TAI */
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].TAIInterruptCause = 0x00000000;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].TAIInterruptMask = 0x00000004;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].TAICtrlReg0 = 0x00000008;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].TAICtrlReg1 = 0x0000000c;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCntrFunctionConfig0 = 0x00000010;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCntrFunctionConfig1 = 0x00000014;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCntrFunctionConfig2 = 0x00000018;
    if(!PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
    {
        /* TODStepNanoConfigHigh deprecated starting from Caelum (BobK) */
        regsAddrPtr->GOP.TAI[gopIndex][taiIndex].TODStepNanoConfigHigh = 0x0000001c;
    }
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].TODStepNanoConfigLow = 0x00000020;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].TODStepFracConfigHigh = 0x00000024;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].TODStepFracConfigLow = 0x00000028;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeAdjustmentPropagationDelayConfigHigh = 0x0000002c;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeAdjustmentPropagationDelayConfigLow = 0x00000030;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].triggerGenerationTODSecHigh = 0x00000034;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].triggerGenerationTODSecMed = 0x00000038;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].triggerGenerationTODSecLow = 0x0000003c;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].triggerGenerationTODNanoHigh = 0x00000040;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].triggerGenerationTODNanoLow = 0x00000044;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].triggerGenerationTODFracHigh = 0x00000048;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].triggerGenerationTODFracLow = 0x0000004c;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeLoadValueSecHigh = 0x00000050;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeLoadValueSecMed = 0x00000054;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeLoadValueSecLow = 0x00000058;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeLoadValueNanoHigh = 0x0000005c;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeLoadValueNanoLow = 0x00000060;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeLoadValueFracHigh = 0x00000064;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeLoadValueFracLow = 0x00000068;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue0SecHigh = 0x0000006c;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue0SecMed = 0x00000070;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue0SecLow = 0x00000074;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue0NanoHigh = 0x00000078;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue0NanoLow = 0x0000007c;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue0FracHigh = 0x00000080;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue0FracLow = 0x00000084;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue1SecHigh = 0x00000088;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue1SecMed = 0x0000008c;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue1SecLow = 0x00000090;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue1NanoHigh = 0x00000094;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue1NanoLow = 0x00000098;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue1FracHigh = 0x0000009c;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureValue1FracLow = 0x000000a0;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeCaptureStatus = 0x000000a4;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].timeUpdateCntr = 0x000000a8;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].PClkClockCycleConfigLow = 0x000000d8;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].PClkClockCycleConfigHigh = 0x000000d4;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].incomingClockInCountingConfigLow = 0x000000f8;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].incomingClockInCountingConfigHigh = 0x000000f4;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].generateFunctionMaskSecMed = 0x000000b0;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].generateFunctionMaskSecLow = 0x000000b4;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].generateFunctionMaskSecHigh = 0x000000ac;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].generateFunctionMaskNanoLow = 0x000000bc;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].generateFunctionMaskNanoHigh = 0x000000b8;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].generateFunctionMaskFracLow = 0x000000c4;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].generateFunctionMaskFracHigh = 0x000000c0;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].externalClockPropagationDelayConfigLow = 0x000000f0;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].externalClockPropagationDelayConfigHigh = 0x000000ec;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].driftThresholdConfigLow = 0x000000e0;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].driftThresholdConfigHigh = 0x000000dc;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].driftAdjustmentConfigLow = 0x000000cc;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].driftAdjustmentConfigHigh = 0x000000c8;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].clockCycleConfigLow = 0x000000e8;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].clockCycleConfigHigh = 0x000000e4;
    regsAddrPtr->GOP.TAI[gopIndex][taiIndex].captureTriggerCntr = 0x000000d0;
}

/*******************************************************************************
* gopFcaInit
*
* DESCRIPTION:
*       init the DB - FCA unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - The PP's device number to init the struct for.
*       macNum - number of MAC unit
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void gopFcaInit
(
    IN  GT_U8  devNum,
    IN  GT_U32 macNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*0018060c+i*0x4*/
        GT_U32    i;
        for(i = 0 ; i <= 7 ; i++) {
            regsAddrPtr->GOP.FCA[macNum].LLFCDatabase[i] =
                0xc+i*0x4;
        }/* end of loop i */
    }/*0018060c+i*0x4*/
    {/*0018062c+p*0x4*/
        GT_U32    p;
        for(p = 0 ; p <= 7 ; p++) {
            regsAddrPtr->GOP.FCA[macNum].portSpeedTimer[p] =
                0x2c+p*0x4;
        }/* end of loop p */
    }/*0018062c+p*0x4*/
    {/*00180684+0x4*p*/
        GT_U32    p;
        for(p = 0 ; p <= 7 ; p++) {
            regsAddrPtr->GOP.FCA[macNum].lastSentTimer[p] =
                0x84+0x4*p;
        }/* end of loop p */
    }/*00180684+0x4*p*/
    regsAddrPtr->GOP.FCA[macNum].FCACtrl               = 0x0;
    regsAddrPtr->GOP.FCA[macNum].DBReg0                = 0x4;
    regsAddrPtr->GOP.FCA[macNum].DBReg1                = 0x8;
    regsAddrPtr->GOP.FCA[macNum].LLFCFlowCtrlWindow0   = 0x4c;
    regsAddrPtr->GOP.FCA[macNum].LLFCFlowCtrlWindow1   = 0x50;
    regsAddrPtr->GOP.FCA[macNum].MACDA0To15            = 0x54;
    regsAddrPtr->GOP.FCA[macNum].MACDA16To31           = 0x58;
    regsAddrPtr->GOP.FCA[macNum].MACDA32To47           = 0x5c;
    regsAddrPtr->GOP.FCA[macNum].MACSA0To15            = 0x60;
    regsAddrPtr->GOP.FCA[macNum].MACSA16To31           = 0x64;
    regsAddrPtr->GOP.FCA[macNum].MACSA32To47           = 0x68;
    regsAddrPtr->GOP.FCA[macNum].L2CtrlReg0            = 0x6c;
    regsAddrPtr->GOP.FCA[macNum].L2CtrlReg1            = 0x70;
    regsAddrPtr->GOP.FCA[macNum].DSATag0To15           = 0x74;
    regsAddrPtr->GOP.FCA[macNum].DSATag16To31          = 0x78;
    regsAddrPtr->GOP.FCA[macNum].DSATag32To47          = 0x7c;
    regsAddrPtr->GOP.FCA[macNum].DSATag48To63          = 0x80;
    regsAddrPtr->GOP.FCA[macNum].PFCPriorityMask       = 0x104;
    regsAddrPtr->GOP.FCA[macNum].sentPktsCntrLSb       = 0x108;
    regsAddrPtr->GOP.FCA[macNum].sentPktsCntrMSb       = 0x10c;
    regsAddrPtr->GOP.FCA[macNum].periodicCntrLSB       = 0x110;
    regsAddrPtr->GOP.FCA[macNum].periodicCntrMSB       = 0x114;

    return;
}

/*******************************************************************************
* gopPtpInit
*
* DESCRIPTION:
*       init the DB - GOP PTP unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - The PP's device number to init the struct for.
*       portNum  - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void gopPtpInit
(
    IN  GT_U8  devNum,
    IN  GT_U32 portNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    /*start of unit PTP */
    regsAddrPtr->GOP.PTP[portNum].PTPInterruptCause = 0x00000000;
    regsAddrPtr->GOP.PTP[portNum].PTPInterruptMask = 0x00000004;
    regsAddrPtr->GOP.PTP[portNum].PTPGeneralCtrl = 0x00000008;
    regsAddrPtr->GOP.PTP[portNum].PTPTXTimestampQueue0Reg0 = 0x0000000c;
    regsAddrPtr->GOP.PTP[portNum].PTPTXTimestampQueue0Reg1 = 0x00000010;
    regsAddrPtr->GOP.PTP[portNum].PTPTXTimestampQueue0Reg2 = 0x00000014;
    regsAddrPtr->GOP.PTP[portNum].PTPTXTimestampQueue1Reg0 = 0x00000018;
    regsAddrPtr->GOP.PTP[portNum].PTPTXTimestampQueue1Reg1 = 0x0000001c;
    regsAddrPtr->GOP.PTP[portNum].PTPTXTimestampQueue1Reg2 = 0x00000020;
    regsAddrPtr->GOP.PTP[portNum].totalPTPPktsCntr = 0x00000024;
    regsAddrPtr->GOP.PTP[portNum].PTPv1PktCntr = 0x00000028;
    regsAddrPtr->GOP.PTP[portNum].PTPv2PktCntr = 0x0000002c;
    regsAddrPtr->GOP.PTP[portNum].Y1731PktCntr = 0x00000030;
    regsAddrPtr->GOP.PTP[portNum].NTPTsPktCntr = 0x00000034;
    regsAddrPtr->GOP.PTP[portNum].NTPReceivePktCntr = 0x00000038;
    regsAddrPtr->GOP.PTP[portNum].NTPTransmitPktCntr = 0x0000003c;
    regsAddrPtr->GOP.PTP[portNum].WAMPPktCntr = 0x00000040;
    regsAddrPtr->GOP.PTP[portNum].addCorrectedTimeActionPktCntr = 0x00000058;
    regsAddrPtr->GOP.PTP[portNum].NTPPTPOffsetHigh = 0x00000070;
    regsAddrPtr->GOP.PTP[portNum].noneActionPktCntr = 0x00000044;
    regsAddrPtr->GOP.PTP[portNum].forwardActionPktCntr = 0x00000048;
    regsAddrPtr->GOP.PTP[portNum].dropActionPktCntr = 0x0000004c;
    regsAddrPtr->GOP.PTP[portNum].captureIngrTimeActionPktCntr = 0x0000006c;
    regsAddrPtr->GOP.PTP[portNum].captureAddTimeActionPktCntr = 0x0000005c;
    regsAddrPtr->GOP.PTP[portNum].captureAddIngrTimeActionPktCntr = 0x00000068;
    regsAddrPtr->GOP.PTP[portNum].captureAddCorrectedTimeActionPktCntr = 0x00000060;
    regsAddrPtr->GOP.PTP[portNum].captureActionPktCntr = 0x00000050;
    regsAddrPtr->GOP.PTP[portNum].addTimeActionPktCntr = 0x00000054;
    regsAddrPtr->GOP.PTP[portNum].addIngrTimeActionPktCntr = 0x00000064;
    regsAddrPtr->GOP.PTP[portNum].NTPPTPOffsetLow = 0x00000074;
    if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
    {
        regsAddrPtr->GOP.PTP[portNum].txPipeStatusDelay = 0x00000078;
    }
}

/*******************************************************************************
* gopPrInit
*
* DESCRIPTION:
*       init the DB - GOP PR unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void gopPrInit
(
    IN  GT_U8  devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr =
                                        PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*start of unit PRMIB */
        regsAddrPtr->GOP.PR.PRMIB.MIBChannelSelect = 0x00001ef8;
        regsAddrPtr->GOP.PR.PRMIB.MIBGeneralConfigs = 0x00001efc;
        regsAddrPtr->GOP.PR.PRMIB.MIBWraparoundInterruptCause0 = 0x00001f00;
        regsAddrPtr->GOP.PR.PRMIB.MIBWraparoundInterruptMask0 = 0x00001f04;
        regsAddrPtr->GOP.PR.PRMIB.MIBWraparoundInterruptCause1 = 0x00001f08;
        regsAddrPtr->GOP.PR.PRMIB.MIBWraparoundInterruptMask1 = 0x00001f0c;
        regsAddrPtr->GOP.PR.PRMIB.MIBWraparoundInterruptCause2 = 0x00001f10;
        regsAddrPtr->GOP.PR.PRMIB.MIBWraparoundInterruptMask2 = 0x00001f14;

        regsAddrPtr->GOP.PR.PRMIB.MIBCountersBase = 0x00001E00;

    }/*end of unit PRMIB */


    {/*start of unit PRInterrupts */
        regsAddrPtr->GOP.PR.PRInterrupts.rxFullInterruptCause0 = 0x00001a00;
        regsAddrPtr->GOP.PR.PRInterrupts.rxFullInterruptMask0 = 0x00001a04;
        regsAddrPtr->GOP.PR.PRInterrupts.rxFullInterruptCause1 = 0x00001a08;
        regsAddrPtr->GOP.PR.PRInterrupts.rxFullInterruptMask1 = 0x00001a0c;
        regsAddrPtr->GOP.PR.PRInterrupts.rxFullInterruptCause2 = 0x00001a10;
        regsAddrPtr->GOP.PR.PRInterrupts.rxFullInterruptMask2 = 0x00001a14;
        regsAddrPtr->GOP.PR.PRInterrupts.noSOPInterruptCause0 = 0x00001a20;
        regsAddrPtr->GOP.PR.PRInterrupts.noSOPInterruptMask0 = 0x00001a24;
        regsAddrPtr->GOP.PR.PRInterrupts.noSOPInterruptCause1 = 0x00001a28;
        regsAddrPtr->GOP.PR.PRInterrupts.noSOPInterruptMask2 = 0x00001a34;
        regsAddrPtr->GOP.PR.PRInterrupts.noSOPInterruptCause2 = 0x00001a30;
        regsAddrPtr->GOP.PR.PRInterrupts.noSOPInterruptMask1 = 0x00001a2c;
        regsAddrPtr->GOP.PR.PRInterrupts.fillOverrunInterruptCause0 = 0x00001a40;
        regsAddrPtr->GOP.PR.PRInterrupts.fillOverrunInterruptMask0 = 0x00001a44;
        regsAddrPtr->GOP.PR.PRInterrupts.fillOverrunInterruptCause1 = 0x00001a48;
        regsAddrPtr->GOP.PR.PRInterrupts.fillOverrunInterruptMask1 = 0x00001a4c;
        regsAddrPtr->GOP.PR.PRInterrupts.noEOPInterruptMask2 = 0x00001aa4;
        regsAddrPtr->GOP.PR.PRInterrupts.fillOverrunInterruptMask2 = 0x00001a54;
        regsAddrPtr->GOP.PR.PRInterrupts.noEOPInterruptMask0 = 0x00001a94;
        regsAddrPtr->GOP.PR.PRInterrupts.fillUnderrunInterruptMask0 = 0x00001a64;
        regsAddrPtr->GOP.PR.PRInterrupts.fillUnderrunInterruptCause1 = 0x00001a68;
        regsAddrPtr->GOP.PR.PRInterrupts.fillUnderrunInterruptMask1 = 0x00001a6c;
        regsAddrPtr->GOP.PR.PRInterrupts.fillUnderrunInterruptCause2 = 0x00001a70;
        regsAddrPtr->GOP.PR.PRInterrupts.fillUnderrunInterruptMask2 = 0x00001a74;
        regsAddrPtr->GOP.PR.PRInterrupts.PRGeneralCauseReg = 0x00001a80;
        regsAddrPtr->GOP.PR.PRInterrupts.PRGeneralMaskReg = 0x00001a84;
        regsAddrPtr->GOP.PR.PRInterrupts.noEOPInterruptCause0 = 0x00001a90;
        regsAddrPtr->GOP.PR.PRInterrupts.fillUnderrunInterruptCause0 = 0x00001a60;
        regsAddrPtr->GOP.PR.PRInterrupts.noEOPInterruptCause1 = 0x00001a98;
        regsAddrPtr->GOP.PR.PRInterrupts.noEOPInterruptMask1 = 0x00001a9c;
        regsAddrPtr->GOP.PR.PRInterrupts.noEOPInterruptCause2 = 0x00001aa0;
        regsAddrPtr->GOP.PR.PRInterrupts.fillOverrunInterruptCause2 = 0x00001a50;
        regsAddrPtr->GOP.PR.PRInterrupts.PRSummaryCauseReg = 0x00001b00;
        regsAddrPtr->GOP.PR.PRInterrupts.PRSummaryMaskReg = 0x00001b04;

    }/*end of unit PRInterrupts */


    {/*start of unit PRFunctional */
        {/*00001808+0x4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 63 ; n++) {
                regsAddrPtr->GOP.PR.PRFunctional.channelConfig[n] =
                    0x00001808+0x4*n;
            }/* end of loop n */
        }/*00001808+0x4*n*/
        regsAddrPtr->GOP.PR.PRFunctional.minMaxPkts = 0x00001800;
        regsAddrPtr->GOP.PR.PRFunctional.general = 0x00001804;

    }/*end of unit PRFunctional */

}

/*******************************************************************************
* gopIlknInit
*
* DESCRIPTION:
*       init the DB - GOP ILKN unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void gopIlknInit
(
    IN  GT_U8  devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr =
                                        PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/* start of unit ILKN */
        {/*1f00140+n*0x4*/
            GT_U32    n;
            for(n = 0 ; n <= 7 ; n++)
            {
                regsAddrPtr->GOP.ILKN.ILKN_COUNT_BLK_TYPE_ERR_H[n] = 0x0000140+n*0x4;
            }/* end of loop n */
        }/*1f00140+n*0x4*/
        {/*1f00280+n*0x4*/
            GT_U32    n;
            for(n = 0 ; n <= 15 ; n++)
            {
                regsAddrPtr->GOP.ILKN.ILKN_COUNT_BLK_TYPE_ERR_L[n] = 0x0000280+n*0x4;
            }/* end of loop n */
        }/*1f00280+n*0x4*/
        regsAddrPtr->GOP.ILKN.ILKN_STAT_EN = 0x00000238;
        regsAddrPtr->GOP.ILKN.ILKN_RX_PKT_GEN = 0x00000074;
        regsAddrPtr->GOP.ILKN.ILKN_REMOTE_LANE_STATUS = 0x00000108;
        regsAddrPtr->GOP.ILKN.ILKN_PTR_RST = 0x00000070;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_0 = 0x00000000;
        regsAddrPtr->GOP.ILKN.ILKN_PCS_PARAM = 0x0000023c;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_1 = 0x00000008;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_2 = 0x0000000c;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_3 = 0x00000010;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_4 = 0x00000014;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_5 = 0x00000018;
        regsAddrPtr->GOP.ILKN.ILKN_PCS_DEBUG_1 = 0x00000220;
        regsAddrPtr->GOP.ILKN.ILKN_PCS_DEBUG_0 = 0x00000210;
        regsAddrPtr->GOP.ILKN.ILKN_PCS_CFG_2 = 0x00000208;
        regsAddrPtr->GOP.ILKN.ILKN_PCS_CFG_1 = 0x00000204;
        regsAddrPtr->GOP.ILKN.ILKN_PCS_CFG_0 = 0x00000200;
        regsAddrPtr->GOP.ILKN.ILKN_OVF_7 = 0x000003ac;
        regsAddrPtr->GOP.ILKN.ILKN_OVF_6 = 0x000003a8;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_RL_CFG_1 = 0x00000048;
        regsAddrPtr->GOP.ILKN.ILKN_OVF_4 = 0x000003a0;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_12 = 0x00000058;
        regsAddrPtr->GOP.ILKN.ILKN_CH_FC_CFG_1 = 0x00000060;
        regsAddrPtr->GOP.ILKN.ILKN_OVF_1 = 0x00000384;
        regsAddrPtr->GOP.ILKN.ILKN_OVF_0 = 0x00000380;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_RL_CFG_2 = 0x00000050;
        regsAddrPtr->GOP.ILKN.ILKN_INT_CLR = 0x00000080;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_RL_CFG_0 = 0x00000040;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_0_1 = 0x000000c4;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_9 = 0x00000030;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_8 = 0x00000024;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_7 = 0x00000020;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_6 = 0x0000001c;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_2_0 = 0x000000d8;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_2_1 = 0x000000dc;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_2_2 = 0x000000e0;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_3_0 = 0x000000e4;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_3_1 = 0x000000e8;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_11 = 0x00000038;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CFG_10 = 0x00000034;
        regsAddrPtr->GOP.ILKN.ILKN_WORD_LOCK_STATUS = 0x00000100;
        regsAddrPtr->GOP.ILKN.ILKN_SYNC_LOCK_STATUS = 0x00000104;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_CNTL = 0x000000f0;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_3_2 = 0x000000ec;
        regsAddrPtr->GOP.ILKN.ILKN_ERR_GEN_STATUS = 0x00000110;
        regsAddrPtr->GOP.ILKN.ILKN_PCS_DEBUG_DATA = 0x00000114;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_FC_STATUS = 0x00000120;
        regsAddrPtr->GOP.ILKN.ILKN_PROT_ERR_STATUS = 0x00000124;
        regsAddrPtr->GOP.ILKN.ILKN_LINK_STATUS = 0x00000128;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_1_2 = 0x000000d4;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_1_1 = 0x000000d0;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_1_0 = 0x000000cc;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_0_2 = 0x000000c8;
        regsAddrPtr->GOP.ILKN.ILKN_EN = 0x0000020c;
        regsAddrPtr->GOP.ILKN.ILKN_MAC_CBFC_BM_0_0 = 0x000000c0;
        regsAddrPtr->GOP.ILKN.ILKN_LL_FC_STATUS = 0x0000012c;
        regsAddrPtr->GOP.ILKN.ILKN_FIFO_RST = 0x00000218;
        regsAddrPtr->GOP.ILKN.ILKN_LANE_SWAP = 0x00000214;
        regsAddrPtr->GOP.ILKN.ILKN_PCS_DEBUG_2 = 0x00000224;
        regsAddrPtr->GOP.ILKN.ILKN_PCS_DEBUG_3 = 0x00000228;
        regsAddrPtr->GOP.ILKN.ILKN_PCS_DEBUG_4 = 0x0000022c;
        regsAddrPtr->GOP.ILKN.ILKN_PCS_DEBUG_5 = 0x00000230;
        {/*1f00300+n*0x4*/
            GT_U32    n;
            for(n = 0 ; n <= 15 ; n++)
            {
                regsAddrPtr->GOP.ILKN.ILKN_COUNT_WORD_SYNC_ERR_L[n] = 0x00000300+n*0x4;
            }/* end of loop n */
        }/*1f00300+n*0x4*/
        {/*1f001c0+n*0x4*/
            GT_U32    n;
            for(n = 0 ; n <= 7 ; n++)
            {
                regsAddrPtr->GOP.ILKN.ILKN_COUNT_WORD_SYNC_ERR_H[n] = 0x000001c0+n*0x4;
            }/* end of loop n */
        }/*1f001c0+n*0x4*/
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_TX_PKT_LOW = 0x00000270;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_TX_PKT_HIGH = 0x00000274;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_TX_BYTE_LOW = 0x00000260;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_TX_BYTE_HIGH = 0x00000264;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_TX_BAD_PKT_LOW = 0x00000268;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_TX_BAD_PKT_HIGH = 0x0000026c;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_PKT_LOW = 0x00000250;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_PKT_HIGH = 0x00000254;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_CRC_ERROR_LOW = 0x00000258;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_CRC_ERROR_HIGH = 0x0000025c;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_BYTE_LOW = 0x00000240;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_BYTE_HIGH = 0x00000244;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_BURST_SIZE_ERROR_L = 0x00000370;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_BURST_SIZE_ERROR_H = 0x00000374;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_BAD_PKT_LOW = 0x00000248;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_BAD_PKT_HIGH = 0x0000024c;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_ALIGNMENT_FAILURE = 0x00000344;
        regsAddrPtr->GOP.ILKN.ILKN_COUNT_RX_ALIGNMENT_ERROR = 0x00000340;
        {/*1f002c0+n*0x4*/
            GT_U32    n;
            for(n = 0 ; n <= 15 ; n++)
            {
                regsAddrPtr->GOP.ILKN.ILKN_COUNT_DIAG_CRC_ERR_L[n] = 0x000002c0+n*0x4;
            }/* end of loop n */
        }/*1f002c0+n*0x4*/
        {/*1f00180+n*0x4*/
            GT_U32    n;
            for(n = 0 ; n <= 7 ; n++)
            {
                regsAddrPtr->GOP.ILKN.ILKN_COUNT_DIAG_CRC_ERR_H[n] = 0x00000180+n*0x4;
            }/* end of loop n */
        }/*1f00180+n*0x4*/
        regsAddrPtr->GOP.ILKN.ILKN_OVF_2 = 0x00000388;
        regsAddrPtr->GOP.ILKN.ILKN_OVF_3 = 0x0000038c;
        regsAddrPtr->GOP.ILKN.ILKN_CH_FC_CFG_2 = 0x00000068;
        regsAddrPtr->GOP.ILKN.ILKN_OVF_5 = 0x000003a4;
        regsAddrPtr->GOP.ILKN.ILKN_CH_FC_CFG_0 = 0x00000004;
        regsAddrPtr->GOP.ILKN.ILKN_ALIGN_STATUS = 0x0000010c;

    }/* enf of unit ILKN */

    {/* start of unit ILKN_WRP*/
        regsAddrPtr->GOP.ILKN_WRP.sdFcLedConvertersControl0 = 0x00000810;
        regsAddrPtr->GOP.ILKN_WRP.txdmaConverterControl0    = 0x00000820;
        regsAddrPtr->GOP.ILKN_WRP.rxdmaConverterPacketSizeRestriction = 0x0000084C;
    }/* enf of unit ILKN_WRP */


    return;
}

/*******************************************************************************
* gopMacTGInit
*
* DESCRIPTION:
*       init the DB - GOP-MacTG unit register file
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void gopMacTGInit
(
    IN  GT_U8  devNum
)
{
    GT_U32 i;
    GT_U32 patterRegId;
    GT_U32 offset;
    GT_U32 tgIdx;

    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *addrPtr =
                                        PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    /* MAC TG register addresses */
    tgIdx = 0;
    for (i=0;i < 72; i+=4, tgIdx++)
    {
        /* support macs that not exists in bobk */
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, i);

        if(i >= 56)
        {
            offset = (i-56) * 0x1000 + 0x200000;
        }
        else
        {
            offset = i * 0x1000;
        }

        addrPtr->packGenConfig[tgIdx].macDa[0]             = 0x00180C00 + offset;
        addrPtr->packGenConfig[tgIdx].macDa[1]             = 0x00180C04 + offset;
        addrPtr->packGenConfig[tgIdx].macDa[2]             = 0x00180C08 + offset;
        addrPtr->packGenConfig[tgIdx].macSa[0]             = 0x00180C0C + offset;
        addrPtr->packGenConfig[tgIdx].macSa[1]             = 0x00180C10 + offset;
        addrPtr->packGenConfig[tgIdx].macSa[2]             = 0x00180C14 + offset;
        addrPtr->packGenConfig[tgIdx].etherType            = 0x00180C18 + offset;
        addrPtr->packGenConfig[tgIdx].vlanTag              = 0x00180C1C + offset;
        addrPtr->packGenConfig[tgIdx].packetLength         = 0x00180C20 + offset;
        addrPtr->packGenConfig[tgIdx].packetCount          = 0x00180C24 + offset;
        addrPtr->packGenConfig[tgIdx].ifg                  = 0x00180C28 + offset;
        addrPtr->packGenConfig[tgIdx].macDaIncrementLimit  = 0x00180C2C + offset;
        addrPtr->packGenConfig[tgIdx].controlReg0          = 0x00180C40 + offset;
        for (patterRegId = 0; patterRegId < 31; patterRegId++)
        {
            addrPtr->packGenConfig[tgIdx].dataPattern[patterRegId] = 0x00180C50 + offset+4*patterRegId;
        }
    }
}

/*******************************************************************************
* ipInit
*
* DESCRIPTION:
*       init the DB - IP unit
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
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void ipInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);

    {/*start of unit IPvX */
        {/*start of unit routerManagementCntrsSets */
            {/*90c+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementInUcNonRoutedExceptionPktsCntrSet[n] =
                        0x90c+0x100*n;
                }/* end of loop n */
            }/*90c+0x100*n*/
            {/*91c+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementInMcTrappedMirroredPktCntrSet[n] =
                        0x91c+0x100*n;
                }/* end of loop n */
            }/*91c+0x100*n*/
            {/*900+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementInUcPktsCntrSet[n] =
                        0x900+0x100*n;
                }/* end of loop n */
            }/*900+0x100*n*/
            {/*904+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementInMcPktsCntrSet[n] =
                        0x904+0x100*n;
                }/* end of loop n */
            }/*904+0x100*n*/
            {/*908+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementInUcNonRoutedNonException[n] =
                        0x908+0x100*n;
                }/* end of loop n */
            }/*908+0x100*n*/
            {/*910+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementInMcNonRoutedNonExceptionPktsCntrSet[n] =
                        0x910+0x100*n;
                }/* end of loop n */
            }/*910+0x100*n*/
            {/*914+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementInMcNonRoutedExceptionPktsCntrSet[n] =
                        0x914+0x100*n;
                }/* end of loop n */
            }/*914+0x100*n*/
            {/*918+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementInUcTrappedMirroredPktCntrSet[n] =
                        0x918+0x100*n;
                }/* end of loop n */
            }/*918+0x100*n*/
            {/*920+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementInMcRPFFailCntrSet[n] =
                        0x920+0x100*n;
                }/* end of loop n */
            }/*920+0x100*n*/
            {/*924+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementOutUcPktCntrSet[n] =
                        0x924+0x100*n;
                }/* end of loop n */
            }/*924+0x100*n*/
            {/*980+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementCntrsSetConfig0[n] =
                        0x980+0x100*n;
                }/* end of loop n */
            }/*980+0x100*n*/
            {/*984+0x100*n*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementCntrsSetConfig1[n] =
                        0x984+0x100*n;
                }/* end of loop n */
            }/*984+0x100*n*/
            regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementIncomingPktCntr = 0x00000944;
            regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementOutgoingPktCntr = 0x00000948;
            if(!isSip5_20)
            {
                regsAddrPtr->IPvX.routerManagementCntrsSets.routerManagementInSIPFilterCntr = 0x00000958;
            }

        }/*end of unit routerManagementCntrsSets */


        {/*start of unit routerGlobalCtrl */
            regsAddrPtr->IPvX.routerGlobalCtrl.routerGlobalCtrl0 = 0x00000000;
            {/*4+n*0x4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.routerGlobalCtrl.routerMTUConfigReg[n] =
                        0x4+n*0x4;
                }/* end of loop n */
            }/*4+n*0x4*/
            regsAddrPtr->IPvX.routerGlobalCtrl.routerGlobalCtrl1 = 0x00000020;
            regsAddrPtr->IPvX.routerGlobalCtrl.routerGlobalCtrl2 = 0x00000024;

        }/*end of unit routerGlobalCtrl */


        {/*start of unit routerDropCntr */
            regsAddrPtr->IPvX.routerDropCntr.routerDropCntr = 0x00000950;
            regsAddrPtr->IPvX.routerDropCntr.routerDropCntrConfig = 0x00000954;

        }/*end of unit routerDropCntr */


        {/*start of unit routerBridgedPktsExceptionCntr */
            regsAddrPtr->IPvX.routerBridgedPktsExceptionCntr.routerBridgedPktExceptionsCntr = 0x00000940;

        }/*end of unit routerBridgedPktsExceptionCntr */


        {/*start of unit IPv6Scope */
            {/*278+n*4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.IPv6Scope.IPv6UcScopeTableReg[n] =
                        0x278+n*4;
                }/* end of loop n */
            }/*278+n*4*/
            {/*288+n*4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.IPv6Scope.IPv6McScopeTableReg[n] =
                        0x288+n*4;
                }/* end of loop n */
            }/*288+n*4*/
            {/*250+n*4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.IPv6Scope.IPv6UcScopePrefix[n] =
                        0x250+n*4;
                }/* end of loop n */
            }/*250+n*4*/
            {/*260+n*4*/
                GT_U32    n;
                for(n = 0 ; n <= 3 ; n++) {
                    regsAddrPtr->IPvX.IPv6Scope.IPv6UcScopeLevel[n] =
                        0x260+n*4;
                }/* end of loop n */
            }/*260+n*4*/

        }/*end of unit IPv6Scope */


        {/*start of unit IPv6GlobalCtrl */
            regsAddrPtr->IPvX.IPv6GlobalCtrl.IPv6CtrlReg0 = 0x00000200;
            regsAddrPtr->IPvX.IPv6GlobalCtrl.IPv6CtrlReg1 = 0x00000204;

        }/*end of unit IPv6GlobalCtrl */


        {/*start of unit IPv4GlobalCtrl */
            regsAddrPtr->IPvX.IPv4GlobalCtrl.IPv4CtrlReg0 = 0x00000100;
            regsAddrPtr->IPvX.IPv4GlobalCtrl.IPv4CtrlReg1 = 0x00000104;

        }/*end of unit IPv4GlobalCtrl */


        {/*start of unit internalRouter */
            regsAddrPtr->IPvX.internalRouter.routerInterruptCause = 0x00000970;
            regsAddrPtr->IPvX.internalRouter.routerInterruptMask = 0x00000974;

        }/*end of unit internalRouter */


        {/*start of unit FCoEGlobalCtrl */
            regsAddrPtr->IPvX.FCoEGlobalCtrl.routerFCoEGlobalConfig = 0x00000380;

        }/*end of unit FCoEGlobalCtrl */


        {/*start of unit ECMPRoutingConfig */
            regsAddrPtr->IPvX.ECMPRoutingConfig.ECMPConfig = 0x00000360;
            regsAddrPtr->IPvX.ECMPRoutingConfig.ECMPSeed = 0x00000364;

        }/*end of unit ECMPRoutingConfig */


        {/*start of unit CPUCodes */
            regsAddrPtr->IPvX.CPUCodes.CPUCodes0 = 0x00000e00;
            regsAddrPtr->IPvX.CPUCodes.CPUCodes1 = 0x00000e04;
            regsAddrPtr->IPvX.CPUCodes.CPUCodes2 = 0x00000e08;
            regsAddrPtr->IPvX.CPUCodes.CPUCodes3 = 0x00000e0c;
            regsAddrPtr->IPvX.CPUCodes.CPUCodes4 = 0x00000e10;
            regsAddrPtr->IPvX.CPUCodes.CPUCodes5 = 0x00000e14;
            regsAddrPtr->IPvX.CPUCodes.CPUCodes6 = 0x00000e18;
            regsAddrPtr->IPvX.CPUCodes.CPUCodes7 = 0x00000e1c;
            regsAddrPtr->IPvX.CPUCodes.CPUCodes8 = 0x00000e20;
            regsAddrPtr->IPvX.CPUCodes.CPUCodes9 = 0x00000e24;
            regsAddrPtr->IPvX.CPUCodes.CPUCodes10 = 0x00000e28;
            regsAddrPtr->IPvX.CPUCodes.CPUCodes11 = 0x00000e2c;

        }/*end of unit CPUCodes */
    }/*end of unit IPvX */
}

/*******************************************************************************
* ipLegacyDestroy
*
* DESCRIPTION:
*       destroy IP registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void ipLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* invalidate previous registers */
    DESTROY_STC_MAC(addrPtr->ipv4Regs);
    DESTROY_STC_MAC(addrPtr->ipv6Regs);
    DESTROY_STC_MAC(addrPtr->ipRegs);
}

/*******************************************************************************
* plrInit
*
* DESCRIPTION:
*       init the DB - PLR unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*       stageNum    - PLR stage number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void plrInit
(
    IN  GT_U8  devNum,
    IN  GT_U32 stageNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);

    /*start of unit PLR */
    {/*c0+n*4*/
        GT_U32    n;
        for(n = 0 ; n <= 63 ; n++) {
            regsAddrPtr->PLR[stageNum].policerInitialDP[n] =
                0xc0+n*4;
        }/* end of loop n */
    }/*c0+n*4*/
    regsAddrPtr->PLR[stageNum].policerCtrl0 = 0x00000000;
    regsAddrPtr->PLR[stageNum].policerCtrl1 = 0x00000004;
    regsAddrPtr->PLR[stageNum].globalBillingCntrIndexingModeConfig0 = 0x00000008;
    regsAddrPtr->PLR[stageNum].policerScanCtrl = 0x000000b8;
    {/*228+n*4*/
        GT_U32    n;
        GT_U32    nMax = isSip5_20 ? 15 : 7;
        for(n = 0 ; n <= nMax ; n++) {
            regsAddrPtr->PLR[stageNum].policerPortMeteringReg[n] =
                0x228+n*4;
        }/* end of loop n */
    }/*228+n*4*/
    {/*250+x*0x4*/
        GT_U32    x;
        GT_U32    nMax = isSip5_20 ? 31 : 15;
        GT_U32    offset = isSip5_20 ? 0x270 : 0x250;
        for(x = 0 ; x <= nMax ; x++) {
            regsAddrPtr->PLR[stageNum].policerPerPortRemarkTableID[x] =
                offset+x*0x4;
        }/* end of loop x */
    }/*250+x*0x4*/
    regsAddrPtr->PLR[stageNum].policerOAM = 0x00000224;
    regsAddrPtr->PLR[stageNum].policerMRU = 0x00000010;
    regsAddrPtr->PLR[stageNum].IPFIXSecondsMSbTimerStampUpload = 0x00000020;
    regsAddrPtr->PLR[stageNum].policerCountingBaseAddr = 0x00000024;
    regsAddrPtr->PLR[stageNum].policerMeteringBaseAddr = 0x00000028;
    regsAddrPtr->PLR[stageNum].policerIPFIXPktCountWAThreshold = 0x00000034;
    regsAddrPtr->PLR[stageNum].policerIPFIXDroppedPktCountWAThreshold = 0x00000030;
    regsAddrPtr->PLR[stageNum].policerIPFIXByteCountWAThresholdMSB = 0x0000003c;
    regsAddrPtr->PLR[stageNum].policerIPFIXByteCountWAThresholdLSB = 0x00000038;
    regsAddrPtr->PLR[stageNum].policerInterruptMask = 0x00000204;
    regsAddrPtr->PLR[stageNum].policerInterruptCause = 0x00000200;
    {/*74+n*4*/
        GT_U32    n;
        for(n = 0 ; n <= 7 ; n++) {
            regsAddrPtr->PLR[stageNum].policerTableAccessData[n] =
                0x74+n*4;
        }/* end of loop n */
    }/*74+n*4*/
    regsAddrPtr->PLR[stageNum].IPFIXSampleEntriesLog1 = 0x0000004c;
    regsAddrPtr->PLR[stageNum].policerError = 0x00000050;
    regsAddrPtr->PLR[stageNum].policerErrorCntr = 0x00000054;
    regsAddrPtr->PLR[stageNum].meteringAndCountingRAMSize0 = 0x00000060;
    regsAddrPtr->PLR[stageNum].meteringAndCountingRAMSize1 = 0x00000064;
    regsAddrPtr->PLR[stageNum].meteringAndCountingRAMSize2 = 0x00000068;
    regsAddrPtr->PLR[stageNum].policerTableAccessCtrl = 0x00000070;
    regsAddrPtr->PLR[stageNum].policerMeteringRefreshScanRateLimit = 0x000000b0;
    regsAddrPtr->PLR[stageNum].policerMeteringRefreshScanAddrRange = 0x000000b4;
    regsAddrPtr->PLR[stageNum].IPFIXSecondsLSbTimerStampUpload = 0x0000001c;
    {/*208+n*4*/
        GT_U32    n;
        for(n = 0 ; n <= 1 ; n++) {
            regsAddrPtr->PLR[stageNum].policerShadow[n] =
                0x208+n*4;
        }/* end of loop n */
    }/*208+n*4*/
    regsAddrPtr->PLR[stageNum].IPFIXSampleEntriesLog0 = 0x00000048;
    regsAddrPtr->PLR[stageNum].IPFIXNanoTimerStampUpload = 0x00000018;
    regsAddrPtr->PLR[stageNum].IPFIXCtrl = 0x00000014;
    regsAddrPtr->PLR[stageNum].hierarchicalPolicerCtrl = 0x0000002c;
    regsAddrPtr->PLR[stageNum].globalBillingCntrIndexingModeConfig1 = 0x0000000c;
    if(isSip5_20)
    {
        GT_U32    p;
        for(p = 0 ; p < 512 ; p ++) {
            regsAddrPtr->PLR[stageNum].portAndPktTypeTranslationTable[p] =
                0x1800+p*0x4;
        }/* end of loop p */
    }
    else
    {/*1800+p*2<perPort>*/
        GT_U32    p;
        for(p = 0 ; p <= 255 ; p += 2) {
            regsAddrPtr->PLR[stageNum].portAndPktTypeTranslationTable[p/2] =
                0x1800+(p/2)*0x4;
        }/* end of loop p */
    }/*1800+p*2*/

    regsAddrPtr->PLR[stageNum].policerManagementCntrsTbl = 0x00000500;
    regsAddrPtr->PLR[stageNum].policerCountingTbl = isSip5_20 ? 0x00200000 : 0x00140000;
    regsAddrPtr->PLR[stageNum].policerTimerTbl = 0x00000300;
    regsAddrPtr->PLR[stageNum].ipfixWrapAroundAlertTbl = 0x00000800;

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        regsAddrPtr->PLR[stageNum].ipfixAgingAlertTbl = 0x00001000;
        regsAddrPtr->PLR[stageNum].policerBadAddressLatch = 0x00002000;
        if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
        {
            regsAddrPtr->PLR[stageNum].meteringAddressingModeCfg0 = 0x00000290;
            regsAddrPtr->PLR[stageNum].meteringAddressingModeCfg1 = 0x00000294;
            {/*2100+(p*4)*/
                GT_U32    p;
                for (p = 0 ; (p <= 127); p++) {
                    regsAddrPtr->PLR[stageNum].mef10_3_bucket0MaxRateArr[p] =
                        0x2100 + (p * 4);
                }/* end of loop p */
            }/*2100+(p*4)*/
            {/*2300+(p*4)*/
                GT_U32    p;
                for (p = 0 ; (p <= 127); p++) {
                    regsAddrPtr->PLR[stageNum].mef10_3_bucket1MaxRateArr[p] =
                        0x2300 + (p * 4);
                }/* end of loop p */
            }/*2300+(p*4)*/
            {/*2500+(p*4)*/
                GT_U32    p;
                for (p = 0 ; (p <= 127); p++) {
                    regsAddrPtr->PLR[stageNum].qosProfile2PriorityMapArr[p] =
                        0x2500 + (p * 4);
                }/* end of loop p */
            }/*2500+(p*4)*/
        }
    }
    else
    {
        regsAddrPtr->PLR[stageNum].ipfixAgingAlertTbl = 0x00000900;
    }

    /*end of unit PLR */
}

/*******************************************************************************
* plrLegacyDestroy
*
* DESCRIPTION:
*       destroy PLR registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void plrLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    DESTROY_STC_MAC(addrPtr->PLR);

    return;
}

/*******************************************************************************
* cncInit
*
* DESCRIPTION:
*       init the DB - CNC unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*       instance    - CNC instance number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void cncInit
(
    IN  GT_U8  devNum,
    IN  GT_U32 instance
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);

    /* unit perBlockRegs */

    /* unit wraparound */
    {/*00001498+n*0x10+m*0x4*/
        GT_U32    n,m;

        for(n = 0; n <= 15; n++)
        {
            for(m = 0; m <= 3; m++)
            {
                regsAddrPtr->CNC[instance].perBlockRegs.
                    wraparound.CNCBlockWraparoundStatusReg[n][m] =
                    0x00001498 + (n * 0x10) + (m * 0x4);
            }/* end of loop m */
        }/* end of loop n */

    }/*00001498+n*0x10+m*0x4*/

    /* unit rangeCount */
    {/*00001c98+0x10*n+j*0x4*/
        GT_U32    j,n;

        for(j = 0; j <= 1; j++)
        {
            for(n = 0; n <= 15; n++)
            {
                regsAddrPtr->CNC[instance].perBlockRegs.
                    rangeCount.CNCBlockRangeCountEnable[j][n] =
                    0x00001c98 + (0x10 * n) + (j * 0x4);
            }/* end of loop n */
        }/* end of loop j */

    }/*00001c98+0x10*n+j*0x4*/

    /* unit clientEnable */
    {/*00001298+n*0x4*/
        GT_U32    n;

        for(n = 0; n <= 15; n++)
        {
            regsAddrPtr->CNC[instance].perBlockRegs.
                clientEnable.CNCBlockConfigReg0[n] =
                0x00001298 + (n * 0x4);
        }/* end of loop n */

    }/*00001298+n*0x4*/

    /* unit entryMode */
    if(isSip5_20)
    {/*00001398+n*0x4*/
        GT_U32    n;

        for(n = 0; n <= 1; n++)
        {
            regsAddrPtr->CNC[instance].perBlockRegs.
                entryMode.CNCBlocksCounterEntryModeRegister[n] =
                0x00001398 + (n * 0x4);
        }/* end of loop n */

    }/*00001398+n*0x4*/


    /* unit memory */
    {/*0x00010000+n*0x2000*/
        GT_U32    n;

        for(n = 0; n <= 15; n++)
        {
            regsAddrPtr->CNC[instance].perBlockRegs.
                memory.baseAddress[n] =
                0x00010000 + (n * 0x2000);
        }/* end of loop n */

    }/*0x00010000+n*0x2000*/

    /* unit globalRegs */
    regsAddrPtr->CNC[instance].globalRegs.CNCGlobalConfigReg                    = 0x00000000;
    regsAddrPtr->CNC[instance].globalRegs.CNCFastDumpTriggerReg                 = 0x00000030;
    regsAddrPtr->CNC[instance].globalRegs.CNCClearByReadValueRegWord0           = 0x00000040;
    regsAddrPtr->CNC[instance].globalRegs.CNCClearByReadValueRegWord1           = 0x00000044;
    regsAddrPtr->CNC[instance].globalRegs.CNCInterruptSummaryCauseReg           = 0x00000100;
    regsAddrPtr->CNC[instance].globalRegs.CNCInterruptSummaryMaskReg            = 0x00000104;
    regsAddrPtr->CNC[instance].globalRegs.wraparoundFunctionalInterruptCauseReg = 0x00000190;
    regsAddrPtr->CNC[instance].globalRegs.wraparoundFunctionalInterruptMaskReg  = 0x000001a4;
    regsAddrPtr->CNC[instance].globalRegs.rateLimitFunctionalInterruptCauseReg  = 0x000001b8;
    regsAddrPtr->CNC[instance].globalRegs.rateLimitFunctionalInterruptMaskReg   = 0x000001cc;
    regsAddrPtr->CNC[instance].globalRegs.miscFunctionalInterruptCauseReg       = 0x000001e0;
    regsAddrPtr->CNC[instance].globalRegs.miscFunctionalInterruptMaskReg        = 0x000001e4;
}

/*******************************************************************************
* cncLegacyDestroy
*
* DESCRIPTION:
*       destroy CNC registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void cncLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    DESTROY_STC_MAC(addrPtr->cncRegs);

    return;
}


/*******************************************************************************
* txqDqInit
*
* DESCRIPTION:
*       This function inits the DB - per TXQ_DQ unit.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       index   - unit index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void txqDqInit
(
    IN  GT_U8 devNum,
    IN  GT_U32  index
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);
    GT_U32  numTxqDqPortsPerUnit_firstBlock = (SIP_5_DQ_NUM_PORTS_CNS - 1);
    GT_U32  numTxqDqPortsPerUnit_secondBlock = isSip5_20 ?
                    (SIP_5_20_DQ_NUM_PORTS_CNS - SIP_5_DQ_NUM_PORTS_CNS) :
                    0;
    GT_U32  ii;

    { /*start of unit TXQ */

        { /*start of unit dq */

            { /*start of unit statisticalAndCPUTrigEgrMirrToAnalyzerPort */
                { /*0000d040+i*0x4*/
                    GT_U32    i;
                    if(isSip5_20 == 0)
                    {
                        for(i = 0; i <= 31; i++)
                        {
                            regsAddrPtr->TXQ.dq[index].statisticalAndCPUTrigEgrMirrToAnalyzerPort.egrAnalyzerEnableIndex[i] =
                            0x0000d040+i*0x4;
                        } /* end of loop i */
                    }
                    else
                    {
                        /* we not need all 72 registers ... only 64 (512/8) */
                        GT_U32  numRegs = 64;
                        /* duplication for multi port groups done by function
                           txqDqUpdateUnit0AsGlobalPortsAddr(...) */

                        for(i = 0; i < numRegs; i++)
                        {
                            regsAddrPtr->TXQ.dq[index].statisticalAndCPUTrigEgrMirrToAnalyzerPort.egrAnalyzerEnableIndex[i] =
                            0x0000d040+i*0x4;
                        } /* end of loop i */
                    }
                } /*0000d040+i*0x4*/
                regsAddrPtr->TXQ.dq[index].statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCStatisticalTxSniffConfig = 0x0000d000;
                regsAddrPtr->TXQ.dq[index].statisticalAndCPUTrigEgrMirrToAnalyzerPort.egrCTMTrigger = 0x0000d004;
                regsAddrPtr->TXQ.dq[index].statisticalAndCPUTrigEgrMirrToAnalyzerPort.CTMAnalyzerIndex = 0x0000d010;
                regsAddrPtr->TXQ.dq[index].statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCAnalyzerIndex = 0x0000d014;

            } /*end of unit statisticalAndCPUTrigEgrMirrToAnalyzerPort */


            { /*start of unit shaper */
                { /*00004100+n*0x4*/
                    GT_U32    n;
                    for(n = 0; n <= numTxqDqPortsPerUnit_firstBlock; n++)
                    {
                        regsAddrPtr->TXQ.dq[index].shaper.portTokenBucketMode[n] =
                            0x00004100+n*0x4;

                    } /* end of loop n */
                    {
                        GT_U32  baseAddr = 0x00010000;
                        /* continue with n */
                        for(ii = 0 ; ii < numTxqDqPortsPerUnit_secondBlock ; ii++,n++)
                        {
                            regsAddrPtr->TXQ.dq[index].shaper.portTokenBucketMode[n] =
                                baseAddr+n*0x4;
                        }
                    }
                } /*00004100+n*0x4*/
                { /*00004300+n * 0x4*/
                    GT_U32    n;
                    for(n = 0; n <= numTxqDqPortsPerUnit_firstBlock; n++)
                    {
                        regsAddrPtr->TXQ.dq[index].shaper.portDequeueEnable[n] =
                        0x00004300+n * 0x4;
                    } /* end of loop n */
                    {
                        GT_U32  baseAddr = 0x00010100;
                        /* continue with n */
                        for(ii = 0 ; ii < numTxqDqPortsPerUnit_secondBlock ; ii++,n++)
                        {
                            regsAddrPtr->TXQ.dq[index].shaper.portDequeueEnable[n] =
                                baseAddr+n*0x4;
                        }
                    }
                } /*00004300+n * 0x4*/
                regsAddrPtr->TXQ.dq[index].shaper.tokenBucketUpdateRate = 0x00004000;
                regsAddrPtr->TXQ.dq[index].shaper.tokenBucketBaseLine = 0x00004004;
                regsAddrPtr->TXQ.dq[index].shaper.CPUTokenBucketMTU = 0x00004008;
                regsAddrPtr->TXQ.dq[index].shaper.portsTokenBucketMTU = 0x0000400c;

            } /*end of unit shaper */

            { /*start of unit scheduler */
                { /*start of unit schedulerConfig */
                    { /*00001040+n * 0x4*/
                        GT_U32    n;
                        for(n = 0; n <= numTxqDqPortsPerUnit_firstBlock; n++)
                        {
                            regsAddrPtr->TXQ.dq[index].scheduler.schedulerConfig.conditionalMaskForPort[n] =
                            0x00001040+n * 0x4;
                        } /* end of loop n */
                        {
                            GT_U32  baseAddr = 0x0000F000;
                            /* continue with n */
                            for(ii = 0 ; ii < numTxqDqPortsPerUnit_secondBlock ; ii++,n++)
                            {
                                regsAddrPtr->TXQ.dq[index].scheduler.schedulerConfig.conditionalMaskForPort[n] =
                                    baseAddr+n*0x4;
                            }
                        }
                    } /*00001040+n * 0x4*/
                    regsAddrPtr->TXQ.dq[index].scheduler.schedulerConfig.schedulerConfig = 0x00001000;
                    regsAddrPtr->TXQ.dq[index].scheduler.schedulerConfig.pktLengthForTailDropDequeue = 0x00001008;
                    regsAddrPtr->TXQ.dq[index].scheduler.schedulerConfig.highSpeedPorts[index] = 0x0000100c;

                } /*end of unit schedulerConfig */


                { /*start of unit priorityArbiterWeights */
                    { /*000012c0+p*0x4*/
                        GT_U32    p;
                        for(p = 0; p <= 15; p++)
                        {
                            regsAddrPtr->TXQ.dq[index].scheduler.priorityArbiterWeights.profileSDWRRGroup[p] =
                            0x000012c0+p*0x4;
                        } /* end of loop p */
                    } /*000012c0+p*0x4*/
                    { /*00001240+p*0x4*/
                        GT_U32    p;
                        for(p = 0; p <= 15; p++)
                        {
                            regsAddrPtr->TXQ.dq[index].scheduler.priorityArbiterWeights.profileSDWRRWeightsConfigReg0[p] =
                            0x00001240+p*0x4;
                        } /* end of loop p */
                    } /*00001240+p*0x4*/
                    { /*00001280+p*0x4*/
                        GT_U32    p;
                        for(p = 0; p <= 15; p++)
                        {
                            regsAddrPtr->TXQ.dq[index].scheduler.priorityArbiterWeights.profileSDWRRWeightsConfigReg1[p] =
                            0x00001280+p*0x4;
                        } /* end of loop p */
                    } /*00001280+p*0x4*/
                    { /*00001300+p*0x4*/
                        GT_U32    p;
                        for(p = 0; p <= 15; p++)
                        {
                            regsAddrPtr->TXQ.dq[index].scheduler.priorityArbiterWeights.profileSDWRREnable[p] =
                            0x00001300+p*0x4;
                        } /* end of loop p */
                    } /*00001300+p*0x4*/
                    { /*00001380+n * 0x4*/
                        GT_U32    n;
                        for(n = 0; n <= numTxqDqPortsPerUnit_firstBlock; n++)
                        {
                            regsAddrPtr->TXQ.dq[index].scheduler.priorityArbiterWeights.portSchedulerProfile[n] =
                            0x00001380+n * 0x4;
                        } /* end of loop n */
                        {
                            GT_U32  baseAddr = 0x0000F100;
                            /* continue with n */
                            for(ii = 0 ; ii < numTxqDqPortsPerUnit_secondBlock ; ii++,n++)
                            {
                                regsAddrPtr->TXQ.dq[index].scheduler.priorityArbiterWeights.portSchedulerProfile[n] =
                                    baseAddr+n*0x4;
                            }
                        }
                    } /*00001380+n * 0x4*/

                } /*end of unit priorityArbiterWeights */


                { /*start of unit portShaper */
                    { /*0000190c+0x4*n*/
                        GT_U32    n;
                        for(n = 0; n <= numTxqDqPortsPerUnit_firstBlock; n++)
                        {
                            regsAddrPtr->TXQ.dq[index].scheduler.portShaper.portRequestMaskSelector[n] =
                            0x0000190c+0x4*n;
                        } /* end of loop n */
                        {
                            GT_U32  baseAddr = 0x0000F300;
                            /* continue with n */
                            for(ii = 0 ; ii < numTxqDqPortsPerUnit_secondBlock ; ii++,n++)
                            {
                                regsAddrPtr->TXQ.dq[index].scheduler.portShaper.portRequestMaskSelector[n] =
                                    baseAddr+n*0x4;
                            }
                        }
                    } /*0000190c+0x4*n*/
                    regsAddrPtr->TXQ.dq[index].scheduler.portShaper.portRequestMask = 0x00001904;

                } /*end of unit portShaper */

                { /*start of unit portArbiterConfig */
                    { /*00001510+n*0x4*/
                        GT_U32    n;
                        for(n = 0; n <= numTxqDqPortsPerUnit_firstBlock; n++)
                        {
                            regsAddrPtr->TXQ.dq[index].scheduler.portArbiterConfig.portsArbiterPortWorkConservingEnable[n] =
                            0x00001510+n*0x4;
                        } /* end of loop n */
                        {
                            GT_U32  baseAddr = 0x0000F200;
                            /* continue with n */
                            for(ii = 0 ; ii < numTxqDqPortsPerUnit_secondBlock ; ii++,n++)
                            {
                                regsAddrPtr->TXQ.dq[index].scheduler.portArbiterConfig.portsArbiterPortWorkConservingEnable[n] =
                                    baseAddr+n*0x4;
                            }
                        }
                    } /*00001510+n*0x4*/
                    { /*00001650+n*4*/
                        GT_U32    n;
                        for(n = 0; n <= 84; n++)
                        {
                            regsAddrPtr->TXQ.dq[index].scheduler.portArbiterConfig.portsArbiterMap[n] =
                            0x00001650+n*4;
                        } /* end of loop n */
                    } /*00001650+n*4*/
                    regsAddrPtr->TXQ.dq[index].scheduler.portArbiterConfig.portsArbiterConfig = 0x00001500;
                    regsAddrPtr->TXQ.dq[index].scheduler.portArbiterConfig.portsArbiterStatus = 0x00001800;

                } /*end of unit portArbiterConfig */


            } /*end of unit scheduler */

            { /*start of unit global */
                { /*start of unit memoryParityError */
                    regsAddrPtr->TXQ.dq[index].global.memoryParityError.tokenBucketPriorityParityErrorCntr = 0x00000800;
                    regsAddrPtr->TXQ.dq[index].global.memoryParityError.parityErrorBadAddr = 0x00000808;

                } /*end of unit memoryParityError */


                { /*start of unit interrupt */
                    { /*00000610+p*0x4*/
                        GT_U32    p;
                        for(p = 0; p <= 3; p++)
                        {
                            regsAddrPtr->TXQ.dq[index].global.interrupt.flushDoneInterruptCause[p] =
                            0x00000610+p*0x4;
                        } /* end of loop p */
                    } /*00000610+p*0x4*/
                    { /*00000620+p*0x4*/
                        GT_U32    p;
                        for(p = 0; p <= 3; p++)
                        {
                            regsAddrPtr->TXQ.dq[index].global.interrupt.flushDoneInterruptMask[p] =
                            0x00000620+p*0x4;
                        } /* end of loop p */
                    } /*00000620+p*0x4*/
                    { /*00000630+p*0x4*/
                        GT_U32    p;
                        for(p = 0; p <= 3; p++)
                        {
                            regsAddrPtr->TXQ.dq[index].global.interrupt.egrSTCInterruptCause[p] =
                            0x00000630+p*0x4;
                        } /* end of loop p */
                    } /*00000630+p*0x4*/
                    { /*00000640+p*0x4*/
                        GT_U32    p;
                        for(p = 0; p <= 3; p++)
                        {
                            regsAddrPtr->TXQ.dq[index].global.interrupt.egrSTCInterruptMask[p] =
                            0x00000640+p*0x4;
                        } /* end of loop p */
                    } /*00000640+p*0x4*/
                    regsAddrPtr->TXQ.dq[index].global.interrupt.txQDQInterruptSummaryCause = 0x00000600;
                    regsAddrPtr->TXQ.dq[index].global.interrupt.txQDQInterruptSummaryMask = 0x00000604;
                    regsAddrPtr->TXQ.dq[index].global.interrupt.txQDQMemoryErrorInterruptCause = 0x00000650;
                    regsAddrPtr->TXQ.dq[index].global.interrupt.txQDQMemoryErrorInterruptMask = 0x00000660;
                    regsAddrPtr->TXQ.dq[index].global.interrupt.txQDQGeneralInterruptCause = 0x00000670;
                    regsAddrPtr->TXQ.dq[index].global.interrupt.txQDQGeneralInterruptMask = 0x00000680;

                } /*end of unit interrupt */


                { /*start of unit globalDQConfig */
                    { /*000002c0+n*0x4*/
                        GT_U32    n;
                        for(n = 0; n <= numTxqDqPortsPerUnit_firstBlock; n++)
                        {
                            regsAddrPtr->TXQ.dq[index].global.globalDQConfig.portToDMAMapTable[n] =
                            0x000002c0+n*0x4;
                        } /* end of loop n */
                        {
                            GT_U32  baseAddr = 0x0000E100;
                            /* continue with n */
                            for(ii = 0 ; ii < numTxqDqPortsPerUnit_secondBlock ; ii++,n++)
                            {
                                regsAddrPtr->TXQ.dq[index].global.globalDQConfig.portToDMAMapTable[n] =
                                    baseAddr+n*0x4;
                            }
                        }
                    } /*000002c0+n*0x4*/
                    { /*0000020c+p*0x4*/
                        GT_U32    p;
                        for(p = 0; p <= 15; p++)
                        {
                            regsAddrPtr->TXQ.dq[index].global.globalDQConfig.profileByteCountModification[p] =
                            0x0000020c+p*0x4;
                        } /* end of loop p */
                    } /*0000020c+p*0x4*/
                    { /*00000004+n*0x4*/
                        GT_U32    n;
                        for(n = 0; n <= numTxqDqPortsPerUnit_firstBlock; n++)
                        {
                            regsAddrPtr->TXQ.dq[index].global.globalDQConfig.portBCConstantValue[n] =
                            0x00000004+n*0x4;
                        } /* end of loop n */
                        {
                            GT_U32  baseAddr = 0x0000E000;
                            /* continue with n */
                            for(ii = 0 ; ii < numTxqDqPortsPerUnit_secondBlock ; ii++,n++)
                            {
                                regsAddrPtr->TXQ.dq[index].global.globalDQConfig.portBCConstantValue[n] =
                                    baseAddr+n*0x4;
                            }
                        }
                    } /*00000004+n*0x4*/
                    regsAddrPtr->TXQ.dq[index].global.globalDQConfig.globalDequeueConfig = 0x00000000;
                    regsAddrPtr->TXQ.dq[index].global.globalDQConfig.BCForCutThrough = 0x00000208;

                    if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
                    {
                        regsAddrPtr->TXQ.dq[index].global.globalDQConfig.creditCountersReset[0] = 0x00000260;
                        regsAddrPtr->TXQ.dq[index].global.globalDQConfig.creditCountersReset[1] = 0x00000264;
                        regsAddrPtr->TXQ.dq[index].global.globalDQConfig.creditCountersReset[2] = 0x00000268;
                    }

                } /*end of unit globalDQConfig */

                { /*start of unit flushTrig */
                    { /*00000400+n * 0x4*/
                        GT_U32    n;
                        for(n = 0; n <= numTxqDqPortsPerUnit_firstBlock; n++)
                        {
                            regsAddrPtr->TXQ.dq[index].global.flushTrig.portTxQFlushTrigger[n] =
                            0x00000400+n * 0x4;
                        } /* end of loop n */
                        {
                            GT_U32  baseAddr = 0x0000E400;
                            /* continue with n */
                            for(ii = 0 ; ii < numTxqDqPortsPerUnit_secondBlock ; ii++,n++)
                            {
                                regsAddrPtr->TXQ.dq[index].global.flushTrig.portTxQFlushTrigger[n] =
                                    baseAddr+n*0x4;
                            }
                        }
                    } /*00000400+n * 0x4*/

                } /*end of unit flushTrig */


                { /*start of unit ECCCntrs */
                    regsAddrPtr->TXQ.dq[index].global.ECCCntrs.DQIncECCErrorCntr = 0x00000920;
                    regsAddrPtr->TXQ.dq[index].global.ECCCntrs.DQClrECCErrorCntr = 0x00000924;

                } /*end of unit ECCCntrs */


                { /*start of unit dropCntrs */
                    regsAddrPtr->TXQ.dq[index].global.dropCntrs.egrMirroredDroppedCntr = 0x00000900;
                    regsAddrPtr->TXQ.dq[index].global.dropCntrs.STCDroppedCntr = 0x00000904;

                } /*end of unit dropCntrs */

                if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
                {
                    { /*00000B00+n * 0x4*/
                        GT_U32    n;
                        for(n = 0; n <= 73; n++)
                        {
                            regsAddrPtr->TXQ.dq[index].global.creditCounters.txdmaPortCreditCounter[n] =
                            0x00000b00+n * 0x4;
                        } /* end of loop n */
                    } /*00000b00+n * 0x4*/
                }

            } /*end of unit global */


            { /*start of unit flowCtrlResponse */
                { /*0000c100+n*0x4*/
                    GT_U32    n;
                    for(n = 0; n <= 15; n++)
                    {
                        regsAddrPtr->TXQ.dq[index].flowCtrlResponse.schedulerProfilePFCTimerToPriorityQueueMapEnable[n] =
                        0x0000c100+n*0x4;
                    } /* end of loop n */
                } /*0000c100+n*0x4*/
                { /*0000c180+0x40*t+0x4*p*/
                    GT_U32    p,t;
                    for(p = 0; p <= 15; p++)
                    {
                        for(t = 0; t <= 7; t++)
                        {
                            regsAddrPtr->TXQ.dq[index].flowCtrlResponse.schedulerProfilePriorityQueuePFCFactor[p][t] =
                            0x0000c180+0x40*t+0x4*p;
                        } /* end of loop t */
                    } /* end of loop p */
                } /*0000c180+0x40*t+0x4*p*/
                { /*0000c008+n*0x4*/
                    GT_U32    n;
                    for(n = 0; n <= 15; n++)
                    {
                        regsAddrPtr->TXQ.dq[index].flowCtrlResponse.schedulerProfileLLFCXOFFValue[n] =
                        0x0000c008+n*0x4;
                    } /* end of loop n */
                } /*0000c008+n*0x4*/
                { /*0000c500+p*4*/
                    GT_U32    p;
                    if(0 == PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
                    {
                        for(p = 0; p <= 255; p++)
                        {
                            regsAddrPtr->TXQ.dq[index].flowCtrlResponse.portToTxQPortMap[p] =
                                0x0000c500+p*4;
                        } /* end of loop p */
                    }
                    else
                    {
                        /* actually implemented as 'table' but we can consider as registers */
                        CPSS_TBD_BOOKMARK_BOBCAT3/* the table need to be 256 entries not 96 */
                        for(p = 0; p <= 95; p++)
                        {
                            regsAddrPtr->TXQ.dq[index].flowCtrlResponse.portToTxQPortMap[p] =
                                0x00011000+p*4;
                        } /* end of loop p */
                    }
                } /*0000c500+p*4*/
                { /*0000c080+n*0x4*/
                    GT_U32    n;
                    for(n = 0; n <= 7; n++)
                    {
                        regsAddrPtr->TXQ.dq[index].flowCtrlResponse.PFCTimerToPriorityQueueMap[n] =
                        0x0000c080+n*0x4;
                    } /* end of loop n */
                } /*0000c080+n*0x4*/
                regsAddrPtr->TXQ.dq[index].flowCtrlResponse.flowCtrlResponseConfig = 0x0000c000;

            } /*end of unit flowCtrlResponse */

        } /*end of unit dq */

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            { /*0000100c+n*0x4*/
                GT_U32    n;
                for(n = 0; n <= 1; n++)
                {
                    regsAddrPtr->TXQ.dq[index].scheduler.schedulerConfig.highSpeedPorts[n] =
                        0x0000100c+n*0x4;
                } /* end of loop n */
            } /*0000100c+n*0x4*/

            regsAddrPtr->TXQ.dq[index].scheduler.portShaper.fastPortShaperThreshold =
                0x00001908;
        }

    } /*end of unit TXQ */

}
/*******************************************************************************
* txqDqUpdateUnit0AsGlobalPortsAddr
*
* DESCRIPTION:
*       update the DB - txq-DQ unit - after multi units set 'per port' addresses.
*       the DB of TXQ.dq[0] updated for the ports with 'global port index'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       currentUnitIndex - the index of current TXQ.dq[] unit
*       portIn_unit_0     - the 'global port' index (in unit 0)
*       portInCurrentUnit - the 'local port'  index (in current unit)
*       numOfPorts - number of ports to update.
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void txqDqUpdateUnit0AsGlobalPortsAddr
(
    IN  GT_U8   devNum,
    IN  GT_U32  currentUnitIndex,
    IN  GT_U32  portIn_unit_0,
    IN  GT_U32  portInCurrentUnit,
    IN  GT_U32  numOfPorts
)
{
    GT_U32    saveLastNumOfPorts=0;/*number of ports from the end of the unitStart1Ptr[]
                                  that need to be saved before overridden by unitStart2Ptr[]*/
    GT_U32    saveToIndex=0;/*the index in unitStart1Ptr[] to save the 'last ports'
                                  valid when saveLastNumOfPorts != 0*/

    /************************************/
    /*   update the per port addresses  */
    /*   of the txq-DQ unit 0 with those */
    /*   of unit 1                      */
    /************************************/

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, TXQ.dq[0] , TXQ.dq[currentUnitIndex] , scheduler.portArbiterConfig.portsArbiterPortWorkConservingEnable,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex, NULL)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, TXQ.dq[0] , TXQ.dq[currentUnitIndex] , scheduler.portShaper.portRequestMaskSelector,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex, NULL)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, TXQ.dq[0] , TXQ.dq[currentUnitIndex] , scheduler.priorityArbiterWeights.portSchedulerProfile,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex, NULL)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, TXQ.dq[0] , TXQ.dq[currentUnitIndex] , scheduler.schedulerConfig.conditionalMaskForPort,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex, NULL)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, TXQ.dq[0] , TXQ.dq[currentUnitIndex] , shaper.portDequeueEnable,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex, NULL)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, TXQ.dq[0] , TXQ.dq[currentUnitIndex] , shaper.portTokenBucketMode,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex, NULL)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, TXQ.dq[0] , TXQ.dq[currentUnitIndex] , global.globalDQConfig.portToDMAMapTable,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex, NULL)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, TXQ.dq[0] , TXQ.dq[currentUnitIndex] , global.globalDQConfig.portBCConstantValue,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex, NULL)
        );

    regUnitPerPortAddrUpdate(
        REG_NAME_AND_SIZE_FOR_2_UNITS_MAC(devNum, TXQ.dq[0] , TXQ.dq[currentUnitIndex] , global.flushTrig.portTxQFlushTrigger,
            portIn_unit_0 , portInCurrentUnit,numOfPorts,saveLastNumOfPorts,saveToIndex, NULL)
        );

}

/*******************************************************************************
* txqDqLegacyDestroy
*
* DESCRIPTION:
*       This function destroys TXQ_DQ registers from the legacy DB.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void txqDqLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* destroy SHAPER structure */
    DESTROY_STC_MAC(addrPtr->txqVer1.dq);

    return;
}

/*******************************************************************************
* txqQcnInit
*
* DESCRIPTION:
*       This function inits the DB - TXQ_QCN unit.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void txqQcnInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);

    {/*start of unit qcn */
        {/*00000b40+0x4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 8 ; n++) {
                regsAddrPtr->TXQ.qcn.CCFCSpeedIndex[n] =
                    0x00000b40+0x4*n;
            }/* end of loop n */
        }/*00000b40+0x4*n*/
        {/*00000090+0x4*n*/
            GT_U32    n;
            for(n = 0 ; n <= 7 ; n++) {
                regsAddrPtr->TXQ.qcn.CNSampleTbl[n] =
                    0x00000090+0x4*n;
            }/* end of loop n */
        }/*00000090+0x4*n*/
        {/*00000300+0x20*p + 0x4*t*/
            GT_U32    p,t, maxProfile;
            if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
            {
                maxProfile = 15;
            }
            else
            {
                maxProfile = 7;
            }

            for(p = 0 ; p <= maxProfile ; p++) {
                for(t = 0 ; t <= 7 ; t++) {
                    regsAddrPtr->TXQ.qcn.profilePriorityQueueCNThreshold[p][t] =
                        0x00000300+0x20*p + 0x4*t;
                }/* end of loop t */
            }/* end of loop p */
        }/*00000300+0x20*p + 0x4*t*/
        regsAddrPtr->TXQ.qcn.feedbackMIN = 0x00000050;
        regsAddrPtr->TXQ.qcn.feedbackMAX = 0x00000060;
        regsAddrPtr->TXQ.qcn.CNGlobalConfig = 0x00000000;
        {/*00000b20+0x4*n*/
            GT_U32    n;
            if(0 == isSip5_20)
            {
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->TXQ.qcn.enFCTriggerByCNFrameOnPort[n] =
                        0x00000b20+0x4*n;
                }/* end of loop n */
            }
            else
            {
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->TXQ.qcn.enFCTriggerByCNFrameOnPort[n] =
                        0x00000c00+0x4*n;
                }/* end of loop n */
            }
        }/*00000b20+0x4*n*/
        {/*00000b00+0x4*n*/
            GT_U32    n;
            if(0 == isSip5_20)
            {
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->TXQ.qcn.enCNFrameTxOnPort[n] =
                        0x00000b00+0x4*n;
                }/* end of loop n */
            }
            else
            {
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->TXQ.qcn.enCNFrameTxOnPort[n] =
                        0x00000b00+0x4*n;
                }/* end of loop n */
            }
        }/*00000b00+0x4*n*/
        regsAddrPtr->TXQ.qcn.feedbackCalcConfigs = 0x00000040;
        regsAddrPtr->TXQ.qcn.txqECCConf = 0x00000130;
        regsAddrPtr->TXQ.qcn.descECCSingleErrorCntr = 0x00000120;
        regsAddrPtr->TXQ.qcn.descECCDoubleErrorCntr = 0x00000124;
        regsAddrPtr->TXQ.qcn.CNTargetAssignment = 0x00000004;
        regsAddrPtr->TXQ.qcn.CNDropCntr = 0x000000f0;
        regsAddrPtr->TXQ.qcn.QCNInterruptCause = 0x00000100;
        regsAddrPtr->TXQ.qcn.QCNInterruptMask = 0x00000110;
        regsAddrPtr->TXQ.qcn.CNDescAttributes = 0x00000008;
        regsAddrPtr->TXQ.qcn.CNBufferFIFOParityErrorsCntr = 0x000000e0;
        regsAddrPtr->TXQ.qcn.CNBufferFIFOOverrunsCntr = 0x000000d0;
        regsAddrPtr->TXQ.qcn.ECCStatus = 0x00000140;

    }/*end of unit qcn */

}


/*******************************************************************************
* txqLLInit
*
* DESCRIPTION:
*       This function inits the DB - TXQ_LL unit.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void txqLLInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);
    GT_U32  isSip5_10 = PRV_CPSS_SIP_5_10_CHECK_MAC(devNum);

    {/*start of unit ll */
        {/*start of unit global */
            {/*start of unit portsLinkEnableStatus */
                GT_U32    p;
                GT_U32    nMax = isSip5_20 ? 17 : 2;
                GT_U32    offset = isSip5_20 ? 0x00112100 : 0x000a0600;
                for(p = 0 ; p <= nMax ; p++) {
                    regsAddrPtr->TXQ.ll.global.portsLinkEnableStatus.portsLinkEnableStatus[p] =
                        offset + 0x4*p;
                }
            }/*end of unit portsLinkEnableStatus */


            {/*start of unit interrupts */
                regsAddrPtr->TXQ.ll.global.interrupts.LLInterruptCause =
                        isSip5_20 ? 0x00112008 : 0x000a0508;
                regsAddrPtr->TXQ.ll.global.interrupts.LLInterruptMask =
                        isSip5_20 ? 0x0011200c : 0x000a050c;

            }/*end of unit interrupts */


            {/*start of unit globalLLConfig */
                {/*000a0300+0x4*p*/
                    GT_U32    p;
                    GT_U32    offset = isSip5_20 ? 0x00111000 : 0x000a0300;
                    for(p = 0 ; p <= 15 ; p++) {
                        regsAddrPtr->TXQ.ll.global.globalLLConfig.profilePriorityQueueToPoolAssociation[p] =
                            offset+0x4*p;
                    }/* end of loop p */
                }/*000a0300+0x4*p*/
                {/*000a0040+n * 0x4*/
                    GT_U32    n;
                    GT_U32    nMax = isSip5_20 ? 575 : 71;
                    GT_U32    offset = isSip5_20 ? 0x00110444 :
                                       isSip5_10 ? 0x000a0044 : 0x000a0040;
                    for(n = 0 ; n <= nMax ; n++) {
                        regsAddrPtr->TXQ.ll.global.globalLLConfig.portProfile[n] =
                            offset+n * 0x4;
                    }/* end of loop n */
                }/*000a0040+n * 0x4*/
                {/*000a0004+n*0x4*/
                    GT_U32    n;
                    GT_U32    nMax = isSip5_20 ? 15 : 7;
                    GT_U32    offset = isSip5_20 ? 0x00110004 : 0x000a0004;

                    for(n = 0 ; n <= nMax ; n++) {
                        regsAddrPtr->TXQ.ll.global.globalLLConfig.highSpeedPort[n] =
                            offset+n*0x4;
                    }/* end of loop n */
                }/*000a0004+n*0x4*/

            }/*end of unit globalLLConfig */


        }/*end of unit global */
    }/*end of unit ll */

}


/*******************************************************************************
* txqLLLegacyDestroy
*
* DESCRIPTION:
*       This function destroys TXQ_LL registers from the legacy DB.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void txqLLLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* destroy SHAPER structure */
    DESTROY_STC_MAC(addrPtr->txqVer1.ll);

    return;
}


/*******************************************************************************
* txqPfcInit
*
* DESCRIPTION:
*       This function inits the DB - TXQ_PFC unit.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void txqPfcInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);

    {/*start of unit pfc */
        {/*start of unit PFCInterrupts */
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCPortGroupGlobalOverUnderflowInterruptCause = 0x00001330;
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCPortGroupGlobalOverUnderflowInterruptMask = 0x00001350;
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCInterruptSummaryCause = 0x00001300;
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCInterruptSummaryMask = 0x00001304;
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCParityInterruptCause = 0x00001310;
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCCntrsOverUnderFlowInterruptMask = 0x00001324;
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCParityInterruptMask = 0x00001314;
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCFIFOsOverrunsInterruptCause = 0x000013b0;
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCFIFOsOverrunsInterruptMask = 0x000013b4;
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCMsgsSetsOverrunsInterruptCause = 0x000013b8;
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCMsgsSetsOverrunsInterruptMask = 0x000013bc;
            regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCCntrsOverUnderFlowInterruptCause = 0x00001320;

        }/*end of unit PFCInterrupts */


        {/*00000170+0x4*n*/
            GT_U32    n;
            if(0 == isSip5_20)
            {
                for(n = 0 ; n <= 63 ; n++) {
                    regsAddrPtr->TXQ.pfc.LLFCTargetPortRemap[n] =
                        0x00000170+0x4*n;
                }/* end of loop n */
            }
            else
            {
                for(n = 0 ; n <= 143 ; n++) {
                    regsAddrPtr->TXQ.pfc.LLFCTargetPortRemap[n] =
                        0x000002F0+0x4*n;
                }/* end of loop n */
            }
        }/*00000170+0x4*n*/
        {/*00000010+0x4*n*/
            GT_U32    n;
            if(0 == isSip5_20)
            {
                for(n = 0 ; n <= 7 ; n++) {
                    regsAddrPtr->TXQ.pfc.portFCMode[n] =
                        0x00000010+0x4*n;
                }/* end of loop n */
            }
            else
            {
                for(n = 0 ; n <= 15 ; n++) {
                    regsAddrPtr->TXQ.pfc.portFCMode[n] =
                        0x000006F0+0x4*n;
                }/* end of loop n */
            }
        }/*00000010+0x4*n*/
        {/*00000900+p*4*/
            GT_U32    p;
            if(0 == isSip5_20)
            {
                for(p = 0 ; p <= 255 ; p++) {
                    regsAddrPtr->TXQ.pfc.PFCSourcePortToPFCIndexMap[p] =
                        0x00000900+p*4;
                }/* end of loop p */
            }
            else
            {
                for(p = 0 ; p <= 511 ; p++) {
                    regsAddrPtr->TXQ.pfc.PFCSourcePortToPFCIndexMap[p] =
                        0x00001C00+p*4;
                }/* end of loop p */
            }
        }/*00000900+p*4*/
        {/*000000f0+0x4*r*/
            GT_U32    r;
            if(0 == isSip5_20)
            {
                for(r = 0 ; r <= 31 ; r++) {
                    regsAddrPtr->TXQ.pfc.PFCPortProfile[r] =
                        0x000000f0+0x4*r;
                }/* end of loop r */
            }
            else
            {
                for(r = 0 ; r <= 63 ; r++) {
                    regsAddrPtr->TXQ.pfc.PFCPortProfile[r] =
                        0x000000f0+0x4*r;
                }/* end of loop r */
            }
        }/*000000f0+0x4*r*/
        {/*00000050+0x4*p*/
            GT_U32    p;
            for(p = 0 ; p <= 0 ; p++) {
                regsAddrPtr->TXQ.pfc.PFCPortGroupFlowCtrlIndicationFIFOOverrunEntry0[p] =
                    0x00000050+0x4*p;
            }/* end of loop p */
        }/*00000050+0x4*p*/
        {/*00000070+0x4*p*/
            GT_U32    p;
            for(p = 0 ; p <= 0 ; p++) {
                regsAddrPtr->TXQ.pfc.PFCPortGroupFlowCtrlIndicationFIFOOverrunEntry1[p] =
                    0x00000070+0x4*p;
            }/* end of loop p */
        }/*00000070+0x4*p*/
        {/*00000090+0x4*p*/
            GT_U32    p;
            for(p = 0 ; p <= 0 ; p++) {
                regsAddrPtr->TXQ.pfc.PFCPortGroupFlowCtrlIndicationFIFOOverrunCntr[p] =
                    0x00000090+0x4*p;
            }/* end of loop p */
        }/*00000090+0x4*p*/
        {/*000000b0+0x4*p*/
            GT_U32    p;
            for(p = 0 ; p <= 0 ; p++) {
                regsAddrPtr->TXQ.pfc.PFCPortGroupEqFIFOOverrunCntr[p] =
                    0x000000b0+0x4*p;
            }/* end of loop p */
        }/*000000b0+0x4*p*/
        {/*00000030+0x4*p*/
            GT_U32    p;
            for(p = 0 ; p <= 0 ; p++) {
                regsAddrPtr->TXQ.pfc.PFCPortGroupCntrsStatus[p] =
                    0x00000030+0x4*p;
            }/* end of loop p */
        }/*00000030+0x4*p*/
        {/*000000d0+0x4*p*/
            GT_U32    p;
            for(p = 0 ; p <= 0 ; p++) {
                regsAddrPtr->TXQ.pfc.PFCPortGroupCntrsParityErrorsCntr[p] =
                    0x000000d0+0x4*p;
            }/* end of loop p */
        }/*000000d0+0x4*p*/
        {/*00014200+0x20*p + 0x4*t*/
            GT_U32    t,p;
            for(t = 0 ; t <= 7 ; t++) {
                for(p = 0 ; p <= 0 ; p++) {
                    regsAddrPtr->TXQ.pfc.PFCGroupOfPortsTCXonThresholds[t][p] =
                        0x00014200+0x20*p + 0x4*t;
                }/* end of loop p */
            }/* end of loop t */
        }/*00014200+0x20*p + 0x4*t*/
        {/*00014000+0x20*p + 0x4*t*/
            GT_U32    t,p;
            for(t = 0 ; t <= 7 ; t++) {
                for(p = 0 ; p <= 0 ; p++) {
                    regsAddrPtr->TXQ.pfc.PFCGroupOfPortsTCXoffThresholds[t][p] =
                        0x00014000+0x20*p + 0x4*t;
                }/* end of loop p */
            }/* end of loop t */
        }/*00014000+0x20*p + 0x4*t*/
        {/*00014400+0x20*p + 0x4*t*/
            GT_U32    t,p;
            for(t = 0 ; t <= 7 ; t++) {
                for(p = 0 ; p <= 0 ; p++) {
                    regsAddrPtr->TXQ.pfc.PFCGroupOfPortsTCDropThresholds[t][p] =
                        0x00014400+0x20*p + 0x4*t;
                }/* end of loop p */
            }/* end of loop t */
        }/*00014400+0x20*p + 0x4*t*/
        {/*00000d00+0x20*p + 0x4*t*/
            GT_U32    p,t;
            for(p = 0 ; p <= 0 ; p++) {
                for(t = 0 ; t <= 7 ; t++) {
                    regsAddrPtr->TXQ.pfc.PFCGroupOfPortsTCCntr[p][t] =
                        0x00000d00+0x20*p + 0x4*t;
                }/* end of loop t */
            }/* end of loop p */
        }/*00000d00+0x20*p + 0x4*t*/
        regsAddrPtr->TXQ.pfc.PFCTriggerGlobalConfig = 0x00000000;

    }/*end of unit pfc */

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        {/*000000a0+0x4*p*/
            GT_U32    p;
            for(p = 0 ; p <= 0 ; p++) {
                regsAddrPtr->TXQ.pfc.PFCPortGroupFlowCtrlIndicationFIFOOverrunCntr[p] =
                    0x000000a0+0x4*p;
            }/* end of loop p */
        }/*000000a0+0x4*p*/
        {/*00000060+0x4*p*/
            GT_U32    p;
            for(p = 0 ; p <= 0 ; p++) {
                regsAddrPtr->TXQ.pfc.PFCPortGroupFlowCtrlIndicationFIFOOverrunEntry1[p] =
                    0x00000060+0x4*p;
            }/* end of loop p */
        }/*00000060+0x4*p*/

        regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCPortGroupGlobalOverUnderflowInterruptCause = 0x00001370;
        regsAddrPtr->TXQ.pfc.PFCInterrupts.PFCPortGroupGlobalOverUnderflowInterruptMask = 0x00001390;

        DESTROY_COMPILATION_ARRAY_REG_MAC(regsAddrPtr->TXQ.pfc.PFCPortGroupEqFIFOOverrunCntr);
    }

}


/*******************************************************************************
* txqQueueInit
*
* DESCRIPTION:
*       This function inits the DB - TXQ_Q unit.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void txqQueueInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*start of unit queue */
        {/*start of unit tailDrop */
            {/*start of unit tailDropLimits */
                {/*000a0a20+t*0x4*/
                    GT_U32    t;
                    for(t = 0 ; t <= 7 ; t++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropLimits.poolBufferLimits[t] =
                            0x000a0a20+t*0x4;
                    }/* end of loop t */
                }/*000a0a20+t*0x4*/
                {/*000a0a00+t*0x4*/
                    GT_U32    t;
                    for(t = 0 ; t <= 7 ; t++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropLimits.poolDescLimits[t] =
                            0x000a0a00+t*0x4;
                    }/* end of loop t */
                }/*000a0a00+t*0x4*/
                {/*000a0820+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 15 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropLimits.profilePortDescLimits[n] =
                            0x000a0820+0x4*n;
                    }/* end of loop n */
                }/*000a0820+0x4*n*/
                {/*000a0900+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 15 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropLimits.profilePortBufferLimits[n] =
                            0x000a0900+0x4*n;
                    }/* end of loop n */
                }/*000a0900+0x4*n*/
                regsAddrPtr->TXQ.queue.tailDrop.tailDropLimits.globalDescsLimit = 0x000a0800;
                regsAddrPtr->TXQ.queue.tailDrop.tailDropLimits.globalBufferLimit = 0x000a0810;

            }/*end of unit tailDropLimits */


            {/*start of unit tailDropCntrs */
                {/*000a4130+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 71 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropCntrs.portBuffersCntr[n] =
                            0x000a4130+n*0x4;
                    }/* end of loop n */
                }/*000a4130+n*0x4*/
                {/*000a4010+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 71 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropCntrs.portDescCntr[n] =
                            0x000a4010+n*0x4;
                    }/* end of loop n */
                }/*000a4010+n*0x4*/
                {/*000a4250+t*0x4*/
                    GT_U32    t;
                    for(t = 0 ; t <= 7 ; t++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropCntrs.priorityDescCntr[t] =
                            0x000a4250+t*0x4;
                    }/* end of loop t */
                }/*000a4250+t*0x4*/
                {/*000a4290+t*0x4*/
                    GT_U32    t;
                    for(t = 0 ; t <= 7 ; t++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropCntrs.priorityBuffersCntr[t] =
                            0x000a4290+t*0x4;
                    }/* end of loop t */
                }/*000a4290+t*0x4*/
                regsAddrPtr->TXQ.queue.tailDrop.tailDropCntrs.totalDescCntr = 0x000a4000;
                regsAddrPtr->TXQ.queue.tailDrop.tailDropCntrs.totalBuffersCntr = 0x000a4004;

            }/*end of unit tailDropCntrs */


            {/*start of unit tailDropConfig */
                {/*000a0200+n * 0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 71 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.portTailDropCNProfile[n] =
                            0x000a0200+n * 0x4;
                    }/* end of loop n */
                }/*000a0200+n * 0x4*/
                {/*000a0050+n * 0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 71 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.portEnqueueEnable[n] =
                            0x000a0050+n * 0x4;
                    }/* end of loop n */
                }/*000a0050+n * 0x4*/
                {/*000a0004+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= 15 ; p++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.profileTailDropConfig[p] =
                            0x000a0004+p*0x4;
                    }/* end of loop p */
                }/*000a0004+p*0x4*/
                {/*000a03d0+0x4*p*/
                    GT_U32    p;
                    for(p = 0 ; p <= 15 ; p++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.profilePriorityQueueToPoolAssociation[p] =
                            0x000a03d0+0x4*p;
                    }/* end of loop p */
                }/*000a03d0+0x4*p*/
                {/*000a03a0+t*0x4*/
                    GT_U32    t;
                    for(t = 0 ; t <= 7 ; t++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.profilePriorityQueueEnablePoolUsage[t] =
                            0x000a03a0+t*0x4;
                    }/* end of loop t */
                }/*000a03a0+t*0x4*/
                {/*000a05c0+0x4*p*/
                    GT_U32    p;
                    for(p = 0 ; p <= 15 ; p++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.profilePoolWRTDEn[p] =
                            0x000a05c0+0x4*p;
                    }/* end of loop p */
                }/*000a05c0+0x4*p*/
                {/*000a0580+0x4*p*/
                    GT_U32    p;
                    for(p = 0 ; p <= 15 ; p++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.profileQueueWRTDEn[p] =
                            0x000a0580+0x4*p;
                    }/* end of loop p */
                }/*000a0580+0x4*p*/
                {/*000a0540+0x4*p*/
                    GT_U32    p;
                    for(p = 0 ; p <= 15 ; p++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.profilePortWRTDEn[p] =
                            0x000a0540+0x4*p;
                    }/* end of loop p */
                }/*000a0540+0x4*p*/
                {/*000a0500+0x4*p*/
                    GT_U32    p;
                    for(p = 0 ; p <= 15 ; p++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.profileMaxQueueWRTDEn[p] =
                            0x000a0500+0x4*p;
                    }/* end of loop p */
                }/*000a0500+0x4*p*/
                {/*000a0600+0x4*p*/
                    GT_U32    p;
                    for(p = 0 ; p <= 15 ; p++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.profileECNMarkEn[p] =
                            0x000a0600+0x4*p;
                    }/* end of loop p */
                }/*000a0600+0x4*p*/
                {/*000a0360+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 15 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.profileCNMTriggeringEnable[n] =
                            0x000a0360+0x4*n;
                    }/* end of loop n */
                }/*000a0360+0x4*n*/
                regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.globalTailDropConfig = 0x000a0000;
                regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.pktLengthForTailDropEnqueue = 0x000a0044;
                regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.maxQueueWRTDMasks = 0x000a0440;
                regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.portWRTDMasks = 0x000a0444;
                regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.queueWRTDMasks = 0x000a0448;
                regsAddrPtr->TXQ.queue.tailDrop.tailDropConfig.poolWRTDMasks = 0x000a044c;

            }/*end of unit tailDropConfig */


            {/*start of unit resourceHistogram */
                {/*start of unit resourceHistogramLimits */
                    regsAddrPtr->TXQ.queue.tailDrop.resourceHistogram.resourceHistogramLimits.resourceHistogramLimitReg1 = 0x000a9500;
                    regsAddrPtr->TXQ.queue.tailDrop.resourceHistogram.resourceHistogramLimits.resourceHistogramLimitReg2 = 0x000a9504;

                }/*end of unit resourceHistogramLimits */


                {/*start of unit resourceHistogramCntrs */
                    {/*000a9510+n*0x4*/
                        GT_U32    n;
                        for(n = 0 ; n <= 3 ; n++) {
                            regsAddrPtr->TXQ.queue.tailDrop.resourceHistogram.resourceHistogramCntrs.resourceHistogramCntr[n] =
                                0x000a9510+n*0x4;
                        }/* end of loop n */
                    }/*000a9510+n*0x4*/

                }/*end of unit resourceHistogramCntrs */


            }/*end of unit resourceHistogram */


            {/*start of unit mcFilterLimits */
                regsAddrPtr->TXQ.queue.tailDrop.mcFilterLimits.mirroredPktsToAnalyzerPortDescsLimit = 0x000a9000;
                regsAddrPtr->TXQ.queue.tailDrop.mcFilterLimits.ingrMirroredPktsToAnalyzerPortBuffersLimit = 0x000a9004;
                regsAddrPtr->TXQ.queue.tailDrop.mcFilterLimits.egrMirroredPktsToAnalyzerPortBuffersLimit = 0x000a9010;
                regsAddrPtr->TXQ.queue.tailDrop.mcFilterLimits.mcDescsLimit = 0x000a9020;
                regsAddrPtr->TXQ.queue.tailDrop.mcFilterLimits.mcBuffersLimit = 0x000a9030;

            }/*end of unit mcFilterLimits */


            {/*start of unit muliticastFilterCntrs */
                regsAddrPtr->TXQ.queue.tailDrop.muliticastFilterCntrs.snifferDescsCntr = 0x000a9200;
                regsAddrPtr->TXQ.queue.tailDrop.muliticastFilterCntrs.ingrSnifferBuffersCntr = 0x000a9204;
                regsAddrPtr->TXQ.queue.tailDrop.muliticastFilterCntrs.egrSnifferBuffersCntr = 0x000a9208;
                regsAddrPtr->TXQ.queue.tailDrop.muliticastFilterCntrs.mcDescsCntr = 0x000a920c;
                regsAddrPtr->TXQ.queue.tailDrop.muliticastFilterCntrs.mcBuffersCntr = 0x000a9210;

            }/*end of unit muliticastFilterCntrs */


            {/*start of unit FCUAndQueueStatisticsLimits */
                {/*000aa1b0+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 7 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.FCUAndQueueStatisticsLimits.XONLimit[n] =
                            0x000aa1b0+n*0x4;
                    }/* end of loop n */
                }/*000aa1b0+n*0x4*/
                {/*000aa110+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 7 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.FCUAndQueueStatisticsLimits.XOFFLimit[n] =
                            0x000aa110+n*0x4;
                    }/* end of loop n */
                }/*000aa110+n*0x4*/
                {/*000aa0d0+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 15 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.FCUAndQueueStatisticsLimits.queueXONLimitProfile[n] =
                            0x000aa0d0+n*0x4;
                    }/* end of loop n */
                }/*000aa0d0+n*0x4*/
                {/*000aa090+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 15 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.FCUAndQueueStatisticsLimits.queueXOFFLimitProfile[n] =
                            0x000aa090+n*0x4;
                    }/* end of loop n */
                }/*000aa090+n*0x4*/
                {/*000aa050+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 15 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.FCUAndQueueStatisticsLimits.portXONLimitProfile[n] =
                            0x000aa050+n*0x4;
                    }/* end of loop n */
                }/*000aa050+n*0x4*/
                {/*000aa004+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 15 ; n++) {
                        regsAddrPtr->TXQ.queue.tailDrop.FCUAndQueueStatisticsLimits.portXOFFLimitProfile[n] =
                            0x000aa004+n*0x4;
                    }/* end of loop n */
                }/*000aa004+n*0x4*/
                regsAddrPtr->TXQ.queue.tailDrop.FCUAndQueueStatisticsLimits.globalXONLimit = 0x000aa230;
                regsAddrPtr->TXQ.queue.tailDrop.FCUAndQueueStatisticsLimits.globalXOFFLimit = 0x000aa210;
                regsAddrPtr->TXQ.queue.tailDrop.FCUAndQueueStatisticsLimits.FCUMode = 0x000aa000;

            }/*end of unit FCUAndQueueStatisticsLimits */


        }/*end of unit tailDrop */


        {/*start of unit global */
            {/*start of unit interrupt */
                {/*start of unit queueStatistics */
                    {/*000908c0+0x4*i*/
                        GT_U32    i;
                        for(i = 0 ; i <= 17 ; i++) {
                            regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.lowCrossingStatusReg[i] =
                                0x000908c0+0x4*i;
                        }/* end of loop i */
                    }/*000908c0+0x4*i*/
                    {/*00090680+p*0x4*/
                        GT_U32    p;
                        for(p = 0 ; p <= 71 ; p++) {
                            regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.highCrossingInterruptMaskPort[p] =
                                0x00090680+p*0x4;
                        }/* end of loop p */
                    }/*00090680+p*0x4*/
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.lowCrossingInterruptSummaryMaskPorts61To31 = 0x0009088c;
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.lowCrossingInterruptSummaryMaskPorts30To0 = 0x00090884;
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.lowCrossingInterruptSummaryCausePorts71To62 = 0x00090890;
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.lowCrossingInterruptSummaryCausePorts61To31 = 0x00090888;
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.lowCrossingInterruptSummaryCausePorts30To0 = 0x00090880;
                    {/*00090280+p*0x4*/
                        GT_U32    p;
                        for(p = 0 ; p <= 71 ; p++) {
                            regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.lowCrossingInterruptMaskPort[p] =
                                0x00090280+p*0x4;
                        }/* end of loop p */
                    }/*00090280+p*0x4*/
                    {/*00090080+0x4*p*/
                        GT_U32    p;
                        for(p = 0 ; p <= 71 ; p++) {
                            regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.lowCrossingInterruptCausePort[p] =
                                0x00090080+0x4*p;
                        }/* end of loop p */
                    }/*00090080+0x4*p*/
                    {/*000909a0+0x4*i*/
                        GT_U32    i;
                        for(i = 0 ; i <= 17 ; i++) {
                            regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.highCrossingStatusReg[i] =
                                0x000909a0+0x4*i;
                        }/* end of loop i */
                    }/*000909a0+0x4*i*/
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.highCrossingInterruptSummaryMaskPorts71To62 = 0x000908ac;
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.lowCrossingInterruptSummaryMaskPorts71To62 = 0x00090894;
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.highCrossingInterruptSummaryCausePorts30To0 = 0x00090898;
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.highCrossingInterruptSummaryMaskPorts30To0 = 0x0009089c;
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.highCrossingInterruptSummaryCausePorts61To31 = 0x000908a0;
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.highCrossingInterruptSummaryMaskPorts61To31 = 0x000908a4;
                    regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.highCrossingInterruptSummaryCausePorts71To62 = 0x000908a8;
                    {/*00090480+0x4*p*/
                        GT_U32    p;
                        for(p = 0 ; p <= 71 ; p++) {
                            regsAddrPtr->TXQ.queue.global.interrupt.queueStatistics.highCrossingInterruptCausePort[p] =
                                0x00090480+0x4*p;
                        }/* end of loop p */
                    }/*00090480+0x4*p*/

                }/*end of unit queueStatistics */


                regsAddrPtr->TXQ.queue.global.interrupt.transmitQueueInterruptSummaryCause = 0x00090000;
                regsAddrPtr->TXQ.queue.global.interrupt.transmitQueueInterruptSummaryMask = 0x00090004;
                regsAddrPtr->TXQ.queue.global.interrupt.generalInterrupt1Cause = 0x00090008;
                regsAddrPtr->TXQ.queue.global.interrupt.generalInterrupt1Mask = 0x0009000c;
                regsAddrPtr->TXQ.queue.global.interrupt.portDescFullInterruptCause0 = 0x00090040;
                regsAddrPtr->TXQ.queue.global.interrupt.portDescFullInterruptMask0 = 0x00090044;
                regsAddrPtr->TXQ.queue.global.interrupt.portDescFullInterruptCause1 = 0x00090048;
                regsAddrPtr->TXQ.queue.global.interrupt.portDescFullInterruptMask1 = 0x0009004c;
                regsAddrPtr->TXQ.queue.global.interrupt.portDescFullInterruptCause2 = 0x00090050;
                regsAddrPtr->TXQ.queue.global.interrupt.portDescFullInterruptMask2 = 0x00090054;
                regsAddrPtr->TXQ.queue.global.interrupt.portBufferFullInterruptCause0 = 0x00090060;
                regsAddrPtr->TXQ.queue.global.interrupt.portBufferFullInterruptMask0 = 0x00090064;
                regsAddrPtr->TXQ.queue.global.interrupt.portBufferFullInterruptCause1 = 0x00090068;
                regsAddrPtr->TXQ.queue.global.interrupt.portBufferFullInterruptMask1 = 0x0009006c;
                regsAddrPtr->TXQ.queue.global.interrupt.portBufferFullInterruptCause2 = 0x00090070;
                regsAddrPtr->TXQ.queue.global.interrupt.portBufferFullInterruptMask2 = 0x00090074;

            }/*end of unit interrupt */


            {/*start of unit ECCCntrs */
                regsAddrPtr->TXQ.queue.global.ECCCntrs.TDClrECCErrorCntr = 0x00090a40;

            }/*end of unit ECCCntrs */


            {/*start of unit dropCntrs */
                {/*00090a00+p*0x4*/
                    GT_U32    p;
                    for(p = 0 ; p <= 3 ; p++) {
                        regsAddrPtr->TXQ.queue.global.dropCntrs.clearPktsDroppedCntrPipe[p] =
                            0x00090a00+p*0x4;
                    }/* end of loop p */
                }/*00090a00+p*0x4*/

            }/*end of unit dropCntrs */


        }/*end of unit global */


        {/*start of unit peripheralAccess */
            {/*start of unit peripheralAccessMisc */
                {/*00093004+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 3 ; n++) {
                        regsAddrPtr->TXQ.queue.peripheralAccess.peripheralAccessMisc.portGroupDescToEQCntr[n] =
                            0x00093004+0x4*n;
                    }/* end of loop n */
                }/*00093004+0x4*n*/
                regsAddrPtr->TXQ.queue.peripheralAccess.peripheralAccessMisc.peripheralAccessMisc = 0x00093000;
                regsAddrPtr->TXQ.queue.peripheralAccess.peripheralAccessMisc.QCNIncArbiterCtrl = 0x00093020;

            }/*end of unit peripheralAccessMisc */


            {/*start of unit egrMIBCntrs */
                {/*00093208+n*0x4*/
                    GT_U32    n;
                    for(n = 0 ; n <= 1 ; n++) {
                        regsAddrPtr->TXQ.queue.peripheralAccess.egrMIBCntrs.txQMIBCntrsPortSetConfig[n] =
                            0x00093208+n*0x4;
                    }/* end of loop n */
                }/*00093208+n*0x4*/
                {/*00093200+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 1 ; n++) {
                        regsAddrPtr->TXQ.queue.peripheralAccess.egrMIBCntrs.txQMIBCntrsSetConfig[n] =
                            0x00093200+0x4*n;
                    }/* end of loop n */
                }/*00093200+0x4*n*/
                {/*00093250+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 1 ; n++) {
                        regsAddrPtr->TXQ.queue.peripheralAccess.egrMIBCntrs.setTailDroppedPktCntr[n] =
                            0x00093250+0x4*n;
                    }/* end of loop n */
                }/*00093250+0x4*n*/
                {/*00093210+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 1 ; n++) {
                        regsAddrPtr->TXQ.queue.peripheralAccess.egrMIBCntrs.setOutgoingUcPktCntr[n] =
                            0x00093210+0x4*n;
                    }/* end of loop n */
                }/*00093210+0x4*n*/
                {/*00093220+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 1 ; n++) {
                        regsAddrPtr->TXQ.queue.peripheralAccess.egrMIBCntrs.setOutgoingMcPktCntr[n] =
                            0x00093220+0x4*n;
                    }/* end of loop n */
                }/*00093220+0x4*n*/
                {/*00093230+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 1 ; n++) {
                        regsAddrPtr->TXQ.queue.peripheralAccess.egrMIBCntrs.setOutgoingBcPktCntr[n] =
                            0x00093230+0x4*n;
                    }/* end of loop n */
                }/*00093230+0x4*n*/
                {/*00093290+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 1 ; n++) {
                        regsAddrPtr->TXQ.queue.peripheralAccess.egrMIBCntrs.setMcFIFO7_4DroppedPktsCntr[n] =
                            0x00093290+0x4*n;
                    }/* end of loop n */
                }/*00093290+0x4*n*/
                {/*00093280+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 1 ; n++) {
                        regsAddrPtr->TXQ.queue.peripheralAccess.egrMIBCntrs.setMcFIFO3_0DroppedPktsCntr[n] =
                            0x00093280+0x4*n;
                    }/* end of loop n */
                }/*00093280+0x4*n*/
                {/*00093260+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 1 ; n++) {
                        regsAddrPtr->TXQ.queue.peripheralAccess.egrMIBCntrs.setCtrlPktCntr[n] =
                            0x00093260+0x4*n;
                    }/* end of loop n */
                }/*00093260+0x4*n*/
                {/*00093240+0x4*n*/
                    GT_U32    n;
                    for(n = 0 ; n <= 1 ; n++) {
                        regsAddrPtr->TXQ.queue.peripheralAccess.egrMIBCntrs.setBridgeEgrFilteredPktCntr[n] =
                            0x00093240+0x4*n;
                    }/* end of loop n */
                }/*00093240+0x4*n*/

            }/*end of unit egrMIBCntrs */


            {/*start of unit CNCModes */
                regsAddrPtr->TXQ.queue.peripheralAccess.CNCModes.CNCModes = 0x000936a0;

            }/*end of unit CNCModes */


        }/*end of unit peripheralAccess */


    }/*end of unit queue */

}


/*******************************************************************************
* txqQueueLegacyDestroy
*
* DESCRIPTION:
*       This function destroys TXQ_Q registers from the legacy DB.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void txqQueueLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* destroy SHAPER structure */
    DESTROY_STC_MAC(addrPtr->txqVer1.queue);

    return;
}


/*******************************************************************************
* tmDropInit
*
* DESCRIPTION:
*       init the DB - TM DROP unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void tmDropInit
(
    IN  GT_U8  devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*0000000+k*0x04*/
        GT_U32    k;
        for (k = 0 ; k <= 7 ; k++)
        {
            regsAddrPtr->TMDROP.portsInterface[k] =
                0x0000000 + (k * 0x04);
        }/* end of loop k */
    }/*0000000+k*0x04*/
    {/*0000020+k*0x04*/
        GT_U32    k;
        for (k = 0 ; k <= 1 ; k++)
        {
            regsAddrPtr->TMDROP.TMDROPTMTCMap8To87[k] =
                0x0000020 + (k * 0x04);
        }/* end of loop k */
    }/*0000020+k*0x04*/
    regsAddrPtr->TMDROP.globalConfigs = 0x00000028;
    regsAddrPtr->TMDROP.TMDROPInterruptCause = 0x00000030;
    regsAddrPtr->TMDROP.TMDROPInterruptMask = 0x00000034;
}

/*******************************************************************************
* tmQMapInit
*
* DESCRIPTION:
*       init the DB - TMQMAP unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void tmQMapInit
(
    IN  GT_U8  devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*0000004+k*0x04*/
        GT_U32    k;
        for(k = 0; (k <= 5); k++) {
            regsAddrPtr->TMQMAP.TC2TCMap8To16[k] =
                0x0000004 + (k * 0x04);
        }/* end of loop k */
    }/*0000004+k*0x04*/

    regsAddrPtr->TMQMAP.TMQMAPGlobalConfig   = 0x00000000;
    regsAddrPtr->TMQMAP.TMQMAPInterruptCause = 0x00000020;
    regsAddrPtr->TMQMAP.TMQMAPInterruptMask  = 0x00000024;
}

/*******************************************************************************
* tmIngressGlueInit
*
* DESCRIPTION:
*       init the DB - TM_INGRESS_GLUE unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void tmIngressGlueInit
(
    IN  GT_U8  devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    regsAddrPtr->TM_INGR_GLUE.globalConfig             = 0x00000000;
    regsAddrPtr->TM_INGR_GLUE.debug                    = 0x00000004;
    regsAddrPtr->TM_INGR_GLUE.TMIngrGlueInterruptCause = 0x00000008;
    regsAddrPtr->TM_INGR_GLUE.TMIngrGlueInterruptMask  = 0x0000000C;
}

/*******************************************************************************
* tmEgressGlueInit
*
* DESCRIPTION:
*       init the DB - TM_EGRESS_GLUE unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void tmEgressGlueInit
(
    IN  GT_U8  devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*00000600+4*c*/
        GT_U32    c;
        for  (c = 0; (c <= 99); c++)
        {
            regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrMsb[c] =
                0x00000600 + (4 * c);
        }/* end of loop c */
    }/*00000600+4*c*/
    {/*00000400+c*4*/
        GT_U32    c;
        for  (c = 0; (c <= 99); c++)
        {
            regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrLsb[c] =
                0x00000400 + (4 * c);
        }/* end of loop c */
    }/*00000400+c*4*/
    regsAddrPtr->TM_EGR_GLUE.TMEgrGlueInterruptMask = 0x00000028;
    regsAddrPtr->TM_EGR_GLUE.TMEgrGlueInterruptCause = 0x00000024;
    {/*00001400+4*c*/
        GT_U32    c;
        for  (c = 0; (c <= 99); c++)
        {
            regsAddrPtr->TM_EGR_GLUE.statisticalMinAgeTime[c] =
                0x00001400 + (4 * c);
        }/* end of loop c */
    }/*00001400+4*c*/
    regsAddrPtr->TM_EGR_GLUE.statisticalMinAgeTimeShadow = 0x0000200c;
    {/*00001200+4*c*/
        GT_U32    c;
        for  (c = 0; (c <= 99); c++)
        {
            regsAddrPtr->TM_EGR_GLUE.statisticalMaxAgeTime[c] =
                0x00001200 + (4 * c);
        }/* end of loop c */
    }/*00001200+4*c*/
    regsAddrPtr->TM_EGR_GLUE.statisticalMaxAgeTimeShadow = 0x00002008;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrsWraparoundInterruptMask3 = 0x0000011c;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrsWraparoundInterruptMask2 = 0x00000118;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrsWraparoundInterruptMask1 = 0x00000114;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrsWraparoundInterruptMask0 = 0x00000110;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrsWraparoundInterruptCause3 = 0x0000010c;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrsWraparoundInterruptCause2 = 0x00000108;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrsWraparoundInterruptCause1 = 0x00000104;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrsWraparoundInterruptCause0 = 0x00000100;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrsMsbShadow = 0x00002004;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingTimeCntrsLsbShadow = 0x00002000;
    regsAddrPtr->TM_EGR_GLUE.TMEgrGlueSummaryCause = 0x0000002c;
    regsAddrPtr->TM_EGR_GLUE.TMEgrGlueSummaryMask = 0x00000030;
    {/*00000200+c*4*/
        GT_U32    c;
        for  (c = 0; (c <= 99); c++)
        {
            regsAddrPtr->TM_EGR_GLUE.statisticalAgingQueueID[c] =
                0x00000200 + (4 * c);
        }/* end of loop c */
    }/*00000200+c*4*/
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingPktCntrsWraparoundInterruptMask3 = 0x00000054;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingPktCntrsWraparoundInterruptMask2 = 0x00000050;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingPktCntrsWraparoundInterruptMask1 = 0x0000004c;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingPktCntrsWraparoundInterruptMask0 = 0x00000048;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingPktCntrsWraparoundInterruptCause3 = 0x00000044;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingPktCntrsWraparoundInterruptCause2 = 0x00000040;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingPktCntrsWraparoundInterruptCause1 = 0x0000003c;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingPktCntrsWraparoundInterruptCause0 = 0x00000038;
    {/*00000800+c*4*/
        GT_U32    c;
        for  (c = 0; (c <= 99); c++)
        {
            regsAddrPtr->TM_EGR_GLUE.statisticalAgingPktCntr[c] =
                0x00000800 + (4 * c);
        }/* end of loop c */
    }/*00000800+c*4*/
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrsWraparoundInterruptMask3 = 0x0000013c;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrsWraparoundInterruptMask2 = 0x00000138;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrsWraparoundInterruptMask1 = 0x00000134;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrsWraparoundInterruptMask0 = 0x00000130;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrsWraparoundInterruptCause3 = 0x0000012c;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrsWraparoundInterruptCause2 = 0x00000128;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrsWraparoundInterruptCause1 = 0x00000124;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrsWraparoundInterruptCause0 = 0x00000120;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrsMsbShadow = 0x00002014;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrsLsbShadow = 0x00002010;
    {/*00001000+c*4*/
        GT_U32    c;
        for  (c = 0; (c <= 99); c++)
        {
            regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrMsb[c] =
                0x00001000 + (4 * c);
        }/* end of loop c */
    }/*00001000+c*4*/
    {/*00000c00+c*4*/
        GT_U32    c;
        for  (c = 0; (c <= 99); c++)
        {
            regsAddrPtr->TM_EGR_GLUE.statisticalAgingOctetCntrLsb[c] =
                0x00000c00 + (4 * c);
        }/* end of loop c */
    }/*00000c00+c*4*/
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingAgedOutPktCntrsWraparoundInterruptMask3 = 0x0000015c;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingAgedOutPktCntrsWraparoundInterruptMask2 = 0x00000158;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingAgedOutPktCntrsWraparoundInterruptMask1 = 0x00000154;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingAgedOutPktCntrsWraparoundInterruptMask0 = 0x00000150;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingAgedOutPktCntrsWraparoundInterruptCause3 = 0x0000014c;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingAgedOutPktCntrsWraparoundInterruptCause2 = 0x00000148;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingAgedOutPktCntrsWraparoundInterruptCause1 = 0x00000144;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingAgedOutPktCntrsWraparoundInterruptCause0 = 0x00000140;
    regsAddrPtr->TM_EGR_GLUE.statisticalAgingAgedOutPktCntrsShadow = 0x00002018;
    {/*00001600+c*4*/
        GT_U32    c;
        for  (c = 0; (c <= 99); c++)
        {
            regsAddrPtr->TM_EGR_GLUE.statisticalAgingAgedOutPktCntr[c] =
                0x00001600 + (4 * c);
        }/* end of loop c */
    }/*00001600+c*4*/
    regsAddrPtr->TM_EGR_GLUE.queueTimerConfig = 0x00000064;
    regsAddrPtr->TM_EGR_GLUE.queueTimer = 0x00000060;
    regsAddrPtr->TM_EGR_GLUE.portInterface2 = 0x00000078;
    regsAddrPtr->TM_EGR_GLUE.portInterface1 = 0x00000074;
    regsAddrPtr->TM_EGR_GLUE.portInterface0 = 0x00000070;
    {/*00001a00+c*4*/
        GT_U32    c;
        for  (c = 0; (c <= 191); c++)
        {
            regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntr[c] =
                0x00001a00 + (4 * c);
        }/* end of loop c */
    }/*00001a00+c*4*/
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptMask6 = 0x00002058;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptMask5 = 0x00002054;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptMask4 = 0x00002050;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptMask3 = 0x0000204c;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptMask2 = 0x00002048;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptMask1 = 0x00002044;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptMask0 = 0x00002040;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptCause6 = 0x00002038;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptCause5 = 0x00002034;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptCause4 = 0x00002030;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptCause3 = 0x0000202c;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptCause2 = 0x00002028;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptCause1 = 0x00002024;
    regsAddrPtr->TM_EGR_GLUE.perPortAgedOutPktCntrsWraparoundInterruptCause0 = 0x00002020;
    regsAddrPtr->TM_EGR_GLUE.globalConfig = 0x00000000;
    regsAddrPtr->TM_EGR_GLUE.ECCStatus = 0x0000000c;
    {/*00002100+4*c*/
        GT_U32    c;
        for  (c = 0; (c <= 191); c++)
        {
            regsAddrPtr->TM_EGR_GLUE.dropPortAllOutgoingPkts[c] =
                0x00002100 + (4 * c);
        }/* end of loop c */
    }/*00002100+4*c*/
    regsAddrPtr->TM_EGR_GLUE.dropAgedPktCntrConfig = 0x00000004;
    regsAddrPtr->TM_EGR_GLUE.dropAgedPktCntr = 0x00000008;
    regsAddrPtr->TM_EGR_GLUE.debug = 0x00000010;
}

/*******************************************************************************
* tmFcuInit
*
* DESCRIPTION:
*       init the DB - TM_FCU unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void tmFcuInit
(
    IN  GT_U8  devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*0000200 + (4 * w)*/
        GT_U32    w;
        for(w = 0; (w <= 63); w++)
        {
            regsAddrPtr->TM_FCU.TMEgrFlowCtrlCntrs[w] =
                0x0000200 + (4 * w);
        }/* end of loop w */
    }/*0000200 + (4 * w)*/
    {/*0000300 + (4 * w)*/
        GT_U32    w;
        for(w = 0; (w <= 63); w++)
        {
            regsAddrPtr->TM_FCU.TMEgrFlowCtrlXOFFThresholds[w] =
                0x0000300 + (4 * w);
        }/* end of loop w */
    }/*0000300 + (4 * w)*/
    {/*0000400 + (4 * w)*/
        GT_U32    w;
        for(w = 0; (w <= 63); w++)
        {
            regsAddrPtr->TM_FCU.TMEgrFlowCtrlXONThresholds[w] =
                0x0000400 + (4 * w);
        }/* end of loop w */
    }/*0000400 + (4 * w)*/
    regsAddrPtr->TM_FCU.TMFCUGlobalConfigs = 0x00000000;
}

/*******************************************************************************
* dfxLegacyDestroy
*
* DESCRIPTION:
*       destroy DFX registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void dfxLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    DESTROY_STC_MAC(addrPtr->dfxUnits);

    return;
}

/*******************************************************************************
* lmsInit
*
* DESCRIPTION:
*       init the DB - LMS unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*       instance    - CNC instance number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void lmsInit
(
    IN  GT_U8  devNum,
    IN  GT_U32 instance
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    {/*start of unit LMS_0 */
        {/*start of unit LMS0 */
            {/*start of unit LMS0Group0 */
                {/*start of unit SMIConfig */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.SMIConfig.PHYAddrReg0forPorts0Through5 = 0x00004030;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.SMIConfig.PHYAutoNegConfigReg0 = 0x00004034;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.SMIConfig.SMI0Management = 0x00004054;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.SMIConfig.LMS0MiscConfigs = 0x00004200;

                }/*end of unit SMIConfig */


                {/*start of unit portsMIBCntrsInterrupts */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.portsMIBCntrsInterrupts.portsGOP0MIBsInterruptCause = 0x00004000;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.portsMIBCntrsInterrupts.portsGOP0MIBsInterruptMask = 0x00004004;

                }/*end of unit portsMIBCntrsInterrupts */


                {/*start of unit portsMACSourceAddr */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.portsMACSourceAddr.sourceAddrMiddle = 0x00004024;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.portsMACSourceAddr.sourceAddrHigh = 0x00004028;

                }/*end of unit portsMACSourceAddr */


                {/*start of unit portsMACMIBCntrs */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.portsMACMIBCntrs.MIBCntrsCtrlReg0forPorts0Through5 = 0x00004020;

                }/*end of unit portsMACMIBCntrs */


                {/*start of unit portsAndMIBCntrsInterruptSummary */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.portsAndMIBCntrsInterruptSummary.MIBCntrsInterruptSummary = 0x00004010;

                }/*end of unit portsAndMIBCntrsInterruptSummary */


                {/*start of unit periodicFCRegs */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.periodicFCRegs.gigPortsPeriodicFCCntrMaxValueReg = 0x00004140;

                }/*end of unit periodicFCRegs */


                {/*start of unit LEDInterfaceConfig */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.LEDInterfaceConfig.LEDInterface0CtrlReg0forPorts0Through11AndStackPort = 0x00004100;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.LEDInterfaceConfig.LEDInterface0Group01ConfigRegforPorts0Through11 = 0x00004104;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.LEDInterfaceConfig.LEDInterface0Class01ManipulationRegforPorts0Through11 = 0x00004108;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.LEDInterfaceConfig.LEDInterface0Class4ManipulationRegforPorts0Through11 = 0x0000410c;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.LEDInterfaceConfig.stackLEDInterface0Class04ManipulationRegforStackPorts = 0x00005100;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.LEDInterfaceConfig.stackLEDInterface0Class59ManipulationRegforStackPorts = 0x00005104;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.LEDInterfaceConfig.LEDInterface0FlexlinkPortsDebugSelectRegforStackPort = 0x00005110;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group0.LEDInterfaceConfig.LEDInterface0FlexlinkPortsDebugSelectReg1forStackPort = 0x00005114;

                }/*end of unit LEDInterfaceConfig */


            }/*end of unit LMS0Group0 */


            {/*start of unit LMS0Group1 */
                {/*start of unit SMIConfig */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.SMIConfig.flowCtrlAdvertiseForFiberMediaSelectedConfigReg0forPorts0Through11 = 0x01004024;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.SMIConfig.PHYAddrReg1forPorts6Through11 = 0x01004030;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.SMIConfig.PHYAutoNegConfigReg1 = 0x01004034;

                }/*end of unit SMIConfig */


                {/*start of unit portsMIBCntrsInterrupts */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.portsMIBCntrsInterrupts.triSpeedPortsGOP1MIBsInterruptCause = 0x01004000;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.portsMIBCntrsInterrupts.triSpeedPortsGOP1MIBsInterruptMask = 0x01004004;

                }/*end of unit portsMIBCntrsInterrupts */


                {/*start of unit portsMACMIBCntrs */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.portsMACMIBCntrs.MIBCntrsCtrlReg0forPorts6Through11 = 0x01004020;

                }/*end of unit portsMACMIBCntrs */


                {/*start of unit portsAndMIBCntrsInterruptSummary */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.portsAndMIBCntrsInterruptSummary.portsMIBCntrsInterruptSummaryMask = 0x01005110;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.portsAndMIBCntrsInterruptSummary.portsInterruptSummaryMask = 0x01005114;

                }/*end of unit portsAndMIBCntrsInterruptSummary */


                {/*start of unit periodicFCRegs */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.periodicFCRegs.twoAndHalfGigPortsPeriodicFCCntrMaxValueReg = 0x01004140;

                }/*end of unit periodicFCRegs */


                {/*start of unit LEDInterfaceConfig */
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.LEDInterfaceConfig.LEDInterface0CtrlReg1AndClass6ManipulationRegforPorts0Through11 = 0x01004100;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.LEDInterfaceConfig.LEDInterface0Group23ConfigRegforPorts0Through11 = 0x01004104;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.LEDInterfaceConfig.LEDInterface0Class23ManipulationRegforPorts0Through11 = 0x01004108;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.LEDInterfaceConfig.LEDInterface0Class5ManipulationRegforPorts0Through11 = 0x0100410c;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.LEDInterfaceConfig.stackLEDInterface0Class1011ManipulationRegforStackPorts = 0x01005100;
                    regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.LEDInterfaceConfig.stackLEDInterface0Group01ConfigRegforStackPort = 0x01005104;

                }/*end of unit LEDInterfaceConfig */


                regsAddrPtr->GOP.LMS[instance].LMS0.LMS0Group1.stackPortsModeReg = 0x01004144;

            }/*end of unit LMS0Group1 */


        }/*end of unit LMS0 */


        {/*start of unit LMS1 */
            {/*start of unit LMS1Group0 */
                {/*start of unit SMIConfig */
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.SMIConfig.PHYAddrReg2forPorts12Through17 = 0x02004030;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.SMIConfig.PHYAutoNegConfigReg2 = 0x02004034;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.SMIConfig.SMI1Management = 0x02004054;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.SMIConfig.LMS1MiscConfigs = 0x02004200;

                }/*end of unit SMIConfig */


                {/*start of unit portsMIBCntrsInterrupts */
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.portsMIBCntrsInterrupts.portsGOP2MIBsInterruptCause = 0x02004000;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.portsMIBCntrsInterrupts.portsGOP2MIBsInterruptMask = 0x02004004;

                }/*end of unit portsMIBCntrsInterrupts */


                {/*start of unit portsMACMIBCntrs */
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.portsMACMIBCntrs.MIBCntrsCtrlReg0forPorts12Through17 = 0x02004020;

                }/*end of unit portsMACMIBCntrs */


                {/*start of unit portsInterruptSummary */
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.portsInterruptSummary.portsInterruptSummary = 0x02004010;

                }/*end of unit portsInterruptSummary */


                {/*start of unit LEDInterfaceConfig */
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.LEDInterfaceConfig.LEDInterface1CtrlReg0forPorts12Through23AndStackPort = 0x02004100;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.LEDInterfaceConfig.LEDInterface1Group01ConfigRegforPorts12Through23 = 0x02004104;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.LEDInterfaceConfig.LEDInterface1Class01ManipulationRegforPorts12Through23 = 0x02004108;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.LEDInterfaceConfig.LEDInterface1Class4ManipulationRegforPorts12Through23 = 0x0200410c;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.LEDInterfaceConfig.stackLEDInterface1Class04ManipulationRegforStackPorts = 0x02005100;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.LEDInterfaceConfig.stackLEDInterface1Class59ManipulationRegforStackPorts = 0x02005104;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.LEDInterfaceConfig.LEDInterface1FlexlinkPortsDebugSelectRegforStackPort = 0x02005110;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group0.LEDInterfaceConfig.LEDInterface1FlexlinkPortsDebugSelectReg1forStackPort = 0x02005114;

                }/*end of unit LEDInterfaceConfig */


            }/*end of unit LMS1Group0 */


            {/*start of unit LMS1Group1 */
                {/*start of unit SMIConfig */
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.SMIConfig.flowCtrlAdvertiseForFiberMediaSelectedConfigReg1forPorts12Through23 = 0x03004024;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.SMIConfig.PHYAddrReg3forPorts18Through23 = 0x03004030;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.SMIConfig.PHYAutoNegConfigReg3 = 0x03004034;

                }/*end of unit SMIConfig */


                {/*start of unit portsMIBCntrsInterrupts */
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.portsMIBCntrsInterrupts.portsGOP3AndStackMIBsInterruptCause = 0x03004000;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.portsMIBCntrsInterrupts.portsGOP3AndStackMIBsInterruptMask = 0x03004004;

                }/*end of unit portsMIBCntrsInterrupts */


                {/*start of unit portsMACMIBCntrs */
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.portsMACMIBCntrs.MIBCntrsCtrlReg0forPorts18Through23 = 0x03004020;

                }/*end of unit portsMACMIBCntrs */


                {/*start of unit periodicFCRegs */
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.periodicFCRegs.stackPorts1GPeriodicFCCntrMaxValueReg = 0x03004140;

                }/*end of unit periodicFCRegs */


                {/*start of unit LEDInterfaceConfig */
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.LEDInterfaceConfig.LEDInterface1CtrlReg1AndClass6ManipulationforPorts12Through23 = 0x03004100;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.LEDInterfaceConfig.LEDInterface1Group23ConfigRegforPorts12Through23 = 0x03004104;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.LEDInterfaceConfig.LEDInterface1Class23ManipulationRegforPorts12Through23 = 0x03004108;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.LEDInterfaceConfig.LEDInterface1Class5ManipulationRegforPorts12Through23 = 0x0300410c;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.LEDInterfaceConfig.stackLEDInterface1Class1011ManipulationRegforStackPort = 0x03005100;
                    regsAddrPtr->GOP.LMS[instance].LMS1.LMS1Group1.LEDInterfaceConfig.stackLEDInterface1Group01ConfigRegforStackPort = 0x03005104;

                }/*end of unit LEDInterfaceConfig */


            }/*end of unit LMS1Group1 */


        }/*end of unit LMS1 */


    }/*end of unit LMS_0 */
}


/*******************************************************************************
* smiInit
*
* DESCRIPTION:
*       init the DB - SMI unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - The PP's device number to init the struct for.
*       instanceCnt    - CNC instance number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void prv_smiInstanceInit
(
    IN  GT_U8  devNum,
    IN  GT_U32 instanceCnt
)
{
    GT_U32 phyRegCnt;
    GT_U32 phyRegN;

    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    regsAddrPtr->GOP.SMI[instanceCnt].SMIManagement            = 0x00000000;
    regsAddrPtr->GOP.SMI[instanceCnt].SMIMiscConfiguration     = 0x00000004;
    regsAddrPtr->GOP.SMI[instanceCnt].PHYAutoNegotiationConfig = 0x00000008;

    phyRegN = sizeof(regsAddrPtr->GOP.SMI[instanceCnt].PHYAddress)/sizeof(regsAddrPtr->GOP.SMI[instanceCnt].PHYAddress[0]);
    for (phyRegCnt = 0 ; phyRegCnt < phyRegN; phyRegCnt++)
    {
        regsAddrPtr->GOP.SMI[instanceCnt].PHYAddress[phyRegCnt] = 0x0000000C + 0x4*phyRegCnt;
    }
}

/*******************************************************************************
* smiInit
*
* DESCRIPTION:
*       init the DB - SMI unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void prv_smiInit
(
    IN  GT_U8  devNum
)
{
    GT_U32 i;
    GT_U32 instanceCnt;
    GT_U32  baseAddr;
    PRV_CPSS_DXCH_PP_CONFIG_STC * pDev;

    pDev = PRV_CPSS_DXCH_PP_MAC(devNum);
    for(i = 0 ; pDev->hwInfo.smi_support.activeSMIList[i] != (GT_U32)CPSS_PHY_SMI_INTERFACE_INVALID_E; i++)
    {
        instanceCnt = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.smi_support.activeSMIList[i];
        prv_smiInstanceInit(devNum,instanceCnt);
        baseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,(PRV_CPSS_DXCH_UNIT_ENT)(PRV_CPSS_DXCH_UNIT_SMI_0_E + instanceCnt),NULL);
        /* set the addresses of the SMI unit registers according to the needed base address */
        regUnitBaseAddrSet(
            REG_UNIT_INFO_MAC(devNum,baseAddr,GOP.SMI[instanceCnt]));
    }
}



/*******************************************************************************
* prv_ledInstanceInit
*
* DESCRIPTION:
*       init the DB - LMS unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - The PP's device number to init the struct for.
*       instanceCnt    - CNC instance number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void prv_ledInstanceInit
(
    IN  GT_U8  devNum,
    IN  GT_U32 instanceCnt
)
{
    GT_U32  ii;
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  offset = 0;

    /* NOTE: this function NOT applicable for bc2_A0 !!! */
#if 0
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_NONE_E &&   /* BC2_B0*/
           instanceCnt == 3)
        {
            /* strange offset for those addresses ...
               take the offset with in the lower 24 bits */
            offset = 0x00FFFFFF & (0xbc00000 * instanceCnt);
        }
    }
#endif

    regsAddrPtr->GOP.LED[instanceCnt].LEDControl                 = offset + 0x00000000;
    regsAddrPtr->GOP.LED[instanceCnt].blinkGlobalControl         = offset + 0x00000004;
    regsAddrPtr->GOP.LED[instanceCnt].classGlobalControl         = offset + 0x00000008;
    regsAddrPtr->GOP.LED[instanceCnt].classesAndGroupConfig      = offset + 0x0000000C;
    for(ii = 0 ; ii < 6 ; ii++)
    {
        regsAddrPtr->GOP.LED[instanceCnt].classForcedData[ii] =
            offset + 0x00000010 + 4*ii;
    }
    for(ii = 0 ; ii < 12 ; ii++)
    {
        regsAddrPtr->GOP.LED[instanceCnt].portIndicationSelect[ii] =
            offset + 0x00000028 + 4*ii;
    }
    for(ii = 0 ; ii < 1 ; ii++)
    {
        regsAddrPtr->GOP.LED[instanceCnt].portTypeConfig[ii] =
            offset + 0x00000120 + 4*ii;
    }
    for(ii = 0 ; ii < 3 ; ii++)
    {
        regsAddrPtr->GOP.LED[instanceCnt].portIndicationPolarity[ii] =
            offset + 0x00000128 + 4*ii;
    }
    for(ii = 0 ; ii < 6 ; ii++)
    {
        regsAddrPtr->GOP.LED[instanceCnt].classRawData[ii] =
            offset + 0x00000148 + 4*ii;
    }
}

/*******************************************************************************
* prv_ledInit
*
* DESCRIPTION:
*       init the DB - LED unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*       instance    - CNC instance number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void prv_ledInit
(
    IN  GT_U8  devNum
)
{
    GT_U32 baseAddr;
    GT_U32 instanceCnt;
    GT_U32 instanceN;
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);

    instanceN = isSip5_20 ? 4 : 5;
    for (instanceCnt = 0 ; instanceCnt < instanceN ; instanceCnt++)
    {
        prv_ledInstanceInit(devNum,instanceCnt);
        /* set the addresses of the SMI unit registers according to the needed base address */
        baseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_LED_0_E + instanceCnt,NULL);
        regUnitBaseAddrSet(
            REG_UNIT_INFO_MAC(devNum,baseAddr,GOP.LED[instanceCnt]));
    }
}


/*******************************************************************************
* mgInit
*
* DESCRIPTION:
*       init the DB - MG unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void mgInit
(
    IN  GT_U8  devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  offset;

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        offset = 0x00030000;
    }
    else
    {
        offset = 0x00040000;
    }

    {
        GT_U32    n;
        for (n = 0; n <= 1; n++)
        {
            regsAddrPtr->MG.XSMI[n].XSMIManagement =
                offset + 0x00000000 + (n * 0x2000);
            regsAddrPtr->MG.XSMI[n].XSMIAddress =
                offset + 0x00000008 + (n * 0x2000);
            regsAddrPtr->MG.XSMI[n].XSMIConfiguration =
                offset + 0x0000000C + (n * 0x2000);
            regsAddrPtr->MG.XSMI[n].XSMIInterruptCause =
                offset + 0x00000010 + (n * 0x2000);
            regsAddrPtr->MG.XSMI[n].XSMIInterruptMask =
                offset + 0x00000014 + (n * 0x2000);
        }
    }
}

/*******************************************************************************
* lmsLegacyDestroy
*
* DESCRIPTION:
*       destroy LMS registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void lmsLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    DESTROY_STC_MAC(addrPtr->smiRegs);
    DESTROY_STC_MAC(addrPtr->ledRegs);
    DESTROY_STC_MAC(addrPtr->macRegs.srcAddrMiddle);
    DESTROY_STC_MAC(addrPtr->macRegs.srcAddrHigh);
    DESTROY_STC_MAC(addrPtr->miscellaneousRegs);

    return;
}




/*******************************************************************************
* bmLegacyDestroy
*
* DESCRIPTION:
*       destroy BM registers from the legacy DB.
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
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void bmLegacyDestroy
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC    *addrPtr;

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* destroy legacy DB register's address data */
    DESTROY_STC_MAC(addrPtr->bufferMng);

    /* destroy also MPPM */
    DESTROY_STC_MAC(addrPtr->mppm);

    /* destroy also interrupts */
    DESTROY_STC_MAC(addrPtr->interrupts);

    return;
}

static PRV_CPSS_DXCH_UNIT_ENT gopMibUnits[MAX_PIPES_CNS] =
{
     PRV_CPSS_DXCH_UNIT_MIB_E
    ,PRV_CPSS_DXCH_UNIT_MIB1_E
};

/*******************************************************************************
* bcat2MacMibCountersInit
*
* DESCRIPTION:
*       Init MIB MAC counters memory address.
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
*       GT_OK - on success
*       GT_BAD_STATE - on bad state of logic
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS bcat2MacMibCountersInit
(
    IN  GT_U8 devNum
)
{
    GT_STATUS                       rc;/* return code */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC  *addrPtr;
    GT_U32                          i;          /* iterator */
    GT_U32                          mibBaseAddr, /* mib memory base address */
                                    mibOffset; /* offset per port */
    GT_U32                          portType;/*iterator*/
    GT_U32                          portIndex;/* 'local port' index (in case of multi-pipe) */
    GT_U32                          pipeIndex;/* pipe index (in case of multi-pipe) */
    GT_U32                          prevPipeIndex = 0xFFFFFFFF;/* previous pipe index (in case of multi-pipe) */

    addrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
    mibBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_MIB_E,NULL);

    for (i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        /* destroy unused GOP registers */
        DESTROY_SINGLE_REG_MAC(addrPtr->macRegs.perPortRegs[i].externalUnitsIntmask);
        for(portType = 0 ; portType < PRV_CPSS_PORT_NOT_APPLICABLE_E ; portType++)
        {
            DESTROY_SINGLE_REG_MAC(addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].pcs40GCommonCtrl);
            DESTROY_SINGLE_REG_MAC(addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].pcs40GCommonStatus);
            DESTROY_SINGLE_REG_MAC(addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].pcs40GGearBoxStatus);
            DESTROY_SINGLE_REG_MAC(addrPtr->macRegs.perPortRegs[i].macRegsPerType[portType].pcs40GFecDecStatus);
        }

        /* skip non exists MAC ports */
        if(!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, i))
        {
            /* destroy unused GOP registers that initialized during 'legacy code' */
            DESTROY_STC_MAC(addrPtr->macRegs.perPortRegs[i]);

            continue;
        }

        if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiPipe.numOfPipes)
        {
            rc = prvCpssDxChHwPpGopGlobalMacPortNumToLocalMacPortInPipeConvert(devNum,
                i/*global port*/,
                &pipeIndex,
                &portIndex/* local port */);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* optimization , calculate base addr only when unitIndex is changed */
            if(pipeIndex != prevPipeIndex)
            {
                /* get base address of the unit according to the pipeIndex */
                mibBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,gopMibUnits[pipeIndex],NULL);
            }

            /* update the previous pipeId */
            prevPipeIndex = pipeIndex;
        }
        else
        {
            portIndex = i;
        }


#ifdef GM_USED
        mibOffset = portIndex * 0x400;
#else
        if(portIndex < 56)
        {
            mibOffset = portIndex * 0x400;
        }
        else
        {
            mibOffset = (portIndex-56) * 0x400;
            mibOffset += 0x800000;
        }
#endif /* GM_USED */

        addrPtr->macRegs.perPortRegs[i].macCounters = mibBaseAddr + mibOffset;
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_TRUE)
    {
        /* destroy other legacy mac registers */
        DESTROY_STC_MAC(addrPtr->macRegs.cpuPortRegs);
    }

    return GT_OK;
}

/*******************************************************************************
* bobkDMAGluePizzaArbiterUnitRegOffsetInit
*
* DESCRIPTION:
*       bobk DMA Glue Pizza Arbiter Init
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       pizzaArbiterPtr  - pointer to glue pizza unit in register structure
*       offset           - ofsset of unit
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID bobkDMAGluePizzaArbiterUnitRegOffsetInit
(
    IN PRV_CPSS_DXCH_PP_DP_GLUE_PIZZA_ARBITER_STC * pizzaArbiterPtr,
    IN GT_U32 offset
)
{
    GT_U32 i;
    pizzaArbiterPtr->pizzaArbiterCtrlReg   = offset + 0x0000000;
    pizzaArbiterPtr->pizzaArbiterStatusReg = offset + 0x0000004;
    for (i = 0; i < sizeof(pizzaArbiterPtr->pizzaArbiterConfigReg) /sizeof(pizzaArbiterPtr->pizzaArbiterConfigReg[0]); i++)
    {
        pizzaArbiterPtr->pizzaArbiterConfigReg[i] = offset + 0x0000008 + i*4;
    }

}


/*******************************************************************************
* bobkRxDMAGlueInit
*
* DESCRIPTION:
*       Init RX-DMA-GLUE register Init
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID bobkRxDMAGlueInit
(
    GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC  *regsAddrVer1;
    regsAddrVer1 = &PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddrVer1;
    bobkDMAGluePizzaArbiterUnitRegOffsetInit(&regsAddrVer1->RX_DMA_GLUE.pizzaArbiter,0);
}


/*******************************************************************************
* bobkTxDMAGlueInit
*
* DESCRIPTION:
*       Init RX-DMA-GLUE register Init
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID bobkTxDMAGlueInit
(
    GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC  *regsAddrVer1;
    regsAddrVer1 = &PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddrVer1;
    bobkDMAGluePizzaArbiterUnitRegOffsetInit(&regsAddrVer1->TX_DMA_GLUE.pizzaArbiter,0);
}



/*******************************************************************************
* bobkMPPMInit
*
* DESCRIPTION:
*       Init RX-DMA-GLUE register Init
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID bobkMPPMInit
(
    GT_U8 devNum
)
{
    GT_U32 i;
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC  *regsAddrVer1;
    PRV_CPSS_DXCH_PP_MPPM_PIZZA_ARBITER_STC *pizzaArbiterPtr;

    regsAddrVer1 = &PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddrVer1;
    pizzaArbiterPtr = &regsAddrVer1->MPPM.pizzaArbiter;


    pizzaArbiterPtr->pizzaArbiterCtrlReg   = 0x0000200;
    pizzaArbiterPtr->pizzaArbiterStatusReg = 0x0000204;
    for (i = 0; i < sizeof(pizzaArbiterPtr->pizzaArbiterConfigReg) /sizeof(pizzaArbiterPtr->pizzaArbiterConfigReg[0]); i++)
    {
        pizzaArbiterPtr->pizzaArbiterConfigReg[i] = 0x0000208 + i*4;
    }
}


/*******************************************************************************
* prvCpssDxChHwRegAddrToUnitIdConvert
*
* DESCRIPTION:
*       This function convert register address to unit id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       regAddr - the register address to get it's base address unit Id.
* OUTPUTS:
*       None.
*
* RETURNS:
*       the unitId for the given address
*
* COMMENTS:
*
*******************************************************************************/
PRV_CPSS_DXCH_UNIT_ENT prvCpssDxChHwRegAddrToUnitIdConvert
(
    IN GT_U8                    devNum,
    IN GT_U32                   regAddr
)
{
    GT_U32  *unitsBaseAddrPtr = NULL;
    GT_U32  regAddrBaseAddr;
    GT_U32  ii;
    PRV_CPSS_DXCH_UNIT_ENT  *unitIdPtr = NULL;

    if(devNum == LION2_DEVICE_INDICATION_CNS)
    {
        /* indication the we need Lion2 DB regardless to the actual devNum */
        goto forceLion2_lbl;
    }
    else
    if (devNum == BOBK_DEVICE_INDICATION_CNS)
    {
        /* indication the we need Bobk DB regardless to the actual devNum */
        goto forceBobk_lbl;
    }

    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS ||
       NULL == prvCpssDrvPpConfig[devNum])
    {
        /* error */
        return PRV_CPSS_DXCH_UNIT_LAST_E;
    }

    switch(prvCpssDrvPpConfig[devNum]->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_LION2_E:
            forceLion2_lbl:
            unitsBaseAddrPtr = prvDxChLion2UnitsBaseAddr;
            break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
            forceBobk_lbl:
            unitIdPtr = prvDxChBobcat2UnitIdPer8MSBitsBaseAddrArr;
            break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
            unitIdPtr = prvDxChBobcat3UnitIdPer8MSBitsBaseAddrArr;
            break;
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
            unitIdPtr = prvDxChAldrinUnitIdPer8MSBitsBaseAddrArr;
            break;
        default:
            /* error */
            return PRV_CPSS_DXCH_UNIT_LAST_E;
    }

    if(unitIdPtr)
    {
        return unitIdPtr[(regAddr >> 24)];
    }
    else if(unitsBaseAddrPtr)
    {
        regAddrBaseAddr = regAddr & UNIT_MASK_9_MSB_CNS;

        for(ii = 0 ; ii < PRV_CPSS_DXCH_UNIT_LAST_E ; ii++,unitsBaseAddrPtr++)
        {
            if(regAddrBaseAddr == (*unitsBaseAddrPtr))
            {
                /*found the unit*/
                return ii;
            }
        }
    }

    return PRV_CPSS_DXCH_UNIT_LAST_E;
}

/*******************************************************************************
* convertRegAddrToNewUnitsBaseAddr
*
* DESCRIPTION:
*        the assumption is that the 'old address' is based on Lion2 units base addresses !
*        so need to convert it to address space of current device
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       oldNewRegPtr  - pointer to the 'old' reg address (from Lion2)
* OUTPUTS:
*       oldNewRegPtr  - pointer to the 'new' reg address - after aligned to the current device.
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
GT_STATUS convertRegAddrToNewUnitsBaseAddr
(
    IN GT_U8    devNum,
    INOUT GT_U32 *oldNewRegPtr
)
{
    GT_BOOL     error;
    PRV_CPSS_DXCH_UNIT_ENT currenyUnitId;
    GT_U32      unitMask = UNIT_MASK_9_MSB_CNS;/* mask of the 9 MSBits */
    GT_U32      currentAddr,currentUnitBaseAddr;
    GT_U32      newBaseAddr;

    currentAddr = *oldNewRegPtr;

    currentUnitBaseAddr = currentAddr & unitMask;

    /* look for the ID of the current unit */
    currenyUnitId =
        prvCpssDxChHwRegAddrToUnitIdConvert(LION2_DEVICE_INDICATION_CNS/*lion2 indication!!!*/,
            currentAddr);
    if(currenyUnitId >= PRV_CPSS_DXCH_UNIT_LAST_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    newBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,currenyUnitId,&error);
    if(error == GT_TRUE )
    {
        /* assume that this unit is not supported by the current device
           so 'invalidate' this register ! */
        *oldNewRegPtr = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        return GT_OK;
    }

    /* update the address value */
    *oldNewRegPtr = currentAddr - currentUnitBaseAddr + newBaseAddr;

    return GT_OK;
}

/*******************************************************************************
* convertOldRegDbToNewUnitsBaseAddr
*
* DESCRIPTION:
*       the rest of the registers that we not manage to 'destroy' in the old DB.
*       we need to convert align their addresses to base addresses of the actual
*       corresponding units
*
*        the assumption is that the 'old DB' is based on Lion2 units base addresses !
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       oldRegPtr  - pointer to the old reg DB that need to be converted
*       numOfEntries - number of entries in oldRegPtr
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
static GT_STATUS convertOldRegDbToNewUnitsBaseAddr_limitedRange
(
    IN GT_U8    devNum,
    IN GT_U32 *oldRegPtr,
    IN GT_U32 numOfEntries
)
{
    GT_U32       *regsAddrPtr32;
    GT_U32        regsAddrPtr32Size;
    GT_U32      ii;
    GT_U32      *currentSwPtr;
    GT_STATUS   rc;
    PRV_CPSS_BOOKMARK_STC *bookmark;
    GT_U32  currentAddr;
    GT_U32      currentNumOfEntries;

    regsAddrPtr32 = oldRegPtr;
    regsAddrPtr32Size = numOfEntries;

    for( ii = 0; ii < regsAddrPtr32Size; ii++ )
    {
        currentAddr = regsAddrPtr32[ii];
        if(currentAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            continue;
        }

        if(currentAddr == PRV_CPSS_SW_PTR_BOOKMARK_CNS)
        {
            /*
                GT_U32      swPortControlBookmark;  [0]
                GT_U32      swPortControlType;      [1]
                GT_U32      swPortControlSize;      [2]
                GT_U32      *portControl GT_PACKED; [3]
            */
            bookmark = (PRV_CPSS_BOOKMARK_STC *)(&(regsAddrPtr32[ii]));

            if(bookmark->type & PRV_CPSS_SW_TYPE_WRONLY_CNS)
            {
                /* there is no 'pointer' to registers array , just the next N
                   registers are indicated as 'write only'
                */

                /* step over the bookmark and lets continue from there ...
                    BUT there is no field of "'GT_U32  *nextPtr" */
                ii += ((sizeof(PRV_CPSS_BOOKMARK_STC) - sizeof(bookmark->nextPtr))/sizeof(GT_U32)) - 1;
            }
            else
            {
                currentNumOfEntries =  bookmark->size / sizeof(GT_U32);
                currentSwPtr        = bookmark->nextPtr;
                /* recursive call */
                rc = convertOldRegDbToNewUnitsBaseAddr_limitedRange(devNum,currentSwPtr,currentNumOfEntries);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* step over the bookmark and lets continue from there */
                ii += (sizeof(PRV_CPSS_BOOKMARK_STC)/sizeof(GT_U32)) - 1;
            }

            continue;
        }

        rc = convertRegAddrToNewUnitsBaseAddr(devNum,&regsAddrPtr32[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }

    }

    return GT_OK;

}

/*******************************************************************************
* convertOldRegDbToNewUnitsBaseAddr
*
* DESCRIPTION:
*       the rest of the registers that we not manage to 'destroy' in the old DB.
*       we need to convert align their addresses to base addresses of the actual
*       corresponding units
*
*        the assumption is that the 'old DB' is based on Lion2 units base addresses !
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
static GT_STATUS convertOldRegDbToNewUnitsBaseAddr
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
    GT_U32       *regsAddrPtr32;
    GT_U32        regsAddrPtr32Size;

    /*Fill all the words in the struct with initial value*/
    regsAddrPtr32 = (GT_U32*)regsAddrPtr;
    regsAddrPtr32Size = sizeof(PRV_CPSS_DXCH_PP_REGS_ADDR_STC) / 4;

    return convertOldRegDbToNewUnitsBaseAddr_limitedRange(devNum,regsAddrPtr32,regsAddrPtr32Size);


}

#ifdef ASIC_SIMULATION/* needed for GM*/
/*******************************************************************************
* prvCpssDxChHwIsUnitUsed
*
* DESCRIPTION:
*       This function checks existences of base addresses units in the VERIFIER
*       of the GM for the given device.
*       since the VERIFIER not supports all units , the CPSS wants to avoid
*       accessing the VERIFIER.
*       the CPSS binds the simulation with this functions so the simulation before
*       accessing the GM (and VERIFIER) can know no to call them on such addresses.
*
*       NOTE: purpose is to reduce VIRIFIER 'ERRORS' in LOG , and even 'crash' of
*       VERIFIER on some unknown addresses.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - thr PP's device number to init the struct for.
*       portGroupId - port group ID
*       regAddr     - register address
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE - the unit is implemented in the VERIFIER
*       GT_FALSE - the unit is NOT implemented in the VERIFIER
*
* COMMENTS:
*
*******************************************************************************/
static GT_BOOL prvCpssDxChHwIsUnitUsed
(
    IN GT_U32                   devNum,
    IN GT_U32                   portGroupId,
    IN GT_U32                   regAddr
)
{
    PRV_CPSS_DXCH_UNIT_ENT unitId;  /* Unit ID */

    portGroupId = portGroupId;

    if (!PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        /* CPSS not aware that this GM device need to skip accessing to any unit */
        return GT_TRUE;
    }

    unitId = prvCpssDxChHwRegAddrToUnitIdConvert((GT_U8)devNum, regAddr);

    switch(unitId)
    {   /* List of existing units */
        case PRV_CPSS_DXCH_UNIT_TTI_E:
        case PRV_CPSS_DXCH_UNIT_PCL_E:
        case PRV_CPSS_DXCH_UNIT_L2I_E:
        case PRV_CPSS_DXCH_UNIT_FDB_E:
        case PRV_CPSS_DXCH_UNIT_EQ_E:
        case PRV_CPSS_DXCH_UNIT_LPM_E:
        case PRV_CPSS_DXCH_UNIT_EGF_EFT_E:
        case PRV_CPSS_DXCH_UNIT_EGF_QAG_E:
        case PRV_CPSS_DXCH_UNIT_EGF_SHT_E:
        case PRV_CPSS_DXCH_UNIT_HA_E:
        case PRV_CPSS_DXCH_UNIT_ETS_E:
        case PRV_CPSS_DXCH_UNIT_MLL_E:
        case PRV_CPSS_DXCH_UNIT_IPLR_E:
        case PRV_CPSS_DXCH_UNIT_IPLR_1_E:
        case PRV_CPSS_DXCH_UNIT_EPLR_E:
        case PRV_CPSS_DXCH_UNIT_IPVX_E:
        case PRV_CPSS_DXCH_UNIT_IOAM_E:
        case PRV_CPSS_DXCH_UNIT_EOAM_E:
        case PRV_CPSS_DXCH_UNIT_TCAM_E:
        case PRV_CPSS_DXCH_UNIT_RXDMA_E:
        case PRV_CPSS_DXCH_UNIT_EPCL_E:
        case PRV_CPSS_DXCH_UNIT_MIB_E:
        case PRV_CPSS_DXCH_UNIT_ERMRK_E:
        case PRV_CPSS_DXCH_UNIT_CNC_0_E:
        case PRV_CPSS_DXCH_UNIT_CNC_1_E:
        case PRV_CPSS_DXCH_UNIT_TM_DROP_E:
        case PRV_CPSS_DXCH_UNIT_TM_QMAP_E:
        case PRV_CPSS_DXCH_UNIT_MG_E:
        case PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E:
        case PRV_CPSS_DXCH_UNIT_TXQ_LL_E:
        /*case PRV_CPSS_DXCH_UNIT_TXQ_PFC_E:*/
        case PRV_CPSS_DXCH_UNIT_TXQ_QCN_E:
        case PRV_CPSS_DXCH_UNIT_TXQ_DQ_E:
        /* new in bobk */
/* GM not implemented the second unit
        case PRV_CPSS_DXCH_UNIT_RXDMA1_E:
        case PRV_CPSS_DXCH_UNIT_TXDMA1_E:
        case PRV_CPSS_DXCH_UNIT_TX_FIFO1_E:
*/
            return GT_TRUE;
        default:
            return GT_FALSE;
    }

}
#endif /*ASIC_SIMULATION*/

/*******************************************************************************
* prvCpssDxChHwRegAddrVer1Init
*
* DESCRIPTION:
*       This function initializes the registers struct for eArch devices.
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
GT_STATUS prvCpssDxChHwRegAddrVer1Init
(
    IN  GT_U8 devNum
)
{
    GT_U32 * unitBasePtr;
    PRV_CPSS_DXCH_UNIT_ENT * unitIdPtr;
    GT_U32  ii,jj;
    GT_U32  subunitBase;
    GT_U32  indexArr[2];
    GT_U32  unit8MSBits;
    GT_U32  maxPtpPorts = 73;/*72 ports + last one for interlaken !!! */
    GT_BOOL supportSingleTaiUnit;
    GT_BOOL supportILKN;
    GT_BOOL supportTM = GT_TRUE;
    GT_BOOL supportLMS;
    GT_STATUS rc;
#ifdef ASIC_SIMULATION
    GT_U32      simDeviceId;
#endif /*ASIC_SIMULATION*/
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    rc = prvCpssDxChUnitBaseTableInit();
    if (rc != GT_OK)
    {
        return rc;
    }

    supportILKN = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_ilkn.supported;
    supportLMS = (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_lms.notSupported == GT_TRUE) ?
                GT_FALSE :/* instead of LMS we have : SMI and LED units */
                GT_TRUE;
    supportSingleTaiUnit = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance;


    rc = prvCpssDxChUnitBaseTableGet(devNum,/*OUT*/&unitBasePtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E)
    {
        /* unitBasePtr = prvDxChBobcat3UnitsBaseAddr;  */
        unitIdPtr = prvDxChBobcat3UnitIdPer8MSBitsBaseAddrArr;

        maxPtpPorts = 72;/* currently only 36 ports from each pipe , no interlaken */
        supportTM = GT_FALSE;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
    {
        unitIdPtr = prvDxChBobcat2UnitIdPer8MSBitsBaseAddrArr;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E)
    {
        unitIdPtr = prvDxChAldrinUnitIdPer8MSBitsBaseAddrArr;
        maxPtpPorts = 33;
        supportTM = GT_FALSE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }


    for(ii = 0 ; ii < NUM_ENTRIES_FOR_8_MSBITS_BASE_ADDR_CNS ; ii++)
    {
        /* init the table with 'not valid' */
        unitIdPtr[ii] = PRV_CPSS_DXCH_UNIT_LAST_E;
    }

    /* array of unitBasePtr[] is with different size per family/sub family
       iterations need to be until it's end and not according to 'constant'  */
    for(ii = 0 ; unitBasePtr[ii] != END_OF_TABLE_INDICATOR_CNS ; ii++)
    {
        /* according to unitId we get the 8 MSBits of the address*/
        unit8MSBits = unitBasePtr[ii] >> 24;
        /* with those 8 MSBits we access the unitId table and save the unitId (ii)*/
        unitIdPtr[unit8MSBits] = ii;
    }

    /* fill also the 'large units' */
    for(ii = 0 ; prvDxChLion3LargeUnitsArr[ii].unitId != END_OF_TABLE_INDICATOR_CNS; ii++)
    {
        unit8MSBits =
            (unitBasePtr[prvDxChLion3LargeUnitsArr[ii].unitId]) >> 24;

        for(jj = 0 ; jj < prvDxChLion3LargeUnitsArr[ii].numOfSubUnits; jj++)
        {
            unitIdPtr[unit8MSBits + jj] = prvDxChLion3LargeUnitsArr[ii].unitId;
        }
    }


#ifdef ASIC_SIMULATION /* needed for GM only*/
    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        /* Convert devNum to simDeviceId */
        rc = prvCpssDrvDevNumToSimDevIdConvert(devNum, 0, &simDeviceId);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*
            bind the simulation with CallBack function to advice about addresses
            that belongs to 'non exists' units in the VERIFIER.

            the simulation will 'ignore' write to such register , and will
            'read' value of '0x00badadd'.
        */

        smemGmUnitUsedFuncInit(simDeviceId, devNum, prvCpssDxChHwIsUnitUsed);
    }
#endif /*ASIC_SIMULATION*/

    /* reset the DB */
    dbReset(devNum);

    /* init the TTI unit*/
    ttiInit(devNum);
    /* set the addresses of the TTI unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TTI_E], TTI));
    /* destroy the TTI legacy registers */
    ttiLegacyDestroy(devNum);

    /* init the L2I unit*/
    l2iInit(devNum);
    /* set the addresses of the L2I unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_L2I_E], L2I));
    /* destroy the L2I legacy registers */
    l2LegacyDestroy(devNum);

    /* init the FDB unit*/
    fdbInit(devNum);
    /* set the addresses of the FDB unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_FDB_E], FDB));
     /* destroy the FDB legacy registers */
    fdbLegacyDestroy(devNum);

    /* init the EQ unit*/
    eqInit(devNum);
    /* set the addresses of the EQ unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_EQ_E], EQ));
    /* destroy the EQ legacy registers */
    eqLegacyDestroy(devNum);

    /* init the TCAM unit*/
    tcamInit(devNum);
    /* set the addresses of the TCAM unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TCAM_E], TCAM));

    /* init the LPM unit */
    lpmInit(devNum);
    /* set the addresses of the LPM unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum,unitBasePtr[PRV_CPSS_DXCH_UNIT_LPM_E], LPM));
    /* no destroy because LPM is a new unit without legacy */

    /* init the EGF unit (EFT,SHT,QAG) */
    egfInit(devNum);
    /* set the addresses of the EGF unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_EGF_EFT_E], EGF_eft));
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_EGF_QAG_E], EGF_qag));
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_EGF_SHT_E], EGF_sht));
    /* destroy the EGF legacy registers */
    egfLegacyDestroy(devNum);


    /* init the HA unit */
    haInit(devNum);
    /* set the addresses of the HA unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_HA_E], HA));
    /* destroy the HA legacy registers */
    haLegacyDestroy(devNum);

    /* init the MLL unit */
    mllInit(devNum);
    /* set the addresses of the MLL unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_MLL_E], MLL));
    /* destroy the MLL legacy registers */
    mllLegacyDestroy(devNum);

    /* init the PCL unit (IPCL) */
    pclInit(devNum);
    /* set the addresses of the PCL unit registers (IPCL) according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_PCL_E], PCL));
    /* destroy the PCL legacy registers (IPCL) */
    pclLegacyDestroy(devNum);

    /* init the EPCL unit (EPCL) */
    epclInit(devNum);
    /* set the addresses of the PCL unit registers (EPCL) according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_EPCL_E], EPCL));

    /* init for RxDma,txDma,txFifo */
    dataPathInit(devNum,unitBasePtr);

    /* init ingress OAM */
    oamInit(devNum,PRV_CPSS_DXCH_UNIT_IOAM_E);
    /* set the addresses of the ingress OAM unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_IOAM_E], OAMUnit[0]));
    /* init egress OAM */
    oamInit(devNum,PRV_CPSS_DXCH_UNIT_EOAM_E);
    /* set the addresses of the egress OAM unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_EOAM_E], OAMUnit[1]));

    /* init ermrk (egress mark/remark) */
    ermrkInit(devNum);
    /* set the addresses of the ermrk unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_ERMRK_E], ERMRK));



    /* init BM */
    bmInit(devNum);
    /* set the addresses of the BM unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_BM_E], BM));
    /* destroy the bm legacy registers */
    bmLegacyDestroy(devNum);


    /* init TAI (Time Application Interface) */
    if(supportSingleTaiUnit == GT_FALSE)
    {
        for (ii = 0; (ii < 10); ii++)
        {
            for (jj = 0; (jj < 2); jj++)
            {
                gopTaiInit(devNum, ii /*gopIndex*/, jj /*taiIndex*/);
                indexArr[0] = ii;
                indexArr[1] = jj;
                subunitBase = prvCpssDxChHwSubunitBaseAddrGet(
                    devNum, PRV_CPSS_DXCH_SUBUNIT_GOP_TAI_E, indexArr);

                /* set the addresses of the TAI unit registers according to the needed base address */
                regUnitBaseAddrSet(
                    REG_UNIT_INFO_MAC(devNum, subunitBase, GOP.TAI[ii][jj]));
            }
        }
    }
    else
    {
        /* single TAI device */
        gopTaiInit(devNum, 0 /*gopIndex*/, 0 /*taiIndex*/);
        subunitBase = unitBasePtr[PRV_CPSS_DXCH_UNIT_TAI_E];
        regUnitBaseAddrSet(
            REG_UNIT_INFO_MAC(devNum, subunitBase, GOP.TAI[0][0]));
    }

    /* init GOP PTP  */
    for (ii = 0; (ii < maxPtpPorts); ii++)
    {
        if(ii != CPU_SDMA_ID)/*NOTE: index 72 used for interlaken (not as PTP of MAC)*/
        {
            /* skip non exists MAC ports */
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum,ii);
        }

        indexArr[0] = ii;
        subunitBase = prvCpssDxChHwSubunitBaseAddrGet(
            devNum, PRV_CPSS_DXCH_SUBUNIT_GOP_PTP_E, indexArr);
        if(subunitBase == 0xFFFFFFFF)
        {
            /* ILKN ... for device that not supports ILKN*/
            continue;
        }

        gopPtpInit(devNum, ii /*macNum*/);

        /* set the addresses of the PTP unit registers according to the needed base address */
        regUnitBaseAddrSet(
            REG_UNIT_INFO_MAC(devNum, subunitBase, GOP.PTP[ii]));

        gopFcaInit(devNum, ii /*macNum*/);
        subunitBase = prvCpssDxChHwSubunitBaseAddrGet(
            devNum, PRV_CPSS_DXCH_SUBUNIT_GOP_FCA_E, indexArr);

        /* set the addresses of the FCA unit registers according to the needed base address */
        regUnitBaseAddrSet(
            REG_UNIT_INFO_MAC(devNum, subunitBase, GOP.FCA[ii]));
    }

    if(supportILKN == GT_TRUE)
    {
        gopPrInit(devNum);
        subunitBase = prvCpssDxChHwSubunitBaseAddrGet(
            devNum, PRV_CPSS_DXCH_SUBUNIT_GOP_PR_E, indexArr);
        /* set the addresses of the GOP.PR unit registers according to the needed base address */
        regUnitBaseAddrSet(
            REG_UNIT_INFO_MAC(devNum, subunitBase, GOP.PR));

        gopIlknInit(devNum);
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_ILKN_E], GOP.ILKN));
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_ILKN_E], GOP.ILKN_WRP));
    }

    ipInit(devNum);
    /* set the addresses of the ip unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_IPVX_E], IPvX));
    /* destroy the ip legacy registers */
    ipLegacyDestroy(devNum);

    /* init the PLR unit (3 stages - IPLR, IPLR_1 & EPLR) */
    for (ii = 0; (ii < 3); ii++)
    {
        plrInit(devNum, ii);
    }
    /* set the addresses of the PLR unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_IPLR_E],   PLR[0]));
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_IPLR_1_E], PLR[1]));
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_EPLR_E],   PLR[2]));
    /* destroy the PLR legacy registers */
    plrLegacyDestroy(devNum);

    /* init the CNC unit (2 instances) */
    for (ii = 0; (ii < 2); ii++)
    {
        cncInit(devNum, ii);
    }
    /* set the addresses of the CNC unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_CNC_0_E], CNC[0]));
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_CNC_1_E], CNC[1]));
    cncLegacyDestroy(devNum);

    /* TXQ unit init & destroy */
    {
        /* init the TXQ_QCN unit*/
        txqQcnInit(devNum);
        /* set the addresses of the TXQ_QCN unit registers according to the needed base address */
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TXQ_QCN_E], TXQ.qcn));

        /* init the TXQ_LL unit*/
        txqLLInit(devNum);
        /* set the addresses of the TXQ_LL unit registers according to the needed base address */
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TXQ_LL_E], TXQ.ll));
        /* destroy the TXQ_LL legacy registers */
        txqLLLegacyDestroy(devNum);

        /* init the TXQ_PFC unit*/
        txqPfcInit(devNum);
        /* set the addresses of the TXQ_PFC unit registers according to the needed base address */
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TXQ_PFC_E], TXQ.pfc));

        /* init the TXQ_QUEUE unit*/
        txqQueueInit(devNum);
        /* set the addresses of the TXQ_QUEUE unit registers according to the needed base address */
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E], TXQ.queue));
        /* destroy the TXQ_QUEUE legacy registers */
        txqQueueLegacyDestroy(devNum);
    }

    /* init the Bobcat2 only units */
    if (supportTM == GT_TRUE)
    {
        tmDropInit(devNum);
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TM_DROP_E], TMDROP));

        tmQMapInit(devNum);
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TM_QMAP_E], TMQMAP));

        tmIngressGlueInit(devNum);
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TM_INGRESS_GLUE_E], TM_INGR_GLUE));

        tmEgressGlueInit(devNum);
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E], TM_EGR_GLUE));

        tmFcuInit(devNum);
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TM_FCU_E], TM_FCU));

        switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
                switch (PRV_CPSS_PP_MAC(devNum)->devSubFamily)
                {
                    case CPSS_PP_SUB_FAMILY_NONE_E: /* BC2 */
                        ethTxFifoInit(devNum,0);
                        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E], SIP_ETH_TXFIFO[0]));
                        if(supportILKN == GT_TRUE)
                        {
                            ilknTxFifoInit(devNum);
                            regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E], SIP_ILKN_TXFIFO));
                        }
                    break;
                    case CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E:
                        ethTxFifoInit(devNum,0);
                        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E], SIP_ETH_TXFIFO[0]));

                        ethTxFifoInit(devNum,1);
                        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_ETH_TXFIFO1_E], SIP_ETH_TXFIFO[1]));
                    break;
                    default:
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                    }
                }
            break;
            case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
                /* do nothing : no Eth-TxFifo */
            break;
            default:
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
        }
    }

    if(supportLMS == GT_TRUE)
    {
        /* init the LMS unit (3 instances)*/
        for (ii = 0; ii < 3; ii++)
        {
            lmsInit(devNum, ii);
        }

        /* set the addresses of the LMS unit registers according to the needed base address */
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_LMS0_0_E], GOP.LMS[0]));
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_LMS1_0_E], GOP.LMS[1]));
        regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_LMS2_0_E], GOP.LMS[2]));
    }
    else /* the LMS unit was split into 2 separate units SMI , LED */
    {
        prv_smiInit(devNum);
        prv_ledInit(devNum);
    }

    lmsLegacyDestroy(devNum);

    /* init TG (gop) registers */
    gopMacTGInit(devNum);
    /* set the addresses of the MG unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_GOP_E], packGenConfig));


    /* init MG unit */
    mgInit(devNum);
    /* set the addresses of the MG unit registers according to the needed base address */
    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_MG_E], MG));
    /* the rest of the registers that we not manage to 'destroy' in the old DB.
       we need to convert align their addresses to base addresses of the actual
       corresponding units

        the assumption is that the 'old DB' is based on Lion2 units base addresses !
    */
    rc = convertOldRegDbToNewUnitsBaseAddr(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = bcat2MacMibCountersInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /*-------------------------------------------
     *   Init RXDMA /TXDMA GLUE if required
     *-------------------------------------------*/
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
            switch (PRV_CPSS_PP_MAC(devNum)->devSubFamily)
            {
                case CPSS_PP_SUB_FAMILY_NONE_E: /* BC2 */
                    /* do nothing */
                break;
                case CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E:
                    bobkRxDMAGlueInit(devNum);
                    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_RXDMA_GLUE_E], RX_DMA_GLUE));
                    bobkTxDMAGlueInit(devNum);
                    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TXDMA_GLUE_E], TX_DMA_GLUE));

                    bobkMPPMInit(devNum);
                    regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_MPPM_E], MPPM));
                break;
                default:
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }
            }
        break;
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
            bobkRxDMAGlueInit(devNum);
            regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_RXDMA_GLUE_E], RX_DMA_GLUE));
            bobkTxDMAGlueInit(devNum);
            regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_TXDMA_GLUE_E], TX_DMA_GLUE));

            bobkMPPMInit(devNum);
            regUnitBaseAddrSet(REG_UNIT_INFO_MAC(devNum, unitBasePtr[PRV_CPSS_DXCH_UNIT_MPPM_E], MPPM));
        break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
            /* currently do nothing , add support in future */
        break;
        default:
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }

    /* the device hold also GOP per pipe */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiPipe.numOfPipes)
    {
        /* update the DB of 'per port' MAC registers */
        updateGopOldRegDbWithMultiPipe(devNum);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChHwResetAndInitControllerRegAddrInit
*
* DESCRIPTION:
*       This function initializes the Reset and Init Controller registers
*       struct for eArch devices.
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
GT_STATUS prvCpssDxChHwResetAndInitControllerRegAddrInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_RESET_AND_INIT_CTRL_REGS_ADDR_STC *regsAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* destroy the DFX legacy registers */
    dfxLegacyDestroy(devNum);

    resetAndInitControllerDbReset(devNum);

    regsAddrPtr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum);

    {/*start of unit DFXServerUnitsBC2SpecificRegs */
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceResetCtrl = 0x000f800c;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.initializationStatusDone = 0x000f8014;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.configSkipInitializationMatrix = 0x000f8020;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.RAMInitSkipInitializationMatrix = 0x000f8030;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.trafficManagerSkipInitializationMatrix = 0x000f8040;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.tableSkipInitializationMatrix = 0x000f8060;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.SERDESSkipInitializationMatrix = 0x000f8064;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.EEPROMSkipInitializationMatrix = 0x000f8068;
        /*0x000f806c : replaced with 0x000f8058 because:
          0x000f806c was not impact 'pex reset' --> erratum.
          so we need to use register 0x000f8058 that is 'skip CPU reset' and it
          also skip PEX registers (BARs : 0,1,2)

          note: registers  0x000f8050,0x000f8054 are 'pipe0,pip1' reset registers

          NOTE: when PCIeSkipInitializationMatrix = 0x000f806c and not 0x000f8058
            bit 5 PCIe0RstOutMaskTrst --> must be 0 in register:
            <Control and Management Sub System> sip_msys_Units/Runit_RFU/SOC_Misc/RSTOUTn Mask
        */
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.PCIeSkipInitializationMatrix = /*0x000f806c*/0x000f8058;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.DDRSkipInitializationMatrix = 0x000f8048;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceSAR1 = 0x000f8200;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceSAR2 = 0x000f8204;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl0 = 0x000f8250;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl4 = 0x000f8260;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl5 = 0x000f8264;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl8 = 0x000f8270;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl10 = 0x000f8278;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl12 = 0x000f8280;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl21 = 0x000f82a4;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl20 = 0x000f82a0;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl19 = 0x000f829c;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl18 = 0x000f8298;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl15 = 0x000f828c;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl14 = 0x000f8288;
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum) == 0)
        {
            regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceStatus0 = 0x000f82b0;
            regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceStatus1 = 0x000f82b4;
        }
        else
        {
            regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceStatus0 = 0x000f8c80;
            regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceStatus1 = 0x000f8c84;
        }
        if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
        {
            /* DFX Registers Configuration Skip Initialization Matrix for BobK*/
            regsAddrPtr->DFXServerUnitsBC2SpecificRegs.DFXSkipInitializationMatrix = 0x000f8098;
        }

        if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
        {   /* Bobcat3 */
            regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl16 = 0x000f8290;
        }


    }/*end of unit DFXServerUnitsBC2SpecificRegs */


    {/*start of unit DFXServerUnits */
        {/*start of unit DFXServerRegs */
            {/*f812c+28*a*/
                GT_U32    a;
                for(a = 0 ; a <= 0 ; a++) {
                    regsAddrPtr->DFXServerUnits.DFXServerRegs.AVSDisabledCtrl2[a] =
                        0xf812c+28*a;
                }/* end of loop a */
            }/*f812c+28*a*/
            {/*f8128+28*a*/
                GT_U32    a;
                for(a = 0 ; a <= 0 ; a++) {
                    regsAddrPtr->DFXServerUnits.DFXServerRegs.AVSDisabledCtrl1[a] =
                        0xf8128+28*a;
                }/* end of loop a */
            }/*f8128+28*a*/
            regsAddrPtr->DFXServerUnits.DFXServerRegs.temperatureSensor28nmCtrlLSB = 0x000f8070;
            {/*f8450+t*4*/
                GT_U32    t;
                for(t = 1 ; t <= 4 ; t++) {
                    regsAddrPtr->DFXServerUnits.DFXServerRegs.serverXBARTargetPortConfig[t-1] =
                        0xf8450+t*4;
                }/* end of loop t */
            }/*f8450+t*4*/
            regsAddrPtr->DFXServerUnits.DFXServerRegs.serverStatus = 0x000f8010;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.serverInterruptSummaryCause = 0x000f8100;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.serverInterruptSummaryMask = 0x000f8104;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.serverInterruptMask = 0x000f810c;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.serverInterruptCause = 0x000f8108;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.serverAddrSpace = 0x000f8018;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.pipeSelect = 0x000f8000;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.temperatureSensor28nmCtrlMSB = 0x000f8074;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.temperatureSensorStatus = 0x000f8078;
            {/*f8134+28*a*/
                GT_U32    a;
                for(a = 0 ; a <= 0 ; a++) {
                    regsAddrPtr->DFXServerUnits.DFXServerRegs.AVSStatus[a] =
                        0xf8134+28*a;
                }/* end of loop a */
            }/*f8134+28*a*/
            {/*f8138+28*a*/
                GT_U32    a;
                for(a = 0 ; a <= 0 ; a++) {
                    regsAddrPtr->DFXServerUnits.DFXServerRegs.AVSMinStatus[a] =
                        0xf8138+28*a;
                }/* end of loop a */
            }/*f8138+28*a*/
            {/*f813c+28*a*/
                GT_U32    a;
                for(a = 0 ; a <= 0 ; a++) {
                    regsAddrPtr->DFXServerUnits.DFXServerRegs.AVSMaxStatus[a] =
                        0xf813c+28*a;
                }/* end of loop a */
            }/*f813c+28*a*/
            {/*f8130+28*a*/
                GT_U32    a;
                for(a = 0 ; a <= 0 ; a++) {
                    regsAddrPtr->DFXServerUnits.DFXServerRegs.AVSEnabledCtrl[a] =
                        0xf8130+28*a;
                }/* end of loop a */
            }/*f8130+28*a*/
            regsAddrPtr->DFXServerUnits.DFXServerRegs.snoopBusStatus = 0x000f8220;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.deviceIDStatus = 0x000f8240;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.JTAGDeviceIDStatus = 0x000f8244;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.PLLCoreConfig = 0x000f82e4;
            regsAddrPtr->DFXServerUnits.DFXServerRegs.PLLCoreParameters = 0x000f82e0;

        }/*end of unit DFXServerRegs */

    }/*end of unit DFXServerUnits */

    {/*start of unit DFXClientUnits*/

       regsAddrPtr->DFXClientUnits.clientControl     = 0x00002000;
       regsAddrPtr->DFXClientUnits.clientDataControl = 0x00002008;
       {
           GT_U32    a;
           for(a = 0 ; a <= 3 ; a++) {
               regsAddrPtr->DFXClientUnits.clientRamBISTInfo[a] = 0x2040+4*a;
           }/* end of loop a */
       }
       regsAddrPtr->DFXClientUnits.BISTControl       = 0x00002070;
       regsAddrPtr->DFXClientUnits.BISTOpCode        = 0x00002074;
       regsAddrPtr->DFXClientUnits.BISTMaxAddress    = 0x00002080;
       regsAddrPtr->DFXClientUnits.dummyWrite        = 0x000020b0;
       {
           GT_U32    a;
           for(a = 0 ; a <= 3 ; a++) {
               regsAddrPtr->DFXClientUnits.clientRamMC_Group[a] = 0x20E0+4*a;
           }/* end of loop a */
       }
    }/*end of unit DFXClientUnits*/

    {/*start of unit DFXRam*/

       regsAddrPtr->DFXRam.memoryControl    = 0x14;

    }/*end of unit DFXRam*/

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXcat3HwResetAndInitControllerRegAddrInit
*
* DESCRIPTION:
*       This function initializes the Reset and Init Controller (DFX) registers
*       struct for xCat3 devices.
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
GT_STATUS prvCpssDxChXcat3HwResetAndInitControllerRegAddrInit
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_DXCH_RESET_AND_INIT_CTRL_REGS_ADDR_STC *regsAddrPtr;

    resetAndInitControllerDbReset(devNum);

    regsAddrPtr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum);

    {/*start of unit DFXServerUnitsBC2SpecificRegs */
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceResetCtrl = 0x000F800C;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.initializationStatusDone = 0x000F8014;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.configSkipInitializationMatrix = 0x000F8020;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.RAMInitSkipInitializationMatrix = 0x000F8030;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.tableSkipInitializationMatrix = 0x000F8060;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.SERDESSkipInitializationMatrix = 0x000F8064;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.EEPROMSkipInitializationMatrix = 0x000F8068;

        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.PCIeSkipInitializationMatrix = 0x000f806c;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceSAR1 = 0x000F8200;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceSAR2 = 0x000F8204;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceSAR1Override = 0x000F82D4;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceSAR2Override = 0x000F82D8;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl16 = 0x000F8290;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl14 = 0x000F8288;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl0 = 0x000F8250;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.pllMiscConfig = 0x000F82F4;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl10 = 0x000F8278;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl6 = 0x000F8268;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl7 = 0x000f826c;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.deviceCtrl9 = 0x000F8274;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.pllMiscParameters = 0x000F82F0;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.CPUSkipInitializationMatrix = 0x000f8044;
        regsAddrPtr->DFXServerUnitsBC2SpecificRegs.DFXSkipInitializationMatrix = 0x000f8048;

    }/*end of unit DFXServerUnitsBC2SpecificRegs */

    {/*start of unit DFXServerUnits */
        regsAddrPtr->DFXServerUnits.DFXServerRegs.serverStatus = 0x000f8010;
        regsAddrPtr->DFXServerUnits.DFXServerRegs.temperatureSensor28nmCtrlLSB = 0x000f8070;
        regsAddrPtr->DFXServerUnits.DFXServerRegs.temperatureSensor28nmCtrlMSB = 0x000f8074;
        regsAddrPtr->DFXServerUnits.DFXServerRegs.temperatureSensorStatus = 0x000f8078;

    }/*end of unit DFXServerUnits */

    return GT_OK;
}


