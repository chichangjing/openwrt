/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterDrv.c
*
* DESCRIPTION:
*       bobcat2 dynamic (algorithmic) pizza arbiter unit driver
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChRegsVer1.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAUnitDrv.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>


/*-------------------------------------------------------------------------
 *   Level UnitId :
 *        bobcat2PizzaArbiterUnitDrvSet
 *        bobcat2PizzaArbiterUnitDrvGet
 *        bobcat2PizzaArbiterUnitDrvWorkConservingModeGet
 *        bobcat2PizzaArbiterUnitDrvWorkConservingModeSet
 *        bobcat2PizzaArbiterUnitDrvInit
 *
 *   Level UnitDescription :
 *        internal_bobcat2PizzaArbiterUnitDescrDrvDescrByUnitIdGet(IN unitId, OUT unitDescrPtr);
 *        internal_bobcat2PizzaArbiterUnitDescrDrvInit
 *        internal_bobcat2PizzaArbiterUnitDescrDrvGet
 *        internal_bobcat2PizzaArbiterUnitDescrDrvSet
 *        internal_bobcat2PizzaArbiterUnitDescrDrvWorkConservingModeGet
 *        internal_bobcat2PizzaArbiterUnitDescrDrvWorkConservingModeSet
 *
 *   Data structures
 *        Unit Description List
 *-------------------------------------------------------------------------
 */




#define FLD_OFF(STR,fld)      (GT_U32)offsetof(STR,fld)
#define FLD_OFFi(STR,idx,fld) idx*sizeof(STR) + offsetof(STR,fld)


typedef struct 
{
    GT_U32 ctrlRegSliceNum2Run_Offs;
    GT_U32 ctrlRegSliceNum2Run_Len ;

    GT_U32 ctrlRegMapLoadEn_Offs    ;
    GT_U32 ctrlRegMapLoadEn_Len     ;

    GT_U32 ctrlRegWorkConservEn_Offs;
    GT_U32 ctrlRegWorkConservEn_Len;
}PRV_CPSS_DXCH_PA_UNIT_CTRL_REG_FLD_DESCR_STC;

typedef struct 
{
    GT_U32 paMapRegNum    ;
    GT_U32 paMapFldPerReg ;
    GT_U32 paMapTotalFlds ;

    GT_U32 paMapRegWorkSliceMapOffs ;
    GT_U32 paMapRegWorkSliceMapLen  ;
    GT_U32 paMapRegWorkSliceEnOffs ;
    GT_U32 paMapRegWorkSliceEnLen  ;
}PRV_CPSS_DXCH_PA_UNIT_MAP_REG_FLD_DESCR_STC;

typedef struct 
{
    CPSS_DXCH_PA_UNIT_ENT                         unitType;
    GT_U32                                        ctrlRegBaseAddrOffs;
    PRV_CPSS_DXCH_PA_UNIT_CTRL_REG_FLD_DESCR_STC *ctrlRegFldDefPtr;
    GT_U32                                        mapRegBaseAddrOffs;
    PRV_CPSS_DXCH_PA_UNIT_MAP_REG_FLD_DESCR_STC  *mapRegFldDefPtr;
}PRV_CPSS_DXCH_PA_UNIT_DEF_STC;


static PRV_CPSS_DXCH_PA_UNIT_CTRL_REG_FLD_DESCR_STC rxDma_TxDma_TxFifo_EthTxFifo_MPPM_CTRL_REG =
{
     PRV_CPSS_DXCH_BC2_PA_CTRLREG_SLICE_NUM_2_RUN_OFFS_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_SLICE_NUM_2_RUN_LEN_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_MAP_LOAD_EN_OFFS_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_MAP_LOAD_EN_LEN_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_WORK_CONSERV_EN_OFFS_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_WORK_CONSERV_EN_LEN_CNS
};

static PRV_CPSS_DXCH_PA_UNIT_CTRL_REG_FLD_DESCR_STC txQ_CTRL_REG =
{
     PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_SLICE_NUM_2_RUN_OFFS_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_SLICE_NUM_2_RUN_LEN_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_MAP_LOAD_EN_OFFS_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_MAP_LOAD_EN_LEN_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_WORK_CONSERV_EN_OFFS_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_WORK_CONSERV_EN_LEN_CNS
};

static PRV_CPSS_DXCH_PA_UNIT_CTRL_REG_FLD_DESCR_STC ilknTxFifo_CTRL_REG =
{
     PRV_CPSS_DXCH_BC2_PA_CTRLREG_ILKN_FX_FIFO_SLICE_NUM_2_RUN_OFFS_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_ILKN_FX_FIFO_SLICE_NUM_2_RUN_LEN_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_ILKN_FX_FIFO_MAP_LOAD_EN_OFFS_CNS
    ,PRV_CPSS_DXCH_BC2_PA_CTRLREG_ILKN_FX_FIFO_MAP_LOAD_EN_LEN_CNS
    ,(GT_U32)(~0)                                                                /* Work Conserving does not exists  */
    ,(GT_U32)(~0)
};


static PRV_CPSS_DXCH_PA_UNIT_CTRL_REG_FLD_DESCR_STC rxDmaGlue_txDmaGlue_CTRL_REG =
{
     PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_SLICE_NUM_2_RUN_OFFS_CNS
    ,PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_SLICE_NUM_2_RUN_LEN_CNS
    ,PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_MAP_LOAD_EN_OFFS_CNS
    ,PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_MAP_LOAD_EN_LEN_CNS
    ,PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_WORK_CONSERV_EN_OFFS_CNS
    ,PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_WORK_CONSERV_EN_LEN_CNS
};



