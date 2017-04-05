/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefSalsa.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables. -- Salsa devices
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsSalsa.h>

/*      nonSumBitMask   - bit mask of non sum bits in the register
 *      rwBitMask       - bit mask of R/W bits that should be cleared by write.
 *      maskRcvIntrEn   - bit mask of interrupts to be masked after receive.
 */
#define BITS(x,y)  (FIELD_MASK_MAC(x, (1 + (y) - (x))))

/* NSBM Non - Sum - Bit - Mask (except self)         */
#define GLOBAL_NSBM (0x00000000)
#define MISC_NSBM   (BITS(0,6))
#define GPP_NSBM    (BITS(0,8))
#define BM_NSBM     (BITS(0,13))
#define GOP0R0_NSBM (BITS(0,27))
#define GOP1R0_NSBM (BITS(0,27))
#define GOP2R0_NSBM (BITS(0,27))
#define GOP3R0_NSBM (BITS(0,27))
#define GOP0R1_NSBM (BITS(0,18))
#define GOP1R1_NSBM (BITS(0,18))
#define GOP2R1_NSBM (BITS(0,18))
#define GOP3R1_NSBM (BITS(0,18))
#define TQ0_NSBM    (BITS(0,25))
#define TQ1_NSBM    (BITS(0,29))
#define TQ2_NSBM    (BITS(0,29))
#define EBRG_NSBM   (BITS(0,22))
#define MAC_NSBM    (0xFFFFFFFF)


/************************************ Salsa 24 G ******************************/
/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 salsaMaskRegMapArr[]=
{
    0x00000034,     /* GLOBAL         interrupt index    0.. 31 */
    0x0000003C,     /* Miscellaneous  interrupt index   32.. 63 */
    0x0000008C,     /* GPP            interrupt index   64.. 95 */
    0x03000044,     /* Buffer Mng     interrupt index   96..127 */
    0x04004004,     /* GOP0           interrupt index  128..159 */
    0x04804004,     /* GOP1           interrupt index  160..191 */
    0x05004004,     /* GOP2           interrupt index  192..223 */
    0x05804004,     /* GOP3           interrupt index  224..255 */
    0x0400400C,     /* GOP0           interrupt index  256..287 */
    0x0480400C,     /* GOP1           interrupt index  288..319 */
    0x0500400C,     /* GOP2           interrupt index  320..351 */
    0x0580400C,     /* GOP3           interrupt index  352..383 */
    0x01800704,     /* TxQ 0          interrupt index  384..415 */
    0x0180070C,     /* TxQ 1          interrupt index  416..447 */
    0x01800188,     /* TxQ 2          interrupt index  448..479 */
    0x02040134,     /* Eth Bridge     interrupt index  480..511 */
    0x0600001C      /* MAC Table      interrupt index  512..541 */
};


/* This array maps an interrupt index to its    */
/* relevant interrupt mask reg default value,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 salsa24MaskRegDefaultArr[]=
{
    BITS(1,15),     /* GLOBAL         */
    0x00000000,     /* Miscellaneous  */
    BITS(1,8),      /* GPP            */
    BITS(1,13),     /* Buffer Mng     */
    BITS(1,27),     /* GOP0 REG0      */
    BITS(1,18),     /* GOP0 REG1      */
    BITS(1,27),     /* GOP1 REG0      */
    BITS(1,18),     /* GOP1 REG1      */
    BITS(1,27),     /* GOP2 REG0      */
    BITS(1,18),     /* GOP2 REG1      */
    BITS(1,27),     /* GOP3 REG0      */
    BITS(1,18),     /* GOP3 REG1      */
    BITS(1,27),     /* TxQ 0          */
    BITS(1,29),     /* TxQ 1          */
    BITS(1,29),     /* TxQ 2          */
    BITS(1,22),     /* Eth Bridge     */
    0xFFFEFDFE      /* MAC Table      */
};
/****************************** END OF Salsa 24 G *****************************/


/************************************ Salsa 16 G ******************************/

/* This array maps an interrupt index to its    */
/* relevant interrupt mask reg default value,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 salsa16MaskRegDefaultArr[]=
{
    BITS(1,15),     /* GLOBAL         */
    0x00000000,     /* Miscellaneous  */
    BITS(1,8),      /* GPP            */
    BITS(1,13),     /* Buffer Mng     */
    BITS(1,27),     /* GOP0 REG0      */
    BITS(1,27),     /* GOP1 REG0      */
    0x0E79E79E,     /* GOP2 REG0      */
    0x0E000000,     /* GOP3 REG0      */
    BITS(1,18),     /* GOP0 REG1      */
    BITS(1,18),     /* GOP1 REG1      */
    0x0003E79E,     /* GOP2 REG1      */
    0x00000000,     /* GOP3 REG1      */
    BITS(1,27),     /* TxQ 0          */
    0x003F807E,     /* TxQ 1          */
    0x3A01FFFE,     /* TxQ 2          */
    BITS(1,22),     /* Eth Bridge     */
    0xFFFEFDFE      /* MAC Table      */
};
/****************************** END OF Salsa 16 G *****************************/


/************************************ Salsa 12 G ******************************/

/* This array maps an interrupt index to its    */
/* relevant interrupt mask reg default value,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 salsa12MaskRegDefaultArr[]=
{
    BITS(1,15),     /* GLOBAL         */
    0x00000000,     /* Miscellaneous  */
    BITS(1,8),      /* GPP            */
    BITS(1,13),     /* Buffer Mng     */
    BITS(1,27),     /* GOP0 REG0      */
    BITS(1,27),     /* GOP1 REG0      */
    0x0E000000,     /* GOP2 REG0      */
    0x0E000000,     /* GOP3 REG0      */
    BITS(1,18),     /* GOP0 REG1      */
    BITS(1,18),     /* GOP1 REG1      */
    0x00000000,     /* GOP2 REG1      */
    0x00000000,     /* GOP3 REG1      */
    BITS(1,27),     /* TxQ 0          */
    0x00038006,     /* TxQ 1          */
    0x3A001FFE,     /* TxQ 2          */
    BITS(1,22),     /* Eth Bridge     */
    0xFFFEFDFE      /* MAC Table      */
};
/****************************** END OF Salsa 12 G *****************************/

/****************************************************************************/
/* IMPORTANT:                                                               */
/*  The GT_XXX_AU_PROCESSED interrupt must not be masked by the ISR, so     */
/*  make sure that it's corresponding bit in maskRcvIntrEn is 0.            */
/****************************************************************************/
/****************************************************************************/
/* IMPORTANT:                                                               */
/*  In Salsa all interrupts are auto claeared -> rwBitMask can be 0         */
/****************************************************************************/

