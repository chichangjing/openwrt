/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssHwInit.h
*
* DESCRIPTION:
*       CPSS definitions for HW init.
*
* FILE REVISION NUMBER:
*       $Revision: 48 $
*
*******************************************************************************/
#ifndef __prvCpssHwInith
#define __prvCpssHwInith

#include <cpss/generic/interrupts/private/prvCpssGenIntDefs.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/config/cpssGenCfg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* This is used to indicate the specific data types (pointer fields or write
   only data) in the registers addresses struct */
#define PRV_CPSS_SW_PTR_BOOKMARK_CNS     0xFFFFFFFF

/* This is used to mark pointer fields or registers that are not initialized in
   the registers addresses struct */
#define PRV_CPSS_SW_PTR_ENTRY_UNUSED     0xFFFFFFFB

/* This is used to mark write only data in the registers addresses struct */
#define PRV_CPSS_SW_TYPE_WRONLY_CNS     0x1

/* This is used to mark dynamically allocated pointer fields in the registers
   addresses struct */
#define PRV_CPSS_SW_TYPE_PTR_FIELD_CNS  0x2

/* macro to free an allocated pointer, and set it to NULL */
#if __WORDSIZE != 64
#define FREE_PTR_IF_NOT_NULL_MAC(ptr)       \
        if(ptr && (((GT_U32)ptr) != PRV_CPSS_SW_PTR_ENTRY_UNUSED)) \
        {                                   \
            cpssOsFree(ptr);                \
            ptr=NULL;                       \
        }
#else
#define FREE_PTR_IF_NOT_NULL_MAC(ptr)       \
        if(ptr && (((GT_UINTPTR)ptr) != 0xFFFFFFFBFFFFFFFB)) \
        {                                   \
            cpssOsFree(ptr);                \
            ptr=NULL;                       \
        }
#endif

/* sip_5_20 convert global txq-dq port number to local txq-dq port number */
#define SIP_5_20_GLOBAL_TXQ_DQ_PORT_TO_LOCAL_TXQ_DQ_PORT_MAC(dev,globalTxqDqPortIndex) \
    ((globalTxqDqPortIndex) % PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.multiDataPath.txqDqNumPortsPerDp)

/* sip_5_20 convert global txq-dq port number to DQ unit index */
#define SIP_5_20_GLOBAL_TXQ_DQ_PORT_TO_DQ_UNIT_INDEX_MAC(dev,globalTxqDqPortIndex) \
    ((globalTxqDqPortIndex) / PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.multiDataPath.txqDqNumPortsPerDp)

/* sip_5_20 convert local txq-dq port number in specific DQ unit to global txq-dq port number */
#define SIP_5_20_LOCAL_TXQ_DQ_PORT_TO_GLOBAL_TXQ_DQ_PORT_MAC(dev,localTxqDqPortIndex,dqIndex) \
    ((localTxqDqPortIndex) + ((dqIndex) * PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.multiDataPath.txqDqNumPortsPerDp))

/* sip_5_20 access to the DB of 'per port' the txq-Dq unit index */
#define SIP_5_20_TXQ_DQ_UNIT_INDEX_OF_PORT_MAC(dev,_physPort)   \
    PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.multiDataPath.mapPhyPortToTxqDqUnitIndex[_physPort]

/* sip_5_20 the max number of global txq-Dq ports */
#define SIP_5_20_TXQ_DQ_MAX_PORTS_MAC(dev) \
    (PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.multiDataPath.txqDqNumPortsPerDp * \
     PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.multiDataPath.maxDp)

/* sip_5_20 the max number of port in TXDMA */
#define SIP_5_20_TXDMA_MAX_PORTS_MAC(dev) \
    (PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.multiDataPath.txNumPortsPerDp)


/*
 * typedef: enum PRV_CPSS_DXCH_UNIT_ENT
 *
 * Description: Defines silicon units.
 *
 * Enumerations:
 *
 *  PRV_CPSS_DXCH_UNIT_TTI_E                  - tti unit
 *  PRV_CPSS_DXCH_UNIT_PCL_E                  - pcl unit
 *  PRV_CPSS_DXCH_UNIT_L2I_E                  - l2 unit
 *  PRV_CPSS_DXCH_UNIT_FDB_E                  - fdb unit
 *  PRV_CPSS_DXCH_UNIT_EQ_E                   - eq unit
 *  PRV_CPSS_DXCH_UNIT_LPM_E                  - lpm unit
 *  PRV_CPSS_DXCH_UNIT_EGF_EFT_E              - egf_eft unit
 *  PRV_CPSS_DXCH_UNIT_EGF_QAG_E              - egf_qag unit
 *  PRV_CPSS_DXCH_UNIT_EGF_SHT_E              - egf_sht unit
 *  PRV_CPSS_DXCH_UNIT_HA_E                   - ha unit
 *  PRV_CPSS_DXCH_UNIT_ETS_E                  - ets unit
 *  PRV_CPSS_DXCH_UNIT_MLL_E                  - mll unit
 *  PRV_CPSS_DXCH_UNIT_IPLR_E                 - iplr unit
 *  PRV_CPSS_DXCH_UNIT_EPLR_E                 - eplr unit
 *  PRV_CPSS_DXCH_UNIT_IPVX_E                 - ipvx unit
 *  PRV_CPSS_DXCH_UNIT_IOAM_E                 - ioam unit
 *  PRV_CPSS_DXCH_UNIT_EOAM_E                 - eoam unit
 *  PRV_CPSS_DXCH_UNIT_TCAM_E                 - tcam unit
 *  PRV_CPSS_DXCH_UNIT_RXDMA_E                - rx_dma unit
 *  PRV_CPSS_DXCH_UNIT_EPCL_E                 - EPCL unit
 *  PRV_CPSS_DXCH_UNIT_TM_FCU_E               -  unit TM_FCU
 *  PRV_CPSS_DXCH_UNIT_TM_DROP_E              -  unit TM_DROP
 *  PRV_CPSS_DXCH_UNIT_TM_QMAP_E              -  unit TM_QMAP
 *  PRV_CPSS_DXCH_UNIT_TM_E                   -  unit TM
 *  PRV_CPSS_DXCH_UNIT_GOP_E                  -  unit GOP
 *  PRV_CPSS_DXCH_UNIT_GOP1_E                 -  unit GOP for SIP5 devices
 *                                               (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *  PRV_CPSS_DXCH_UNIT_MIB_E                  -  unit MIB
 *  PRV_CPSS_DXCH_UNIT_SERDES_E               -  unit SERDES
 *  PRV_CPSS_DXCH_UNIT_ERMRK_E                -  unit ERMRK
 *  PRV_CPSS_DXCH_UNIT_BM_E                   -  unit BM
 *  PRV_CPSS_DXCH_UNIT_TM_INGRESS_GLUE_E      -  unit TM_INGRESS_GLUE
 *  PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E       -  unit TM_EGRESS_GLUE
 *  PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E           -  unit ETH_TXFIFO
 *  PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E          -  unit ILKN_TXFIFO
 *  PRV_CPSS_DXCH_UNIT_ILKN_E                 -  unit ILKN
 *  PRV_CPSS_DXCH_UNIT_LMS0_0_E               -  unit LMS0_0
 *  PRV_CPSS_DXCH_UNIT_LMS0_1_E               -  unit LMS0_1
 *  PRV_CPSS_DXCH_UNIT_LMS0_2_E               -  unit LMS0_2
 *  PRV_CPSS_DXCH_UNIT_LMS0_3_E               -  unit LMS0_3
 *  PRV_CPSS_DXCH_UNIT_TX_FIFO_E              -  unit TX_FIFO
 *  PRV_CPSS_DXCH_UNIT_BMA_E                  -  unit BMA
 *  PRV_CPSS_DXCH_UNIT_CNC_0_E                -  unit CNC_0
 *  PRV_CPSS_DXCH_UNIT_CNC_1_E                -  unit CNC_1
 *  PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E            -  unit TXQ_QUEUE
 *  PRV_CPSS_DXCH_UNIT_TXQ_LL_E               -  unit TXQ_LL
 *  PRV_CPSS_DXCH_UNIT_TXQ_PFC_E              -  unit TXQ_PFC
 *  PRV_CPSS_DXCH_UNIT_TXQ_QCN_E              -  unit TXQ_QCN
 *  PRV_CPSS_DXCH_UNIT_TXQ_DQ_E               -  unit TXQ_DQ
 *  PRV_CPSS_DXCH_UNIT_DFX_SERVER_E           -  unit DFX_SERVER
 *  PRV_CPSS_DXCH_UNIT_MPPM_E                 -  unit MPPM
 *  PRV_CPSS_DXCH_UNIT_LMS1_0_E               -  unit LMS1_0
 *  PRV_CPSS_DXCH_UNIT_LMS1_1_E               -  unit LMS1_1
 *  PRV_CPSS_DXCH_UNIT_LMS1_2_E               -  unit LMS1_2
 *  PRV_CPSS_DXCH_UNIT_LMS1_3_E               -  unit LMS1_3
 *  PRV_CPSS_DXCH_UNIT_LMS2_0_E               -  unit LMS2_0
 *  PRV_CPSS_DXCH_UNIT_LMS2_1_E               -  unit LMS2_1
 *  PRV_CPSS_DXCH_UNIT_LMS2_2_E               -  unit LMS2_2
 *  PRV_CPSS_DXCH_UNIT_LMS2_3_E               -  unit LMS2_3
 *  PRV_CPSS_DXCH_UNIT_MPPM_1_E               -  unit MPPM_1
 *  PRV_CPSS_DXCH_UNIT_CTU_0_E                -  unit CTU_0
 *  PRV_CPSS_DXCH_UNIT_CTU_1_E                -  unit CTU_1
 *  PRV_CPSS_DXCH_UNIT_TXQ_SHT_E              -  unit TXQ_SHT
 *  PRV_CPSS_DXCH_UNIT_TXQ_EGR0_E             -  unit TXQ_EGR0
 *  PRV_CPSS_DXCH_UNIT_TXQ_EGR1_E             -  unit TXQ_EGR1
 *  PRV_CPSS_DXCH_UNIT_TXQ_DIST_E             -  unit TXQ_DIST
 *  PRV_CPSS_DXCH_UNIT_IPLR_1_E               -  unit IPLR_1
 *  PRV_CPSS_DXCH_UNIT_TXDMA_E                -  unit TXDMA
 *  PRV_CPSS_DXCH_UNIT_MG_E                   -  unit MG
 *  PRV_CPSS_DXCH_UNIT_TCC_IPCL_E             -  unit TCC_IPCL
 *  PRV_CPSS_DXCH_UNIT_TCC_IPVX_E             -  unit TCC_IPVX
 *  PRV_CPSS_DXCH_UNIT_SMI_0_E                - SMI unit 0
 *  PRV_CPSS_DXCH_UNIT_SMI_1_E                - SMI unit 1
 *  PRV_CPSS_DXCH_UNIT_SMI_2_E                - SMI unit 2
 *  PRV_CPSS_DXCH_UNIT_SMI_3_E                - SMI unit 3
 *  PRV_CPSS_DXCH_UNIT_LED_0_E                - LED unit 0
 *  PRV_CPSS_DXCH_UNIT_LED_1_E                - LED unit 1
 *  PRV_CPSS_DXCH_UNIT_LED_2_E                - LED unit 2
 *  PRV_CPSS_DXCH_UNIT_LED_3_E                - LED unit 3
 *  PRV_CPSS_DXCH_UNIT_LED_4_E                - LED unit 4
 *
 *  PRV_CPSS_DXCH_UNIT_RXDMA1_E               - RXDMA   unit index 1 (bobk)
 *  PRV_CPSS_DXCH_UNIT_TXDMA1_E               - TXDMA   unit index 1 (bobk)
 *  PRV_CPSS_DXCH_UNIT_TX_FIFO1_E             - TX_FIFO unit index 1 (bobk)
 *  PRV_CPSS_DXCH_UNIT_ETH_TX_FIFO1_E          - ETH_TXFIFO1 unit index 1 (bobk)
 *
 *  PRV_CPSS_DXCH_UNIT_RXDMA_GLUE_E           - RXDMA GLUE unit (bobk)
 *  PRV_CPSS_DXCH_UNIT_TXDMA_GLUE_E           - TXDMA GLUE unit (bobk)
 *  PRV_CPSS_DXCH_UNIT_MPPM_E                 - MPPM unit (bobk)
 *
 *
 *   PRV_CPSS_DXCH_UNIT_RXDMA2_E,             - RxDma unit index 2 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_RXDMA3_E,             - RxDma unit index 3 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_RXDMA4_E,             - RxDma unit index 4 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_RXDMA5_E,             - RxDma unit index 5 (bobcat3)
 *
 *   PRV_CPSS_DXCH_UNIT_TXDMA2_E,             - TxDma unit index 2 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_TXDMA3_E,             - TxDma unit index 3 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_TXDMA4_E,             - TxDma unit index 4 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_TXDMA5_E,             - TxDma unit index 5 (bobcat3)
 *
 *   PRV_CPSS_DXCH_UNIT_TX_FIFO2_E,           - TxFifo unit index 2 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_TX_FIFO3_E,           - TxFifo unit index 3 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_TX_FIFO4_E,           - TxFifo unit index 4 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_TX_FIFO5_E,           - TxFifo unit index 5 (bobcat3)
 *
 *   PRV_CPSS_DXCH_UNIT_TXQ_DQ1_E,            - txq-DQ unit index 1 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_TXQ_DQ2_E,            - txq-DQ unit index 2 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_TXQ_DQ3_E,            - txq-DQ unit index 3 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_TXQ_DQ4_E,            - txq-DQ unit index 4 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_TXQ_DQ5_E,            - txq-DQ unit index 5 (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_TAI_E                 - TAI for single PRV_CPSS_DXCH_UNIT_TAI_E device
 *   PRV_CPSS_DXCH_UNIT_TAI1_E                - TAI for single PRV_CPSS_DXCH_UNIT_TAI_E device of pipe 1 (bobcat3)
 *
 *   PRV_CPSS_DXCH_UNIT_EGF_SHT_1_E           - egf_sht unit of pipe 1 (bobcat3)
 *
 *   PRV_CPSS_DXCH_UNIT_TTI_1_E               - TTI pipe 1        (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_IPCL_1_E              - IPCL pipe 1       (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_L2I_1_E               - L2I pipe 1        (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_IPVX_1_E              - IPVX pipe 1       (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_IPLR_1_E              - IPLR pipe 1       (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_IPLR1_1_E             - IPLR1 pipe 1      (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_IOAM_1_E              - IOAM pipe 1       (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_MLL_1_E               - MLL pipe 1        (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_EQ_1_E                - EQ pipe 1         (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_EGF_EFT_1_E           - EGF_EFT pipe 1    (bobcat3)
 *
 *   PRV_CPSS_DXCH_UNIT_CNC_0_1_E             - CNC0 pipe 1       (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_CNC_1_1_E             - CNC1 pipe 1       (bobcat3)
 *
 *   PRV_CPSS_DXCH_UNIT_SERDES_1_E            - SERDES pipe 1     (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_HA_1_E                - HA pipe 1         (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_ERMRK_1_E             - ERMRK pipe 1      (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_EPCL_1_E              - EPCL pipe 1       (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_EPLR_1_E              - EPLR pipe 1       (bobcat3)
 *   PRV_CPSS_DXCH_UNIT_EOAM_1_E              - EOAM pipe 1       (bobcat3)
 *
 *   PRV_CPSS_DXCH_UNIT_RX_DMA_GLUE_1_E       - RXDMA GLUE pipe 1 (bobcat3)
 *
 *  PRV_CPSS_DXCH_UNIT_LAST_E       - must be last
 *
 */