static PRV_CPSS_DXCH_PA_UNIT_MAP_REG_FLD_DESCR_STC rxDma_TxDma_TxQ_TxFifo_EthTxFifo_MAP_REG =
{
    /* paMapRegNum              */  PRV_CPSS_DXCH_BC2_PA_MAP_REGNUM_CNS
    /* paMapFldPerReg           */ ,PRV_CPSS_DXCH_BC2_PA_MAP_FLD_PER_REG_CNS
    /* paMapTotalFlds           */ ,PRV_CPSS_DXCH_BC2_PA_MAP_TOTAL_FLDS_REG_CNS
    /* paMapRegWorkSliceMapOffs */ ,PRV_CPSS_DXCH_BC2_PA_MAPREG_WORK_SLICEMAP_OFFS_CNS
    /* paMapRegWorkSliceMapLen  */ ,PRV_CPSS_DXCH_BC2_PA_MAPREG_WORK_SLICEMAP_LEN_CNS
    /* paMapRegWorkSliceEnOffs  */ ,PRV_CPSS_DXCH_BC2_PA_MAPREG_WORK_SLICE_EN_OFFS_CNS
    /* paMapRegWorkSliceEnLen   */ ,PRV_CPSS_DXCH_BC2_PA_MAPREG_WORK_SLICE_EN_LEN_CNS
};

static PRV_CPSS_DXCH_PA_UNIT_MAP_REG_FLD_DESCR_STC ilknTxFifo_MAP_REG =
{
    /* paMapRegNum              */  PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAP_FLD_PER_REG_CNS
    /* paMapFldPerReg           */ ,PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAP_REGNUM_CNS
    /* paMapTotalFlds           */ ,PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAP_TOTAL_FLDS_REG_CNS
    /* paMapRegWorkSliceMapOffs */ ,PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAPREG_WORK_SLICEMAP_OFFS_CNS
    /* paMapRegWorkSliceMapLen  */ ,PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAPREG_WORK_SLICEMAP_LEN_CNS
    /* paMapRegWorkSliceEnOffs  */ ,PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAPREG_WORK_SLICE_EN_OFFS_CNS
    /* paMapRegWorkSliceEnLen   */ ,PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAPREG_WORK_SLICE_EN_LEN_CNS
};


static PRV_CPSS_DXCH_PA_UNIT_MAP_REG_FLD_DESCR_STC rxDmaGlue_txDmaGlue_MAP_REG =
{
    /* paMapRegNum              */  PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAP_REGNUM_CNS        
    /* paMapFldPerReg           */ ,PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAP_FLD_PER_REG_CNS   
    /* paMapTotalFlds           */ ,PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAP_TOTAL_FLDS_REG_CNS
    /* paMapRegWorkSliceMapOffs */ ,PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAPREG_WORK_SLICEMAP_OFFS_CNS  
    /* paMapRegWorkSliceMapLen  */ ,PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAPREG_WORK_SLICEMAP_LEN_CNS   
    /* paMapRegWorkSliceEnOffs  */ ,PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAPREG_WORK_SLICE_EN_OFFS_CNS 
    /* paMapRegWorkSliceEnLen   */ ,PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAPREG_WORK_SLICE_EN_LEN_CNS  
};

static PRV_CPSS_DXCH_PA_UNIT_MAP_REG_FLD_DESCR_STC MPPM_MAP_REG =
{
    /* paMapRegNum              */  PRV_CPSS_DXCH_BobK_PA_MPPM_MAP_REGNUM_CNS        
    /* paMapFldPerReg           */ ,PRV_CPSS_DXCH_BobK_PA_MPPM_MAP_FLD_PER_REG_CNS   
    /* paMapTotalFlds           */ ,PRV_CPSS_DXCH_BobK_PA_MPPM_MAP_TOTAL_FLDS_REG_CNS
    /* paMapRegWorkSliceMapOffs */ ,PRV_CPSS_DXCH_BobK_PA_MPPM_MAPREG_WORK_SLICEMAP_OFFS_CNS  
    /* paMapRegWorkSliceMapLen  */ ,PRV_CPSS_DXCH_BobK_PA_MPPM_MAPREG_WORK_SLICEMAP_LEN_CNS   
    /* paMapRegWorkSliceEnOffs  */ ,PRV_CPSS_DXCH_BobK_PA_MPPM_MAPREG_WORK_SLICE_EN_OFFS_CNS 
    /* paMapRegWorkSliceEnLen   */ ,PRV_CPSS_DXCH_BobK_PA_MPPM_MAPREG_WORK_SLICE_EN_LEN_CNS  
};


/*---------------------------------------------------------
 * unit definition
 *---------------------------------------------------------
 */
static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_rxDmaDef =
{
     CPSS_DXCH_PA_UNIT_RXDMA_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, rxDMA[0].SIP_COMMON_MODULES.pizzaArbiter.pizzaArbiterCtrlReg)
    ,&rxDma_TxDma_TxFifo_EthTxFifo_MPPM_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, rxDMA[0].SIP_COMMON_MODULES.pizzaArbiter.pizzaArbiterConfigReg[0])
    ,&rxDma_TxDma_TxQ_TxFifo_EthTxFifo_MAP_REG
};

