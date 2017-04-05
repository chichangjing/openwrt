/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* mpc107Dma.h - Header File for :  DMA functions and definitions
*
* DESCRIPTION:
*       This file gives the user an interface to the powerful DMA engines,
*       To fully understand the capabilities of the DMA engines please spare
*       some time to go trough the Datasheet of the MPC8245.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __INCmpc107Dmah
#define __INCmpc107Dmah

/* includes */
#include "vxworks.h"
#include "MPC107.h"

/* defines */
#define DMA_FIRST_ENGINE          0
#define DMA_LAST_ENGINE           1
#define DMA_ENGINE_REG_OFFSET 0x100 
    
/* Access to the additional DMA engine. */
#define DMA_ENGIN_OFFSET(engin) ((engine) * DMA_ENGINE_REG_OFFSET)

/* DMA DMRs  (offset 0x00) - Dma Mode Register bits */
#define DMA_CHANNEL_START          (1<<0)  /* Channel Start                 */
#define DMA_CHANNEL_CONTINUE       (1<<1)  /* Channel Continue              */
#define DMA_DIRECT_MODE     	   (1<<2)  /* Direct Transfer Mode          */
#define DMA_DESC_ADDR_IN_PCI       (1<<3)  /* Descriptor Location (in PCI)  */
#define DMA_ENABLE_EOT_INTERRUPT   (1<<7)  /* EOT   Interrupt Enable        */
#define DMA_ENABLE_ERR_INTERRUPT   (1<<8)  /* Error Interrupt Enable        */
#define DMA_PCI_READ                  0    /* PCI Read Commands             */
#define DMA_PCI_READ_LINE          (1<<10) /* PCI Read Commands (line)      */
#define DMA_PCI_READ_MULTIPLE      (1<<11) /* PCI Read Commands (multiple)  */
#define DMA_HOLD_SOURCE_ADDR       (1<<12) /* Source      Address Hold Enable */
#define DMA_HOLD_DEST_ADDR		   (1<<13) /* Destination Address Hold Enable */
                                           /* Sorce Adress Hold Transfer Size */
#define DMA_HOLD_SRC_ADDR_SIZE_1      0       /*  1 byte */
#define DMA_HOLD_SRC_ADDR_SIZE_2   (1<<14)    /*  2 byte */
#define DMA_HOLD_SRC_ADDR_SIZE_4   (1<<15)    /*  4 byte */
#define DMA_HOLD_SRC_ADDR_SIZE_8   (3<<14)    /*  8 byte */
                                          /* Destenation Adress Hold Transfer */
#define DMA_HOLD_DEST_ADDR_SIZE_1     0       /*  1 byte */
#define DMA_HOLD_DEST_ADDR_SIZE_2  (1<<16)    /*  2 byte */
#define DMA_HOLD_DEST_ADDR_SIZE_4  (1<<17)    /*  4 byte */
#define DMA_HOLD_DEST_ADDR_SIZE_8  (3<<16)    /*  8 byte */
#define DMA_PERIODIC_ENABLE        (1<<18) /* Periodic DMA Enable        */
#define DMA_IRQ_STEER_PCI          (1<<19) /* Interrupt Steer to PCI     */
                                           /* Local Mem Delay Count      */
#define DMA_MEMORY_DELAY_4_CYCLES     0       /*  4 sys_logic_clk cycles */
#define DMA_MEMORY_DELAY_8_CYCLES  (1<<20)    /*  8 sys_logic_clk cycles */
#define DMA_MEMORY_DELAY_16_CYCLES (1<<21)    /* 16 sys_logic_clk cycles */
#define DMA_MEMORY_DELAY_32_CYCLES (3<<20)    /* 32 sys_logic_clk cycles */

/* DMA DSRs  (offset 0x04) - Dma Status Register bits */
#define DMA_CHAIN_END              (1<<0) /* End Of Chain/direct Interrupt   */ 
#define DMA_SEGMENT_END            (1<<1) /* End Of Segment Interrupt        */ 
#define DMA_ACTIVITY_STATUS        (1<<2) /* DMA Channel Busy                */
#define DMA_PCI_ERROR              (1<<4) /* PCI bus Error in DMA transfer   */
#define DMA_MEM_ERROR              (1<<7) /* Local Mem Error in DMA transfer */

/* DMA CDARs (offset 0x08) - Current Descriptor Address Register bits */
#define DMA_TRANSFER_TYPE_MEM2MEM     0
#define DMA_TRANSFER_TYPE_MEM2PCI  (1<<1)
#define DMA_TRANSFER_TYPE_PCI2MEM  (1<<2)
#define DMA_TRANSFER_TYPE_PCI2PCI  (3<<1)
#define DMA_END_SEGMENT_INT_ENABLE (1<<3) /* End Of Segment Interrupt enable  */ 
#define DMA_SNOOP_ENABLE           (1<<4) /* Enable DMA snoop for transaction */

/* DMA BCRs  (offset 0x20) - Byte Count Register bits */
#define DMA_MAX_BYTES_TRANSFER     0x03ffffff /* maximum 64M - 1 bytes */
                  
/* DMA NDARs (offset 0x24) - Next Descriptor Address Registers bits */
#define DMA_ND_END_OF_TRANSFER     (1<<0) /* End of transfer descriptors */
/* bits 1 - 4 are the same as CDARs */


/* typedefs */
typedef enum mpc107DmaEngine
{
    DMA_ENG_ALL_BUSY=-1,
    DMA_ENG_0,
    DMA_ENG_1
} DMA_ENGINE;
 
typedef struct mpc107DmaRecord
{
    unsigned int srcAdd;
    unsigned int dummy1;    
    unsigned int destAdd;
    unsigned int dummy2;    
    unsigned int nextRecPtr;
    unsigned int dummy3;    
    unsigned int byteCnt;                                            
    unsigned int dummy4;    
} DMA_RECORD;

typedef enum _mpc107DmaStatus
{
    DMA_ENGINE_IDLE,
    DMA_ENGINE_BUSY,
    DMA_NO_SUCH_ENGINE,
    DMA_OK,
    DMA_GENERAL_ERROR
} DMA_STATUS;

/* mpc107Dma.h API list */

/* Performs DMA transfer according to: Engine, Byte Count,source & destination*/ 
/* Addresses and a pointer to the first descriptor (chain mode)               */
DMA_STATUS   mpc107DmaTransfer (DMA_ENGINE engine,    unsigned int sourceAddr,
                                unsigned int destAddr,unsigned int numOfBytes);

DMA_STATUS   mpc107DmaIsEngineActive (DMA_ENGINE engine);
DMA_STATUS   mpc107DmaEngineDisable  (DMA_ENGINE engine);
DMA_ENGINE   mpc107DmaFindFreeEngine (void);
unsigned int mpc107DmaGetStatusReg   (DMA_ENGINE engine);

#endif /* __INCmpc107Dmah */

