/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortDynamicPAUnitDrv.h
*
* DESCRIPTION:
*       bobcat2 and higher dynamic (algorithmic) pizza arbiter 
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_PORT_DYNAMIC_PA_UNIT_DRV_H
#define __PRV_CPSS_DXCH_PORT_DYNAMIC_PA_UNIT_DRV_H

#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define PA_INVALID_PORT_CNS  0xFFFF


typedef struct 
{
    GT_U32 offset;
    GT_U32 len;
}PRV_CPSS_DXCH_PA_REG_FIELDC_STC;

typedef struct 
{
    GT_U32                           baseAddressPtrOffs;
    PRV_CPSS_DXCH_PA_REG_FIELDC_STC  fldSliceNum2run;
    PRV_CPSS_DXCH_PA_REG_FIELDC_STC  fldMapLoadEn;
    PRV_CPSS_DXCH_PA_REG_FIELDC_STC  fldWorkConservModeEn;
}PRV_CPSS_DXCH_PA_CTRL_REG_STC;

typedef struct 
{
    GT_U32                           baseAddressPtrOffs;
    PRV_CPSS_DXCH_PA_REG_FIELDC_STC  fldSlice2PortMap;
    PRV_CPSS_DXCH_PA_REG_FIELDC_STC  fldSliceEn;
    GT_U32                           totalReg;
    GT_U32                           numberFldsPerReg;
    GT_U32                           totalFlds;
}PRV_CPSS_DXCH_PA_MAP_REG_STC;


typedef struct 
{
    CPSS_DXCH_PA_UNIT_ENT unit;
    PRV_CPSS_DXCH_PA_CTRL_REG_STC ctrlReg;
    PRV_CPSS_DXCH_PA_MAP_REG_STC  mapReg;
}PRV_CPSS_DXCH_BC2_PA_UNIT_STC;

/* RXDMA TXDMA TX-FIFO ETH-TX_FIFO */

#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_SLICE_NUM_2_RUN_OFFS_CNS  0
#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_SLICE_NUM_2_RUN_LEN_CNS   9

#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_MAP_LOAD_EN_OFFS_CNS      14
#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_MAP_LOAD_EN_LEN_CNS       1

#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_WORK_CONSERV_EN_OFFS_CNS  15
#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_WORK_CONSERV_EN_LEN_CNS   1

/* TXQ */
#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_SLICE_NUM_2_RUN_OFFS_CNS  0
#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_SLICE_NUM_2_RUN_LEN_CNS   9

#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_WORK_CONSERV_EN_OFFS_CNS  9
#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_WORK_CONSERV_EN_LEN_CNS   1

#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_MAP_LOAD_EN_OFFS_CNS      12
#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_TXQ_MAP_LOAD_EN_LEN_CNS       1


/* Ilkn-Tx-Fifo */
#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_ILKN_FX_FIFO_SLICE_NUM_2_RUN_OFFS_CNS  0
#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_ILKN_FX_FIFO_SLICE_NUM_2_RUN_LEN_CNS   11

#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_ILKN_FX_FIFO_MAP_LOAD_EN_OFFS_CNS      11
#define PRV_CPSS_DXCH_BC2_PA_CTRLREG_ILKN_FX_FIFO_MAP_LOAD_EN_LEN_CNS       1


/* tx-Dma-Glue rx-Dma-Glue */
#define PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_SLICE_NUM_2_RUN_OFFS_CNS  0
#define PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_SLICE_NUM_2_RUN_LEN_CNS   8

#define PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_MAP_LOAD_EN_OFFS_CNS      9
#define PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_MAP_LOAD_EN_LEN_CNS       1

#define PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_WORK_CONSERV_EN_OFFS_CNS 10
#define PRV_CPSS_DXCH_BobK_PA_CTRLREG_RX_TX_DMA_GLUE_WORK_CONSERV_EN_LEN_CNS   1


/*-------------------------*/
/* MAP register definition */
/*-------------------------*/
/* RXDMA TXDMA TXQ TX-FIFO ETH-TX-FIFO */
#define PRV_CPSS_DXCH_BC2_PA_MAP_REGNUM_CNS                    85
#define PRV_CPSS_DXCH_BC2_PA_MAP_FLD_PER_REG_CNS               4
#define PRV_CPSS_DXCH_BC2_PA_MAP_TOTAL_FLDS_REG_CNS            340

