/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefTiger.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables.
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
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsTiger.h>

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 tiger52MaskRegMapArr[] =
{
    0x00000118,     /* PCI */
    0x02040134,     /* Bridge */
    0x028001C4,     /* LX */
    0x03024018,     /* EPF Buffer Mng */
    0x0302401C,     /* EPF Buffer Mng 1 */
    0x03024020,     /* EPF Buffer Mng 2 */
    0x03800094,     /* MAC0 GOP 0 */
    0x0380009C,     /* MAC0 GOP 1 */
    0x038000A4,     /* MAC0 GOP 2 */
    0x038000AC,     /* MAC0 GOP 3 */
    0x038000B4,     /* MAC0 GOP 4 */
    0x01800704,     /* TxQ 0 */
    0x0180070C,     /* TxQ 1 */
    0x01800188,     /* TxQ 2 */
    0x0180008C,     /* TxQ 3 */
    0x01800388,     /* TxQ 4 */
    0x0180038C,     /* TxQ 5 */
    0x00000044,     /* Misc  */
    0x00002814,     /* Rx SDMA */
    0x00002818,     /* Tx SDMA */
    0x038000BC,     /* MAC1 GOP5 */
    0x038000C4,     /* MAC1 GOP6 */
    0x038000CC,     /* MAC1 GOP7 */
    0x038000D4,     /* MAC1 GOP8 */
    0x038000DC,     /* MAC1 GOP9 */
    0x038000E4,     /* MAC1 GOP10 */
    0x038000EC,     /* MAC1 GOP11 */
    0x0180068C,     /* Txq ExRmon low Shadow*/
    0x01800688,     /* Txq ExRmon high Shadow */
    0x0180058C,     /* Txq ExRmon low Total smaples*/
    0x01800588,     /* Txq ExRmon High Total smaples*/
    0x03024030,     /* Epf ExRmon low Shadow*/
    0x03024034,     /* Epf ExRmon high Shadow */
    0x03024038,     /* Epf ExRmon low Total smaples*/
    0x0302403C      /* Epf ExRmon High Total smaples*/
};

/* This array maps an interrupt index to its    */
/* relevant interrupt mask reg default value,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 tiger52MaskRegDefaultArr[] =
{
    0xFFFFFFFE,     /* 0xFFFFFFFF,PCI */
    0xFFFFFFFF,     /* 0xFFFFFFFF,Bridge */
    0xF1FE0C03,     /* 0xC1F80C03,LX*/
    0xFFFFFFFF,     /* 0xFFFFFFFF,EPF Buffer Mng */
    0xFFFFFFFF,     /* 0xFFFFFFFF,EPF Buffer Mng 1 */
    0xFFFFFFFF,     /* 0xFFFFFFFF,EPF Buffer Mng 2 */
    0xF83E0580,     /* 0xFFFFFFFF,MAC0 GOP 0 */
    0xFFFFFFFF,     /* 0xFFFFFFFF,MAC0 GOP 1 */
    0x01FFFFFE,     /* 0xFFFFFFFF,MAC0 GOP 2 */
    0x01FFFFFE,     /* 0xFFFFFFFF,MAC0 GOP 3 */
    0x01FFFFFE,     /* 0xFFFFFFFF,MAC0 GOP 4 */
    0xFFFFFFFF,     /* 0xFFFFFFFF,TxQ 0 */
    0xFFFFFFFF,     /* 0xFFFFFFFF,TxQ 1 */
    0xFFFFFFFF,     /* 0xFFFFFFFF,TxQ 2 */
    0xFFFFFFFF,     /* 0xFFFFFFFF,TxQ 3 */
    0xFFFFFFFF,     /* 0xFFFFFFFF,TxQ 4 */
    0xFFFFFFFF,     /* 0xFFFFFFFF,TxQ 5 */
    0xFFFFFFFF,     /* 0xFFFFFFFF,Misc  */
    0xFFFFFFFF,     /* 0xFFFFFFFF,Rx SDMA */
    0xFFFFFFFF,     /* 0xFFFFFFFF,Tx SDMA */
    0x00000000,     /* 0xFFFFFFFF,MAC1 GOP5 */
    0x00000000,     /* 0xFFFFFFFF,MAC1 GOP6 */
    0x00000000,     /* 0xFFFFFFFF,MAC1 GOP7 */
    0x00000000,     /* 0xFFFFFFFF,MAC1 GOP8 */
    0x00000000,     /* 0xFFFFFFFF,MAC1 GOP9 */
    0x00000000,     /* 0xFFFFFFFF,MAC1 GOP10 */
    0x00000000,      /* 0xFFFFFFFF MAC1 GOP11 */
    0xFFFFFFFE,     /* Txq ExRmon low Shadow*/
    0x1C3FFFFE,     /* Txq ExRmon high Shadow */
    0xFFFFFFFE,     /* Txq ExRmon low Total smaples*/
    0x003FFFFE,     /* Txq ExRmon High Total smaples*/
    0xFFFFFFFE,     /* Epf ExRmon low Shadow*/
    0x003FFFFE,     /* Epf ExRmon high Shadow */
    0xFFFFFFFE,     /* Epf ExRmon low Total smaples*/
    0x003FFFFE      /* Epf ExRmon High Total smaples*/
};


/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 tiger12MaskRegMapArr[] =
{
    0x00000118,     /* PCI */
    0x02040134,     /* Bridge */
    0x028001C4,     /* LX */
    0x03024018,     /* EPF Buffer Mng */
    0x03024020,     /* EPF Buffer Mng 2 */
    0x03800094,     /* MAC0  */
    0x01800704,     /* TxQ 0 */
    0x0180070C,     /* TxQ 1 */
    0x0180008C,     /* TxQ 3 */
    0x0180038C,     /* TxQ 5 */
    0x00000044,     /* Misc  */
    0x00002814,     /* Rx SDMA */
    0x00002818,     /* Tx SDMA */
    0x038000BC,      /* MAC1  */
    0x0180068C,     /* Txq ExRmon low Shadow*/
    0x01800688,     /* Txq ExRmon high Shadow */
    0x0180058C,     /* Txq ExRmon low Total smaples*/
    0x01800588,     /* Txq ExRmon High Total smaples*/
    0x03024030,     /* Epf ExRmon low Shadow*/
    0x03024034,     /* Epf ExRmon high Shadow */
    0x03024038,     /* Epf ExRmon low Total smaples*/
    0x0302403C      /* Epf ExRmon High Total smaples*/
};

/* This array maps an interrupt index to its    */
/* relevant interrupt mask reg default value,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 tiger12MaskRegDefaultArr[] =
{
    0x001FF9FE,     /* PCI              */
    0xFFFFFFFF,     /* Bridge           */
    0xC1F80C03,     /* 0xC1F80C03, LX*/
    0xFA405FFF,     /* EPF Buffer Mng   */
    0xFFFFFFFF,     /* EPF Buffer Mng 2 */
    0xFFFFFFFF,     /* MAC0             */
    0xFFFFFFFF,     /* TxQ 0            */
    0x0FFFFFFF,     /* TxQ 1            */
    0x0FFFFFFF,     /* TxQ 3            */
    0x0FFFFFFF,     /* TxQ 5            */
    0x00000BE7,     /* Misc             */
    0xFFFFFFFF,     /* Rx SDMA          */
    0xFFFFFFFF,     /* Tx SDMA          */
    0xFFFeFF80,     /*0xFFFFFF80*/      /* MAC1             */
    0xFFFFFFFE,     /* Txq ExRmon low Shadow*/
    0x003FFFFE,     /* Txq ExRmon high Shadow */
    0xFFFFFFFE,     /* Txq ExRmon low Total smaples*/
    0x003FFFFE,     /* Txq ExRmon High Total smaples*/
    0xFFFFFFFE,     /* Epf ExRmon low Shadow*/
    0x003FFFFE,     /* Epf ExRmon high Shadow */
    0xFFFFFFFE,     /* Epf ExRmon low Total smaples*/
    0x003FFFFE      /* Epf ExRmon High Total smaples*/
};


/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 tigerXgMaskRegMapArr[] =
{
    0x00000118,     /* PCI */
    0x02040134,     /* Bridge */
    0x028001C4,     /* LX */
    0x03024018,     /* EPF Buffer Mng */
    0x03024020,     /* EPF Buffer Mng 2 */
    0x03800030,     /* MAC0  */
    0x01800704,     /* TxQ 0 */
    0x0180070C,     /* TxQ 1 */
    0x0180008C,     /* TxQ 3 */
    0x01800388,     /* TxQ 4 */
    0x0180038C,     /* TxQ 5 */
    0x00000044,     /* Misc  */
    0x00002814,     /* Rx SDMA */
    0x00002818,     /* Tx SDMA */
    0x038000BC,      /* MAC1  */
    0x0180068C,     /* Txq ExRmon low Shadow*/
    0x01800688,     /* Txq ExRmon high Shadow */
    0x0180058C,     /* Txq ExRmon low Total smaples*/
    0x01800588,     /* Txq ExRmon High Total smaples*/
    0x03024030,     /* Epf ExRmon low Shadow*/
    0x03024034,     /* Epf ExRmon high Shadow */
    0x03024038,     /* Epf ExRmon low Total smaples*/
    0x0302403C      /* Epf ExRmon High Total smaples*/
};


/* This array maps an interrupt index to its    */
/* relevant interrupt mask reg default value,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 tigerXgMaskRegDefaultArr[] =
{
    0x001FF9FE,   /* PCI */
    0xFFFFFFFF,   /* Bridge */
    0xF9FF0F83,   /* LX */
    0xFFFFFFFF,   /* EPF Buffer Mng */
    0xFFFFFFFF,   /* EPF Buffer Mng 2 */
    0xFFFFFFFF,   /* MAC0  */
    0xFFFFFFFF,   /* TxQ 0 */
    0x0FFFFFFF,   /* TxQ 1 */
    0xFFFFFFFF,   /* TxQ 3 */
    0x0FFFFFFF,   /* TxQ 4 */
    0x0FFFFFFF,   /* TxQ 5 */
    0x01FF0FEF,   /* Misc  */
    0xFFFFFFFF,   /* Rx SDMA */
    0xFFFFFFFF,   /* Tx SDMA */
    0xFFFeFF80,   /* MAC1  */
    0xFFFFFFFE,   /* Txq ExRmon low Shadow*/
    0x1C3FFFFE,   /* Txq ExRmon high Shadow */
    0xFFFFFFFE,   /* Txq ExRmon low Total smaples*/
    0x003FFFFE,   /* Txq ExRmon High Total smaples*/
    0xFFFFFFFE,   /* Epf ExRmon low Shadow*/
    0x003FFFFE,   /* Epf ExRmon high Shadow */
    0xFFFFFFFE,   /* Epf ExRmon low Total smaples*/
    0x003FFFFE  /* Epf ExRmon High Total smaples*/
};


/****************************************************************************/
/* IMPORTANT:                                                               */
/*  The GT_XXX_AU_PROCESSED interrupt must not be masked by the ISR, so     */
/*  make sure that it's corresponding bit in maskRcvIntrEn is 0.            */
/****************************************************************************/
static const PRV_CPSS_DRV_INTERRUPT_SCAN_STC tiger52IntrScanArr[] =
{
    /* PCI Interrupt Summary Cause*/
    {0, GT_FALSE,0, NULL, 0x114, 0x118,
        prvCpssDrvHwPpPortGroupReadInternalPciReg,
        prvCpssDrvHwPpPortGroupWriteInternalPciReg,
        1, 10, 0x7ff, 0x7ff, 0x0, 10, NULL, NULL},
        /* Ethernet bridge  */
    {11, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_52_ETH_RES_0_E+1,PRV_CPSS_TG_52_ETH_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFF7, 0,NULL, NULL},
        /* Lx               */
    {12, GT_FALSE,0, NULL, 0x028001C0, 0x028001C4,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_52_LX_RES_0_E+1,PRV_CPSS_TG_52_LX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},

        /* EPF buffer Management */
    {13, GT_FALSE,0, NULL, 0x03020000, 0x03024018,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_52_BM_RES_0_E+1,PRV_CPSS_TG_52_BM_RES_0_E + 31, 0x1FFFFFFF, 0x0,
        0x1FFFFFFF, 3,NULL, NULL},
            /*
                UPLINK GPP  See FEr#9i fixes the UPLINK is no more GPP
                Therefore we have to clear the interrupt in the FA/Crossbar.
            */
            /* UPLINK GPP       */
            {29, GT_TRUE, CPSS_EVENT_GPP_UPLINK_E, prvCpssDrvDxExMxGppIsrCall, 0x00000000,
                0x00000000,NULL, NULL, 0, 0, 0xFFFFFFFF, 0x0, 0x0, 0,NULL,NULL},
            /* EPF Buffer Management Interrupt Cause 1 */
            {30, GT_FALSE,0, NULL, 0x03020004, 0x0302401C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_BM1_RES_0_E + 1,PRV_CPSS_TG_52_BM1_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* EPF Buffer Management Interrupt Cause 2 */
            {31, GT_FALSE,0, NULL, 0x03020008, 0x03024020,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_BM2_RES_0_E + 1,PRV_CPSS_TG_52_BM2_RES_0_E + 31, 0x0FFFFFFF,
                0x0, 0x0FFFFFFF, 4, NULL, NULL},

                /* ExRMON Shadow Register Empty Interrupt Low ports 0-30 */
                {28, GT_FALSE,0, NULL, 0x03020010, 0x03024030,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_52_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 1,
                   PRV_CPSS_TG_52_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 31,
                    0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL} ,

                /* ExRMON Shadow Register Empty Interrupt High ports 31- 51 */
                {29, GT_FALSE,0, NULL, 0x03020014, 0x03024034,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_52_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E + 1,
                   PRV_CPSS_TG_52_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E + 21,
                    0x003FFFFE, 0x0, 0x003FFFFE, 0,NULL, NULL},

                /* ExRMON Total Samples Full Interrupt Low - ports 0-30 */
                {30, GT_FALSE,0, NULL, 0x03020018, 0x03024038,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_52_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 1,
                   PRV_CPSS_TG_52_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 31,
                    0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL} ,

                /* ExRMON Total Samples Full Interrupt high - ports 31-51 */
                {31, GT_FALSE,0, NULL, 0x0302001C, 0x0302403C,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_52_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E + 1,
                   PRV_CPSS_TG_52_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E + 21,
                    0x003FFFFE, 0x0, 0x003FFFFE, 0,NULL, NULL},

        /* MACSumInt0             */
    {14, GT_FALSE,0, NULL, 0x03800090, 0x03800094,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_52_MAC0_RES_0_E+1,PRV_CPSS_TG_52_MAC0_RES_0_E + 31, 0xC7FFFFFF, 0x0,
        0xC7FFFFFF, 3,NULL, NULL},
            /* GPP0             */
            {27, GT_TRUE, CPSS_EVENT_GPP_INTERNAL_1_E,prvCpssDrvDxExMxGppIsrCall, 0x00000000, 0x00000000,
                NULL, NULL, 0, 0, 0xFFFFFFFF, 0x0, 0x0, 0, NULL, NULL},
            /* GPP1             */
            {28, GT_TRUE, CPSS_EVENT_GPP_INTERNAL_2_E,prvCpssDrvDxExMxGppIsrCall, 0x00000000, 0x00000000,
                NULL, NULL,0, 0, 0xFFFFFFFF, 0x0, 0x0, 0, NULL, NULL},
            /* GPP2             */
            {29, GT_TRUE, CPSS_EVENT_GPP_INTERNAL_3_E,prvCpssDrvDxExMxGppIsrCall, 0x00000000, 0x00000000,
                NULL, NULL,0, 0, 0xFFFFFFFF, 0x0, 0x0, 0, NULL, NULL},


        /* Transmit Queue   */
    {15, GT_FALSE,0, NULL, 0x01800700, 0x01800704,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_52_TXQ_RES_0_E+1,PRV_CPSS_TG_52_TXQ_RES_0_E + 31, 0x03FFFFFF, 0x0,
        0x03FFFFFF, 7,NULL, NULL},
            /* Transmit Queue Interrupt Cause 1 */
            {26, GT_FALSE,0, NULL, 0x01800080, 0x0180070C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_TXQ1_RES_0_E + 1,PRV_CPSS_TG_52_TXQ1_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* Transmit Queue Interrupt Cause 2 */
            {27, GT_FALSE,0, NULL, 0x01800084, 0x01800188,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_TXQ2_RES_0_E + 1,PRV_CPSS_TG_52_TXQ2_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* Transmit Queue Interrupt Cause 3 */
            {28, GT_FALSE,0, NULL, 0x01800088, 0x0180008C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_TXQ3_RES_0_E + 1,PRV_CPSS_TG_52_TXQ3_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* Transmit Queue Interrupt Cause 4 */
            {29, GT_FALSE,0, NULL, 0x01800380, 0x01800388,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_TXQ4_RES_0_E + 1,PRV_CPSS_TG_52_TXQ4_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* Transmit Queue Interrupt Cause 5 */
            {30, GT_FALSE,0, NULL, 0x01800384, 0x0180038C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_TXQ5_RES_0_E + 1,PRV_CPSS_TG_52_TXQ5_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* Transmit Q caus sum of Summary bit of Int Cause reg :
            It is the summary of the couse bits of the
            ExRMON shadow register empty interrupt high 1 - 21
            and the summary of the summary bits 26 - 28
            */
            {31, GT_FALSE,0, NULL, 0x01800680, 0x01800688,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E + 1,
               PRV_CPSS_TG_52_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E + 21, 0x003FFFFE,
                0x0, 0x003FFFFE, 0,NULL, NULL} ,

            {31, GT_FALSE,0, NULL, 0x01800680, 0x01800688,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E + 26,
               PRV_CPSS_TG_52_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E + 28, 0x003FFFFE,
                0x0, 0x003FFFFE, 3,NULL, NULL} ,

                /* TxQ ExRMON Shadow Register Empty Interrupt Low ports 0-30 */
                {26, GT_FALSE,0, NULL, 0x01800684, 0x0180068C,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_52_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 1,
                   PRV_CPSS_TG_52_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 31,
                    0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL},

                /* TxQ ExRMON Total Samples Full Interrupt Low - ports 0-30 */
                {27, GT_FALSE,0, NULL, 0x01800584, 0x0180058C,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_52_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 1,
                   PRV_CPSS_TG_52_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 31,
                    0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL} ,

                /* TxQ ExRMON Total Samples Full Interrupt high - ports 31-51 */
                {28, GT_FALSE,0, NULL, 0x01800580, 0x01800588,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_52_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E + 1,
                   PRV_CPSS_TG_52_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E + 21,
                    0x003FFFFE, 0x0, 0x003FFFFE, 0,NULL, NULL} ,

        /* Miscellaneous    */
    {16, GT_FALSE,0, NULL, 0x40, 0x44,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_52_MISC_RES_0_E+1,PRV_CPSS_TG_52_MISC_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFDF, 0,NULL, NULL},

        /* Rx SDMA          */
    {17, GT_FALSE,0, NULL, 0x280C, 0x2814,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_52_RX_RES_0_E+1,PRV_CPSS_TG_52_RX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},

        /* Tx SDMA          */
    {18, GT_FALSE,0, NULL, 0x2810, 0x2818,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_52_TX_RES_0_E+1,PRV_CPSS_TG_52_TX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},

        /* MAC1             */
    {19, GT_FALSE,0, NULL, 0x038000B8, 0x038000BC,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_52_MAC1_RES_0_E+1,PRV_CPSS_TG_52_MAC1_RES_0_E + 31, 0xFFFFFF81, 0x0,
        0xFFFFFF81, 6,NULL, NULL},
            /* GOP Interrupt Cause Register 6 */
            {1, GT_FALSE,0, NULL, 0x038000C0, 0x038000C4,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_GOP6_RES_0_E+1,PRV_CPSS_TG_52_GOP6_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* GOP Interrupt Cause Register 7 */
            {2, GT_FALSE,0, NULL, 0x038000C8, 0x038000CC,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_GOP7_RES_0_E+1,PRV_CPSS_TG_52_GOP7_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* GOP Interrupt Cause Register 8 */
            {3, GT_FALSE,0, NULL, 0x038000D0, 0x038000D4,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_GOP8_RES_0_E+1,PRV_CPSS_TG_52_GOP8_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* GOP Interrupt Cause Register 9 */
            {4, GT_FALSE,0, NULL, 0x038000D8, 0x038000DC,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_GOP9_RES_0_E+1,PRV_CPSS_TG_52_GOP9_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0x0, 0, NULL, NULL},
            /* GOP Interrupt Cause Register 10 */
            {5, GT_FALSE,0, NULL, 0x038000E0, 0x038000E4,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_GOP10_RES_0_E+1,PRV_CPSS_TG_52_GOP10_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* GOP Interrupt Cause Register 11 */
            {6, GT_FALSE,0, NULL, 0x038000E8, 0x038000EC,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_GOP11_RES_0_E+1,PRV_CPSS_TG_52_GOP11_RES_0_E + 31, 0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* MAC2  GOP 1      */
    {20, GT_FALSE,0, NULL, 0x03800098, 0x0380009C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_52_MAC2_GOP1_RES_0_E+1,PRV_CPSS_TG_52_MAC2_GOP1_RES_0_E + 31, 0xFFFFFFF1, 0x0,
        0xFFFFFFF1, 3, NULL, NULL},
            /* GOP Interrupt Cause Register 2 */
            {1, GT_FALSE,0, NULL, 0x038000A0, 0x038000A4,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_MAC2_GOP2_RES_0_E+1,PRV_CPSS_TG_52_MAC2_GOP2_RES_0_E + 31,
                0xFFFFFFFF, 0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* GOP Interrupt Cause Register 3 */
            {2, GT_FALSE,0, NULL, 0x038000A8, 0x038000AC,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_MAC2_GOP3_RES_0_E+1,PRV_CPSS_TG_52_MAC2_GOP3_RES_0_E + 31,
                0xFFFFFFFF, 0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* GOP Interrupt Cause Register 4 */
            {3, GT_FALSE,0, NULL, 0x038000B0, 0x038000B4,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_52_MAC2_GOP4_RES_0_E+1,PRV_CPSS_TG_52_MAC2_GOP4_RES_0_E + 31,
                0xFFFFFFFF, 0x0, 0xFFFFFFFF, 0, NULL, NULL}
};


/****************************** END OF TIGER 48+4 *****************************/

/****************************************************************************/
/* IMPORTANT:                                                               */
/*  The GT_XXX_AU_PROCESSED interrupt must not be masked by the ISR, so     */
/*  make sure that it's corresponding bit in maskRcvIntrEn is 0.            */
/****************************************************************************/
static const PRV_CPSS_DRV_INTERRUPT_SCAN_STC tiger12IntrScanArr[] =
{
    /* PCI Interrupt Summary Cause*/
    {0, GT_FALSE,0, NULL, 0x114, 0x118,
        prvCpssDrvHwPpPortGroupReadInternalPciReg,
        prvCpssDrvHwPpPortGroupWriteInternalPciReg,
        1, 10, 0x7ff, 0x7ff, 0x0, 9, NULL, NULL},
        /* Ethernet bridge  */
    {11, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_12_ETH_RES_0_E+1,PRV_CPSS_TG_12_ETH_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFF7, 0,NULL, NULL},
        /* Lx               */
    {12, GT_FALSE,0, NULL, 0x028001C0, 0x028001C4,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_12_LX_RES_0_E+1,PRV_CPSS_TG_12_LX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},
        /* EPF buffer Management*/

    {13, GT_FALSE,0, NULL, 0x03020000, 0x03024018,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_12_BM_RES_0_E+1,PRV_CPSS_TG_12_BM_RES_0_E + 31, 0x1FFFFFFF, 0x0,
        0x1FFFFFFF,0x2,NULL, NULL},
            /*
                UPLINK GPP  See FEr#9i fixes the UPLINK is no more GPP
                Therefore we have to clear the interrupt in the FA/Crossbar.
            */
            /* UPLINK GPP       */
            {29, GT_TRUE, CPSS_EVENT_GPP_UPLINK_E, prvCpssDrvDxExMxGppIsrCall, 0x00000000,
                0x00000000,NULL, NULL, 0, 0, 0xFFFFFFFF, 0x0, 0x0, 0,NULL,NULL},

            /* EPF Buffer Management Interrupt Cause 2 not used in tiger 12G*/

            /* EPF Buffer Management Interrupt Cause 2 */
            {31, GT_FALSE,0, NULL, 0x03020008, 0x03024020,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_12_BM2_RES_0_E + 1,PRV_CPSS_TG_12_BM2_RES_0_E + 31, 0x0FFFFFFF,
                0x0, 0x0FFFFFFF, 4, NULL, NULL},

                /* ExRMON Shadow Register Empty Interrupt Low ports 0-30 */
                {28, GT_FALSE,0, NULL, 0x03020010, 0x03024030,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_12_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 1,
                   PRV_CPSS_TG_12_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 31,
                    0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL} ,

                /* ExRMON Shadow Register Empty Interrupt High ports 31- 51 */
                {29, GT_FALSE,0, NULL, 0x03020014, 0x03024034,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_12_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E+ 1,
                   PRV_CPSS_TG_12_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E+ 21,
                    0x003FFFFE, 0x0, 0x003FFFFE, 0,NULL, NULL},

                /* ExRMON Total Samples Full Interrupt Low - ports 0-30 */
                {30, GT_FALSE,0, NULL, 0x03020018, 0x03024038,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_12_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 1,
                   PRV_CPSS_TG_12_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 31,
                    0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL} ,

                /* ExRMON Total Samples Full Interrupt high - ports 31-51 */
                {31, GT_FALSE,0, NULL, 0x0302001C, 0x0302403C,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_12_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E+ 1,
                   PRV_CPSS_TG_12_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E+ 21,
                    0x003FFFFE, 0x0, 0x003FFFFE, 0,NULL, NULL},

    /* MACSumInt0             */
    {14, GT_FALSE,0, NULL, 0x03800090, 0x03800094,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_12_MAC0_RES_0_E+1,PRV_CPSS_TG_12_MAC0_RES_0_E + 31, 0xC7FFFFFF, 0x0,
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
       PRV_CPSS_TG_12_TXQ_RES_0_E+1,PRV_CPSS_TG_12_TXQ_RES_0_E + 31, 0xABFFFFFF, 0x0,
        0xABFFFFFF, 5,NULL, NULL},
            /* Transmit Queue Interrupt Cause 1 */
            {26, GT_FALSE,0, NULL, 0x01800080, 0x0180070C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_12_TXQ1_RES_0_E + 1,PRV_CPSS_TG_12_TXQ1_RES_0_E + 31,0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* Transmit Queue Interrupt Cause 3 */
            {28, GT_FALSE,0, NULL, 0x01800088, 0x0180008C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_12_TXQ3_RES_0_E + 1,PRV_CPSS_TG_12_TXQ3_RES_0_E + 31,0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /* Transmit Queue Interrupt Cause 5 */
            {30, GT_FALSE,0, NULL, 0x01800384, 0x0180038C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_12_TXQ5_RES_0_E + 1,PRV_CPSS_TG_12_TXQ5_RES_0_E + 31,0xFFFFFFFF,
                0x0, 0xFFFFFFFF, 0, NULL, NULL},
            /*
            It is the summary of the couse bits of the
            ExRMON shadow register empty interrupt high 1 - 21
            and the summary of the summary bits 26 - 28
            */
            {31, GT_FALSE,0, NULL, 0x01800680, 0x01800688,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_12_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E+ 1,
               PRV_CPSS_TG_12_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E+ 21, 0x003FFFFE,
                0x0, 0x003FFFFE, 0,NULL, NULL} ,

            {31, GT_FALSE,0, NULL, 0x01800680, 0x01800688,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_12_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E+ 26,
               PRV_CPSS_TG_12_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E+ 28, 0x003FFFFE,
                0x0, 0x003FFFFE, 3,NULL, NULL} ,

                /* TxQ ExRMON Shadow Register Empty Interrupt Low ports 0-30 */
                {26, GT_FALSE,0, NULL, 0x01800684, 0x0180068C,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_12_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 1,
                   PRV_CPSS_TG_12_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 31,
                    0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL},

                /* TxQ ExRMON Total Samples Full Interrupt Low - ports 0-30 */
                {27, GT_FALSE,0, NULL, 0x01800584, 0x0180058C,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_12_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 1,
                   PRV_CPSS_TG_12_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 31,
                    0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL} ,

                /* TxQ ExRMON Total Samples Full Interrupt high - ports 31-51 */
                {28, GT_FALSE,0, NULL, 0x01800580, 0x01800588,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_12_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E+ 1,
                   PRV_CPSS_TG_12_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E+ 21,
                    0x003FFFFE, 0x0, 0x003FFFFE, 0,NULL, NULL} ,

        /* Miscellaneous    */
    {16, GT_FALSE,0, NULL, 0x40, 0x44,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_12_MISC_RES_0_E+1,PRV_CPSS_TG_12_MISC_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFDF, 0,NULL, NULL},

        /* Rx SDMA          */
    {17, GT_FALSE,0, NULL, 0x280C, 0x2814,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_12_RX_RES_0_E+1,PRV_CPSS_TG_12_RX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},

        /* Tx SDMA          */
    {18, GT_FALSE,0, NULL, 0x2810, 0x2818,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_12_TX_RES_0_E+1,PRV_CPSS_TG_12_TX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},

        /* MAC1             */
    {19, GT_FALSE,0, NULL, 0x038000B8, 0x038000BC,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_12_MAC1_RES_0_E+1,PRV_CPSS_TG_12_MAC1_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL}
};

/****************************** END OF TIGER 12 G *****************************/


/************************************ tiger XG ******************************/

/****************************************************************************/
/* IMPORTANT:                                                               */
/*  The GT_XXX_AU_PROCESSED interrupt must not be masked by the ISR, so     */
/*  make sure that it's corresponding bit in maskRcvIntrEn is 0.            */
/****************************************************************************/
static const PRV_CPSS_DRV_INTERRUPT_SCAN_STC tigerXgIntrScanArr[] =
{
    /* PCI Interrupt Summary Cause*/
    {0, GT_FALSE,0, NULL, 0x114, 0x118,
        prvCpssDrvHwPpPortGroupReadInternalPciReg,
        prvCpssDrvHwPpPortGroupWriteInternalPciReg,
        1, /* should be 1 to avoid scaning the summary bit
            * of the PCI main cause interrupt
            */
        10, 0x7ff, 0x7ff, 0x0, 9, NULL, NULL},
        /* Ethernet bridge  */
    {11, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_ETH_RES_0_E+1,PRV_CPSS_TG_XG_ETH_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},
        /* Lx               */
    {12, GT_FALSE,0, NULL, 0x028001C0, 0x028001C4,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_LX_RES_0_E+1,PRV_CPSS_TG_XG_LX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},
        /* EPF buffer Management*/
    {13, GT_FALSE,0, NULL, 0x03020000, 0x03024018,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_BM_RES_0_E+1,PRV_CPSS_TG_XG_BM_RES_0_E + 31, 0x1FFFFFFF, 0x0,
        0x1FFFFFFF, 0x2,NULL, NULL},
        /*
            UPLINK GPP  See FEr#9i fixes the UPLINK is no more GPP
            Therefore we have to clear the interrupt in the FA/Crossbar.
        */
        /* UPLINK GPP       */
            {29, GT_TRUE, CPSS_EVENT_GPP_UPLINK_E, prvCpssDrvDxExMxGppIsrCall, 0x00000000,
                0x00000000,NULL, NULL, 0, 0, 0xFFFFFFFF, 0x0, 0x0, 0,NULL,NULL},

            /* EPF Buffer Management Interrupt Cause 2 not used in tiger 12G*/

            /* EPF Buffer Management Interrupt Cause 2 */
            {31, GT_FALSE,0, NULL, 0x03020008, 0x03024020,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
               PRV_CPSS_TG_XG_BM2_RES_0_E + 1,PRV_CPSS_TG_XG_BM2_RES_0_E + 31, 0x0FFFFFFF,
                0x0, 0x0FFFFFFF, 4, NULL, NULL},

                /* ExRMON Shadow Register Empty Interrupt Low ports 0-30 */
                {28, GT_FALSE,0, NULL, 0x03020010, 0x03024030,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_XG_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 1,
                   PRV_CPSS_TG_XG_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 31,
                    0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL} ,

                /* ExRMON Shadow Register Empty Interrupt High ports 31- 51 */
                {29, GT_FALSE,0, NULL, 0x03020014, 0x03024034,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_XG_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E+ 1,
                   PRV_CPSS_TG_XG_EPF_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E+ 21,
                    0x003FFFFE, 0x0, 0x003FFFFE, 0,NULL, NULL},

                /* ExRMON Total Samples Full Interrupt Low - ports 0-30 */
                {30, GT_FALSE,0, NULL, 0x03020018, 0x03024038,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_XG_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 1,
                   PRV_CPSS_TG_XG_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 31,
                    0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL} ,

                /* ExRMON Total Samples Full Interrupt high - ports 31-51 */
                {31, GT_FALSE,0, NULL, 0x0302001C, 0x0302403C,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                   PRV_CPSS_TG_XG_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E+ 1,
                   PRV_CPSS_TG_XG_EPF_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E+ 21,
                    0x003FFFFE, 0x0, 0x003FFFFE, 0,NULL, NULL},

    /* MACSumInt0             */
    {14, GT_FALSE,0, NULL, 0x0380002c, 0x03800030,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_MAC0_RES_0_E+1,PRV_CPSS_TG_XG_MAC0_RES_0_E + 31, 0xFFFFFFFF, 0x0,
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
       PRV_CPSS_TG_XG_TXQ_RES_0_E+1,PRV_CPSS_TG_XG_TXQ_RES_0_E + 31, 0x0BFFFFFF, 0x0,
        0x0BFFFFFF, 5,NULL, NULL},
    /* Transmit Queue Interrupt Cause 1 */
    {26, GT_FALSE,0, NULL, 0x01800080, 0x0180070C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_TXQ1_RES_0_E + 1,PRV_CPSS_TG_XG_TXQ1_RES_0_E + 31,0xFFFFFFFF,
        0x0, 0xFFFFFFFF, 0, NULL, NULL},
    /* Transmit Queue Interrupt Cause 3 */
    {28, GT_FALSE,0, NULL, 0x01800088, 0x0180008C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_TXQ3_RES_0_E + 1,PRV_CPSS_TG_XG_TXQ3_RES_0_E + 31,0xFFFFFFFF,
        0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Transmit Queue Interrupt Cause 4 */
    {29, GT_FALSE,0, NULL, 0x01800380, 0x01800388,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_TXQ4_RES_0_E + 1,PRV_CPSS_TG_XG_TXQ4_RES_0_E + 31, 0xFFFFFFFF,
        0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Transmit Queue Interrupt Cause 5 */
    {30, GT_FALSE,0, NULL, 0x01800384, 0x0180038C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_TXQ5_RES_0_E + 1,PRV_CPSS_TG_XG_TXQ5_RES_0_E + 31,0xFFFFFFFF,
        0x0, 0xFFFFFFFF, 0, NULL, NULL},
    /*
    It is the summary of the couse bits of the
    ExRMON shadow register empty interrupt high 1 - 21
    and the summary of the summary bits 26 - 28
    */
    {31, GT_FALSE,0, NULL, 0x01800680, 0x01800688,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E+ 1,
       PRV_CPSS_TG_XG_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_HIGH_E + 31, 0xE3FFFFFF,
        0x0, 0xE3FFFFFF, 3,NULL, NULL} ,

        /* TxQ ExRMON Shadow Register Empty Interrupt Low ports 0-30 */
        {26, GT_FALSE,0, NULL, 0x01800684, 0x0180068C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
           PRV_CPSS_TG_XG_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 1,
           PRV_CPSS_TG_XG_TXQ_EXRMON_SHADOW_REG_EMPTY_SUM_LOW_E + 31,
            0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL},

        /* TxQ ExRMON Total Samples Full Interrupt Low - ports 0-30 */
        {27, GT_FALSE,0, NULL, 0x01800584, 0x0180058C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
           PRV_CPSS_TG_XG_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 1,
           PRV_CPSS_TG_XG_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_LOW_E + 31,
            0xFFFFFFFE, 0x0, 0xFFFFFFFE, 0,NULL, NULL} ,

        /* TxQ ExRMON Total Samples Full Interrupt high - ports 31-51 */
        {28, GT_FALSE,0, NULL, 0x01800580, 0x01800588,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
           PRV_CPSS_TG_XG_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E + 1,
           PRV_CPSS_TG_XG_TXQ_EXRMON_TOTAL_SAMP_FULL_SUM_HIGH_E + 21,
            0x003FFFFE, 0x0, 0x003FFFFE, 0,NULL, NULL} ,

        /* Miscellaneous    */
    {16, GT_FALSE,0, NULL, 0x40, 0x44,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_MISC_RES_0_E+1,PRV_CPSS_TG_XG_MISC_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFDF, 0,NULL, NULL},

        /* Rx SDMA          */
    {17, GT_FALSE,0, NULL, 0x280C, 0x2814,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_RX_RES_0_E+1,PRV_CPSS_TG_XG_RX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},

        /* Tx SDMA          */
    {18, GT_FALSE,0, NULL, 0x2810, 0x2818,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_TX_RES_0_E+1,PRV_CPSS_TG_XG_TX_RES_0_E + 31, 0xFFFFFFFF, 0x0,
        0xFFFFFFFF, 0,NULL, NULL},

        /* MAC1             */
    {19, GT_FALSE,0, NULL, 0x038000B8, 0x038000BC,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
       PRV_CPSS_TG_XG_MAC1_RES_0_E+1,PRV_CPSS_TG_XG_MAC1_RES_0_E + 31, 0xFFFFFFFF, 0x0,
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

/****************************** END OF TIGER XG *****************************/

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
void prvCpssDrvPpIntDefTigerInit(void)
{
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devFamilyPtr;

    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TIGER_XG_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_TG_XG_LAST_INT;
    devFamilyPtr->maskRegistersAddressesArray = tigerXgMaskRegMapArr;
    devFamilyPtr->maskRegistersDefaultValuesArray = tigerXgMaskRegDefaultArr;
    devFamilyPtr->numOfScanElements = sizeof(tigerXgIntrScanArr)/sizeof(tigerXgIntrScanArr[0]);
    devFamilyPtr->interruptsScanArray = tigerXgIntrScanArr;
    devFamilyPtr->fdbTrigEndedId = PRV_CPSS_TG_XG_MAC_AGE_VIA_TRIGGER_ENDED_E;
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpIntExMxIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = numOfInterruptRegistersNotAccessibleBeforeStartInit;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=notAccessibleBeforeStartInitArray;

    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TIGER_12_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_TG_12_LAST_INT;
    devFamilyPtr->maskRegistersAddressesArray = tiger12MaskRegMapArr;
    devFamilyPtr->maskRegistersDefaultValuesArray = tiger12MaskRegDefaultArr;
    devFamilyPtr->numOfScanElements = sizeof(tiger12IntrScanArr)/sizeof(tiger12IntrScanArr[0]);
    devFamilyPtr->interruptsScanArray = tiger12IntrScanArr;
    devFamilyPtr->fdbTrigEndedId = PRV_CPSS_TG_12_MAC_AGE_VIA_TRIGGER_ENDED_E;
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpIntExMxIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = numOfInterruptRegistersNotAccessibleBeforeStartInit;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=notAccessibleBeforeStartInitArray;

    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TIGER_52_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_TG_52_LAST_INT;
    devFamilyPtr->maskRegistersAddressesArray = tiger52MaskRegMapArr;
    devFamilyPtr->maskRegistersDefaultValuesArray = tiger52MaskRegDefaultArr;
    devFamilyPtr->numOfScanElements = sizeof(tiger52IntrScanArr)/sizeof(tiger52IntrScanArr[0]);
    devFamilyPtr->interruptsScanArray = tiger52IntrScanArr;
    devFamilyPtr->fdbTrigEndedId = PRV_CPSS_TG_52_MAC_AGE_VIA_TRIGGER_ENDED_E;
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpIntExMxIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = numOfInterruptRegistersNotAccessibleBeforeStartInit;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=notAccessibleBeforeStartInitArray;

}