typedef enum {
    PRV_CPSS_DXCH_UNIT_TTI_E,
    PRV_CPSS_DXCH_UNIT_PCL_E,
    PRV_CPSS_DXCH_UNIT_L2I_E,
    PRV_CPSS_DXCH_UNIT_FDB_E,
    PRV_CPSS_DXCH_UNIT_EQ_E,
    PRV_CPSS_DXCH_UNIT_LPM_E,
    PRV_CPSS_DXCH_UNIT_EGF_EFT_E,
    PRV_CPSS_DXCH_UNIT_EGF_QAG_E,
    PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    PRV_CPSS_DXCH_UNIT_HA_E,
    PRV_CPSS_DXCH_UNIT_ETS_E,
    PRV_CPSS_DXCH_UNIT_MLL_E,
    PRV_CPSS_DXCH_UNIT_IPLR_E,
    PRV_CPSS_DXCH_UNIT_EPLR_E,
    PRV_CPSS_DXCH_UNIT_IPVX_E,
    PRV_CPSS_DXCH_UNIT_IOAM_E,
    PRV_CPSS_DXCH_UNIT_EOAM_E,
    PRV_CPSS_DXCH_UNIT_TCAM_E,
    PRV_CPSS_DXCH_UNIT_RXDMA_E,
    PRV_CPSS_DXCH_UNIT_EPCL_E,
    PRV_CPSS_DXCH_UNIT_TM_FCU_E,
    PRV_CPSS_DXCH_UNIT_TM_DROP_E,
    PRV_CPSS_DXCH_UNIT_TM_QMAP_E,
    PRV_CPSS_DXCH_UNIT_TM_E,
    PRV_CPSS_DXCH_UNIT_GOP_E,
    PRV_CPSS_DXCH_UNIT_MIB_E,
    PRV_CPSS_DXCH_UNIT_SERDES_E,
    PRV_CPSS_DXCH_UNIT_ERMRK_E,
    PRV_CPSS_DXCH_UNIT_BM_E,
    PRV_CPSS_DXCH_UNIT_TM_INGRESS_GLUE_E,
    PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E,
    PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E,
    PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E,
    PRV_CPSS_DXCH_UNIT_ILKN_E,
    PRV_CPSS_DXCH_UNIT_LMS0_0_E,
    PRV_CPSS_DXCH_UNIT_LMS0_1_E,
    PRV_CPSS_DXCH_UNIT_LMS0_2_E,
    PRV_CPSS_DXCH_UNIT_LMS0_3_E,
    PRV_CPSS_DXCH_UNIT_TX_FIFO_E,
    PRV_CPSS_DXCH_UNIT_BMA_E,
    PRV_CPSS_DXCH_UNIT_CNC_0_E,
    PRV_CPSS_DXCH_UNIT_CNC_1_E,
    PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,
    PRV_CPSS_DXCH_UNIT_TXQ_LL_E,
    PRV_CPSS_DXCH_UNIT_TXQ_PFC_E,
    PRV_CPSS_DXCH_UNIT_TXQ_QCN_E,
    PRV_CPSS_DXCH_UNIT_TXQ_DQ_E,
    PRV_CPSS_DXCH_UNIT_DFX_SERVER_E,
    PRV_CPSS_DXCH_UNIT_MPPM_E,
    PRV_CPSS_DXCH_UNIT_LMS1_0_E,
    PRV_CPSS_DXCH_UNIT_LMS1_1_E,
    PRV_CPSS_DXCH_UNIT_LMS1_2_E,
    PRV_CPSS_DXCH_UNIT_LMS1_3_E,
    PRV_CPSS_DXCH_UNIT_LMS2_0_E,
    PRV_CPSS_DXCH_UNIT_LMS2_1_E,
    PRV_CPSS_DXCH_UNIT_LMS2_2_E,
    PRV_CPSS_DXCH_UNIT_LMS2_3_E,
    PRV_CPSS_DXCH_UNIT_MPPM_1_E,
    PRV_CPSS_DXCH_UNIT_CTU_0_E,
    PRV_CPSS_DXCH_UNIT_CTU_1_E,
    PRV_CPSS_DXCH_UNIT_TXQ_SHT_E ,
    PRV_CPSS_DXCH_UNIT_TXQ_EGR0_E,
    PRV_CPSS_DXCH_UNIT_TXQ_EGR1_E,
    PRV_CPSS_DXCH_UNIT_TXQ_DIST_E,
    PRV_CPSS_DXCH_UNIT_IPLR_1_E,
    PRV_CPSS_DXCH_UNIT_TXDMA_E,
    PRV_CPSS_DXCH_UNIT_MG_E,
    PRV_CPSS_DXCH_UNIT_TCC_IPCL_E,
    PRV_CPSS_DXCH_UNIT_TCC_IPVX_E,

    /* no more LMS unit : was split to SMI , LED units */
    PRV_CPSS_DXCH_UNIT_SMI_0_E,
    PRV_CPSS_DXCH_UNIT_SMI_1_E,
    PRV_CPSS_DXCH_UNIT_SMI_2_E,
    PRV_CPSS_DXCH_UNIT_SMI_3_E,

    /* no more LMS unit : was split to SMI , LED units */
    PRV_CPSS_DXCH_UNIT_LED_0_E,
    PRV_CPSS_DXCH_UNIT_LED_1_E,
    PRV_CPSS_DXCH_UNIT_LED_2_E,
    PRV_CPSS_DXCH_UNIT_LED_3_E,
    PRV_CPSS_DXCH_UNIT_LED_4_E,

    PRV_CPSS_DXCH_UNIT_RXDMA1_E,
    PRV_CPSS_DXCH_UNIT_TXDMA1_E,
    PRV_CPSS_DXCH_UNIT_TX_FIFO1_E,
    PRV_CPSS_DXCH_UNIT_ETH_TXFIFO1_E,

    PRV_CPSS_DXCH_UNIT_RXDMA_GLUE_E,
    PRV_CPSS_DXCH_UNIT_TXDMA_GLUE_E,


    PRV_CPSS_DXCH_UNIT_RXDMA2_E,
    PRV_CPSS_DXCH_UNIT_RXDMA3_E,                      /* pipe 1*/
    PRV_CPSS_DXCH_UNIT_RXDMA4_E,                      /* pipe 1*/
    PRV_CPSS_DXCH_UNIT_RXDMA5_E,                      /* pipe 1*/

    PRV_CPSS_DXCH_UNIT_TXDMA2_E,
    PRV_CPSS_DXCH_UNIT_TXDMA3_E,                      /* pipe 1*/
    PRV_CPSS_DXCH_UNIT_TXDMA4_E,                      /* pipe 1*/
    PRV_CPSS_DXCH_UNIT_TXDMA5_E,                      /* pipe 1*/

    PRV_CPSS_DXCH_UNIT_TX_FIFO2_E,
    PRV_CPSS_DXCH_UNIT_TX_FIFO3_E,                    /* pipe 1*/
    PRV_CPSS_DXCH_UNIT_TX_FIFO4_E,                    /* pipe 1*/
    PRV_CPSS_DXCH_UNIT_TX_FIFO5_E,                    /* pipe 1*/

    PRV_CPSS_DXCH_UNIT_TXQ_DQ1_E,
    PRV_CPSS_DXCH_UNIT_TXQ_DQ2_E,
    PRV_CPSS_DXCH_UNIT_TXQ_DQ3_E,                     /* pipe 1*/
    PRV_CPSS_DXCH_UNIT_TXQ_DQ4_E,                     /* pipe 1*/
    PRV_CPSS_DXCH_UNIT_TXQ_DQ5_E,                     /* pipe 1*/

    PRV_CPSS_DXCH_UNIT_MIB1_E,                        /* MIB pipe 1*/
    PRV_CPSS_DXCH_UNIT_GOP1_E,                        /* GOP pipe 1*/

    PRV_CPSS_DXCH_UNIT_TAI_E,
    PRV_CPSS_DXCH_UNIT_TAI1_E,

    PRV_CPSS_DXCH_UNIT_EGF_SHT_1_E,                   /* EGF SHT pipe 1*/

    PRV_CPSS_DXCH_UNIT_TTI_1_E                       ,/* TTI pipe 1 */
    PRV_CPSS_DXCH_UNIT_IPCL_1_E                      ,/* IPCL pipe 1 */
    PRV_CPSS_DXCH_UNIT_L2I_1_E                       ,/* L2I pipe 1 */
    PRV_CPSS_DXCH_UNIT_IPVX_1_E                      ,/* IPVX pipe 1 */
    PRV_CPSS_DXCH_UNIT_IPLR_0_1_E                    ,/* IPLR pipe 1 */
    PRV_CPSS_DXCH_UNIT_IPLR_1_1_E                    ,/* IPLR1 pipe 1 */
    PRV_CPSS_DXCH_UNIT_IOAM_1_E                      ,/* IOAM pipe 1 */
    PRV_CPSS_DXCH_UNIT_MLL_1_E                       ,/* MLL pipe 1 */
    PRV_CPSS_DXCH_UNIT_EQ_1_E                        ,/* EQ pipe 1 */
    PRV_CPSS_DXCH_UNIT_EGF_EFT_1_E                   ,/* EGF_EFT pipe 1 */
    PRV_CPSS_DXCH_UNIT_CNC_0_1_E                     ,/* CNC0 pipe 1 */
    PRV_CPSS_DXCH_UNIT_CNC_1_1_E                     ,/* CNC1 pipe 1 */
    PRV_CPSS_DXCH_UNIT_SERDES_1_E                    ,/* SERDES pipe 1 */
    PRV_CPSS_DXCH_UNIT_HA_1_E                        ,/* HA pipe 1 */
    PRV_CPSS_DXCH_UNIT_ERMRK_1_E                     ,/* ERMRK pipe 1 */
    PRV_CPSS_DXCH_UNIT_EPCL_1_E                      ,/* EPCL pipe 1 */
    PRV_CPSS_DXCH_UNIT_EPLR_1_E                      ,/* EPLR pipe 1 */
    PRV_CPSS_DXCH_UNIT_EOAM_1_E                      ,/* EOAM pipe 1 */
    PRV_CPSS_DXCH_UNIT_RX_DMA_GLUE_1_E               ,/* RXDMA GLUE pipe 1 */

    PRV_CPSS_DXCH_UNIT_LAST_E /*must be last*/
}PRV_CPSS_DXCH_UNIT_ENT;

