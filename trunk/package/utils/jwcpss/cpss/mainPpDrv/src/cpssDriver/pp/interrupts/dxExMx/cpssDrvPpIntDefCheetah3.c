/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefCheetah3.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables. -- Cheetah-3 devices
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsCheetah3.h>

#include <cpss/generic/bridge/private/prvCpssGenBrgFdbIsrSig.h>

/************************************ Cheetah 3 ******************************/
/*      nonSumBitMask   - bit mask of non sum bits in the register
 *      rwBitMask       - bit mask of R/W bits that should be cleared by write.
 *      maskRcvIntrEn   - bit mask of interrupts to be masked after receive.
 */
#define BITS(x,y)  (FIELD_MASK_MAC(x, (1 + (y) - (x))))

/* NSBM Non - Sum - Bit - Mask (except self)         */
#define GLOBAL_NSBM          0x0e0007f1
#define MISC_NSBM            (BITS(0,15))
#define BUFFERS_MEMORY_MAIN_NSBM    0x00000007
#define GPP_NSBM             (BITS(0,8))
#define RXSDMA_NSBM          0x003ffffd
#define TXSDMA_NSBM          (BITS(0,24))
#define PEX_NSBM             (BITS(24,27))
#define PEX_ERR_NSBM         (BITS(0,23))
#define PORT_NSBM            (BITS(0,9))
#define XG_PORTS_NSBM        0x70beebe1
#define HGS_MIB_NSBM         (BITS(0,10))
#define POLICY_NSBM          (BITS(0,6))
#define BRIDGE_NSBM          (BITS(0,28))
#define PRE_EGRESS_NSBM      (BITS(0,0))
#define SCT_RATE_LIM_NSBM    0xFFFFFFFF
#define INGRESS_STC_NSBM     (BITS(0,28))
#define EGRESS_STC_NSBM      (BITS(0,28))
#define TXQ_FLASH_NSBM       (BITS(0,29))
#define TXQ_WD_NSBM          (BITS(0,29))
#define TXQ_GEN_NSBM         (BITS(0,10))
#define TXQ_FULL_NSBM        0xFFFFFFFF
#define BUF_MEM_R0_NSBM      0xFFFFFFFF
#define BUF_MEM_R1_NSBM      (BITS(0,4))
#define TRANSMIT_QUEUE_NSBM  0x00000041
#define BUF_MNG_R0_NSBM      (BITS(0,20))
#define BUF_MNG_R1_NSBM      0xFFFFFFFF
#define MAC_NSBM             0x0001ffff
#define GOP_0_NSBM           0x0000000f
#define GOP_1_NSBM           0x0000000f
#define GOP_2_NSBM           0x0000000f
#define GOP_3_NSBM           0x0000000f
#define XG_PORT_NSBM         (BITS(0,11))
#define INGRESS_POLICER_NSBM (BITS(0,2))
#define CNC_NSBM             (BITS(0,26))


