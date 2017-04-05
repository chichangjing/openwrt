/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
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
* gt88e6218Reg.h
*
* DESCRIPTION:
*       This file contains the registers base address for all the units in the 
*       FireFox.                            
*
* DEPENDENCIES:
*
*       $Revision: 1.1.2.1 $
*******************************************************************************/

#ifndef __gt88e6218Reg
#define __gt88e6218Reg

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines     */
/* base units  */

#define I2C_BASE_REG_ADDR               0x8000c100
#define LBU_BASE_REG_ADDR               0x8000c000           
#define DMA_BASE_REG_ADDR               0x8000e000
#define MEMORY_BASE_REG_ADDR            0x80006000
#define INTERRUPT_BASE_REG_ADDR         0x90008000 
#define GPIO_BASE_REG_ADDR              0x8000d000
#define UNIMAC_BASE_REG_ADDR            0x80008000
#define TIMER_BASE_REG_ADDR             0x90009000
#define UART_BASE_REG_ADDR              0x8000c840


/* LBU general registers */
#define     LBU_PRESET                  0x00   /* LBU peripheral reset - WO */
#define     LBU_CLK_DIV                 0x04   /* LBU peripheral clock select */

/* UNIMAC general registers */
#define    UNIMAC_SMI_REG                       0x10 

/* GPIO general registers */
#define     GPIO_SELECT_0_7                     0x00
#define     GPIO_SELECT_8_15                    0x04
#define     CTRL_BIDIREC_GPIO_OUTPUT_EN_PINS    0x08 
#define     GPIO_OUTPUT                         0x0C 
#define     GPIO_INPUT                          0x10 
#define     GPIO_IER                            0x14 
#define     GPIO_IMR                            0x18 
#define     GPIO_RSR                            0x1C 
#define     GPIO_ISR                            0x20 
#define     GPIO_SW_LED_OUTPUT_EN               0x24 
#define     GPIO_SW_LED_CYCLE_3_0               0x28 
#define     GPIO_SW_LED_CYCLE_7_4               0x2C 
#define     GPIO_SW_LED_CYCLE_11_8              0x30 
#define     GPIO_SW_LED_CYCLE_15_12             0x34 
#define     GPIO_SW_LED_DUTY_CYCLE_3_0          0x38 
#define     GPIO_SW_LED_DUTY_CYCLE_7_4          0x3C 
#define     GPIO_SW_LED_DUTY_CYCLE_11_8         0x40 
#define     GPIO_SW_LED_DUTY_CYCLE_15_12        0x44 
#define     GPIO_LED_OUTPUT_EN_STATUS           0x48 
#define     GPIO_LED_3_0_STATUS                 0x4C 
#define     GPIO_PCLK_DIV_GENERATE_LED_CLK      0x50


/* Register offsets */

#define I2C_PRER                  0x0100    /* Clock prescalar register       */
#define I2C_GLOBAL_CTRL           0x0104    /* Global control register        */
#define I2C_CTRL                  0x0110    /* I2c control register           */
#define I2C_STATUS                0x011c    /* CSU status register            */
#define I2C_INTERRUPT_MASK        0x0120    /* Interrupt mask register        */
#define I2C_DATA                  0x0124    /* Virtual data register          */
#define I2C_INTERRUPT_SRC         0x012c    /* Interrupt source register      */

/* Register fields */

/* Global control register */
#define I2C_GCR_AC_SEL            BIT0      
#define I2C_GCR_MEM_SEL           BIT1
#define I2C_GCR_INTERRUPT_MASK    BIT5
#define I2C_GCR_MODE_CTRL_I2C     NO_BIT

/* I2c control register */        
#define I2C_CR_STOP               BIT0
#define I2C_CR_START              BIT1
                                  
/* I2C status register */         
#define I2C_SR_BUSY               BIT2
#define RX_SR_ACK                 BIT3

/* SMI status register */         
#define SMI_OP_CODE_BIT_READ      BIT26
#define SMI_OP_CODE_BIT_WRITE     NO_BIT
#define SMI_STAT_BUSY             BIT28
#define SMI_STAT_READ_VALID       BIT27

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gt88e6218Reg */