static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_txDmaDef =
{
     CPSS_DXCH_PA_UNIT_TXDMA_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, txDMA[0].txDMAPizzaArb.pizzaArbiter.pizzaArbiterCtrlReg)
    ,&rxDma_TxDma_TxFifo_EthTxFifo_MPPM_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, txDMA[0].txDMAPizzaArb.pizzaArbiter.pizzaArbiterConfigReg[0])
    ,&rxDma_TxDma_TxQ_TxFifo_EthTxFifo_MAP_REG
};

static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_txFifoDef =
{
     CPSS_DXCH_PA_UNIT_TX_FIFO_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, txFIFO[0].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterCtrlReg)
    ,&rxDma_TxDma_TxFifo_EthTxFifo_MPPM_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, txFIFO[0].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterConfigReg[0])
    ,&rxDma_TxDma_TxQ_TxFifo_EthTxFifo_MAP_REG
};

static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_ethTxFifoDef =
{
     CPSS_DXCH_PA_UNIT_ETH_TX_FIFO_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, SIP_ETH_TXFIFO[0].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterCtrlReg)
    ,&rxDma_TxDma_TxFifo_EthTxFifo_MPPM_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, SIP_ETH_TXFIFO[0].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterConfigReg[0])
    ,&rxDma_TxDma_TxQ_TxFifo_EthTxFifo_MAP_REG
};

static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_txQDef =
{
     CPSS_DXCH_PA_UNIT_TXQ_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, TXQ.dq[0].scheduler.portArbiterConfig.portsArbiterConfig)
    ,&txQ_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, TXQ.dq[0].scheduler.portArbiterConfig.portsArbiterMap[0])
    ,&rxDma_TxDma_TxQ_TxFifo_EthTxFifo_MAP_REG
};

static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_ilknTxFifoDef =
{
     CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, SIP_ILKN_TXFIFO.ilknTxFIFOArbiter.pizzaArbiterCtrlReg)
    ,&ilknTxFifo_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, SIP_ILKN_TXFIFO.ilknTxFIFOArbiter.pizzaSlicePortMap[0])
    ,&ilknTxFifo_MAP_REG
};


static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_rxDma_1_Def =
{
     CPSS_DXCH_PA_UNIT_RXDMA_1_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, rxDMA[1].SIP_COMMON_MODULES.pizzaArbiter.pizzaArbiterCtrlReg)
    ,&rxDma_TxDma_TxFifo_EthTxFifo_MPPM_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, rxDMA[1].SIP_COMMON_MODULES.pizzaArbiter.pizzaArbiterConfigReg[0])
    ,&rxDma_TxDma_TxQ_TxFifo_EthTxFifo_MAP_REG
};

static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_txDma_1_Def =
{
     CPSS_DXCH_PA_UNIT_TXDMA_1_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, txDMA[1].txDMAPizzaArb.pizzaArbiter.pizzaArbiterCtrlReg)
    ,&rxDma_TxDma_TxFifo_EthTxFifo_MPPM_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, txDMA[1].txDMAPizzaArb.pizzaArbiter.pizzaArbiterConfigReg[0])
    ,&rxDma_TxDma_TxQ_TxFifo_EthTxFifo_MAP_REG
};

static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_txFifo_1_Def =
{
     CPSS_DXCH_PA_UNIT_TX_FIFO_1_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, txFIFO[1].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterCtrlReg)
    ,&rxDma_TxDma_TxFifo_EthTxFifo_MPPM_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, txFIFO[1].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterConfigReg[0])
    ,&rxDma_TxDma_TxQ_TxFifo_EthTxFifo_MAP_REG
};

static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_ethTxFifo_1_Def =
{
     CPSS_DXCH_PA_UNIT_ETH_TX_FIFO_1_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, SIP_ETH_TXFIFO[1].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterCtrlReg)
    ,&rxDma_TxDma_TxFifo_EthTxFifo_MPPM_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, SIP_ETH_TXFIFO[1].txFIFOPizzaArb.pizzaArbiter.pizzaArbiterConfigReg[0])
    ,&rxDma_TxDma_TxQ_TxFifo_EthTxFifo_MAP_REG
};

static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_rxDmaGlue_Def =
{
     CPSS_DXCH_PA_UNIT_RXDMA_GLUE_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, RX_DMA_GLUE.pizzaArbiter.pizzaArbiterCtrlReg)
    ,&rxDmaGlue_txDmaGlue_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, RX_DMA_GLUE.pizzaArbiter.pizzaArbiterConfigReg[0])
    ,&rxDmaGlue_txDmaGlue_MAP_REG
};


static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_txDmaGlue_Def =
{
     CPSS_DXCH_PA_UNIT_TXDMA_GLUE_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, TX_DMA_GLUE.pizzaArbiter.pizzaArbiterCtrlReg )
    ,&rxDmaGlue_txDmaGlue_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, TX_DMA_GLUE.pizzaArbiter.pizzaArbiterConfigReg[0] )
    ,&rxDmaGlue_txDmaGlue_MAP_REG
};


static PRV_CPSS_DXCH_PA_UNIT_DEF_STC prv_MPPM_Def =
{
     CPSS_DXCH_PA_UNIT_MPPM_E
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, MPPM.pizzaArbiter.pizzaArbiterCtrlReg )
    ,&rxDma_TxDma_TxFifo_EthTxFifo_MPPM_CTRL_REG
    ,FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC, MPPM.pizzaArbiter.pizzaArbiterConfigReg[0] )
    ,&MPPM_MAP_REG
};