/* default for Giga Port interrupt mask register */
#define GIG_PORT_DEF_MASK  0xE6

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 cheetah3MaskRegMapArr[] =
{
    0x00000034,     /* GLOBAL          interrupt index    0.. 31 */
    0x0000003C,     /* Miscellaneous   interrupt index   32.. 63 */
    0x018001C0,     /* GPP             interrupt index   64.. 95 */
    0x00002814,     /* RxSDMA          interrupt index   96..127 */
    0x00002818,     /* TxSDMA          interrupt index  128..159 */
    0x00071910,     /* PCI Express (PEX)
                                       interrupt index  160..191 */
    0x04805110,     /* Tri-speed ports and
                         MIB Counters  interrupt index  192..223 */
    0x04805114,     /* Tri-speed ports interrupt index  224..255 */
    0x0A800024,     /* PORT0           interrupt index  256..287 */
    0x0A800424,     /* PORT1           interrupt index  288..319 */
    0x0A800824,     /* PORT2           interrupt index  320..351 */
    0x0A800C24,     /* PORT3           interrupt index  352..383 */
    0x0A801024,     /* PORT4           interrupt index  384..415 */
    0x0A801424,     /* PORT5           interrupt index  416..447 */
    0x0A801824,     /* PORT6           interrupt index  448..479 */
    0x0A801C24,     /* PORT7           interrupt index  480..511*/
    0x0A802024,     /* PORT8           interrupt index  512..543 */
    0x0A802424,     /* PORT9           interrupt index  544..575 */
    0x0A802824,     /* PORT10          interrupt index  576..607 */
    0x0A802C24,     /* PORT11          interrupt index  608..639 */
    0x0A803024,     /* PORT12          interrupt index  640..671 */
    0x0A803424,     /* PORT13          interrupt index  672..703 */
    0x0A803824,     /* PORT14          interrupt index  704..735 */
    0x0A803C24,     /* PORT15          interrupt index  736..767 */
    0x0A804024,     /* PORT16          interrupt index  768..799 */
    0x0A804424,     /* PORT17          interrupt index  800..831 */
    0x0A804824,     /* PORT18          interrupt index  832..863 */
    0x0A804C24,     /* PORT19          interrupt index  864..895 */
    0x0A805024,     /* PORT20          interrupt index  896..927 */
    0x0A805424,     /* PORT21          interrupt index  928..959 */
    0x0A805824,     /* PORT22          interrupt index  960..991 */
    0x0A805C24,     /* PORT23          interrupt index  992..1023 */
    0x0A80FC24,     /* PORT63 (CPU)    interrupt index  1024..1055 */
    0x0180013C,     /* HG.StackMIB     interrupt index  1056..1087 */
    0x08801018,     /* XG Port4        interrupt index  1088..1119 */
    0x08802818,     /* XG Port10       interrupt index  1120..1151 */
    0x08803018,     /* XG Port12       interrupt index  1152..1183 */
    0x08804018,     /* XG Port16       interrupt index  1184..1215 */
    0x08805818,     /* XG Port22       interrupt index  1216..1247 */
    0x08806018,     /* XG Port24       interrupt index  1248..1279 */
    0x08806418,     /* XG Port25       interrupt index  1280..1311 */
    0x08806818,     /* XG Port26       interrupt index  1312..1343 */
    0x08806C18,     /* XG Port27       interrupt index  1344..1375 */
    0x08800018,     /* XG Port0        interrupt index  1376..1407 */
    0x0B800014,     /* PolicyEngine    interrupt index  1408..1439 */
    0x02040134,     /* BRIDGE          interrupt index  1440..1471 */
    0x0B00005C,     /* Pre-Egress      interrupt index  1472..1503 */
    0x0B000064,     /* SCT Rate Limiter interrupt index 1504..1535 */
    0x0B000024,     /* IngressSTC      interrupt index  1536..1567 */
    0x01800104,     /* TXQ             interrupt index  1568..1599 */
    0x01800114,     /* TXQFlash        interrupt index  1600..1631 */
    0x0180010C,     /* TXQWD           interrupt index  1632..1663 */
    0x0180011C,     /* TXQGEN          interrupt index  1664..1695 */
    0x018001EC,     /* TXQFull         interrupt index  1696..1727 */
    0x01800134,     /* EgrerssSTC      interrupt index  1728..1759 */
    0x07800094,     /* BuffMem Main    interrupt index  1760..1791 */
    0x07800028,     /* BuffMem0        interrupt index  1792..1823 */
    0x0780009C,     /* BuffMem1        interrupt index  1824..1855 */
    0x03000044,     /* BuffMngR0       interrupt index  1856..1887 */
    0x0300004C,     /* BuffMngR1       interrupt index  1888..1919 */
    0x0600001C,     /* MACTable        interrupt index  1920..1951 */
    0x04004004,     /* GOP0 MIB        interrupt index  1952..1983 */
    0x04804004,     /* GOP1 MIB        interrupt index  1984..2015 */
    0x05004004,     /* GOP2 MIB        interrupt index  2016..2047 */
    0x05804004,     /* GOP3 MIB        interrupt index  2048..2079 */
    0x08000104,     /* CNC             interrupt index  2080..2111*/
    0x00000084,     /* XG Ports        interrupt index  2112..2143  */
    0x0C000104      /* Ingress Policer (IPLR)
                                       interrupt mask   2144..2175 */

};

#ifdef PRV_CPSS_DRV_ERRATA_REFERENCE_CNS  /* macro never defined --> for references purpose only  */
    /* see below treatment for : PRV_CPSS_DXCH3_TXQ_FULL_INTERRUPT_NOT_FUNCTION_WA_E */
    PRV_CPSS_DXCH3_TXQ_FULL_INTERRUPT_NOT_FUNCTION_WA_E