/*
 * typedef: enum PRV_CPSS_DXCH_SUBUNIT_ENT
 *
 * Description: Defines silicon subunits - having multiple instances.
 *
 * Enumerations:
 *
 *  PRV_CPSS_DXCH_SUBUNIT_GOP_TAI_E           - TAI (Time Application Interface) subunit
 *                                                  pair per port group
 *  PRV_CPSS_DXCH_SUBUNIT_GOP_PTP_E           - PTP Timestamp subunit (per port)
 *  PRV_CPSS_DXCH_SUBUNIT_GOP_FCA_E           - FCA (flow control aggregator)
 *                                                  subunit (per port)
 *  PRV_CPSS_DXCH_SUBUNIT_GOP_PR_E            - PR (Packet Reassembly) subunit
 *
 */
typedef enum {
    PRV_CPSS_DXCH_SUBUNIT_GOP_TAI_E,
    PRV_CPSS_DXCH_SUBUNIT_GOP_PTP_E,
    PRV_CPSS_DXCH_SUBUNIT_GOP_FCA_E,
    PRV_CPSS_DXCH_SUBUNIT_GOP_PR_E
}PRV_CPSS_DXCH_SUBUNIT_ENT;

#define LION2_DEVICE_INDICATION_CNS   0xFF
#define BOBK_DEVICE_INDICATION_CNS    0xFE

