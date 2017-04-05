/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDriverTwistDInterruptDef.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsTwistD.h>

/* This array maps an interrupt index to its    */
/* relevant interrupt mask reg default value,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 twistD52MaskRegDefault[] =
{
    0x001FF9FE,     /* 0x114,      PCI              */
    0x80000048,     /* 0x02040130, Bridge. Bit31 must be set */
    0x00000980,     /* 0x028001C0, LX               */
    0x28000000,     /* 0x03020000, EPF Buffer Mng   */
    0x00000000,     /* 0x03020004, EPF Buffer Mng 1 */
    0x00000000,     /* 0x03020008, EPF Buffer Mng 2 */
    0xF83F0581,     /* 0x03800090, MAC0 GOP 0 */
    0xFFFFFF0F,     /* 0x03800098, MAC0 GOP 1 */
    0x01FFFFFF,     /* 0x038000A0, MAC0 GOP 2 */
    0x01FFFFFF,     /* 0x038000A8, MAC0 GOP 3 */
    0x01FFFFFF,     /* 0x038000B0, MAC0 GOP 4 */
    0x00000000,     /* 0x01800700, TxQ 0      */
    0x00000000,     /* 0x01800080, TxQ 1      */
    0x00000000,     /* 0x01800084, TxQ 2      */
    0x00000000,     /* 0x01800088, TxQ 3      */
    0x00000000,     /* 0x01800380, TxQ 4      */
    0x00000000,     /* 0x01800384, TxQ 5      */
    0x00000BE7,     /* 0x40,       Misc       */
    0x0007FBFD,     /* 0x280C,     Rx SDMA    */
    0x000001FF,     /* 0x2810,     Tx SDMA    */
    0x00000000,     /* 0x038000B8, MAC1 GOP5  */
    0x00000000,     /* 0x038000C0, MAC1 GOP6  */
    0x00000000,     /* 0x038000C8, MAC1 GOP7  */
    0x00000000,     /* 0x038000D0, MAC1 GOP8  */
    0x00000000,     /* 0x038000D8, MAC1 GOP9  */
    0x00000000,     /* 0x038000E0, MAC1 GOP10 */
    0x00000000      /* 0x038000E8, MAC1 GOP11 */
};


/* This array maps an interrupt index to its    */
/* relevant interrupt mask reg default value,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 twistD12MaskRegDefault[] =
{
    0x000F79FE,     /* 0x114,      PCI            */
    0x80000048,     /* 0x02040130, Bridge. Bit31 must be set */
    0x00000980,     /* 0x028001C0, LX             */
    0x28000000,     /* 0x03020000, EPF Buffer Mng */
    0xF9FFFFFF,     /* 0x03800090, MAC0           */
    0x00000000,     /* 0x01800700, TxQ 0          */
    0x00000000,     /* 0x01800080, TxQ 1          */
    0x00000000,     /* 0x01800088, TxQ 3          */
    0x00000000,     /* 0x01800384, TxQ 5          */
    0x00000BE7,     /* 0x40,       Misc           */
    0x0007FBFD,     /* 0x280C,     Rx SDMA        */
    0x000001FF,     /* 0x2810,     Tx SDMA        */
    0xFFFEFF80      /* 0x038000B8, MAC1           */
};

/****************************** END OF TWISTD 12 *****************************/


/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 twistdXgMaskRegMapArr[] =
{
    0x00000118,     /* PCI */
    0x02040134,     /* Bridge */
    0x028001C4,     /* LX */
    0x03024018,     /* EPF Buffer Mng */
    0x03800030,     /* MAC0  */
    0x01800704,     /* TxQ 0 */
    0x0180008C,     /* TxQ 3 */
    0x0180038C,     /* TxQ 5 */
    0x00000044,     /* Misc  */
    0x00002814,     /* Rx SDMA */
    0x00002818      /* Tx SDMA */
};