#endif /*PRV_CPSS_DRV_ERRATA_REFERENCE_CNS*/
/* This array maps an interrupt index to its relevant interrupt mask reg    */
/* default value, cell i indicates the address for interrupts               */
/* (32 * i  --> 32 * (i + 1) - 1).                                          */
/* This map is used when PSS is in polling mode only, summary bits are      */
/* turned on and the specific cause mask is enabled when binded by appl.    */
static GT_U32 cheetah3MaskRegDefaultSummaryArr[] =
{
    0x11FFF80E,         /* GLOBAL         */
    0x00000000,         /* Miscellaneous  */
    0x00000000,         /* GPP            */
    0x00000000,         /* RxSDMA         */
    0x00000000,         /* TxSDMA         */
    0x00000000,         /* Express PCI (PEX) */
    0x0000003E,         /* Tri-speed ports and MIB Counters */
    0x01FFFFFE,         /* Tri-speed ports */
    0x00000000,         /* PORT0          */
    0x00000000,         /* PORT1          */
    0x00000000,         /* PORT2          */
    0x00000000,         /* PORT3          */
    0x00000000,         /* PORT4          */
    0x00000000,         /* PORT5          */
    0x00000000,         /* PORT6          */
    0x00000000,         /* PORT7          */
    0x00000000,         /* PORT8          */
    0x00000000,         /* PORT9          */
    0x00000000,         /* PORT10         */
    0x00000000,         /* PORT11         */
    0x00000000,         /* PORT12         */
    0x00000000,         /* PORT13         */
    0x00000000,         /* PORT14         */
    0x00000000,         /* PORT15         */
    0x00000000,         /* PORT16         */
    0x00000000,         /* PORT17         */
    0x00000000,         /* PORT18         */
    0x00000000,         /* PORT19         */
    0x00000000,         /* PORT20         */
    0x00000000,         /* PORT21         */
    0x00000000,         /* PORT22         */
    0x00000000,         /* PORT23         */
    0x00000000,         /* PORT63 (CPU)   */
    0x00000000,         /* HG.StackMIB    */
    0x00000000,         /* XG Port4       */
    0x00000000,         /* XG Port10      */
    0x00000000,         /* XG Port12      */
    0x00000000,         /* XG Port16      */
    0x00000000,         /* XG Port22      */
    0x00000000,         /* XG Port24      */
    0x00000000,         /* XG Port25      */
    0x00000000,         /* XG Port26      */
    0x00000000,         /* XG Port27      */
    0x00000000,         /* XG Port0       */
    0x00000000,         /* PolicyEngine   */
    0x00000000,         /* BRIDGE         */
    0x00000006,         /* Pre-Egress     */
    0x00000000,         /* SCT Rate Limiters */
    0x00000000,         /* IngressSTC     */
    0x0000003E,         /* TXQ, bit 7 must be 0 see
                           PRV_CPSS_DXCH3_TXQ_FULL_INTERRUPT_NOT_FUNCTION_WA_E */
    0x00000000,         /* TXQFlush       */
    0x00000000,         /* TXQWD          */
    0x00000000,         /* TXQGEN         */
    0x00000000,         /* TXFull         */
    0x00000000,         /* EgrerssSTC     */
    0x00000006,         /* BuffMem Main   */
    0x00000000,         /* BuffMem0       */
    0x00000000,         /* BuffMem1       */
    0x00000000,         /* BuffMngR0      */
    0x00000000,         /* MACTable       */
    0x00000000,         /* GOP0 MIB       */
    0x00000000,         /* GOP1 MIB       */
    0x00000000,         /* GOP2 MIB       */
    0x00000000,         /* GOP3 MIB       */
    0x00000000,         /* BuffMngR1      */
    0x00000000,         /* CNC            */
    0x8F411410,         /* XG Ports       */
    0x00000000          /* Ingress Policer (IPLR) */
};

/* the same as cheetah3MaskRegDefaultSummaryArr except that PEX Summary bits are masked */
static GT_U32 cheetah3MaskRegDefaultSummaryArr_SMI[sizeof(cheetah3MaskRegDefaultSummaryArr)/
                                                   sizeof(cheetah3MaskRegDefaultSummaryArr[0])];