/*
 * Typedef: struct PRV_CPSS_BOOKMARK_STC
 *
 * Description: This is used to indicate the specific data types (pointer fields or write only data) in the registers addresses struct
 *
 * Fields:
 *
 *              bookmark - bookmark itself
 *              type - SW type (PRV_CPSS_SW_TYPE_WRONLY_CNS or PRV_CPSS_SW_TYPE_PTR_FIELD_CNS)
 *              size - size of bookmark
 *              nextPtr - pointer to the next bookmark
  */

/* The registers DB (PRV_CPSS_DXCH_PP_REGS_ADDR_STC) is used in several generic
   algorithm like registers dump or base address auto conversion. These
   algorithms require that the PRV_CPSS_DXCH_PP_REGS_ADDR_STC structure
   and PRV_CPSS_BOOKMARK_STC have packed members. This is native in 32 Bit
   machines but not true in 64 Bit machines.
   Use 4 byte alignment to guaranty that nextPtr will be right after size in
   both registers DB and in the PRV_CPSS_BOOKMARK_STC. */
#if __WORDSIZE == 64
#pragma pack(4)
#endif
typedef struct GT_PACKED bkmark
{
                GT_U32  bookmark;
                GT_U32  type;
                GT_U32  size;
                GT_U32  *nextPtr;
} PRV_CPSS_BOOKMARK_STC;
/* restore structure packing to default */
#if __WORDSIZE == 64
#pragma pack()
#endif
/*
 * Typedef: struct PRV_CPSS_RE_INIT_RX_DESC_INFO_STC
 *
 * Description: The information block of a single Rx descriptors list used to re-initialise.
 *
 * Fields:
 *
 *      rxDescBlock         - (Points to) the HW descriptors block.
 *      totalDescNum        - total Number of Descriptors
 *      freeDescNum         - Number of all descriptors in list.
 *      next2ReturnOffset   - Offset in the list of the descriptor to which the next returned
 *                            buffer should be attached.
 *      next2ReceiveOffset  - Offset in the list of the descriptor from which the next packet
 *                            will be fetched when an Rx interrupt is received.
 *      freeDescNum         - Number of free descriptors in list.
 *      headerOffset        - Number of reserved bytes before each buffer, to be
 *                            kept for application and internal use.
 *      forbidQEn           - When set to GT_TRUE enabling the Rx SDMA on buffer
 *                            release is forbidden.
 */