/************************************ Salsa 12/16/24 G ******************************/
static const PRV_CPSS_DRV_INTERRUPT_SCAN_STC salsaIntrScanArray[] =
{
    /* GLOBAL (PCI LIKE, OK) */
    {0, GT_FALSE,0, NULL, 0x00000030, 0x00000034,
         prvCpssDrvHwPpPortGroupIsrRead,
         prvCpssDrvHwPpPortGroupIsrWrite,
        0, 0,
        GLOBAL_NSBM, 0x0, 0xFFFE, 14, NULL, NULL},

        /* Miscellaneous    */
        {1, GT_FALSE,0, NULL, 0x00000038, 0x0000003C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_MISC_TWSI_TIME_OUT_E, PRV_CPSS_SAL_TX_CRC_ERROR_PORT_CPU_31_E,
            MISC_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* Transmit Queue */
        {3, GT_FALSE,0, NULL, 0x01800700, 0x01800704,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT0_E, PRV_CPSS_SAL_TQ_MG_READ_ERR_E,
            TQ0_NSBM, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

            /* Transmit Queue Interrupt Cause 1 */
            {26, GT_FALSE,0, NULL, 0x01800080, 0x0180070C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_SAL_TQ_WATCHDOG_EX_PORT10_E, PRV_CPSS_SAL_TQ_TXQ2_FLUSH_PORT23_E,
                TQ1_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* Transmit Queue Interrupt Cause 2 */
            {27, GT_FALSE,0, NULL, 0x01800084, 0x01800188,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_SAL_TQ_HOL_REACHED_PORT0_E, PRV_CPSS_SAL_TQ_SNIFF_DESC_DROP_E,
                TQ2_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


        /* Ethernet bridge */
        {4, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_EB_NA_FIFO_FULL_E,
                PRV_CPSS_SAL_EB_VLAN_RANGE_DROP_SECURITY_BREACH_E,
            EBRG_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


        /* buffer Management */
        {5, GT_FALSE,0, NULL, 0x03000040, 0x03000044,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_BM_EMPTY_CLEAR_E, PRV_CPSS_SAL_BM_CTRL1_PARITY_E,
            BM_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* MAC Table */
        {6, GT_FALSE,0, NULL, 0x06000018, 0x0600001C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_MAC_NUM_OF_HOP_EXP_E, PRV_CPSS_SAL_MAC_2ECC_ERRORS_ODD_E,
            MAC_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* GOP0 REG0 */
        {7, GT_FALSE,0, NULL, 0x04004000, 0x04004004,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT0_E,
            PRV_CPSS_SAL_GOP_COUNT_COPY_DONE_0_E,
            GOP0R0_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* GOP0 REG1 */
        {8, GT_FALSE,0, NULL, 0x04004008, 0x0400400C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT0_E,
            PRV_CPSS_SAL_TX_CRC_ERROR_PORT5_E,
            GOP0R1_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* GOP1 REG0 */
        {9, GT_FALSE,0, NULL, 0x04804000, 0x04804004,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT6_E,
            PRV_CPSS_SAL_GOP_COUNT_COPY_DONE_1_E,
            GOP1R0_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* GOP1 REG1 */
        {10,GT_FALSE,0, NULL, 0x04804008, 0x0480400C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT6_E,
            PRV_CPSS_SAL_TX_CRC_ERROR_PORT11_E,
            GOP1R1_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* GOP2 REG0 */
        {11,GT_FALSE,0, NULL, 0x05004000, 0x05004004,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT12_E,
            PRV_CPSS_SAL_GOP_COUNT_COPY_DONE_2_E,
            GOP2R0_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* GOP2 REG1 */
        {12,GT_FALSE,0, NULL, 0x05004008, 0x0500400C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT12_E,
            PRV_CPSS_SAL_TX_CRC_ERROR_PORT17_E,
            GOP2R1_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* GOP3 REG0 */
        {13,GT_FALSE,0, NULL, 0x05804000, 0x05804004,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_LINK_STATUS_CHANGED_PORT18_E,
            PRV_CPSS_SAL_GOP_COUNT_COPY_DONE_3_E,
            GOP3R0_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* GOP3 REG1 */
        {14,GT_FALSE,0, NULL, 0x05804008, 0x0580400C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_SYNC_STATUS_CHANGED_PORT18_E,
            PRV_CPSS_SAL_TX_CRC_ERROR_PORT23_E,
            GOP3R1_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* GPP SUM */
        {15,GT_FALSE,0, NULL, 0x00000090, 0x0000008C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_SAL_GPP_INTERRUPT1_E, PRV_CPSS_SAL_GPP_INTERRUPT8_E,
            GPP_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL}
};
/****************************** END OF Salsa 12/16/24G *****************************/





/*******************************************************************************
* prvCpssDrvPpIntDefSalsaInit
*
* DESCRIPTION:
*       Interrupts initialization for the Salsa devices.
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
void prvCpssDrvPpIntDefSalsaInit(void)
{
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devFamilyPtr;

    /* Salsa 24G info */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SALSA_24_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_SAL_LAST_INT;
    devFamilyPtr->maskRegistersAddressesArray = salsaMaskRegMapArr;
    devFamilyPtr->maskRegistersDefaultValuesArray = salsa24MaskRegDefaultArr;
    devFamilyPtr->interruptsScanArray = salsaIntrScanArray;
    devFamilyPtr->numOfScanElements = sizeof(salsaIntrScanArray)/sizeof(salsaIntrScanArray[0]);
    devFamilyPtr->fdbTrigEndedId = PRV_CPSS_SAL_MAC_AGE_VIA_TRIGGER_ENDED_E;
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_TRUE;
    devFamilyPtr->firstFakeInterruptId = PRV_CPSS_SAL_EB_AUQ_PENDING_E;/* from this interrupt Id
                                        the "fake interrupts" start */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpIntSalsaIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = 0;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=NULL;

    /* Salsa 16G info */
    /* start with same info as 24G , then override some of it */
    cpssOsMemCpy(&prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SALSA_16_E],
                 &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SALSA_24_E],
                 sizeof(PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC));

    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SALSA_16_E];
    devFamilyPtr->maskRegistersDefaultValuesArray = salsa16MaskRegDefaultArr;


    /* Salsa 12 info */
    /* start with same info as 24G , then override some of it */
    cpssOsMemCpy(&prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SALSA_12_E],
                 &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SALSA_24_E],
                 sizeof(PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC));

    devFamilyPtr->maskRegistersDefaultValuesArray = salsa12MaskRegDefaultArr;


}