#define PRV_CPSS_DXCH_BC2_PA_MAPREG_WORK_SLICEMAP_OFFS_CNS     0
#define PRV_CPSS_DXCH_BC2_PA_MAPREG_WORK_SLICEMAP_LEN_CNS      7

#define PRV_CPSS_DXCH_BC2_PA_MAPREG_WORK_SLICE_EN_OFFS_CNS     7
#define PRV_CPSS_DXCH_BC2_PA_MAPREG_WORK_SLICE_EN_LEN_CNS      1

/* Ilkn-Tx-FIFO  */
#define PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAP_REGNUM_CNS                    1064
#define PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAP_FLD_PER_REG_CNS               1
#define PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAP_TOTAL_FLDS_REG_CNS            1064

#define PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAPREG_WORK_SLICEMAP_OFFS_CNS     0
#define PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAPREG_WORK_SLICEMAP_LEN_CNS      6

#define PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAPREG_WORK_SLICE_EN_OFFS_CNS     6
#define PRV_CPSS_DXCH_BC2_PA_ILKN_TX_FIFO_MAPREG_WORK_SLICE_EN_LEN_CNS      1


/* rx-dma-glue tx-dma-glue  */
#define PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAP_REGNUM_CNS                    37
#define PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAP_FLD_PER_REG_CNS               5
#define PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAP_TOTAL_FLDS_REG_CNS            185

#define PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAPREG_WORK_SLICEMAP_OFFS_CNS     0
#define PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAPREG_WORK_SLICEMAP_LEN_CNS      1

#define PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAPREG_WORK_SLICE_EN_OFFS_CNS     1
#define PRV_CPSS_DXCH_BobK_PA_RX_TX_DMA_GLUE_MAPREG_WORK_SLICE_EN_LEN_CNS      1


/* MPPM  */
#define PRV_CPSS_DXCH_BobK_PA_MPPM_MAP_REGNUM_CNS                    93
#define PRV_CPSS_DXCH_BobK_PA_MPPM_MAP_FLD_PER_REG_CNS               4
#define PRV_CPSS_DXCH_BobK_PA_MPPM_MAP_TOTAL_FLDS_REG_CNS            372

#define PRV_CPSS_DXCH_BobK_PA_MPPM_MAPREG_WORK_SLICEMAP_OFFS_CNS     0
#define PRV_CPSS_DXCH_BobK_PA_MPPM_MAPREG_WORK_SLICEMAP_LEN_CNS      7

#define PRV_CPSS_DXCH_BobK_PA_MPPM_MAPREG_WORK_SLICE_EN_OFFS_CNS     7
#define PRV_CPSS_DXCH_BobK_PA_MPPM_MAPREG_WORK_SLICE_EN_LEN_CNS      1


GT_STATUS bobcat2PizzaArbiterUnitDrvGet
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PA_UNIT_ENT unitId,
    OUT CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC * unitStatePtr
);


GT_STATUS bobcat2PizzaArbiterUnitDrvSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unitId,
    IN GT_U32   sliceNum,
    IN GT_U32  *slice2PortMapArr
);

/*------------------------------------*/
/* Work conserving mode per TxQ port  */
/*------------------------------------*/
#define PRV_CPSS_DXCH_BC2B0_PORT_WORK_PORTN_CNS       72
#define PRV_CPSS_DXCH_BC2B0_PORT_WORK_CONSERV_OFFS_CNS 0
#define PRV_CPSS_DXCH_BC2B0_PORT_WORK_CONSERV_LEN_CNS  1

GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfTxQPortWorkConservingModeSet
(
    IN GT_U8                     devNum,
    IN GT_U32                    txqPort,
    IN GT_BOOL                   status
);

GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfTxQPortWorkConservingModeGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    txqPort,
    OUT GT_BOOL                  *statusPtr
);


/*------------------------------------*/
/* Work conserving mode per unit      */
/*------------------------------------*/
GT_STATUS bobcat2PizzaArbiterUnitDrvWorkConservingModeSet
(
    IN GT_U8                 devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unitId,
    IN GT_BOOL               status
);

GT_STATUS bobcat2PizzaArbiterUnitDrvWorkConservingModeGet
(
    IN GT_U8                 devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unitId,
    IN GT_BOOL              *statusPtr
);



GT_STATUS bobcat2PizzaArbiterUnitDrvInit
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unitId
);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