static PRV_CPSS_DXCH_PA_UNIT_DEF_STC *prv_paUnitDef_All[] = 
{
     &prv_rxDmaDef
    ,&prv_txDmaDef
    ,&prv_txFifoDef
    ,&prv_ethTxFifoDef
    ,&prv_ilknTxFifoDef
    ,&prv_txQDef
    ,&prv_rxDma_1_Def
    ,&prv_txDma_1_Def
    ,&prv_txFifo_1_Def
    ,&prv_ethTxFifo_1_Def
    ,&prv_rxDmaGlue_Def
    ,&prv_txDmaGlue_Def
    ,&prv_MPPM_Def
    ,(PRV_CPSS_DXCH_PA_UNIT_DEF_STC *)NULL
};



typedef struct PRV_CPSS_DXCH_BC2_PA_UNITS_DRV_STC
{
    GT_BOOL                         isInitilaized;
    PRV_CPSS_DXCH_BC2_PA_UNIT_STC   unitsDescrAll                 [CPSS_DXCH_PA_UNIT_MAX_E];
}PRV_CPSS_DXCH_BC2_PA_UNITS_DRV_STC;


static GT_STATUS internal_bobcat2PizzaArbiterSpecificPPUnitDescrDrvInit
(
    PRV_CPSS_DXCH_BC2_PA_UNIT_STC       *unitsDescrArr,
    GT_U32                               size,
    PRV_CPSS_DXCH_PA_UNIT_DEF_STC      **prv_paUnitDef_All
)
{
    GT_U32 i;
    PRV_CPSS_DXCH_PA_UNIT_DEF_STC  *pUnitDefPtr;
    PRV_CPSS_DXCH_BC2_PA_UNIT_STC  *unitsDescrPtr;

    /* init all possible definitions as invalide */
    for (i = 0 ; i < size; i++)
    {
        cpssOsMemSet(&unitsDescrArr[i],0xFF,sizeof(unitsDescrArr[i]));
        unitsDescrArr[i].unit = CPSS_DXCH_PA_UNIT_UNDEFINED_E;
    }

    /* init just specified units */
    for (i = 0 ;  prv_paUnitDef_All[i] != NULL ; i++)
    {
        pUnitDefPtr = prv_paUnitDef_All[i];
        unitsDescrPtr = &unitsDescrArr[pUnitDefPtr->unitType];

        if (unitsDescrPtr->unit == pUnitDefPtr->unitType)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* module already initialized !!! something bad  */
        }
        unitsDescrPtr->unit                                = pUnitDefPtr->unitType;
        unitsDescrPtr->ctrlReg.baseAddressPtrOffs          = pUnitDefPtr->ctrlRegBaseAddrOffs;
        unitsDescrPtr->ctrlReg.fldSliceNum2run.offset      = pUnitDefPtr->ctrlRegFldDefPtr->ctrlRegSliceNum2Run_Offs;
        unitsDescrPtr->ctrlReg.fldSliceNum2run.len         = pUnitDefPtr->ctrlRegFldDefPtr->ctrlRegSliceNum2Run_Len;
        unitsDescrPtr->ctrlReg.fldMapLoadEn.offset         = pUnitDefPtr->ctrlRegFldDefPtr->ctrlRegMapLoadEn_Offs;
        unitsDescrPtr->ctrlReg.fldMapLoadEn.len            = pUnitDefPtr->ctrlRegFldDefPtr->ctrlRegMapLoadEn_Len;
        unitsDescrPtr->ctrlReg.fldWorkConservModeEn.offset = pUnitDefPtr->ctrlRegFldDefPtr->ctrlRegWorkConservEn_Offs;
        unitsDescrPtr->ctrlReg.fldWorkConservModeEn.len    = pUnitDefPtr->ctrlRegFldDefPtr->ctrlRegWorkConservEn_Len;

        unitsDescrPtr->mapReg.baseAddressPtrOffs           = pUnitDefPtr->mapRegBaseAddrOffs;
        unitsDescrPtr->mapReg.totalReg                     = pUnitDefPtr->mapRegFldDefPtr->paMapRegNum;
        unitsDescrPtr->mapReg.numberFldsPerReg             = pUnitDefPtr->mapRegFldDefPtr->paMapFldPerReg;
        unitsDescrPtr->mapReg.totalFlds                    = pUnitDefPtr->mapRegFldDefPtr->paMapTotalFlds;
        unitsDescrPtr->mapReg.fldSlice2PortMap.offset      = pUnitDefPtr->mapRegFldDefPtr->paMapRegWorkSliceMapOffs;
        unitsDescrPtr->mapReg.fldSlice2PortMap.len         = pUnitDefPtr->mapRegFldDefPtr->paMapRegWorkSliceMapLen;
        unitsDescrPtr->mapReg.fldSliceEn.offset            = pUnitDefPtr->mapRegFldDefPtr->paMapRegWorkSliceEnOffs;
        unitsDescrPtr->mapReg.fldSliceEn.len               = pUnitDefPtr->mapRegFldDefPtr->paMapRegWorkSliceEnLen;
    }
    return GT_OK;
}


static GT_STATUS internal_bobcat2PizzaArbiterUnitDescrDrvInit
(
    PRV_CPSS_DXCH_BC2_PA_UNITS_DRV_STC  *prv_paUnitsDrvPtr
)
{
    GT_STATUS rc;
    CPSS_NULL_PTR_CHECK_MAC(prv_paUnitsDrvPtr);
    if (prv_paUnitsDrvPtr->isInitilaized == GT_TRUE)
    {
        return GT_OK;
    }

    rc = internal_bobcat2PizzaArbiterSpecificPPUnitDescrDrvInit(&prv_paUnitsDrvPtr->unitsDescrAll[0],CPSS_DXCH_PA_UNIT_MAX_E,
                                                                      &prv_paUnitDef_All[0]);
    if (rc != GT_OK)
    {
        return rc;
    }
    prv_paUnitsDrvPtr->isInitilaized = GT_TRUE;
    return GT_OK;
}




/*------------------------------------------------------------*/
/* Unit Desctition Level                                      */
/*   - Init                                                   */
/*   - Set (All)                                              */
/*   - Get (All) (still absent)                               */
/*------------------------------------------------------------*/
static GT_STATUS bobcat2PizzaArbiterUnitDrvRegAddrGet
(
    IN GT_U8     devNum,
    IN GT_U32    offsInStr,
    OUT GT_U32  **regAddrPtrPtr
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC  *regsAddrVer1;
    GT_U8                                *regsAddrVer1_U8;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(regAddrPtrPtr);

    regsAddrVer1 = &PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddrVer1;
    regsAddrVer1_U8 = (GT_U8 *)regsAddrVer1;
    *regAddrPtrPtr = (GT_U32*)&regsAddrVer1_U8[offsInStr];
    return GT_OK;
}

static GT_STATUS bobcat2PizzaArbiterUnitDrvCtrlRegAddr
(
    IN GT_U8                         devNum,
    IN PRV_CPSS_DXCH_BC2_PA_UNIT_STC *unitDescrPtr,
    OUT GT_U32                       *regAddrPtr
)
{
    GT_STATUS rc;
    GT_U32    *regBaseAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    CPSS_NULL_PTR_CHECK_MAC(unitDescrPtr);
    CPSS_NULL_PTR_CHECK_MAC(regAddrPtr);


    rc = bobcat2PizzaArbiterUnitDrvRegAddrGet(devNum,unitDescrPtr->ctrlReg.baseAddressPtrOffs,/*OUT*/&regBaseAddrPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    *regAddrPtr = *regBaseAddrPtr;
    if (*regAddrPtr == 0xfffffffb)  /* bad address stored at DB */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}


static GT_STATUS bobcat2PizzaArbiterUnitDrvMapRegAddrFldOffsLenGet
(
    IN GT_U8                         devNum,
    IN PRV_CPSS_DXCH_BC2_PA_UNIT_STC *unitDescrPtr,
    IN GT_U32                         sliceId,
    OUT GT_U32                       *regAddrPtr,
    OUT GT_U32                       *fldLenPtr,
    OUT GT_U32                       *fldOffsPtr
)
{
    GT_STATUS rc;
    GT_U32    *regBaseAddrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    CPSS_NULL_PTR_CHECK_MAC(unitDescrPtr);
    CPSS_NULL_PTR_CHECK_MAC(regAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(fldLenPtr );
    CPSS_NULL_PTR_CHECK_MAC(fldOffsPtr);


    rc = bobcat2PizzaArbiterUnitDrvRegAddrGet(devNum,unitDescrPtr->mapReg.baseAddressPtrOffs,/*OUT*/&regBaseAddrPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (sliceId >= unitDescrPtr->mapReg.totalFlds)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *regAddrPtr = regBaseAddrPtr[sliceId / unitDescrPtr->mapReg.numberFldsPerReg];
    *fldLenPtr  = unitDescrPtr->mapReg.fldSlice2PortMap.len + unitDescrPtr->mapReg.fldSliceEn.len;
    *fldOffsPtr = (*fldLenPtr) * (sliceId % unitDescrPtr->mapReg.numberFldsPerReg);
    return GT_OK;
}


static GT_STATUS internal_bobcat2PizzaArbiterUnitDescrDrvGet
(
    IN  GT_U8                                             devNum,
    IN  PRV_CPSS_DXCH_BC2_PA_UNIT_STC                    *unitDescrPtr,
    OUT CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC *unitStatePtr
)
{
    GT_STATUS   rc;
    GT_U32      sliceId;
    GT_U32      regAddrCtrl;
    GT_U32      regAddrMap;
    GT_U32      fldOffs;
    GT_U32      fldLen;
    GT_U32      fldVal;
    GT_PORT_NUM portNum;
    GT_U32      portNumMask;
    GT_U32      workConservModeEn;
    GT_U32      sliceNum;
    GT_U32      mapLoadEn;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    CPSS_NULL_PTR_CHECK_MAC(unitStatePtr);

    /*---------------------------*/
    /* control register          */
    /*---------------------------*/
    rc = bobcat2PizzaArbiterUnitDrvCtrlRegAddr(devNum,unitDescrPtr,&regAddrCtrl);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddrCtrl,
                                            unitDescrPtr->ctrlReg.fldMapLoadEn.offset,
                                            unitDescrPtr->ctrlReg.fldMapLoadEn.len,
                                            &mapLoadEn);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddrCtrl,
                                            unitDescrPtr->ctrlReg.fldWorkConservModeEn.offset,
                                            unitDescrPtr->ctrlReg.fldWorkConservModeEn.len,
                                            &workConservModeEn);


    /*------------------*/
    /* slice number get */
    /*------------------*/
    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddrCtrl,
                                            unitDescrPtr->ctrlReg.fldSliceNum2run.offset,
                                            unitDescrPtr->ctrlReg.fldSliceNum2run.len,
                                            &sliceNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    unitStatePtr->totalSlicesOnUnit     = unitDescrPtr->mapReg.totalFlds;
    unitStatePtr->totalConfiguredSlices = sliceNum;
    unitStatePtr->workConservingBit     = (GT_BOOL)workConservModeEn;
    unitStatePtr->pizzaMapLoadEnBit     = (GT_BOOL)mapLoadEn;

    /*-----------------------------*/
    /* mapping register            */
    /*-----------------------------*/
    /*----------------------*/
    /* enable mapped slices */
    /*----------------------*/
    for (sliceId = 0; sliceId < unitDescrPtr->mapReg.totalFlds; sliceId++)
    {
        rc = bobcat2PizzaArbiterUnitDrvMapRegAddrFldOffsLenGet(devNum,unitDescrPtr,sliceId,
                                                                /*OUT*/&regAddrMap,&fldLen,&fldOffs);
        if (rc != GT_OK)
        {
            return rc;
        }

        fldVal  = 0;
        rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddrMap,
                                                fldOffs, fldLen, &fldVal);
        if (rc != GT_OK)
        {
            return rc;
        }
        portNumMask = (1 << unitDescrPtr->mapReg.fldSlice2PortMap.len) - 1;
        portNum  = (fldVal>>unitDescrPtr->mapReg.fldSlice2PortMap.offset) & portNumMask;

        unitStatePtr->slice_occupied_by[sliceId] = portNum;
        if (0 != (fldVal >>unitDescrPtr->mapReg.fldSliceEn.offset))
        {
            unitStatePtr->slice_enable[sliceId] = GT_TRUE;
        }
        else
        {
            unitStatePtr->slice_enable[sliceId] = GT_FALSE;
        }
    }
    return GT_OK;
}