typedef struct
{
    void                *rxDescBlock;
    GT_U32              totalDescNum;
    GT_U32              next2ReturnOffset;
    GT_U32              next2ReceiveOffset;

    GT_U32              freeDescNum;
    GT_U32              headerOffset;
    GT_BOOL             forbidQEn;
}PRV_CPSS_RE_INIT_RX_DESC_INFO_STC;

/*
 * Typedef: struct PRV_CPSS_DMA_RE_INIT_INFO_STC
 *
 * Description:
 *      The structure that hold info for DMA related settings
 *      done during 'PP logical init'.
 *      This info needed for devices that doing re-init
 *      without resetting the device HW.
 *
 * Fields:
 *
 *      netIfCfg                  - network interface configuration
 *      auqCfg                    - Address Update Messages Queue CFG
 *      fuqUseSeparate            - Use separate Queue for FDB Upload Messages
 *      fuqCfg                    - FDB Upload Messages Queue CFG
 *      auqInternalInfo           - Address Update Messages Queue info
 *      fuqInternalInfo           - FDB Upload Messages Queue info
 *      secondaryAuqInternalInfo  - Address Update Messages Queue seconary info
 *      rxDescInfo                - RX Descriotors info per RX Queue
 *      auq1InternalInfo          - Address Update Messages Queue1 info
 *      secondaryAuq1InternalInfo - Address Update Messages Queue1 seconary info
 */
typedef struct{
    CPSS_NET_IF_CFG_STC     netIfCfg;       /* parameter as given in 'phase 2' from application */
    CPSS_AUQ_CFG_STC        auqCfg;         /* parameter as given in 'phase 2' from application */
    GT_BOOL                 fuqUseSeparate; /* parameter as given in 'phase 2' from application */
    CPSS_AUQ_CFG_STC        fuqCfg;         /* parameter as given in 'phase 2' from application */

    PRV_CPSS_AU_DESC_CTRL_STC   auqInternalInfo[CPSS_MAX_PORT_GROUPS_CNS];/* AUQ cpss internal info -- per port group */
    PRV_CPSS_AU_DESC_CTRL_STC   fuqInternalInfo[CPSS_MAX_PORT_GROUPS_CNS];/* FUQ cpss internal info -- per port group */
    PRV_CPSS_AU_DESC_CTRL_STC   secondaryAuqInternalInfo[CPSS_MAX_PORT_GROUPS_CNS];/* secondary AUQ cpss internal info -- per port group */
    PRV_CPSS_RE_INIT_RX_DESC_INFO_STC   rxDescInfo[NUM_OF_RX_QUEUES]; /* Rx descriptors internal info */
    PRV_CPSS_AU_DESC_CTRL_STC   auq1InternalInfo[CPSS_MAX_PORT_GROUPS_CNS];/* AUQ cpss internal info -- per port group */
    PRV_CPSS_AU_DESC_CTRL_STC   secondaryAuq1InternalInfo[CPSS_MAX_PORT_GROUPS_CNS];/* secondary AUQ cpss internal info -- per port group */
}PRV_CPSS_DMA_RE_INIT_INFO_STC;

/*
 * typedef: enum PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_ENT
 *
 * Description: Defines pre phase 1 init mode .
 *
 * Enumerations:
 *  PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_DEFAULT_INIT_E - regular pre phase 1 init mode
 *  PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_NO_PP_INIT_E   - no Pp initialization should be done
 *
 */
typedef enum
{
    PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_DEFAULT_INIT_E,
    PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_NO_PP_INIT_E
}
PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_ENT;

/*******************************************************************************
* prvCpssPrePhase1PpInit
*
* DESCRIPTION:
*       private (internal) function to make phase1 pre-init
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       prePhase1InitMode  - pre phase 1 init mode.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPrePhase1PpInit
(
    IN    PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_ENT     prePhase1InitMode
);


/*******************************************************************************
* prvCpssPrePhase1PpInitModeGet
*
* DESCRIPTION:
*       private (internal) function retrieving  pre phase1 init mode
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
* OUTPUTS:
*       prePhase1InitModePtr  - pointer to pre phase 1 init mode .
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPrePhase1PpInitModeGet
(
    OUT PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_ENT     *prePhase1InitModePtr
);

/*******************************************************************************
* prvCpssPpConfigDevDbRelease
*
* DESCRIPTION:
*       private (internal) function to release all the DB of the device.
*       NOTE: function 'free' the allocated memory ,buffers, semaphores
*             and restore DB to 'pre-init' state
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbRelease
(
    IN    GT_U8     devNum
);

/*******************************************************************************
* prvCpssPpConfigDevDbBusBaseAddrAttach
*
* DESCRIPTION:
*       private (internal) function to attach the base address to devNum
*
*       this needed for the 'initialization after the DB release processing'
*       when the initialization for a device during 'phase 1' will be we will
*       know what 'old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*       busBaseAddr -  the address of the device on the interface bus (pci/pex/smi/twsi)
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbBusBaseAddrAttach
(
    IN GT_U8        devNum,
    IN GT_UINTPTR   busBaseAddr
);

/*******************************************************************************
* prvCpssPpConfigDevDbInfoSet
*
* DESCRIPTION:
*       private (internal) function to :
*       1. first call for device or device did HW reset :
*           function set the info about the device regarding DMA allocations
*           given by the application.
*       2. else the function check that the info given now match the info given
*          of first time after the HW reset.
*
*       this needed for the 'Initialization after the DB release processing'
*       when the initialization for a device during 'Phase 1' will be we will
*       know what 'Old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*       netIfCfgPtr - (pointer to)Network interface configuration.
*       auqCfgPtr - (pointer to)AU queue info
*       fuqUseSeparate - do we have another Queue for FU message
*       fuqCfgPtr - (pointer to)FU queue info
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_STATE             - the new info it not the same info that was
*                                  set for this device (according to the bus
*                                  base address) before the 'shutdown'
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*       GT_BAD_PTR               - one of the pointers is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbInfoSet
(
    IN GT_U8                        devNum,
    IN CPSS_NET_IF_CFG_STC          *netIfCfgPtr,
    IN CPSS_AUQ_CFG_STC             *auqCfgPtr,
    IN GT_BOOL                      fuqUseSeparate,
    IN CPSS_AUQ_CFG_STC             *fuqCfgPtr
);

/*******************************************************************************
* prvCpssPpConfigDevDbHwResetSet
*
* DESCRIPTION:
*       private (internal) function to state that the device did HW reset or that
*       the device finished the 'pp logic init' function
*
*       this needed for the 'initialization after the DB release processing'
*       when the initialization for a device during 'phase 1' will be we will
*       know what 'old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number
*       didHwReset - GT_TRUE - Did HW reset ,
*                    GT_FALSE -  finished pp logical init.
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbHwResetSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   didHwReset
);

/*******************************************************************************
* prvCpssPpConfigDevDbHwResetGet
*
* DESCRIPTION:
*       private (internal) function get the state that the device did HW reset
*       or that the device finished the 'pp logic init' function.
*
*       this needed for the 'initialization after the DB release processing'
*       when the initialization for a device during 'phase 1' will be we will
*       know what 'old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number
* OUTPUTS:
*       didHwResetPtr - GT_TRUE - Did HW reset ,
*                       GT_FALSE -  finished pp logical init.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbHwResetGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *didHwResetPtr
);

/*******************************************************************************
* prvCpssPpConfigDevDbNetIfInitSet
*
* DESCRIPTION:
*       Private (internal) function to set if network initialization is needed or
*       if configuration will be based on previously saved re-initialization parameters.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The device number
*       needNetIfInit   - GT_TRUE  - Need network interface init
*                         GT_FALSE - Network interface init using previously saved values.
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbNetIfInitSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   needNetIfInit
);

/*******************************************************************************
* prvCpssPpConfigDevDbNetIfInitGet
*
* DESCRIPTION:
*       Private (internal) function to get whether network initialization is needed or
*       whether configuration will be based on previously saved re-initialization parameters.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number
* OUTPUTS:
*       needNetIfInitPtr    - (pointer to):
*                             GT_TRUE  - Need network interface init
*                             GT_FALSE - Network interface init using previously saved values.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbNetIfInitGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *needNetIfInitPtr
);

/*******************************************************************************
* prvCpssPpConfigDevDbPrvInfoSet
*
* DESCRIPTION:
*       private (internal) function to :
*       set the AUQ,FUQ private cpss info.
*
*       this needed for the 'Initialization after the DB release processing'
*       when the initialization for a device during 'Phase 1' will be we will
*       know what 'Old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*       auqInternalInfoPtr - (pointer to)AU queue private info , of all port groups
*       fuqInternalInfoPtr - (pointer to)FU queue private info , of all port groups
*       secondaryAuqInternalInfoPtr - (pointer to)secondary AU queue private info , of all port groups
*       rxDescListPtr      - (pointer to)Rx descriptors list private info
*       auq1InternalInfoPtr - (pointer to)AU1 queue private info , of all port groups
*       secondaryAuq1InternalInfoPtr - (pointer to)secondary AU1 queue private info , of all port groups
* OUTPUTS:
*       none
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*
* COMMENTS:
*      NOTE: not checking for NULL pointers to speed up performances
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbPrvInfoSet
(
    IN GT_U8                        devNum,
    IN PRV_CPSS_AU_DESC_CTRL_STC    *auqInternalInfoPtr,
    IN PRV_CPSS_AU_DESC_CTRL_STC    *fuqInternalInfoPtr,
    IN PRV_CPSS_AU_DESC_CTRL_STC    *secondaryAuqInternalInfoPtr,
    IN PRV_CPSS_RX_DESC_LIST_STC    *rxDescListPtr,
    IN PRV_CPSS_AU_DESC_CTRL_STC    *auq1InternalInfoPtr,
    IN PRV_CPSS_AU_DESC_CTRL_STC    *secondaryAuq1InternalInfoPtr
);

/*******************************************************************************
* prvCpssPpConfigDevDbPrvInfoGet
*
* DESCRIPTION:
*       private (internal) function to :
*       get the AUQ,FUQ private cpss info.
*
*       this needed for the 'Initialization after the DB release processing'
*       when the initialization for a device during 'Phase 1' will be we will
*       know what 'Old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*       portGroupId - The port group Id , to support multi-port-groups device
* OUTPUTS:
*       auqInternalInfoPtr - (pointer to)AU queue private info
*       fuqInternalInfoPtr - (pointer to)FU queue private info (can be NULL when FUQ not used)
*       secondaryAuqInternalInfoPtr - (pointer to)secondary AU queue private info (can be NULL when queue not used)
*       auq1InternalInfoPtr - (pointer to)AU1 queue private info
*       secondaryAuq1InternalInfoPtr - (pointer to)secondary AU1 queue private info (can be NULL when queue not used)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*       GT_BAD_PTR               - one of the pointers is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbPrvInfoGet
(
    IN GT_U8                        devNum,
    IN GT_U32                       portGroupId,
    OUT PRV_CPSS_AU_DESC_CTRL_STC    *auqInternalInfoPtr,
    OUT PRV_CPSS_AU_DESC_CTRL_STC    *fuqInternalInfoPtr,
    OUT PRV_CPSS_AU_DESC_CTRL_STC    *secondaryAuqInternalInfoPtr,
    OUT PRV_CPSS_AU_DESC_CTRL_STC    *auq1InternalInfoPtr,
    OUT PRV_CPSS_AU_DESC_CTRL_STC    *secondaryAuq1InternalInfoPtr
);

/*******************************************************************************
* prvCpssPpConfigDevDbPrvNetInfInfoSet
*
* DESCRIPTION:
*       private (internal) function to :
*       set the Rx descriptors list private cpss info.
*
*       this needed for the 'Initialization after the DB release processing'
*       when the initialization for a device during 'Phase 1' will be we will
*       know what 'Old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - The device number.
*       rxDescPtr          - (pointer to) the start of Rx descriptors block.
*       rxDescListPtr      - (pointer to)Rx descriptors list private info.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*       GT_BAD_PTR               - one of the pointers is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbPrvNetInfInfoSet
(
    IN GT_U8                        devNum,
    IN PRV_CPSS_RX_DESC_STC         *rxDescPtr,
    IN PRV_CPSS_RX_DESC_LIST_STC    *rxDescListPtr
);

/*******************************************************************************
* prvCpssPpConfigDevDbPrvNetInfInfoGet
*
* DESCRIPTION:
*       private (internal) function to :
*       get the Rx descriptors list private cpss info.
*
*       this needed for the 'Initialization after the DB release processing'
*       when the initialization for a device during 'Phase 1' will be we will
*       know what 'Old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
* OUTPUTS:
*       rxDescListPtr      - (pointer to) Rx descriptors list private info
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbPrvNetInfInfoGet
(
    IN GT_U8                        devNum,
    OUT PRV_CPSS_RX_DESC_LIST_STC   *rxDescListPtr
);

/*******************************************************************************
* prvCpssPpConfigDevDbRenumber
*
* DESCRIPTION:
*       the function set parameters for the driver to renumber it's DB.
*       the function "renumber" the current device number of the device to a
*       new device number.
*       NOTE:
*       this function MUST be called under 'Interrupts are locked'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       oldDevNum  - The PP's "old" device number .
*       newDevNum  - The PP's "new" device number swap the DB to.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL     - on error
*       GT_ALREADY_EXIST - the new device number is already used
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbRenumber
(
    IN GT_U8    oldDevNum,
    IN GT_U8    newDevNum
);

/*******************************************************************************
* prvCpssPpConfigDevInfoGet
*
* DESCRIPTION:
*    Private (internal) function returns device static information
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*
* OUTPUTS:
*       devInfoPtr   - (pointer to) device information
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevInfoGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_GEN_CFG_DEV_INFO_STC   *devInfoPtr
);

/*******************************************************************************
* prvCpssPpConfigPortToPhymacObjectBind
*
* DESCRIPTION:
*         The function binds port phymac pointer
*         to 1540M PHYMAC object
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*    portNum        - port number
*    macDrvObjPtr   - port phymac object pointer
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK       - success
*       GT_FAIL     - error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigPortToPhymacObjectBind
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_MACDRV_OBJ_STC * const macDrvObjPtr
);

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
);

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
*
*******************************************************************************/
GT_U32 prvCpssDxChHwSubunitBaseAddrGet
(
    IN GT_U8                       devNum,
    IN PRV_CPSS_DXCH_SUBUNIT_ENT   subunitId,
    IN GT_U32                      *indexArr
);

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
);


