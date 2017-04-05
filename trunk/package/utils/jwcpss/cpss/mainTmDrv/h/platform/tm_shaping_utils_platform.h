/*
 * (c), Copyright 2009-2013, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief shaping calculation constants  &tm_shaping_uitils interface.
 *
 * @file tm_shaping_utils_platform.h
 *
 * $Revision: 1.0 $
 */
#ifndef TM_SHAPING_UTILS_PLATFORM_H
#define TM_SHAPING_UTILS_PLATFORM__H


/******************************************************************
*       scheduling  scheme for BC2 
* according to preliminary analysis
*******************************************************************/
#define MAX_DIVIDER_EXP		7		/* maximum value of divider exponent for BC2 nodes */
#define OPTIMAL_NODE_FREQUENCY	64000000 	/* 64 MHz  - this frequency can provide granularity of 1kbit/s for nodes - */
											/* greater frequency causes to raise node units  more than 1 k             */

#define Q_UNIT				1000		/* bit/second  - minimum bw in case of no resolution & maximal divider */
#define A_UNIT				1000		/* bit/second  - minimum bw in case of no resolution & maximal divider */
#define B_UNIT				1000		/* bit/second  - minimum bw in case of no resolution & maximal divider */
#define C_UNIT				1000		/* bit/second  - minimum bw in case of no resolution & maximal divider */

/*
* port configuration:
* port has not limitation by max frequency  in order to provide 1kbit unit. we can select it equal to base frequency
*/
#define OPTIMAL_PORT_FREQUENCY	80000000 	/* 100 MHz  - this frequency can provide granularity of 1kbit/s for nodes - */

#define PORT_UNIT				1000	/* bit/second  - minimum bw in case of no resolution & maximal divider */


/*******************************************************************************************************
*  shaping configuration definitions
********************************************************************************************************/

#define 	TOKEN_WIDTH_BITS			11
#define 	MAX_POSSIBLE_TOKEN			(1<<TOKEN_WIDTH_BITS)-1

/* all values below  provides max b/w  100Gbit/s   and maximum accuracy */
#define		QUEUE_OPTIMAL_RES_EXP		9
#define		A_NODE_OPTIMAL_RES_EXP		9
#define		B_NODE_OPTIMAL_RES_EXP		9
#define		C_NODE_OPTIMAL_RES_EXP		9
#define		PORT_OPTIMAL_RES_EXP		4



#define		PORT_PERIODS_MAX			4000



#endif   /* TM_SHAPING_UTILS_PLATFORM_H */