/* This array maps an interrupt index to its    */
/* relevant interrupt mask reg default value,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 twistdXgMaskRegDefaultArr[] =
{
    0x001FF9FE,     /* PCI              */
    0x80000008,     /* 0x02040130, Bridge. Bit31 must be set */
    0xFFFFFFFF,     /* LX               */
    0xFA405FFF,     /* EPF Buffer Mng   */
    0xFFFFFFFF,     /* MAC0             */
    0xFFFFFFFF,     /* TxQ 0            */
    0x0FFFFFFF,     /* TxQ 3            */
    0x0FFFFFFF,     /* TxQ 5            */
    0x00000BE7,     /* Misc             */
    0x0007FBFD,     /* Rx SDMA          */
    0xFFFFFFFF      /* Tx SDMA          */
};


/****************************************************************************/
/* IMPORTANT:                                                               */
/*  The GT_XXX_AU_PROCESSED interrupt must not be masked by the ISR, so     */
/*  make sure that it's corresponding bit in maskRcvIntrEn is 0.            */
/****************************************************************************/
static const PRV_CPSS_DRV_INTERRUPT_SCAN_STC twistdXgIntrScanArr[] =
{
    /* PCI Interrupt Summary Cause*/
    {0, GT_FALSE,0, NULL, 0x114, 0x118,
        prvCpssDrvHwPpPortGroupReadInternalPciReg,
        prvCpssDrvHwPpPortGroupWriteInternalPciReg,
        1, /* should be 1 to avoid scaning the summary bit
            * of the PCI main cause interrupt
            */
        10, 0x7ff, 0x7ff, 0x0, 8, NULL, NULL},
        /* Ethernet bridge  */
    {11, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_TD_XG_ETH_RES_0_E+1, PRV_CPSS_TD_XG_ETH_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFF7, 0,NULL, NULL},
        /* Lx               */
    {12, GT_FALSE,0, NULL, 0x028001C0, 0x028001C4,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_TD_XG_LX_RES_0_E+1, PRV_CPSS_TD_XG_LX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},
        /* EPF buffer Management*/
    {13, GT_FALSE,0, NULL, 0x03020000, 0x03024018,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_TD_XG_BM_RES_0_E+1, PRV_CPSS_TD_XG_BM_RES_0_E + 31, 0x1FFFFFFF, 0x0,
        0x1FFFFFFF, 1,NULL, NULL},
            /* UPLINK GPP       */
            {29, GT_TRUE, CPSS_EVENT_GPP_UPLINK_E, prvCpssDrvDxExMxGppIsrCall, 0x00000000,
                0x00000000,NULL, NULL, 0, 0, 0xFFFFFFFF, 0x0, 0x0, 0,NULL,NULL},

        /* MAC0             */
    {14, GT_FALSE,0, NULL, 0x0380002C, 0x03800030,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_TD_XG_MAC0_RES_0_E+1, PRV_CPSS_TD_XG_MAC0_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 3,NULL, NULL},
                /* GPP0             */
            {27, GT_TRUE,CPSS_EVENT_GPP_INTERNAL_1_E, prvCpssDrvDxExMxGppIsrCall, 0x00000000, 0x00000000,
                NULL, NULL, 0, 0, 0xFFFFFFFF, 0x0, 0x0, 0, NULL, NULL},
            /* GPP1             */
            {28, GT_TRUE,CPSS_EVENT_GPP_INTERNAL_2_E, prvCpssDrvDxExMxGppIsrCall, 0x00000000, 0x00000000,
                NULL, NULL, 0, 0, 0xFFFFFFFF, 0x0, 0x0, 0, NULL, NULL},
            /* GPP2             */
            {29, GT_TRUE,CPSS_EVENT_GPP_INTERNAL_3_E, prvCpssDrvDxExMxGppIsrCall, 0x00000000, 0x00000000,
                NULL, NULL, 0, 0, 0xFFFFFFFF, 0x0, 0x0, 0, NULL, NULL},

         /* Transmit Queue   */
    {15, GT_FALSE,0, NULL, 0x01800700, 0x01800704,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_TD_XG_TXQ_RES_0_E+1, PRV_CPSS_TD_XG_TXQ_RES_0_E + 31, 0xABFFFFFF, 0x0,
        0xABFFFFFF, 2,NULL, NULL},
            /* Transmit Queue Interrupt Cause 3 */
            {28, GT_FALSE,0, NULL, 0x01800088, 0x0180008C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_TD_XG_TXQ3_RES_0_E + 1, PRV_CPSS_TD_XG_TXQ3_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0x0, 0, NULL, NULL},
            /* Transmit Queue Interrupt Cause 5 */
            {30, GT_FALSE,0, NULL, 0x01800384, 0x0180038C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_TD_XG_TXQ5_RES_0_E + 1, PRV_CPSS_TD_XG_TXQ5_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0x0, 0, NULL, NULL},

        /* Miscellaneous    */
    {16, GT_FALSE,0, NULL, 0x40, 0x44,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_TD_XG_MISC_RES_0_E+1, PRV_CPSS_TD_XG_MISC_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFDF, 0,NULL, NULL},

        /* Rx SDMA          */
    {17, GT_FALSE,0, NULL, 0x280C, 0x2814,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_TD_XG_RX_RES_0_E+1, PRV_CPSS_TD_XG_RX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},

        /* Tx SDMA          */
    {18, GT_FALSE,0, NULL, 0x2810, 0x2818,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_TD_XG_TX_RES_0_E+1, PRV_CPSS_TD_XG_TX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL}

};

