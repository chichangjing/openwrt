/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvExMxEventsSalsa.h
*
* DESCRIPTION:
*       This file includes all different hardware driven Event types - Salsa
*
*       Applicable devices:
*                          All ports - 98DX240 , 98DX241 , 98DX240_2 , 98DX241_2
*                          (12,16,24 ports)
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvCpssDrvExMxEventsSalsah
#define __prvCpssDrvExMxEventsSalsah

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Typedef: enum GT_SAL_INT_TYPE
 *
 * Description: All interrupt cause indexes, to be used for interrupts handling,
 *              and parameters change.
 *
 * NOTE : If needed, new interrupt bits can replace the XXX_RES_i bits, in case
 *        that these bits fall into the same cause registers.
 *
 * Comment:
 *      Please see the description of each of the following interrupt event
 *      cause registers in the packet processor datasheet.
 */

typedef enum
{
    /* Global Interrupt Cause Register */
    /* Summary and PCI-LIKE interrupts */
    /* Indexes 0 - 31                  */
    /* Cause 0x00000030  */
    /* mask  0x00000034  */                      /* bit# */
    PRV_CPSS_SAL_SUM_GLOBAL_E = 0,                     /*  0 */
    PRV_CPSS_SAL_SUM_MISC_SUM_E,                       /*  1 */
    PRV_CPSS_SAL_SUM_RES2_E,                           /*  2 */
    PRV_CPSS_SAL_SUM_TXQ_E,                            /*  3 */
    PRV_CPSS_SAL_SUM_L2_E,                             /*  4 */
    PRV_CPSS_SAL_SUM_BM_E,                             /*  5 */
    PRV_CPSS_SAL_SUM_MT_E,                             /*  6 */
    PRV_CPSS_SAL_SUM_GOP0_SUM0_E,                      /*  7 */
    PRV_CPSS_SAL_SUM_GOP0_SUM1_E,                      /*  8 */
    PRV_CPSS_SAL_SUM_GOP1_SUM0_E,                      /*  9 */
    PRV_CPSS_SAL_SUM_GOP1_SUM1_E,                      /* 10 */
    PRV_CPSS_SAL_SUM_GOP2_SUM0_E,                      /* 11 */
    PRV_CPSS_SAL_SUM_GOP2_SUM1_E,                      /* 12 */
    PRV_CPSS_SAL_SUM_GOP3_SUM0_E,                      /* 13 */
    PRV_CPSS_SAL_SUM_GOP3_SUM1_E,                      /* 14 */
    PRV_CPSS_SAL_SUM_GPP_E,                            /* 15 */
    PRV_CPSS_SAL_SUM_RES16_E,                          /* 16 */
    PRV_CPSS_SAL_SUM_RES17_E,                          /* 17 */
    PRV_CPSS_SAL_SUM_RES18_E,                          /* 18 */
    PRV_CPSS_SAL_SUM_RES19_E,                          /* 19 */
    PRV_CPSS_SAL_SUM_RES20_E,                          /* 20 */
    PRV_CPSS_SAL_SUM_RES21_E,                          /* 21 */
    PRV_CPSS_SAL_SUM_RES22_E,                          /* 22 */
    PRV_CPSS_SAL_SUM_RES23_E,                          /* 23 */
    PRV_CPSS_SAL_SUM_RES24_E,                          /* 24 */
    PRV_CPSS_SAL_SUM_RES25_E,                          /* 25 */
    PRV_CPSS_SAL_SUM_RES26_E,                          /* 26 */
    PRV_CPSS_SAL_SUM_RES27_E,                          /* 27 */
    PRV_CPSS_SAL_SUM_RES28_E,                          /* 28 */
    PRV_CPSS_SAL_SUM_RES29_E,                          /* 29 */
    PRV_CPSS_SAL_SUM_RES30_E,                          /* 30 */
    PRV_CPSS_SAL_SUM_RES31_E,                          /* 31 */

    /* Miscellaneous Interrupt Cause Register */
    /* Indexes 32 - 63               */
    /* Cause 0x00000038  */
    /* mask  0x0000003C  */
    PRV_CPSS_SAL_MISC_SUM_E,                           /*  0 */
    PRV_CPSS_SAL_MISC_TWSI_TIME_OUT_E,                 /*  1 */
    PRV_CPSS_SAL_MISC_TWSI_STATUS_E,                   /*  2 */
    PRV_CPSS_SAL_MISC_ILLEGAL_ADDR_E,                  /*  3 */
    PRV_CPSS_SAL_MISC_CPU_PORT_RX_OVERRUN_E,          /*  4 */
    PRV_CPSS_SAL_MISC_CPU_PORT_TX_OVERRUN_E,          /*  5 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT_CPU_31_E,           /*  6 */
    PRV_CPSS_SAL_MISC_RES39_E,                         /*  7 */
    PRV_CPSS_SAL_MISC_RES40_E,                         /*  8 */
    PRV_CPSS_SAL_MISC_RES41_E,                         /*  9 */
    PRV_CPSS_SAL_MISC_RES42_E,                         /* 10 */
    PRV_CPSS_SAL_MISC_RES43_E,                         /* 11 */
    PRV_CPSS_SAL_MISC_RES44_E,                         /* 12 */
    PRV_CPSS_SAL_MISC_RES45_E,                         /* 13 */
    PRV_CPSS_SAL_MISC_RES46_E,                         /* 14 */
    PRV_CPSS_SAL_MISC_RES47_E,                         /* 15 */
    PRV_CPSS_SAL_MISC_RES48_E,                         /* 16 */
    PRV_CPSS_SAL_MISC_RES49_E,                         /* 17 */
    PRV_CPSS_SAL_MISC_RES50_E,                         /* 18 */
    PRV_CPSS_SAL_MISC_RES51_E,                         /* 19 */
    PRV_CPSS_SAL_MISC_RES52_E,                         /* 20 */
    PRV_CPSS_SAL_MISC_RES53_E,                         /* 21 */
    PRV_CPSS_SAL_MISC_RES54_E,                         /* 22 */
    PRV_CPSS_SAL_MISC_RES55_E,                         /* 23 */
    PRV_CPSS_SAL_MISC_RES56_E,                         /* 24 */
    PRV_CPSS_SAL_MISC_RES57_E,                         /* 25 */
    PRV_CPSS_SAL_MISC_RES58_E,                         /* 26 */
    PRV_CPSS_SAL_MISC_RES59_E,                         /* 27 */
    PRV_CPSS_SAL_MISC_RES60_E,                         /* 28 */
    PRV_CPSS_SAL_MISC_RES61_E,                         /* 29 */
    PRV_CPSS_SAL_MISC_RES62_E,                         /* 30 */
    PRV_CPSS_SAL_MISC_RES63_E,                         /* 31 */

    /* GPP Interrupt Cause Register */
    /* Indexes 64 - 95               */
    /* Cause 0x00000090  */
    /* mask  0x0000008C  */
    PRV_CPSS_SAL_GPP_SUM_E,                            /*  0 */
    PRV_CPSS_SAL_GPP_INTERRUPT1_E,                     /*  1 */
    PRV_CPSS_SAL_GPP_INTERRUPT2_E,                     /*  2 */
    PRV_CPSS_SAL_GPP_INTERRUPT3_E,                     /*  3 */
    PRV_CPSS_SAL_GPP_INTERRUPT4_E,                     /*  4 */
    PRV_CPSS_SAL_GPP_INTERRUPT5_E,                     /*  5 */
    PRV_CPSS_SAL_GPP_INTERRUPT6_E,                     /*  6 */
    PRV_CPSS_SAL_GPP_INTERRUPT7_E,                     /*  7 */
    PRV_CPSS_SAL_GPP_INTERRUPT8_E,                     /*  8 */
    PRV_CPSS_SAL_GPP_RES73_E,                          /*  9 */
    PRV_CPSS_SAL_GPP_RES74_E,                          /* 10 */
    PRV_CPSS_SAL_GPP_RES75_E,                          /* 11 */
    PRV_CPSS_SAL_GPP_RES76_E,                          /* 12 */
    PRV_CPSS_SAL_GPP_RES77_E,                          /* 13 */
    PRV_CPSS_SAL_GPP_RES78_E,                          /* 14 */
    PRV_CPSS_SAL_GPP_RES79_E,                          /* 15 */
    PRV_CPSS_SAL_GPP_RES80_E,                          /* 16 */
    PRV_CPSS_SAL_GPP_RES81_E,                          /* 17 */
    PRV_CPSS_SAL_GPP_RES82_E,                          /* 18 */
    PRV_CPSS_SAL_GPP_RES83_E,                          /* 19 */
    PRV_CPSS_SAL_GPP_RES84_E,                          /* 20 */
    PRV_CPSS_SAL_GPP_RES85_E,                          /* 21 */
    PRV_CPSS_SAL_GPP_RES86_E,                          /* 22 */
    PRV_CPSS_SAL_GPP_RES87_E,                          /* 23 */
    PRV_CPSS_SAL_GPP_RES88_E,                          /* 24 */
    PRV_CPSS_SAL_GPP_RES89_E,                          /* 25 */
    PRV_CPSS_SAL_GPP_RES90_E,                          /* 26 */
    PRV_CPSS_SAL_GPP_RES91_E,                          /* 27 */
    PRV_CPSS_SAL_GPP_RES92_E,                          /* 28 */
    PRV_CPSS_SAL_GPP_RES93_E,                          /* 29 */
    PRV_CPSS_SAL_GPP_RES94_E,                          /* 30 */
    PRV_CPSS_SAL_GPP_RES95_E,                          /* 31 */

    /* Buffer Management Interrupt Cause Register */
    /* Indexes 96 - 127               */
    /* Cause 0x03000040  */
    /* mask  0x03000044  */
    PRV_CPSS_SAL_BM_SUM_E,                             /*  0 */
    PRV_CPSS_SAL_BM_EMPTY_CLEAR_E,                     /*  1 */
    PRV_CPSS_SAL_BM_EMPTY_INC_E,                       /*  2 */
    PRV_CPSS_SAL_BM_AGED_PACKET_E,                     /*  3 */
    PRV_CPSS_SAL_BM_MAX_BUFF_REACHED_E,                /*  4 */
    PRV_CPSS_SAL_BM_PORT_RX_BUFFERS_CNT_UNDERRUN_E,    /*  5 */
    PRV_CPSS_SAL_BM_PORT_RX_BUFFERS_CNT_OVERRUN_E,     /*  6 */
    PRV_CPSS_SAL_BM_INVALID_ADDRESS_E,                 /*  7 */
    PRV_CPSS_SAL_BM_WRONG_SRC_PORT_E,                  /*  8 */
    PRV_CPSS_SAL_BM_MC_INC_OVERFLOW_E,                 /*  9 */
    PRV_CPSS_SAL_BM_TRIGGERED_AGING_FINISHED_E,        /* 10 */
    PRV_CPSS_SAL_BM_FLL_PARITY_E,                      /* 11 */
    PRV_CPSS_SAL_BM_CTRL0_PARITY_E,                    /* 12 */
    PRV_CPSS_SAL_BM_CTRL1_PARITY_E,                    /* 13 */
    PRV_CPSS_SAL_BM_RES110_E,                          /* 14 */
    PRV_CPSS_SAL_BM_RES111_E,                          /* 15 */
    PRV_CPSS_SAL_BM_RES112_E,                          /* 16 */
    PRV_CPSS_SAL_BM_RES113_E,                          /* 17 */
    PRV_CPSS_SAL_BM_RES114_E,                          /* 18 */
    PRV_CPSS_SAL_BM_RES115_E,                          /* 19 */
    PRV_CPSS_SAL_BM_RES116_E,                          /* 20 */
    PRV_CPSS_SAL_BM_RES117_E,                          /* 21 */
    PRV_CPSS_SAL_BM_RES118_E,                          /* 22 */
    PRV_CPSS_SAL_BM_RES119_E,                          /* 23 */
    PRV_CPSS_SAL_BM_RES120_E,                          /* 24 */
    PRV_CPSS_SAL_BM_RES121_E,                          /* 25 */
    PRV_CPSS_SAL_BM_RES122_E,                          /* 26 */
    PRV_CPSS_SAL_BM_RES123_E,                          /* 27 */
    PRV_CPSS_SAL_BM_RES124_E,                          /* 28 */
    PRV_CPSS_SAL_BM_RES125_E,                          /* 29 */
    PRV_CPSS_SAL_BM_RES126_E,                          /* 30 */
    PRV_CPSS_SAL_BM_RES127_E,                          /* 31 */

    /* GOP<n> Interrupt Cause Register0 (0<=n<4) */
    /* GOP<0> Interrupt Cause Register0 */
    /* Indexes 128 - 159               */
    /* Cause 0x04004000  */
    /* mask  0x04004004  */
    PRV_CPSS_SAL_GOP0_R0_SUM_E,                        /*  0 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT0_E,      /*  1 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT1_E,      /*  2 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT2_E,      /*  3 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT3_E,      /*  4 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT4_E,      /*  5 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT5_E,      /*  6 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT0_E,          /*  7 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT1_E,          /*  8 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT2_E,          /*  9 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT3_E,          /* 10 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT4_E,          /* 11 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT5_E,          /* 12 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT0_E,          /* 13 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT1_E,          /* 14 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT2_E,          /* 15 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT3_E,          /* 16 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT4_E,          /* 17 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT5_E,          /* 18 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT0_E,         /* 19 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT1_E,         /* 20 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT2_E,         /* 21 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT3_E,         /* 22 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT4_E,         /* 23 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT5_E,         /* 24 */
    PRV_CPSS_SAL_GOP_ADDRESS_OUT_OF_RANGE_0_E,          /* 25 */
    PRV_CPSS_SAL_GOP_COUNT_EXPIRED_0_E,                 /* 26 */
    PRV_CPSS_SAL_GOP_COUNT_COPY_DONE_0_E,               /* 27 */
    PRV_CPSS_SAL_GOP_RES156_E,                         /* 28 */
    PRV_CPSS_SAL_GOP_RES157_E,                         /* 29 */
    PRV_CPSS_SAL_GOP_RES158_E,                         /* 30 */
    PRV_CPSS_SAL_GOP_RES159_E,                         /* 31 */

    /* GOP<1> Interrupt Cause Register0 */
    /* Indexes 160 - 191               */
    /* Cause 0x04804000  */
    /* mask  0x04804004  */
    PRV_CPSS_SAL_GOP1_R0_SUM_E,                        /*  0 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT6_E,      /*  1 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT7_E,      /*  2 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT8_E,      /*  3 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT9_E,      /*  4 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT10_E,     /*  5 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT11_E,     /*  6 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT6_E,          /*  7 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT7_E,          /*  8 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT8_E,          /*  9 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT9_E,          /* 10 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT10_E,         /* 11 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT11_E,         /* 12 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT6_E,          /* 13 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT7_E,          /* 14 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT8_E,          /* 15 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT9_E,          /* 16 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT10_E,         /* 17 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT11_E,         /* 18 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT6_E,         /* 19 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT7_E,         /* 20 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT8_E,         /* 21 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT9_E,         /* 22 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT10_E,        /* 23 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT11_E,        /* 24 */
    PRV_CPSS_SAL_GOP_ADDRESS_OUT_OF_RANGE_1_E,          /* 25 */
    PRV_CPSS_SAL_GOP_COUNT_EXPIRED_1_E,                 /* 26 */
    PRV_CPSS_SAL_GOP_COUNT_COPY_DONE_1_E,               /* 27 */
    PRV_CPSS_SAL_GOP_RES188_E,                         /* 28 */
    PRV_CPSS_SAL_GOP_RES189_E,                         /* 29 */
    PRV_CPSS_SAL_GOP_RES190_E,                         /* 30 */
    PRV_CPSS_SAL_GOP_RES191_E,                         /* 31 */

    /* GOP<2> Interrupt Cause Register0 */
    /* Indexes 192 - 223               */
    /* Cause 0x05804000  */
    /* mask  0x05004004  */
    PRV_CPSS_SAL_GOP2_R0_SUM_E,                        /*  0 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT12_E,     /*  1 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT13_E,     /*  2 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT14_E,     /*  3 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT15_E,     /*  4 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT16_E,     /*  5 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT17_E,     /*  6 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT12_E,         /*  7 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT13_E,         /*  8 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT14_E,         /*  9 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT15_E,         /* 10 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT16_E,         /* 11 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT17_E,         /* 12 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT12_E,         /* 13 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT13_E,         /* 14 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT14_E,         /* 15 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT15_E,         /* 16 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT16_E,         /* 17 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT17_E,         /* 18 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT12_E,        /* 19 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT13_E,        /* 20 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT14_E,        /* 21 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT15_E,        /* 22 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT16_E,        /* 23 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT17_E,        /* 24 */
    PRV_CPSS_SAL_GOP_ADDRESS_OUT_OF_RANGE_2_E,          /* 25 */
    PRV_CPSS_SAL_GOP_COUNT_EXPIRED_2_E,                 /* 26 */
    PRV_CPSS_SAL_GOP_COUNT_COPY_DONE_2_E,               /* 27 */
    PRV_CPSS_SAL_GOP_RES220_E,                         /* 28 */
    PRV_CPSS_SAL_GOP_RES221_E,                         /* 29 */
    PRV_CPSS_SAL_GOP_RES222_E,                         /* 30 */
    PRV_CPSS_SAL_GOP_RES223_E,                         /* 31 */

    /* GOP<3> Interrupt Cause Register0 */
    /* Indexes 224 - 255               */
    /* Cause 0x05004000  */
    /* mask  0x0580400C  */
    PRV_CPSS_SAL_GOP3_R0_SUM_E,                        /*  0 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT18_E,     /*  1 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT19_E,     /*  2 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT20_E,     /*  3 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT21_E,     /*  4 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT22_E,     /*  5 */
    PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT23_E,     /*  6 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT18_E,         /*  7 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT19_E,         /*  8 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT20_E,         /*  9 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT21_E,         /* 10 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT22_E,         /* 11 */
    PRV_CPSS_SAL_AN_COMPLETED_PORT23_E,         /* 12 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT18_E,         /* 13 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT19_E,         /* 14 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT20_E,         /* 15 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT21_E,         /* 16 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT22_E,         /* 17 */
    PRV_CPSS_SAL_RX_FIFO_OVERRUN_PORT23_E,         /* 18 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT18_E,        /* 19 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT19_E,        /* 20 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT20_E,        /* 21 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT21_E,        /* 22 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT22_E,        /* 23 */
    PRV_CPSS_SAL_TX_FIFO_UNDERRUN_PORT23_E,        /* 24 */
    PRV_CPSS_SAL_GOP_ADDRESS_OUT_OF_RANGE_3_E,          /* 25 */
    PRV_CPSS_SAL_GOP_COUNT_EXPIRED_3_E,                 /* 26 */
    PRV_CPSS_SAL_GOP_COUNT_COPY_DONE_3_E,               /* 27 */
    PRV_CPSS_SAL_GOP_RES252_E,                         /* 28 */
    PRV_CPSS_SAL_GOP_RES253_E,                         /* 29 */
    PRV_CPSS_SAL_GOP_RES254_E,                         /* 30 */
    PRV_CPSS_SAL_GOP_RES255_E,                         /* 31 */


    /* GOP<n> Interrupt Cause Register1 (0<=n<4) */
    /* GOP<0> Interrupt Cause Register1 */
    /* Indexes 256 - 287               */
    /* Cause 0x04004008  */
    /* mask  0x0400400C  */
    PRV_CPSS_SAL_GOP0_R1_SUM_E,                        /*  0 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT0_E,   /*  1 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT1_E,   /*  2 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT2_E,   /*  3 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT3_E,   /*  4 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT4_E,   /*  5 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT5_E,   /*  6 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT0_E,            /*  7 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT1_E,            /*  8 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT2_E,            /*  9 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT3_E,            /* 10 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT4_E,            /* 11 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT5_E,            /* 12 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT0_E,          /* 13 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT1_E,          /* 14 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT2_E,          /* 15 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT3_E,          /* 16 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT4_E,          /* 17 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT5_E,          /* 18 */
    PRV_CPSS_SAL_GOP_RES275_E,                         /* 19 */
    PRV_CPSS_SAL_GOP_RES276_E,                         /* 20 */
    PRV_CPSS_SAL_GOP_RES277_E,                         /* 21 */
    PRV_CPSS_SAL_GOP_RES278_E,                         /* 22 */
    PRV_CPSS_SAL_GOP_RES279_E,                         /* 23 */
    PRV_CPSS_SAL_GOP_RES280_E,                         /* 24 */
    PRV_CPSS_SAL_GOP_RES281_E,                         /* 25 */
    PRV_CPSS_SAL_GOP_RES282_E,                         /* 26 */
    PRV_CPSS_SAL_GOP_RES283_E,                         /* 27 */
    PRV_CPSS_SAL_GOP_RES284_E,                         /* 28 */
    PRV_CPSS_SAL_GOP_RES285_E,                         /* 29 */
    PRV_CPSS_SAL_GOP_RES286_E,                         /* 30 */
    PRV_CPSS_SAL_GOP_RES287_E,                         /* 31 */

    /* GOP<1> Interrupt Cause Register1 */
    /* Indexes 288 - 319               */
    /* Cause 0x04804008  */
    /* mask  0x0480400C  */
    PRV_CPSS_SAL_GOP1_R1_SUM_E,                        /*  0 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT6_E,   /*  1 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT7_E,   /*  2 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT8_E,   /*  3 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT9_E,   /*  4 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT10_E,  /*  5 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT11_E,  /*  6 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT6_E,            /*  7 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT7_E,            /*  8 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT8_E,            /*  9 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT9_E,            /* 10 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT10_E,           /* 11 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT11_E,           /* 12 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT6_E,          /* 13 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT7_E,          /* 14 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT8_E,          /* 15 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT9_E,          /* 16 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT10_E,         /* 17 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT11_E,         /* 18 */
    PRV_CPSS_SAL_GOP_RES307_E,                         /* 19 */
    PRV_CPSS_SAL_GOP_RES308_E,                         /* 20 */
    PRV_CPSS_SAL_GOP_RES309_E,                         /* 21 */
    PRV_CPSS_SAL_GOP_RES310_E,                         /* 22 */
    PRV_CPSS_SAL_GOP_RES311_E,                         /* 23 */
    PRV_CPSS_SAL_GOP_RES312_E,                         /* 24 */
    PRV_CPSS_SAL_GOP_RES313_E,                         /* 25 */
    PRV_CPSS_SAL_GOP_RES314_E,                         /* 26 */
    PRV_CPSS_SAL_GOP_RES315_E,                         /* 27 */
    PRV_CPSS_SAL_GOP_RES316_E,                         /* 28 */
    PRV_CPSS_SAL_GOP_RES317_E,                         /* 29 */
    PRV_CPSS_SAL_GOP_RES318_E,                         /* 30 */
    PRV_CPSS_SAL_GOP_RES319_E,                         /* 31 */

    /* GOP<2> Interrupt Cause Register1 */
    /* Indexes 320 - 351               */
    /* Cause 0x05004008  */
    /* mask  0x0500400C  */
    PRV_CPSS_SAL_GOP2_R1_SUM_E,                        /*  0 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT12_E,  /*  1 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT13_E,  /*  2 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT14_E,  /*  3 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT15_E,  /*  4 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT16_E,  /*  5 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT17_E,  /*  6 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT12_E,           /*  7 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT13_E,           /*  8 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT14_E,           /*  9 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT15_E,           /* 10 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT16_E,           /* 11 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT17_E,           /* 12 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT12_E,         /* 13 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT13_E,         /* 14 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT14_E,         /* 15 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT15_E,         /* 16 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT16_E,         /* 17 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT17_E,         /* 18 */
    PRV_CPSS_SAL_GOP_RES339_E,                         /* 19 */
    PRV_CPSS_SAL_GOP_RES340_E,                         /* 20 */
    PRV_CPSS_SAL_GOP_RES341_E,                         /* 21 */
    PRV_CPSS_SAL_GOP_RES342_E,                         /* 22 */
    PRV_CPSS_SAL_GOP_RES343_E,                         /* 23 */
    PRV_CPSS_SAL_GOP_RES344_E,                         /* 24 */
    PRV_CPSS_SAL_GOP_RES345_E,                         /* 25 */
    PRV_CPSS_SAL_GOP_RES346_E,                         /* 26 */
    PRV_CPSS_SAL_GOP_RES347_E,                         /* 27 */
    PRV_CPSS_SAL_GOP_RES348_E,                         /* 28 */
    PRV_CPSS_SAL_GOP_RES349_E,                         /* 29 */
    PRV_CPSS_SAL_GOP_RES350_E,                         /* 30 */
    PRV_CPSS_SAL_GOP_RES351_E,                         /* 31 */

    /* GOP<3> Interrupt Cause Register1 */
    /* Indexes 352 - 383               */
    /* Cause 0x05804008  */
    /* mask  0x0580400C  */
    PRV_CPSS_SAL_GOP3_R1_SUM_E,                        /*  0 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT18_E,  /*  1 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT19_E,  /*  2 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT20_E,  /*  3 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT21_E,  /*  4 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT22_E,  /*  5 */
    PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT23_E,  /*  6 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT18_E,           /*  7 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT19_E,           /*  8 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT20_E,           /*  9 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT21_E,           /* 10 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT22_E,           /* 11 */
    PRV_CPSS_SAL_PRBS_ERROR_PORT23_E,           /* 12 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT18_E,         /* 13 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT19_E,         /* 14 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT20_E,         /* 15 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT21_E,         /* 16 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT22_E,         /* 17 */
    PRV_CPSS_SAL_TX_CRC_ERROR_PORT23_E,         /* 18 */
    PRV_CPSS_SAL_GOP_RES371_E,                         /* 19 */
    PRV_CPSS_SAL_GOP_RES372_E,                         /* 20 */
    PRV_CPSS_SAL_GOP_RES373_E,                         /* 21 */
    PRV_CPSS_SAL_GOP_RES374_E,                         /* 22 */
    PRV_CPSS_SAL_GOP_RES375_E,                         /* 23 */
    PRV_CPSS_SAL_GOP_RES376_E,                         /* 24 */
    PRV_CPSS_SAL_GOP_RES377_E,                         /* 25 */
    PRV_CPSS_SAL_GOP_RES378_E,                         /* 26 */
    PRV_CPSS_SAL_GOP_RES379_E,                         /* 27 */
    PRV_CPSS_SAL_GOP_RES380_E,                         /* 28 */
    PRV_CPSS_SAL_GOP_RES381_E,                         /* 29 */
    PRV_CPSS_SAL_GOP_RES382_E,                         /* 30 */
    PRV_CPSS_SAL_GOP_RES383_E,                         /* 31 */

    /* Transmit Queue Interrupt Cause Register */
    /* Indexes 384 - 415               */
    /* Cause 0x01800700  */
    /* mask  0x01800704  */
    PRV_CPSS_SAL_TQ_SUM_E,                             /*  0 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT0_E,               /*  1 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT1_E,               /*  2 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT2_E,               /*  3 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT3_E,               /*  4 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT4_E,               /*  5 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT5_E,               /*  6 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT6_E,               /*  7 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT7_E,               /*  8 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT8_E,               /*  9 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT9_E,               /* 10 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT0_E,                /* 11 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT1_E,                /* 12 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT2_E,                /* 13 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT3_E,                /* 14 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT4_E,                /* 15 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT5_E,                /* 16 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT6_E,                /* 17 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT7_E,                /* 18 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT8_E,                /* 19 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT9_E,                /* 20 */
    PRV_CPSS_SAL_TQ_TXQ2_MG_FLUSH_E,                   /* 21 */
    PRV_CPSS_SAL_TQ_ONE_ECC_ERROR_E,                   /* 22 */
    PRV_CPSS_SAL_TQ_TWO_ECC_ERROR_E,                   /* 23 */
    PRV_CPSS_SAL_TQ_RES398_E,                          /* 24 */
    PRV_CPSS_SAL_TQ_MG_READ_ERR_E,                     /* 25 */
    PRV_CPSS_SAL_TQ_SUM1_E,                            /* 26 */
    PRV_CPSS_SAL_TQ_SUM2_E,                            /* 27 */
    PRV_CPSS_SAL_TQ_RES412_E,                          /* 28 */
    PRV_CPSS_SAL_TQ_RES413_E,                          /* 29 */
    PRV_CPSS_SAL_TQ_RES414_E,                          /* 30 */
    PRV_CPSS_SAL_TQ_RES415_E,                          /* 31 */

    /* Transmit Queue Interrupt Cause 1 Register */
    /* Indexes 416 - 447               */
    /* Cause 0x01800080  */
    /* mask  0x0180070C  */
    PRV_CPSS_SAL_TQ1_SUM_E,                            /*  1 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT10_E,              /*  2 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT11_E,              /*  3 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT12_E,              /*  4 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT13_E,              /*  5 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT14_E,              /*  6 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT15_E,              /*  7 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT16_E,              /*  8 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT17_E,              /*  9 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT18_E,              /* 10 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT19_E,              /* 11 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT20_E,              /* 12 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT21_E,              /* 13 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT22_E,              /* 14 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT23_E,              /* 15 */
    PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT_CPU_31_E,                 /* 16 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT10_E,               /* 17 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT11_E,               /* 18 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT12_E,               /* 19 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT13_E,               /* 20 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT14_E,               /* 21 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT15_E,               /* 22 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT16_E,               /* 23 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT17_E,               /* 24 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT18_E,               /* 25 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT19_E,               /* 26 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT20_E,               /* 27 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT21_E,               /* 28 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT22_E,               /* 29 */
    PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT23_E,               /* 30 */
    PRV_CPSS_SAL_TQ_RES446_E,                          /* 31 */
    PRV_CPSS_SAL_TQ_RES447_E,

    /* Transmit Queue Interrupt Cause 2 Register */
    /* Indexes 448 - 479               */
    /* Cause 0x01800184  */
    /* mask  0x01800188  */
    PRV_CPSS_SAL_TQ2_SUM_E,                            /*  0 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT0_E,               /*  1 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT1_E,               /*  2 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT2_E,               /*  3 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT3_E,               /*  4 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT4_E,               /*  5 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT5_E,               /*  6 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT6_E,               /*  7 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT7_E,               /*  8 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT8_E,               /*  9 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT9_E,               /* 10 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT10_E,              /* 11 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT11_E,              /* 12 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT12_E,              /* 13 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT13_E,              /* 14 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT14_E,              /* 15 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT15_E,              /* 16 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT16_E,              /* 17 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT17_E,              /* 18 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT18_E,              /* 19 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT19_E,              /* 20 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT20_E,              /* 21 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT21_E,              /* 22 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT22_E,              /* 23 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT23_E,              /* 24 */
    PRV_CPSS_SAL_TQ_HOL_REACHED_PORT_CPU_31_E,         /* 25 */
    PRV_CPSS_SAL_TQ2_RES474_E,                         /* 26 */
    PRV_CPSS_SAL_TQ_TOTAL_DESC_UNDERFLOW_E,            /* 27 */
    PRV_CPSS_SAL_TQ_TOTAL_DESC_OVERFLOW_E,             /* 28 */
    PRV_CPSS_SAL_TQ_SNIFF_DESC_DROP_E,                 /* 29 */
    PRV_CPSS_SAL_TQ2_RES478_E,                         /* 30 */
    PRV_CPSS_SAL_TQ2_RES479_E,                         /* 31 */


    /* Ethernet Bridge Interrupt Cause Register */
    /* Indexes 480 - 511               */
    /* Cause 0x02040130  */
    /* mask  0x02040134  */
    PRV_CPSS_SAL_EB_SUM_E,                                        /*  0 */
    PRV_CPSS_SAL_EB_NA_FIFO_FULL_E,                           /*  1 */
    PRV_CPSS_SAL_EB_MG_ADDR_OUT_OF_RANGE_E,                       /*  2 */
    PRV_CPSS_SAL_EB_VLAN_SECURITY_BREACH_E,                       /*  3 */
    PRV_CPSS_SAL_EB_NA_NOT_LEARNED_SECURITY_BREACH_E,             /*  4 */
    PRV_CPSS_SAL_EB_SA_MSG_DISCARDED_E,                           /*  5 */
    PRV_CPSS_SAL_EB_QA_MSG_DISCARDED_E,                           /*  6 */
    PRV_CPSS_SAL_EB_RES487_E,                                     /*  7 */
    PRV_CPSS_SAL_EB_RES488_E,                                     /*  8 */
    PRV_CPSS_SAL_EB_RES489_E,                                     /*  9 */
    PRV_CPSS_SAL_EB_RES490_E,                                     /* 10 */
    PRV_CPSS_SAL_EB_RES491_E,                                     /* 11 */
    PRV_CPSS_SAL_EB_RES492_E,                                     /* 12 */
    PRV_CPSS_SAL_EB_RES493_E,                                     /* 13 */
    PRV_CPSS_SAL_EB_SA_DROP_SECURITY_BREACH_E,                    /* 14 */
    PRV_CPSS_SAL_EB_DA_DROP_SECURITY_BREACH_E,                    /* 15 */
    PRV_CPSS_SAL_EB_DA_SA_DROP_SECURITY_BREACH_E,                 /* 16 */
    PRV_CPSS_SAL_EB_NA_ON_LOCKED_DROP_SECURITY_BREACH_E,          /* 17 */
    PRV_CPSS_SAL_EB_MAC_RANGE_DROP_SECURITY_BREACH_E,             /* 18 */
    PRV_CPSS_SAL_EB_INVALID_SA_DROP_SECURITY_BREACH_E,            /* 19 */
    PRV_CPSS_SAL_EB_VLAN_NOT_VALID_DROP_SECURITY_BREACH_E,        /* 20 */
    PRV_CPSS_SAL_EB_VLAN_NOT_MEMBER_DROP_DROP_SECURITY_BREACH_E,  /* 21 */
    PRV_CPSS_SAL_EB_VLAN_RANGE_DROP_SECURITY_BREACH_E,            /* 22 */
    PRV_CPSS_SAL_EB_RES503_E,                                     /* 23 */
    PRV_CPSS_SAL_EB_RES504_E,                                     /* 24 */
    PRV_CPSS_SAL_EB_RES505_E,                                     /* 25 */
    PRV_CPSS_SAL_EB_RES506_E,                                     /* 26 */
    PRV_CPSS_SAL_EB_RES507_E,                                     /* 27 */
    PRV_CPSS_SAL_EB_RES508_E,                                     /* 28 */
    PRV_CPSS_SAL_EB_RES509_E,                                     /* 29 */
    PRV_CPSS_SAL_EB_RES510_E,                                     /* 30 */
    PRV_CPSS_SAL_EB_RES511_E,                                     /* 31 */

    /* MAC Table Interrupt Cause Register */
    /* Indexes 512 - 543               */
    /* Cause 0x06000018  */
    /* mask  0x0600001C  */
    PRV_CPSS_SAL_MAC_SUM_E,                            /*  0 */
    PRV_CPSS_SAL_MAC_NUM_OF_HOP_EXP_E,                 /*  1 */
    PRV_CPSS_SAL_MAC_NA_LEARNED_E,                     /*  2 */
    PRV_CPSS_SAL_MAC_NA_NOT_LEARNED_E,                 /*  3 */
    PRV_CPSS_SAL_MAC_AGE_VIA_TRIGGER_ENDED_E,          /*  4 */
    PRV_CPSS_SAL_MAC_RES517_E,                         /*  5 */
    PRV_CPSS_SAL_MAC_MG_ADDR_OUT_OF_RANGE_E,           /*  6 */
    PRV_CPSS_SAL_MAC_RES519_E,                         /*  7 */
    PRV_CPSS_SAL_MAC_RES520_E,                         /*  8 */
    PRV_CPSS_SAL_MAC_UPDATE_FROM_CPU_DONE_E,           /*  9 */
    PRV_CPSS_SAL_MAC_MESSAGE_TO_CPU_READY_E,           /* 10 */
    PRV_CPSS_SAL_MAC_RES523_E,                         /* 11 */
    PRV_CPSS_SAL_MAC_NA_SELF_LEARNED_E,                /* 12 */
    PRV_CPSS_SAL_MAC_NA_FROM_CPU_LEARNED_E,            /* 13 */
    PRV_CPSS_SAL_MAC_NA_FROM_CPU_DROPPED_E,            /* 14 */
    PRV_CPSS_SAL_MAC_AGED_OUT_E,                       /* 15 */
    PRV_CPSS_SAL_MAC_FIFO_2_CPU_EXCEEDED_E,            /* 16 */
    PRV_CPSS_SAL_MAC_RES529_E,                         /* 17 */
    PRV_CPSS_SAL_MAC_RES530_E,                         /* 18 */
    PRV_CPSS_SAL_MAC_RES531_E,                         /* 19 */
    PRV_CPSS_SAL_MAC_RES532_E,                         /* 20 */
    PRV_CPSS_SAL_MAC_RES533_E,                         /* 21 */
    PRV_CPSS_SAL_MAC_RES534_E,                         /* 22 */
    PRV_CPSS_SAL_MAC_RES535_E,                         /* 23 */
    PRV_CPSS_SAL_MAC_RES536_E,                         /* 24 */
    PRV_CPSS_SAL_MAC_RES537_E,                         /* 25 */
    PRV_CPSS_SAL_MAC_RES538_E,                         /* 26 */
    PRV_CPSS_SAL_MAC_RES539_E,                         /* 27 */
    PRV_CPSS_SAL_MAC_1ECC_ERRORS_EVEN_E,               /* 28 */
    PRV_CPSS_SAL_MAC_1ECC_ERRORS_ODD_E,                /* 29 */
    PRV_CPSS_SAL_MAC_2ECC_ERRORS_EVEN_E,               /* 30 */
    PRV_CPSS_SAL_MAC_2ECC_ERRORS_ODD_E,                /* 31 */
    /* FAKE - NIC */
    PRV_CPSS_SAL_EB_AUQ_PENDING_E,                        /*  0 */
    PRV_CPSS_SAL_RX_BUFFER_QUEUE0_E,                   /*  1 */
    PRV_CPSS_SAL_RX_BUFFER_QUEUE1_E,                   /*  2 */
    PRV_CPSS_SAL_RX_BUFFER_QUEUE2_E,                   /*  3 */
    PRV_CPSS_SAL_RX_BUFFER_QUEUE3_E,                   /*  4 */
    PRV_CPSS_SAL_RX_BUFFER_QUEUE4_E,                   /*  5 */
    PRV_CPSS_SAL_RX_BUFFER_QUEUE5_E,                   /*  6 */
    PRV_CPSS_SAL_RX_BUFFER_QUEUE6_E,                   /*  7 */
    PRV_CPSS_SAL_RX_BUFFER_QUEUE7_E,                   /*  8 */
    PRV_CPSS_SAL_TX_END_E,                             /*  9 */
    PRV_CPSS_SAL_LAST_INT         /* should be always last in enum */
}GT_SAL_INT_CAUSE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvExMxEventsSalsah */

