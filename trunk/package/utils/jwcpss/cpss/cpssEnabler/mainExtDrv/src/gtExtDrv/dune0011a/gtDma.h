/*******************************************************************************
*              Copyright 2002, MARVELL TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtDma.h - DMA functions header and definitions
*
* DESCRIPTION
* This file gives the user a complete interface to the powerful DMA engines,
* including functions for controling the priority mechanism.
* To fully understand the capabilities of the DMA engines please spare some
* time to go trough the spec.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __INCgtDmah
#define __INCgtDmah

/* includes */
#include <gtOs/gtGenTypes.h>


#define NO_BIT          0x00000000
#define BIT0            0x00000001
#define BIT1            0x00000002
#define BIT2            0x00000004
#define BIT3            0x00000008
#define BIT4            0x00000010
#define BIT5            0x00000020
#define BIT6            0x00000040
#define BIT7            0x00000080
#define BIT8            0x00000100
#define BIT9            0x00000200
#define BIT10           0x00000400
#define BIT11           0x00000800
#define BIT12           0x00001000
#define BIT13           0x00002000
#define BIT14           0x00004000
#define BIT15           0x00008000
#define BIT16           0x00010000
#define BIT17           0x00020000
#define BIT18           0x00040000
#define BIT19           0x00080000
#define BIT20           0x00100000
#define BIT21           0x00200000
#define BIT22           0x00400000
#define BIT23           0x00800000
#define BIT24           0x01000000
#define BIT25           0x02000000
#define BIT26           0x04000000
#define BIT27           0x08000000
#define BIT28           0x10000000
#define BIT29           0x20000000
#define BIT30           0x40000000
#define BIT31           0x80000000


/* typedefs */

typedef enum gtDmaEngine{DMA_ENG_0,DMA_ENG_1,DMA_ENG_2,DMA_ENG_3,DMA_ENG_4,
                         DMA_ENG_5,DMA_ENG_6,DMA_ENG_7} DMA_ENGINE;

/*****************************************************************************
*
*   Pizza arbiter slices Struct: Defined by user and used for updating the
*   DMA Arbiter with priority for each channel.
*   By default DMA_PIZZA is configured to ROUND_ROBIN.
*   Each field represent one of 16 slices, in order to configurate it to one
*   of the four engine simply write the engine number at the slice field.
*
******************************************************************************/

typedef struct gtDmaPizza0_3
{
    unsigned char    slice0;
    unsigned char    slice1;
    unsigned char    slice2;
    unsigned char    slice3;
    unsigned char    slice4;
    unsigned char    slice5;
    unsigned char    slice6;
    unsigned char    slice7;
    unsigned char    slice8;
    unsigned char    slice9;
    unsigned char    slice10;
    unsigned char    slice11;
    unsigned char    slice12;
    unsigned char    slice13;
    unsigned char    slice14;
    unsigned char    slice15;
} DMA_PIZZA0_3;

typedef struct gtDmaPizza4_7
{
    unsigned char    slice0;
    unsigned char    slice1;
    unsigned char    slice2;
    unsigned char    slice3;
    unsigned char    slice4;
    unsigned char    slice5;
    unsigned char    slice6;
    unsigned char    slice7;
    unsigned char    slice8;
    unsigned char    slice9;
    unsigned char    slice10;
    unsigned char    slice11;
    unsigned char    slice12;
    unsigned char    slice13;
    unsigned char    slice14;
    unsigned char    slice15;
} DMA_PIZZA4_7;

typedef struct gtDmaRecord
{
    unsigned int    ByteCnt;
    unsigned int    SrcAdd;
    unsigned int    DestAdd;
    unsigned int    NextRecPtr;
} DMA_RECORD;

typedef enum __gtDma_status{DMA_CHANNEL_IDLE,DMA_CHANNEL_BUSY,
                            DMA_NO_SUCH_CHANNEL,DMA_OK,DMA_GENERAL_ERROR}
                            DMA_STATUS;

/* defines */

#define DMA_FIRST_ENGINE   0
#define DMA_LAST_ENGINE    7


/* defines command for the LOW control register*/

#define DMA_DEST_DTL_8BYTES             BIT1
/* actually DMA_DEST_DTL_8BYTES is NO_BIT, but it defined as BIT1 for use
   the case of bits[0...2] == '000' as a flag, telling gtDmaTransfer
   to use the same DTL for source and destination */
#define DMA_DEST_DTL_16BYTES            BIT0
#define DMA_DEST_DTL_32BYTES            BIT0 | BIT1
#define DMA_DEST_DTL_64BYTES            BIT0 | BIT1 | BIT2
#define DMA_DEST_DTL_128BYTES           BIT2
#define DMA_HOLD_SOURCE_ADDR            BIT3
#define DMA_HOLD_DEST_ADDR              BIT5
#define DMA_DTL_8BYTES                  NO_BIT
#define DMA_DTL_16BYTES                 BIT6
#define DMA_DTL_32BYTES                 BIT6 | BIT7
#define DMA_DTL_64BYTES                 BIT6 | BIT7 | BIT8
#define DMA_DTL_128BYTES                BIT8
#define DMA_NON_CHAIN_MODE              BIT9
#define DMA_CHAIN_MODE                  NO_BIT
#define DMA_INT_EVERY_NULL_POINTER      BIT10
#define DMA_BLOCK_TRANSFER_MODE         BIT11
#define DMA_DEMAND_MODE                 NO_BIT
#define DMA_CHANNEL_ENABLE              BIT12
#define DMA_FETCH_NEXT_RECORD           BIT13
#define DMA_ACTIVITY_STATUS             BIT14
#define DMA_ALIGN_TOWARD_DEST           BIT15
#define DMA_MASK_REQ                    BIT16
#define DMA_ENABLE_DESCRIPTOR           BIT17
#define DMA_ENABLE_EOT                  BIT18
#define DMA_ENABLE_EOT_INTERRUPT        BIT19
#define DMA_ABORT                       BIT20
#define DMA_SOURCE_ADDR_IN_PCI0         BIT21
#define DMA_SOURCE_ADDR_IN_PCI1         BIT22
#define DMA_DEST_ADDR_IN_PCI0           BIT23
#define DMA_DEST_ADDR_IN_PCI1           BIT24
#define DMA_REC_ADDR_IN_PCI0            BIT25
#define DMA_REC_ADDR_IN_PCI1            BIT26
#define DMA_REQ_FROM_TIMER_COUNTER      BIT28
#define DMA_REQ_FROM_MPP_PIN            BIT28
#define DMA_OLD_DESCRIPTOR_MODE         NO_BIT
#define DMA_NEW_DESCRIPTOR_MODE         BIT31

#define DMA_ROUND_ROBIN                 0xe4e4e4e4

/* Performs DMA transfer according to: Engine, Byte Count,source & destination
   Addresses and a pointer to the first descriptor (chain mode) */
DMA_STATUS gtDmaTransfer (DMA_ENGINE engine,unsigned int sourceAddr,
                        unsigned int destAddr,unsigned int numOfBytes,
                        unsigned int command,DMA_RECORD * nextRecordPointer);
/* returns bit 14 in the gtDma control register. '1' - active '0' - not active*/
DMA_STATUS gtDmaIsChannelActive (DMA_ENGINE channel);
/* Configured a command to a selected gtDma Engine. */
GT_BOOL gtDmaCommand (DMA_ENGINE channel,unsigned int command);
/* The user fill the STRUCT DMA_PIZZA0_3 (see above), and this function update
   it to the registers. */
GT_BOOL    gtDmaEngineDisable(DMA_ENGINE engine);
GT_BOOL    gtDmaUpdateArbiter0_3(DMA_PIZZA0_3* prio0_3);
GT_BOOL    gtDmaUpdateArbiter4_7(DMA_PIZZA4_7* prio4_7);
#endif /* __INCgtDmah */