/* array of interrupt registers that can't be accessed before 'Start Init' */
static GT_U32   notAccessibleBeforeStartInitArray[] =
{
    0x02040134 /*Bridge Interrupt Mask*/
};

static GT_U32 numOfInterruptRegistersNotAccessibleBeforeStartInit =
                (sizeof(notAccessibleBeforeStartInitArray)/
                sizeof(notAccessibleBeforeStartInitArray[0]));

/****************************** END OF TWIST XG *****************************/
/*******************************************************************************
* prvCpssDrvPpIntDefTigerInit
*
* DESCRIPTION:
*       Interrupts initialization for the Tiger devices.
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
void prvCpssDrvPpIntDefTwistDInit(void)
{
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devFamilyPtr;

    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TWISTD_XG_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_TD_XG_LAST_INT;
    devFamilyPtr->maskRegistersAddressesArray = twistdXgMaskRegMapArr;
    devFamilyPtr->maskRegistersDefaultValuesArray = twistdXgMaskRegDefaultArr;
    devFamilyPtr->interruptsScanArray = twistdXgIntrScanArr;
    devFamilyPtr->numOfScanElements = sizeof(twistdXgIntrScanArr)/sizeof(twistdXgIntrScanArr[0]);
    devFamilyPtr->fdbTrigEndedId = PRV_CPSS_TD_XG_MAC_AGE_VIA_TRIGGER_ENDED_E;
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpIntExMxIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = numOfInterruptRegistersNotAccessibleBeforeStartInit;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=notAccessibleBeforeStartInitArray;

    /* Some of the info for the Twist-D 12,52 are like Twist-C */
    prvCpssDrvPpIntDefTwistCInit(GT_TRUE);

    /* overRide by the specific TwistD info */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TWISTD_12_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_TD_12_LAST_INT;
    devFamilyPtr->maskRegistersDefaultValuesArray = twistD12MaskRegDefault;
    devFamilyPtr->fdbTrigEndedId = PRV_CPSS_TD_12_MAC_AGE_VIA_TRIGGER_ENDED_E;
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpIntExMxIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = numOfInterruptRegistersNotAccessibleBeforeStartInit;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=notAccessibleBeforeStartInitArray;

    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TWISTD_52_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_TD_52_LAST_INT;
    devFamilyPtr->maskRegistersDefaultValuesArray = twistD52MaskRegDefault;
    devFamilyPtr->fdbTrigEndedId = PRV_CPSS_TD_52_MAC_AGE_VIA_TRIGGER_ENDED_E;
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpIntExMxIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = numOfInterruptRegistersNotAccessibleBeforeStartInit;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=notAccessibleBeforeStartInitArray;

}