static GT_STATUS internal_bobcat2PizzaArbiterUnitDescrDrvSet
(
    IN GT_U8                          devNum,
    IN PRV_CPSS_DXCH_BC2_PA_UNIT_STC *unitDescrPtr,
    IN GT_U32                         sliceNum,
    IN GT_U32                        *slice2PortMapArr
)
{
    GT_STATUS rc;
    GT_U32    sliceId;
    GT_U32    regAddrCtrl;
    GT_U32    regAddrMap;
    GT_U32    fldOffs;
    GT_U32    fldLen;
    GT_U32    fldVal;
    GT_PORT_NUM maxPort;
    GT_PORT_NUM portNum;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    if (sliceNum > 0)
    {
        CPSS_NULL_PTR_CHECK_MAC(slice2PortMapArr);
        if (sliceNum > unitDescrPtr->mapReg.totalFlds)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

    }
    if (unitDescrPtr->unit == CPSS_DXCH_PA_UNIT_ETH_TX_FIFO_1_E)
    {
        unitDescrPtr->unit = unitDescrPtr->unit;
    }

    /*---------------------------*/
    /* load disable              */
    /*---------------------------*/
    rc = bobcat2PizzaArbiterUnitDrvCtrlRegAddr(devNum,unitDescrPtr,/*OUT*/&regAddrCtrl);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 0, regAddrCtrl,
                                            unitDescrPtr->ctrlReg.fldMapLoadEn.offset,
                                            unitDescrPtr->ctrlReg.fldMapLoadEn.len,
                                            0);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*----------------------------------------------*/
    /* Check PZ Map Load Hold bit[2] to become 0x1  */
    /*----------------------------------------------*/
#if 0
    if (unitDescrPtr->unit == CPSS_DXCH_PA_UNIT_TXQ_E)
    {
        GT_U32 mapLoadHold;
        GT_U32 regAddrStatus;
        PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC  *regsAddrVer1;

        regsAddrVer1 = &PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddrVer1;



        regAddrStatus = regsAddrVer1->TXQ.dq[0].scheduler.portArbiterConfig.portsArbiterStatus;

        do 
        {
            #ifndef ASIC_SIMULATION
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, 0, regAddrStatus,
                                                    2,
                                                    1,
                                                    /*OUT*/&mapLoadHold);
            #else
                mapLoadHold = 1;
            #endif
        }while (mapLoadHold != 1);

    }
#endif

    /*------------------*/
    /* slice number set */
    /*------------------*/
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 0, regAddrCtrl,
                                            unitDescrPtr->ctrlReg.fldSliceNum2run.offset,
                                            unitDescrPtr->ctrlReg.fldSliceNum2run.len,
                                            sliceNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*-----------------------------*/
    /* mapping register            */
    /*----------------------*/
    /* enable mapped slices */
    /*----------------------*/
    if (slice2PortMapArr != NULL)
    {
        maxPort = 1<<unitDescrPtr->mapReg.fldSlice2PortMap.len;
        for (sliceId = 0; sliceId < sliceNum ; sliceId++)
        {
            rc = bobcat2PizzaArbiterUnitDrvMapRegAddrFldOffsLenGet(devNum,unitDescrPtr,sliceId,
                                                                    /*OUT*/&regAddrMap,&fldLen,&fldOffs);
            if (rc != GT_OK)
            {
                return rc;
            }

            fldVal = 0;
            portNum = slice2PortMapArr[sliceId];
            if (portNum != PA_INVALID_PORT_CNS)
            {
                if (portNum >= maxPort)
                {
                    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
                    {
                        /* patch for SIP_5_20 */
                        CPSS_TBD_BOOKMARK_BOBCAT3
                        /* allow the writing , any value is OK since
                           simulation/GM not care about it */
                        portNum %= maxPort;
                    }
                    else
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }
                }
                fldVal  = fldVal | (portNum << unitDescrPtr->mapReg.fldSlice2PortMap.offset);
                fldVal  = fldVal | (1 << unitDescrPtr->mapReg.fldSliceEn.offset);
            }

            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 0, regAddrMap,
                                                    fldOffs, fldLen, fldVal);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    /*---------------------------*/
    /* disable non-mapped slices */
    /*---------------------------*/
    for (sliceId = sliceNum; sliceId < unitDescrPtr->mapReg.totalFlds; sliceId++)
    {
        rc = bobcat2PizzaArbiterUnitDrvMapRegAddrFldOffsLenGet(devNum,unitDescrPtr,sliceId,
                                                                /*OUT*/&regAddrMap,&fldLen,&fldOffs);
        if (rc != GT_OK)
        {
            return rc;
        }

        fldVal  = 0;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 0, regAddrMap,
                                                fldOffs, fldLen, fldVal);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /*---------------------------*/
    /* load enable               */
    /*---------------------------*/
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 0, regAddrCtrl,
                                            unitDescrPtr->ctrlReg.fldMapLoadEn.offset,
                                            unitDescrPtr->ctrlReg.fldMapLoadEn.len,
                                            1);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

GT_STATUS internal_bobcat2PizzaArbiterUnitDescrDrvWorkConservingModeSet
(
    IN GT_U8    devNum,
    IN PRV_CPSS_DXCH_BC2_PA_UNIT_STC * unitDescrPtr,
    IN GT_BOOL  status
)
{
    GT_STATUS rc;
    GT_U32    regAddrCtrl;
    GT_U32    value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(unitDescrPtr);
    /*------------------*/
    /* control register */
    /*------------------*/
    rc = bobcat2PizzaArbiterUnitDrvCtrlRegAddr(devNum,unitDescrPtr,&regAddrCtrl);
    if (rc != GT_OK)
    {
        return rc;
    }

    value = (GT_U32)status;
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 0, regAddrCtrl,
                                            unitDescrPtr->ctrlReg.fldWorkConservModeEn.offset,
                                            unitDescrPtr->ctrlReg.fldWorkConservModeEn.len,
                                            value);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

GT_STATUS internal_bobcat2PizzaArbiterUnitDescrDrvWorkConservingModeGet
(
    IN GT_U8    devNum,
    IN PRV_CPSS_DXCH_BC2_PA_UNIT_STC * unitDescrPtr,
    IN GT_BOOL *statusPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddrCtrl;
    GT_U32    value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(unitDescrPtr);
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);
    /*------------------*/
    /* control register */
    /*------------------*/
    rc = bobcat2PizzaArbiterUnitDrvCtrlRegAddr(devNum,unitDescrPtr,&regAddrCtrl);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddrCtrl,
                                            unitDescrPtr->ctrlReg.fldWorkConservModeEn.offset,
                                            unitDescrPtr->ctrlReg.fldWorkConservModeEn.len,
                                            &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *statusPtr = (GT_BOOL)value;
    return GT_OK;
}

GT_STATUS interval_bobcat2PizzaArbiterUnitDrvInit
(
    IN GT_U8                          devNum,
    IN PRV_CPSS_DXCH_BC2_PA_UNIT_STC *unitDescrPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddrCtrl;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    /*------------------*/
    /* control register */
    /*------------------*/
    rc = bobcat2PizzaArbiterUnitDrvCtrlRegAddr(devNum,unitDescrPtr,&regAddrCtrl);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 0, regAddrCtrl,
                                            unitDescrPtr->ctrlReg.fldSliceNum2run.offset,
                                            unitDescrPtr->ctrlReg.fldSliceNum2run.len,
                                            0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 0, regAddrCtrl,
                                            unitDescrPtr->ctrlReg.fldWorkConservModeEn.offset,
                                            unitDescrPtr->ctrlReg.fldWorkConservModeEn.len,
                                            0);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*---------------------------*/
    /* disable non-mapped slices */
    /*---------------------------*/
    rc = internal_bobcat2PizzaArbiterUnitDescrDrvSet(devNum,unitDescrPtr,0,(GT_U32*)NULL);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}




/*-------------------------------------------------------------------------
 *   Level UnitId :
 *        bobcat2PizzaArbiterUnitDrvSet
 *        bobcat2PizzaArbiterUnitDrvGet
 *        bobcat2PizzaArbiterUnitDrvWorkConservingModeSet
 *        bobcat2PizzaArbiterUnitDrvWorkConservingModeGet
 *        bobcat2PizzaArbiterUnitDrvInit
 *-------------------------------------------------------------------------
 */
PRV_CPSS_DXCH_BC2_PA_UNITS_DRV_STC prv_paUnitsDrv;