/*******************************************************************************
* convertOldRegDbToNewUnitsBaseAddr
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
);

/*******************************************************************************
* prvCpssDxChHwPpSdmaCpuPortReservedIndexGet
*
* DESCRIPTION:
*       get the index that is reserved for the CPU port for 'link status' filtering.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum             - The PP's device number.
*
* OUTPUTS:
*       indexPtr - pointer to the index
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpSdmaCpuPortReservedIndexGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *indexPtr
);


/*******************************************************************************
* prvCpssDxChHwPpGopGlobalMacPortNumToLocalMacPortInPipeConvert
*
* DESCRIPTION:
*       convert the global GOP MAC port number in device to local GOP MAC port
*       in the pipe , and the pipeId.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - The PP's device number.
*       globalMacPortNum   - the MAC global port number.
* OUTPUTS:
*       pipeIndexPtr - (pointer to) the pipe Index of the MAC port
*       localMacPortNumPtr - (pointer to) the MAC local port number
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpGopGlobalMacPortNumToLocalMacPortInPipeConvert
(
    IN  GT_U8   devNum,
    IN  GT_U32  globalMacPortNum,
    OUT GT_U32  *pipeIndexPtr,
    OUT GT_U32  *localMacPortNumPtr
);

/*******************************************************************************
* prvCpssDxChHwPpDmaGlobalNumToLocalNumInDpConvert
*
* DESCRIPTION:
*       convert the global DMA number in device to local DMA number
*       in the DataPath (DP), and the Data Path Id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - The PP's device number.
*       globalDmaNum        - the DMA global number.
* OUTPUTS:
*       dpIndexPtr          - (pointer to) the Data Path (DP) Index
*       localDmaNumPtr      - (pointer to) the DMA local number
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpDmaGlobalNumToLocalNumInDpConvert
(
    IN  GT_U8   devNum,
    IN  GT_U32  globalDmaNum,
    OUT GT_U32  *dpIndexPtr,
    OUT GT_U32  *localDmaNumPtr
);

/*******************************************************************************
* prvCpssDxChHwPpDmaLocalNumInDpToGlobalNumConvert
*
* DESCRIPTION:
*       convert the local DMA number in the DataPath (DP), to global DMA number
*       in the device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - The PP's device number.
*       dpIndex             - the Data Path (DP) Index
*       localDmaNum         - the DMA local number
* OUTPUTS:
*       globalDmaNumPtr     - (pointer to) the DMA global number.
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpDmaLocalNumInDpToGlobalNumConvert
(
    IN  GT_U8   devNum,
    IN  GT_U32  dpIndex,
    IN  GT_U32  localDmaNum,
    OUT GT_U32  *globalDmaNumPtr
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssHwInith */

