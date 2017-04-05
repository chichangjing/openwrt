/*******************************************************************************
*                   Copyright 2002, GALILEO TECHNOLOGY, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).    *
********************************************************************************
* marvell.h - Marvell Development Board BSP definition.
*
* DESCRIPTION:
*       This file contains I/O addresses and related constants. 
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __INCmarvellh
#define __INCmarvellh

/* defines  */

/* GPIO definition */    
#define GPIO_SELECT_7_0     0x0
#define GPIO_SELECT_15_8    0x4
#define GPIO_OE             0x8
#define GPIO_OUT            0xC
#define GPIO_IN             0x10
#define GPIO_IER            0x14
#define GPIO_IMR            0x18
#define GPIO_RSR            0x1C
#define GPIO_ISR            0x20


/* GPIO port bit-wise descriptions */
#define NO_BIT	 (0<<0)
#define	GPIO_0	 (1<<0)
#define GPIO_1   (1<<1)
#define	GPIO_2   (1<<2)
#define	GPIO_3   (1<<3)
#define	GPIO_4   (1<<4)
#define	GPIO_5   (1<<5)
#define	GPIO_6   (1<<6)
#define	GPIO_7   (1<<7)
#define	GPIO_8   (1<<8)
#define	GPIO_9   (1<<9)
#define	GPIO_10  (1<<10)
#define	GPIO_11  (1<<11)
#define	GPIO_12  (1<<12)
#define	GPIO_13  (1<<13)
#define	GPIO_14  (1<<14)
#define	GPIO_15  (1<<15)

/* Board debug LEDs connection to GPIO */
#define DEBUG_LED_D13   GPIO_0
#define DEBUG_LED_D12   GPIO_1
#define DEBUG_LED_D11   GPIO_2
#define DEBUG_LED_D10   GPIO_3
#define DEBUG_LED_D9    GPIO_4
#define DEBUG_LED_D8    GPIO_5
#define DEBUG_LED_D7    GPIO_6

/* ARM964es TCM definition */
#undef  INCLUDE_ITCM
#define INCLUDE_DTCM

/* Internal SRAM */
#undef INCLUDE_INTERNAL_SRAM

/* System address mappings */

/* Memory segments Addresses */
#define ARM_DSRAM_BASE      0x98000000
#define UINIMAC_PCR_ADRS    0x80008400	/* port configuration register */
#define ARM_RESET_VECTOR    0xFFFF0000
#define GPIO_BASE_ADRS      0x8000D000
#define INTERNAL_REG_BASE   0x80000000
#define	SDRAM0_BASE	        CS0_BASE

#define INTERNAL_REG_BASE1   0x80000000
#define INTERNAL_REG_BASE2   0x90000000
#define INTERNAL_REGS_SIZE   0x00100000

/* Device CS addressing */
#define CS2_BASE			0x20000000
#define CS1_BASE			0x10000000
#define CS0_BASE			0x00000000

/* Memory segments size */
#define ARM_DSRAM_SIZE      0x00002000
#define INTERNAL_REG_SIZE   0x20000000
#define CS2_MAX_SIZE		0x10000000
#define CS1_MAX_SIZE		0x10000000
#define CS0_MAX_SIZE		0x10000000

/* Device CS supported on board */
#undef	INCLUDE_CS3
#undef	INCLUDE_CS1

/* internal address space segments */
#define AMBA_TIMER_BASE			(INTERNAL_REG_BASE + 0x10009000)
#define ICU_BASE_ADRS		    (INTERNAL_REG_BASE + 0x10008000)
#define UART_BASE_ADRS		    (INTERNAL_REG_BASE + 0xC840)
#define DMA_BASE_ADRS		    (INTERNAL_REG_BASE + 0xE000)
#define FAST_ETH_MAC_BASE  		(INTERNAL_REG_BASE + 0x8000)
#define MEM_CTRL_BASE_ADRS  	(INTERNAL_REG_BASE + 0x6000)
#define CIU_CONFIG_BASE_ADRS  	(INTERNAL_REG_BASE + 0x2000)


	#define BOARD_MODEL			"DB-88E6218"
	
	/* Flash definitions */
#define FLASH_BASE         ROM_BASE_ADRS   /* Base address of Flash part    */
    #define FLASH_BUS_WIDTH		4	  	    /* Four byte width bus 			*/
    #define FLASH_WIDTH_MODE	16	  	    /* 16-bit width Flash mode 		*/

/* Platform Tools */
#ifdef LE /* Little Endian */
    #define MV_SHORT_SWAP(X) (X)
    #define MV_WORD_SWAP(X)  (X)
    #define MV_LONG_SWAP(X)  ((l64)(X))
#else    /* Big Endian */
    #ifndef MV_SHORT_SWAP
        #define MV_SHORT_SWAP(X) ((((X)&0xff)<<8) | (((X)&0xff00)>>8))
    #endif /* MV_SHORT_SWAP */
    #ifndef MV_WORD_SWAP
        #define MV_WORD_SWAP(X)  ((((X)&0xff)<<24) |                           \
                                  (((X)&0xff00)<<8) |                          \
                                  (((X)&0xff0000)>>8) |                        \
                                  (((X)&0xff000000)>>24))
    #endif /* MV_WORD_SWAP */
    #ifndef MV_LONG_SWAP
        #define MV_LONG_SWAP(X) ((l64) ((((X)&0xffULL)<<56) |                  \
                                        (((X)&0xff00ULL)<<40) |                \
                                        (((X)&0xff0000ULL)<<24) |              \
                                        (((X)&0xff000000ULL)<<8) |             \
                                        (((X)&0xff00000000ULL)>>8) |           \
                                        (((X)&0xff0000000000ULL)>>24) |        \
                                        (((X)&0xff000000000000ULL)>>40) |      \
                                        (((X)&0xff00000000000000ULL)>>56)))
    #endif /* MV_LONG_SWAP */
#endif /* LE */

#define SWAP_BIT_NUM(bitNum)     MV_WORD_SWAP(1 << ((bitNum) % 32))

#define BUS		NONE

/*
 * interrupt control stuff
 * Note: FIQ is not handled within VxWorks so this is just IRQ
 */
#define IC_BASE 	(0x90008000)
#define IRQ_STATUS	(IC_BASE+0x000)	/* Read */
#define IRQ_RAW_STATUS	(IC_BASE+0x004)	/* Read */
#define IRQ_ENABLE	(IC_BASE+0x008)	/* Read/Write */
#define IRQ_DISABLE	(IC_BASE+0x00C)	/* Write */
#define IRQ_SOFT	(IC_BASE+0x010)	/* Read/Write */
#define FIQ_STATUS	(IC_BASE+0x100)	/* Read */
#define FIQ_RAW_STATUS	(IC_BASE+0x104)	/* Read */
#define FIQ_ENABLE	(IC_BASE+0x108)	/* Read/Write */
#define FIQ_DISABLE	(IC_BASE+0x10C)	/* Write */

#define AMBA_INT_NUM_LEVELS	18
#define AMBA_INT_CSR_PEND	IRQ_STATUS
#define AMBA_INT_CSR_ENB	IRQ_ENABLE
#define AMBA_INT_CSR_DIS	IRQ_DISABLE
#define AMBA_INT_CSR_MASK	0x0001FFFF /* Mask out invalid status bits */


/* Interrupt levels */

#define INT_LVL_FIQ_SHARE	0	/* may share the same int source as FIQ       */
#define INT_LVL_SOFT		1	/* software interrupt                         */
#define INT_LVL_COMMS_RX  	2	/* communication receiveing channel           */
#define INT_LVL_COMMS_TX  	3	/* communication transmitting channel         */
#define INT_LVL_TIMER_1		4	/* timer 1 interrupt request                  */
#define INT_LVL_TIMER_2		5	/* timer 2 interrupt request                  */
#define INT_LVL_TIMER_3		6	/* timer 3 interrupt request                  */
#define INT_LVL_TIMER_4		7	/* timer 4 interrupt request                  */
#define INT_LVL_HOST_IF		8	/* host interface interrupt request           */
#define INT_LVL_FST_ETH_MAC 9	/* fast Ethernet MAC interrupt request        */
#define INT_LVL_WL_ETH_MAC  10	/* Wireless Ethernet MAC interrupt request    */
#define INT_LVL_UART		11	/* UART interrupt request                     */
#define INT_LVL_GPIO	    12	/* GPIO interrupt request                     */
#define INT_LVL_CSIU        13	/* Clocked Serial Interface interrupt request */
#define INT_LVL_PMU	        14	/* Power Managment Unit interrupt request     */
#define INT_LVL_DMA_0		15	/* DMA Channel 0 interrupt request            */
#define INT_LVL_DMA_1		16	/* DMA Channel 1 interrupt request            */
#define INT_LVL_AEU	        17	/* Advanced Encryption Unit interrupt request */


/* interrupt vectors */

#define INT_VEC_FIQ_SHARE    IVEC_TO_INUM(INT_LVL_FIQ_SHARE)
#define INT_VEC_SOFT         IVEC_TO_INUM(INT_LVL_SOFT)
#define INT_VEC_COMMS_RX     IVEC_TO_INUM(INT_LVL_COMMS_RX)
#define INT_VEC_COMMS_TX     IVEC_TO_INUM(INT_LVL_COMMS_TX)
#define INT_VEC_TIMER_1      IVEC_TO_INUM(INT_LVL_TIMER_1)
#define INT_VEC_TIMER_2      IVEC_TO_INUM(INT_LVL_TIMER_2)
#define INT_VEC_TIMER_3      IVEC_TO_INUM(INT_LVL_TIMER_3)
#define INT_VEC_TIMER_4      IVEC_TO_INUM(INT_LVL_TIMER_4)
#define INT_VEC_HOST_IF      IVEC_TO_INUM(INT_LVL_HOST_IF)
#define INT_VEC_FST_ETH_MAC  IVEC_TO_INUM(INT_LVL_FST_ETH_MAC)
#define INT_VEC_WL_ETH_MAC   IVEC_TO_INUM(INT_LVL_WL_ETH_MAC)
#define INT_VEC_UART         IVEC_TO_INUM(INT_LVL_UART)
#define INT_VEC_GPIO         IVEC_TO_INUM(INT_LVL_GPIO)
#define INT_VEC_CSIU         IVEC_TO_INUM(INT_LVL_CSIU)
#define INT_VEC_PMU          IVEC_TO_INUM(INT_LVL_PMU)
#define INT_VEC_DMA_0        IVEC_TO_INUM(INT_LVL_DMA_0)
#define INT_VEC_DMA_1        IVEC_TO_INUM(INT_LVL_DMA_1)
#define INT_VEC_AEU          IVEC_TO_INUM(INT_LVL_AEU)


/* definitions for the UART */

#define UART_XTAL_FREQ		8000000     /* UART baud rate clk freq */
#define N_SIO_CHANNELS		1

/* definitions for the AMBA Timer */

#define AMBA_TIMER1_BASE        (AMBA_TIMER_BASE)
#define AMBA_TIMER2_BASE        (AMBA_TIMER_BASE + 0x4)
#define AMBA_TIMER3_BASE        (AMBA_TIMER_BASE + 0x8)
#define AMBA_TIMER4_BASE        (AMBA_TIMER_BASE + 0xC)
#define AMBA_TIMER_CONTROL      (AMBA_TIMER_BASE + 0x10)
#define AMBA_TIMER_INT_SOURCE   (AMBA_TIMER_BASE + 0x24)
#define AMBA_TIMER_INT_REQUEST  (AMBA_TIMER_BASE + 0x28)

#define SYS_TIMER_BASE          AMBA_TIMER3_BASE    /* Slow event timer 1KHz */
#define AUX_TIMER_BASE          AMBA_TIMER4_BASE    /* Fast event timer 1MHz */

#define SYS_TIMER_INT_LVL       INT_LVL_TIMER_3
#define AUX_TIMER_INT_LVL       INT_LVL_TIMER_4

/* Add corresponding INT_VEC definitions for intConnect calls. */

#define SYS_TIMER_INT_VEC       INT_VEC_TIMER_3
#define AUX_TIMER_INT_VEC       INT_VEC_TIMER_4

/* Frequency of counter/timers */

#define SYS_TIMER_CLK	        (1000000)       /* Fast event timer 1MHz */ 
#define AUX_TIMER_CLK	        SYS_TIMER_CLK   /* Fast event timer 1MHz */

#define AMBA_RELOAD_TICKS	    0	/* No overhead */

/* Mask out unused bits from timer register. */

#define AMBA_TIMER_VALUE_MASK	0xFFFF

/* Sys Clk is timer 1 */

#define SYS_TIMER_LOAD	(AMBA_TIMER_LOAD(SYS_TIMER_BASE))
#define SYS_TIMER_VALUE	(AMBA_TIMER_VALUE(SYS_TIMER_BASE))

/* Aux Clk is timer 2 */

#define AUX_TIMER_LOAD	(AMBA_TIMER_LOAD(AUX_TIMER_BASE))
#define AUX_TIMER_VALUE	(AMBA_TIMER_VALUE(AUX_TIMER_BASE))


/*
 * Clock rates depend upon CPU power and work load of application.
 * The values below are minimum and maximum allowed by the hardware.
 * So:
 * min frequency = roundup(clock_rate/(max_counter_value))
 * max frequency = rounddown(clock_rate/(min_counter_value))
 * i.e. SYS_CLK_RATE_MAX = SYS_TIMER_CLK
 *      AUX_CLK_RATE_MAX = AUX_TIMER_CLK
 *
 * However, we must set maxima that are sustainable on a running
 * system determined by experimentation.
 *
 */

#define SYS_CLK_RATE_MIN    1
#define SYS_CLK_RATE_MAX    SYS_TIMER_CLK

#define AUX_CLK_RATE_MIN    ((AUX_TIMER_CLK)/0x10000)
#define AUX_CLK_RATE_MAX    AUX_TIMER_CLK


/* Ethernet Ports Board depended information. */
#define PORT_0_PHY_ADDR     0x0
#define ETH_PORTS_GAP		0x0     /* There is only one MAC in this device */

/* Address translation macros. These improve performace */
#define VIRTUAL_TO_PHY(address) ((unsigned int)address)
#define PHY_TO_VIRTUAL(address) ((unsigned int)address)

/* Bit field definitions */

#define BIT0                            0x00000001
#define BIT1                            0x00000002
#define BIT2                            0x00000004
#define BIT3                            0x00000008
#define BIT4                            0x00000010
#define BIT5                            0x00000020
#define BIT6                            0x00000040
#define BIT7                            0x00000080
#define BIT8                            0x00000100
#define BIT9                            0x00000200
#define BIT10                           0x00000400
#define BIT11                           0x00000800
#define BIT12                           0x00001000
#define BIT13                           0x00002000
#define BIT14                           0x00004000
#define BIT15                           0x00008000
#define BIT16                           0x00010000
#define BIT17                           0x00020000
#define BIT18                           0x00040000
#define BIT19                           0x00080000
#define BIT20                           0x00100000
#define BIT21                           0x00200000
#define BIT22                           0x00400000
#define BIT23                           0x00800000
#define BIT24                           0x01000000
#define BIT25                           0x02000000
#define BIT26                           0x04000000
#define BIT27                           0x08000000
#define BIT28                           0x10000000
#define BIT29                           0x20000000
#define BIT30                           0x40000000
#define BIT31                           0x80000000

/* Handy sizes */

#define _1K                           0x00000400
#define _4K                           0x00001000
#define _8K                           0x00002000
#define _16K                          0x00004000
#define _64K                          0x00010000
#define _128K                         0x00020000
#define _256K                         0x00040000
#define _512K                         0x00080000

#define _1M                           0x00100000
#define _2M                           0x00200000
#define _4M                           0x00400000
#define _8M                           0x00800000
#define _16M                          0x01000000
#define _32M                          0x02000000
#define _64M                          0x04000000
#define _128M                         0x08000000
#define _256M                         0x10000000
#define _512M                         0x20000000

#define _1G                           0x40000000
#define _2G                           0x80000000

#endif	/* __INCmarvellh */