static GT_STATUS internal_bobcat2PizzaArbiterUnitDescrDrvDescrByUnitIdGet
(
    IN  CPSS_DXCH_PA_UNIT_ENT unitId,
    OUT PRV_CPSS_DXCH_BC2_PA_UNIT_STC ** unitDescrPtrPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(unitDescrPtrPtr);

    if (unitId < 0 || unitId >= CPSS_DXCH_PA_UNIT_MAX_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (prv_paUnitsDrv.isInitilaized == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* something bad, driver is called to get data BEFORE it is initialized */
    }

    *unitDescrPtrPtr = &prv_paUnitsDrv.unitsDescrAll[unitId];
    return GT_OK;
}

GT_STATUS bobcat2PizzaArbiterUnitDrvSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unitId,
    IN GT_U32   sliceNum,
    IN GT_U32  *slice2PortMapArr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_BC2_PA_UNIT_STC * unitDescrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    CPSS_NULL_PTR_CHECK_MAC(slice2PortMapArr);

    rc = internal_bobcat2PizzaArbiterUnitDescrDrvDescrByUnitIdGet(unitId, /*OUT*/&unitDescrPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = internal_bobcat2PizzaArbiterUnitDescrDrvSet(devNum,unitDescrPtr,sliceNum,slice2PortMapArr);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

GT_STATUS bobcat2PizzaArbiterUnitDrvGet
(
    IN GT_U8                                           devNum,
    IN CPSS_DXCH_PA_UNIT_ENT                           unitId,
    CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC * unitStatePtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_BC2_PA_UNIT_STC * unitDescrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    CPSS_NULL_PTR_CHECK_MAC(unitStatePtr);
    rc = internal_bobcat2PizzaArbiterUnitDescrDrvDescrByUnitIdGet(unitId, /*OUT*/&unitDescrPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = internal_bobcat2PizzaArbiterUnitDescrDrvGet(devNum,unitDescrPtr,unitStatePtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


GT_STATUS bobcat2PizzaArbiterUnitDrvWorkConservingModeSet
(
    IN GT_U8                 devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unitId,
    IN GT_BOOL               status
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_BC2_PA_UNIT_STC * unitDescrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = internal_bobcat2PizzaArbiterUnitDescrDrvDescrByUnitIdGet(unitId, /*OUT*/&unitDescrPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = internal_bobcat2PizzaArbiterUnitDescrDrvWorkConservingModeSet(devNum,unitDescrPtr,status);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

GT_STATUS bobcat2PizzaArbiterUnitDrvWorkConservingModeGet
(
    IN GT_U8                 devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unitId,
    IN GT_BOOL              *statusPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_BC2_PA_UNIT_STC * unitDescrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = internal_bobcat2PizzaArbiterUnitDescrDrvDescrByUnitIdGet(unitId, /*OUT*/&unitDescrPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = internal_bobcat2PizzaArbiterUnitDescrDrvWorkConservingModeGet(devNum,unitDescrPtr,statusPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;

}


GT_STATUS bobcat2PizzaArbiterUnitDrvInit
(
    IN GT_U8                 devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unitId
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_BC2_PA_UNIT_STC * unitDescrPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (prv_paUnitsDrv.isInitilaized == GT_FALSE)
    {
        rc = internal_bobcat2PizzaArbiterUnitDescrDrvInit(&prv_paUnitsDrv);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = internal_bobcat2PizzaArbiterUnitDescrDrvDescrByUnitIdGet(unitId, /*OUT*/&unitDescrPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E)
    {
        rc = interval_bobcat2PizzaArbiterUnitDrvInit(devNum,unitDescrPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
   
    return GT_OK;
}



/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfTxQPortWorkConservingModeSet
*
* DESCRIPTION:
*       Configure TxQ port work conserving mode
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       txqPort  - txq port
*       status   - enable (1) disable (0)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfTxQPortWorkConservingModeSet
(
    IN GT_U8                     devNum,
    IN GT_U32                    txqPort,
    IN GT_BOOL                   status
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
    {
        if (PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_NONE_E && PRV_CPSS_PP_MAC(devNum)->revision == 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }
    if (txqPort >= PRV_CPSS_DXCH_BC2B0_PORT_WORK_PORTN_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).scheduler.portArbiterConfig.portsArbiterPortWorkConservingEnable[txqPort];
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 0, regAddr,
                                            PRV_CPSS_DXCH_BC2B0_PORT_WORK_CONSERV_OFFS_CNS,
                                            PRV_CPSS_DXCH_BC2B0_PORT_WORK_CONSERV_LEN_CNS,
                                            (GT_U32)status);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfTxQPortWorkConservingModeGet
*
* DESCRIPTION:
*       Configure TxQ port work conserving mode
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       txqPort  - txq port
*
* OUTPUTS:
*       statusPtr- enable (1) disable (0)
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfTxQPortWorkConservingModeGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    txqPort,
    OUT GT_BOOL                  *statusPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    value;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
    {
        if (PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_NONE_E && PRV_CPSS_PP_MAC(devNum)->revision == 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }
    if (txqPort >= PRV_CPSS_DXCH_BC2B0_PORT_WORK_PORTN_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).scheduler.portArbiterConfig.portsArbiterPortWorkConservingEnable[txqPort];
    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddr,
                                            PRV_CPSS_DXCH_BC2B0_PORT_WORK_CONSERV_OFFS_CNS,
                                            PRV_CPSS_DXCH_BC2B0_PORT_WORK_CONSERV_LEN_CNS,
                                            &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (value > GT_TRUE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }
    *statusPtr = (GT_BOOL)value;
    return GT_OK;
}