/*
 * Typedef: struct INTERRUPT_SCAN
 *
 * Description: Basic struct for accessing interrupt register in hierarchy
 *              tree.
 *
 * Fields:
 *      bitNum          - Sum bit num in upper hierarchy, representing this
 *                        resigter.
 *      isGpp           - Is hierarcy connected to another device.
 *      gppId           - The gpp Id represented by this node (Valid only if
 *                        isGpp is GT_TRUE).
 *      gppFuncPtr      - Pointer to isrFunc to be called if isGPP == GT_TRUE.
 *      devNum          - Device number for isrFunc Call.
 *      causeRegAddr    - Address of the interrupt cause register to scan.
 *      maskRegAddr     - Address of the interrupt mask register to update after
 *                        scanning.
 *      pRegReadFunc    - A pointer to a function to be used for reading the
 *                        interrupt cause register.
 *      pRegWriteFunc   - A pointer to a function to be used for writing the
 *                        interrupt mask register.
 *      startIdx        - The start interrupt index representing the interrupts
 *                        to be scanned.
 *      endIdx          - The end interrupt index representing the interrupts to
 *                        be scanned.
 *      nonSumBitMask   - bit mask of non sum bits in the register
 *      rwBitMask       - bit mask of R/W bits that should be cleared by write.
 *      maskRcvIntrEn   - bit mask of interrupts to be masked after receive.
 *      subIntrListLen  - List length sub interrupts list.
 *      subIntrScan     - pointer to sum bit interrupt register structure.
 *      nextIntrScan    - pointer to next interrupt struct in hirarchy.
 */

static const PRV_CPSS_DRV_INTERRUPT_SCAN_STC cheetah3IntrScanArr[] =
{
        /* Global */
    {0, GT_FALSE,0, NULL, 0x30, 0x34,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_CH3_GLOBAL_PEX_SUM_E, /* should be 1 to avoid scaning the
                                              * summary bit of the PCI main
                                              * cause interrupt */
        PRV_CPSS_CH3_GLOBAL_PRE_EGRESS_SUM_E, /* enumerators range 1- > 10*/
        GLOBAL_NSBM, 0x0, 0x0, 18, NULL, NULL},


             /* PEX */
        {1,GT_FALSE,0, NULL, 0x00071900, 0x00071910,
            prvCpssDrvHwPpPortGroupReadInternalPciReg,
            prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            PRV_CPSS_CH3_PEX_RCV_A_E,
            PRV_CPSS_CH3_PEX_RCV_D_E,
            PEX_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


             /* PEX ERROR */
        {2,GT_FALSE,0, NULL, 0x00071900, 0x00071910,
            prvCpssDrvHwPpPortGroupReadInternalPciReg,
            prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            PRV_CPSS_CH3_PEX_DL_DOWN_TX_ACC_ERR_E,
            PRV_CPSS_CH3_PEX_PEX_LINK_FAIL_E,
            PEX_ERR_NSBM, 0xFF1F1B, 0xFFFFFFFF, 0,NULL, NULL},


             /* CNC */
        {3,GT_FALSE,0, NULL, 0x08000100, 0x08000104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_CNC_WRAPAROUND_BLOCK0_E,
            PRV_CPSS_CH3_CNC_UPDATE_LOST_E,
            CNC_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* Miscellaneous */
        {11,GT_FALSE,0, NULL, 0x00000038, 0x0000003C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_MISC_TWSI_TIME_OUT_E,
            PRV_CPSS_CH3_MISC_PEX_ADDR_UNMAPPED_E,
            MISC_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* Buffers Memory Main */
        {12,GT_FALSE,0, NULL, 0x07800090, 0x07800094,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_BM_MAIN_REG0_SUM_E,
            PRV_CPSS_CH3_BM_MAIN_REG1_SUM_E,
            BUFFERS_MEMORY_MAIN_NSBM, 0x0, 0xFFFFFFFF, 2, NULL, NULL},

                 /* Buffers Memory Register 0  */
            {0,GT_FALSE,0, NULL, 0x07800024, 0x07800028,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_BM_TX_FIFO_OVER_RUN_PORT0_E,
                PRV_CPSS_CH3_BUF_MEM_BURST_FIFO_OVER_RUN_E,
                BUF_MEM_R0_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

                 /* Buffers Memory Register 1  */
            {0,GT_FALSE,0, NULL, 0x07800098, 0x0780009C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_BUF_MEM_BANK0_ONE_ECC_ERROR_E,
                PRV_CPSS_CH3_BUF_MEM_BANK1_TWO_OR_MORE_ECC_ERRORS_E,
                BUF_MEM_R1_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


            /* Transmit Queue */
        {13, GT_FALSE,0, NULL, 0x01800100, 0x01800104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_TXQ_SUM_WATCHDOG_E,
            PRV_CPSS_CH3_TXQ_SUM_FULL_E,
            TRANSMIT_QUEUE_NSBM, 0x0, 0xFFFFFFFF, 6, NULL, NULL},

             /* Transmit Queue WatchDog */
             {1, GT_FALSE,0, NULL, 0x01800108, 0x0180010C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_TQ_WATCHDOG_EX_PORT0_E,
                PRV_CPSS_CH3_TQ_WATCHDOG_EX_PORT_CPU_63_E,
                TXQ_WD_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* Transmit Queue Flash */
             {2, GT_FALSE,0, NULL, 0x01800110, 0x01800114,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH3_TQ_TXQ2_FLUSH_PORT0_E,
                 PRV_CPSS_CH3_TQ_TXQ2_FLUSH_PORT_CPU_63_E,
                 TXQ_FLASH_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* Transmit Queue General */
             {3, GT_FALSE,0, NULL, 0x01800118, 0x0180011C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH3_TQ_SNIFF_DESC_DROP_E,
                 PRV_CPSS_CH3_TXQ_GEN_BUF_FULL_E,
                 TXQ_GEN_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* GPP */
             {4, GT_FALSE,0, NULL, 0x018001CC, 0x018001C0,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH3_GPP_INTERRUPT1_E,
                 PRV_CPSS_CH3_GPP_INTERRUPT8_E,
                 GPP_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

               /* Egress STC */
             {5, GT_FALSE,0, NULL, 0x01800130, 0x01800134,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH3_EGRESS_STC_PORT0_E,
                 PRV_CPSS_CH3_EGRESS_STC_PORT27_E,
                 EGRESS_STC_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* Transmit Queue Full */
             {7, GT_FALSE,0, NULL, 0x0180018C, 0x018001EC,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH3_TQ_DESC_FULL_PORT0_E,
                 PRV_CPSS_CH3_TQ_XG_MC_FIFO_FULL_E,
                 TXQ_FULL_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


        /* L2 Bridge  */
        {14, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_EB_NA_FIFO_FULL_E,
            PRV_CPSS_CH3_EB_SECURITY_BREACH_UPDATE_E,
            BRIDGE_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


           /* Buffer Management Register 0*/
        {15, GT_FALSE,0, NULL, 0x03000040, 0x03000044,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_BM_EMPTY_CLEAR_E,
            PRV_CPSS_CH3_BM_PORT_RX_FULL_E,
            BUF_MNG_R0_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

           /* Buffer Management Register 1*/
        {16, GT_FALSE,0, NULL, 0x03000048, 0x0300004C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_BM_MAX_BUFF_REACHED_PORT0_E,
            PRV_CPSS_CH3_BM_MAX_BUFF_REACHED_PORT_CPU_63_E,
            BUF_MNG_R1_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


        /* MAC Table,
           maskRcvIntrEn - both AUProcCompletedInt and AgeViaTriggerEnded
           should never be masked to avoid missed events situation.  */
        {17, GT_FALSE,0, NULL, 0x06000018, 0x0600001C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_MAC_NUM_OF_HOP_EXP_E,
            PRV_CPSS_CH3_MAC_FIFO_2_CPU_EXCEEDED_E,
            MAC_NSBM, 0x0, 0xFFFFFDEF, 0,NULL, NULL},

        /* Ports - Tri-speed ports MIB counters  */
        {18, GT_FALSE,0, NULL, 0x04004010, 0x04805110,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_3SPPMIB_GOP0MIB_SUM_E,
            PRV_CPSS_CH3_3SPPMIB_PORTS_SUM_E,
            0x0, 0x0, 0xFFFFFFFF, 5, NULL, NULL},

            /* GOP0 MIB  */
            {1, GT_FALSE,0, NULL, 0x04004000, 0x04004004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_GOP_ADDRESS_OUT_OF_RANGE_0_E,
                PRV_CPSS_CH3_GOP_COUNT_COPY_DONE_0_E,
                GOP_0_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* GOP1 MIB  */
            {2, GT_FALSE,0, NULL, 0x04804000, 0x04804004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_GOP_ADDRESS_OUT_OF_RANGE_1_E,
                PRV_CPSS_CH3_GOP_COUNT_COPY_DONE_1_E,
                GOP_1_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* GOP2 MIB  */
            {3, GT_FALSE,0, NULL, 0x05004000, 0x05004004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_GOP_ADDRESS_OUT_OF_RANGE_2_E,
                PRV_CPSS_CH3_GOP_COUNT_COPY_DONE_2_E,
                GOP_2_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* GOP3 MIB  */
            {4, GT_FALSE,0, NULL, 0x05804000, 0x05804004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_GOP_ADDRESS_OUT_OF_RANGE_3_E,
                PRV_CPSS_CH3_GOP_COUNT_COPY_DONE_3_E,
                GOP_3_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Tri-speed Ports Register */
            {5, GT_FALSE,0, NULL, 0x05004010, 0x04805114,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_3SPP_PORT0_SUM_E,
                PRV_CPSS_CH3_3SPP_PORT23_SUM_E,
                0x0, 0x0, 0xFFFFFFFF, 24, NULL, NULL},

                    /* Port 0  */
                 {1, GT_FALSE,0, NULL, 0x0A800020, 0x0A800024,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT0_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT0_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 1  */
                 {2, GT_FALSE,0, NULL, 0x0A800420, 0x0A800424,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT1_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT1_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 2  */
                 {3, GT_FALSE,0, NULL, 0x0A800820, 0x0A800824,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT2_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT2_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 3  */
                 {4, GT_FALSE,0, NULL, 0x0A800C20, 0x0A800C24,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT3_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT3_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 4  */
                 {5, GT_FALSE,0, NULL, 0x0A801020, 0x0A801024,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT4_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT4_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 5  */
                 {6, GT_FALSE,0, NULL, 0x0A801420, 0x0A801424,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT5_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT5_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 6  */
                 {7, GT_FALSE,0, NULL, 0x0A801820, 0x0A801824,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT6_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT6_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


                    /* Port 7  */
                 {8, GT_FALSE,0, NULL, 0x0A801C20, 0x0A801C24,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT7_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT7_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 8  */
                 {9, GT_FALSE,0, NULL, 0x0A802020, 0x0A802024,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT8_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT8_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 9  */
                 {10, GT_FALSE,0, NULL, 0x0A802420, 0x0A802424,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT9_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT9_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 10  */
                 {11, GT_FALSE,0, NULL, 0x0A802820, 0x0A802824,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT10_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT10_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 11  */
                 {12, GT_FALSE,0, NULL, 0x0A802C20, 0x0A802C24,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT11_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT11_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 12  */
                 {13, GT_FALSE,0, NULL, 0x0A803020, 0x0A803024,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT12_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT12_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 13  */
                 {14, GT_FALSE,0, NULL, 0x0A803420, 0x0A803424,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT13_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT13_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 14  */
                 {15, GT_FALSE,0, NULL, 0x0A803820, 0x0A803824,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT14_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT14_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 15  */
                 {16, GT_FALSE,0, NULL, 0x0A803C20, 0x0A803C24,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT15_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT15_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 16  */
                 {17, GT_FALSE,0, NULL, 0x0A804020, 0x0A804024,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT16_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT16_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 17  */
                 {18, GT_FALSE,0, NULL, 0x0A804420, 0x0A804424,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT17_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT17_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 18  */
                 {19, GT_FALSE,0, NULL, 0x0A804820, 0x0A804824,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT18_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT18_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 19  */
                 {20, GT_FALSE,0, NULL, 0x0A804C20, 0x0A804C24,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT19_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT19_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 20  */
                 {21, GT_FALSE,0, NULL, 0x0A805020, 0x0A805024,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT20_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT20_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 21  */
                 {22, GT_FALSE,0, NULL, 0x0A805420, 0x0A805424,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT21_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT21_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 22  */
                 {23, GT_FALSE,0, NULL, 0x0A805820, 0x0A805824,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT22_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT22_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                    /* Port 23  */
                 {24, GT_FALSE,0, NULL, 0x0A805C20, 0x0A805C24,
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT23_E,
                     PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT23_E,
                     PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


            /* CPU Port SUM  */
        {19, GT_FALSE,0, NULL, 0x0A80FC20, 0x0A80FC24,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
             PRV_CPSS_CH3_LINK_STATUS_CHANGED_PORT_CPU_63_E,
             PRV_CPSS_CH3_TX_FIFO_UNDERRUN_PORT_CPU_63_E,
             PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


            /* XG Ports  */
        {20, GT_FALSE,0, NULL, 0x00000080, 0x00000084,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
             PRV_CPSS_CH3_XG_PORT4_SUM_E,
             PRV_CPSS_CH3_XG_PORT0_SUM_E,
             XG_PORTS_NSBM, 0x0, 0xFFFFFFFF, 10,NULL, NULL},

                /* XG Port 4 */
            {4, GT_FALSE,0, NULL, 0x08801014, 0x08801018,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_XG_LINK_STATUS_CHANGED_PORT4_E,
                PRV_CPSS_CH3_XG_COUNT_EXPIRED_PORT4_E,
                XG_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* XG Port 10 */
            {10, GT_FALSE,0, NULL, 0x08802814, 0x08802818,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_XG_LINK_STATUS_CHANGED_PORT10_E,
                PRV_CPSS_CH3_XG_COUNT_EXPIRED_PORT10_E,
                XG_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* XG Port 12 */
            {12, GT_FALSE,0, NULL, 0x08803014, 0x08803018,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_XG_LINK_STATUS_CHANGED_PORT12_E,
                PRV_CPSS_CH3_XG_COUNT_EXPIRED_PORT12_E,
                XG_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* XG Port 16 */
            {16, GT_FALSE,0, NULL, 0x08804014, 0x08804018,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_XG_LINK_STATUS_CHANGED_PORT16_E,
                PRV_CPSS_CH3_XG_COUNT_EXPIRED_PORT16_E,
                XG_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* XG Port 22 */
            {22, GT_FALSE,0, NULL, 0x08805814, 0x08805818,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_XG_LINK_STATUS_CHANGED_PORT22_E,
                PRV_CPSS_CH3_XG_COUNT_EXPIRED_PORT22_E,
                XG_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* XG Port 24 */
            {24, GT_FALSE,0, NULL, 0x08806014, 0x08806018,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_XG_LINK_STATUS_CHANGED_PORT24_E,
                PRV_CPSS_CH3_XG_COUNT_EXPIRED_PORT24_E,
                XG_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* XG Port 25 */
            {25, GT_FALSE,0, NULL, 0x08806414, 0x08806418,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_XG_LINK_STATUS_CHANGED_PORT25_E,
                PRV_CPSS_CH3_XG_COUNT_EXPIRED_PORT25_E,
                XG_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* XG Port 26 */
            {26, GT_FALSE,0, NULL, 0x08806814, 0x08806818,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_XG_LINK_STATUS_CHANGED_PORT26_E,
                PRV_CPSS_CH3_XG_COUNT_EXPIRED_PORT26_E,
                XG_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* XG Port 27 */
            {27, GT_FALSE,0, NULL, 0x08806C14, 0x08806C18,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_XG_LINK_STATUS_CHANGED_PORT27_E,
                PRV_CPSS_CH3_XG_COUNT_EXPIRED_PORT27_E,
                XG_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* XG Port 0 */
            {31, GT_FALSE,0, NULL, 0x08800014, 0x08800018,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_XG_LINK_STATUS_CHANGED_PORT0_E,
                PRV_CPSS_CH3_XG_COUNT_EXPIRED_PORT0_E,
                XG_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Tx SDMA  */
        {21, GT_FALSE,0, NULL, 0x00002810, 0x00002818,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_TX_BUFFER_QUEUE0_E,
            PRV_CPSS_CH3_TX_END_QUEUE7_E,
            TXSDMA_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Rx SDMA  */
        {22, GT_FALSE,0, NULL, 0x0000280C, 0x00002814,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_RX_BUFFER_QUEUE0_E,
            PRV_CPSS_CH3_SDMA_RX_PACKET_CNT_OFF_E,
            RXSDMA_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* PCL  */
        {23, GT_FALSE,0, NULL, 0x0B800010, 0x0B800014,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_PCL_ADDR_OUT_RANGE_E,
            PRV_CPSS_CH3_PCL_TCC_ECC_ERR_E,
            POLICY_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* Ingress Policer  */
        {24, GT_FALSE,0, NULL, 0x0C000100, 0x0C000104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_POLICER_DATA_ERR_E,
            PRV_CPSS_CH3_POLICER_ADDR_OUT_OF_MEMORY_E,
            INGRESS_POLICER_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* Pre-Egress */
        {28, GT_FALSE,0, NULL, 0x0B000058, 0x0B00005C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH3_PREEGR_INGR_STC_SUM_E,
            PRV_CPSS_CH3_PREEGR_SCT_RATE_LIM_SUM_E,
            PRE_EGRESS_NSBM, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

                /* Ingress STC  */
            {1, GT_FALSE,0, NULL, 0x0B000020, 0x0B000024,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_MAC_SFLOW_PORT0_E,
                PRV_CPSS_CH3_MAC_SFLOW_PORT27_E,
                INGRESS_STC_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* STC Rate Limiters */
            {2, GT_FALSE,0, NULL, 0x0B000060, 0x0B000064,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH3_SCT_RATE_LIMITER_1_PKT_DROP_E,
                PRV_CPSS_CH3_SCT_RATE_LIMITER_31_PKT_DROP_E,
                SCT_RATE_LIM_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},
};

/*******************************************************************************
* prvCpssDrvPpIntDefCheetah3Init
*
* DESCRIPTION:
*       Interrupts initialization for the Cheetah3 devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefCheetah3Init(void)
{
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devFamilyPtr;

    /* copy PCI registers default mask, to SMI registers default mask */
    cpssOsMemCpy(cheetah3MaskRegDefaultSummaryArr_SMI,
                 (void*)cheetah3MaskRegDefaultSummaryArr,
                 sizeof(cheetah3MaskRegDefaultSummaryArr));

    /* change in SMI default mask, the mask of Global register, so that PEX interrupts
       will be masked */
    cheetah3MaskRegDefaultSummaryArr_SMI[0] = 0x119FF808;

    /* cheetah-3 PCI info */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH3_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_CH3_LAST_INT_E;
    devFamilyPtr->maskRegistersAddressesArray = cheetah3MaskRegMapArr;
    devFamilyPtr->maskRegistersDefaultValuesArray = cheetah3MaskRegDefaultSummaryArr;
    devFamilyPtr->interruptsScanArray = cheetah3IntrScanArr;
    devFamilyPtr->numOfScanElements = sizeof(cheetah3IntrScanArr)/sizeof(cheetah3IntrScanArr[0]);
    devFamilyPtr->fdbTrigEndedId = PRV_CPSS_CH3_MAC_AGE_VIA_TRIGGER_ENDED_E;
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = 0;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=NULL;

    /* cheetah-3 SMI info */
    /* start with same info as PCI , then override some of it */
    cpssOsMemCpy(&prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH3_SMI_E],
                 &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH3_E],
                 sizeof(PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC));

    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH3_SMI_E];
    devFamilyPtr->maskRegistersDefaultValuesArray = cheetah3MaskRegDefaultSummaryArr_SMI;
}

#ifdef DUMP_DEFAULT_INFO
/*******************************************************************************
* prvCpssDrvPpIntDefDxCh3Print
*
* DESCRIPTION:
*      print the interrupts arrays info, of xcat devices
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       void
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void  prvCpssDrvPpIntDefDxCh3Print(
    void
)
{
    prvCpssDrvPpIntDefPrint(sizeof(cheetah3IntrScanArr)/sizeof(cheetah3IntrScanArr[0]),
                            (PRV_CPSS_DRV_INTERRUPT_SCAN_STC*)cheetah3IntrScanArr,/*casting to remove the 'const'*/
                            sizeof(cheetah3MaskRegMapArr)/sizeof(cheetah3MaskRegMapArr[0]),cheetah3MaskRegMapArr,
                            cheetah3MaskRegDefaultSummaryArr);
}
#endif/*DUMP_DEFAULT_INFO*/

