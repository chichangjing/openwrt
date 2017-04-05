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
 * @file tm_shaping_utils.h
 *
 * $Revision: 3.0 $
 */
#ifndef TM_SHAPING_UTILS_H
#define TM_SHAPING_UTILS_H

#include "tm_platform_implementation_definitions.h"
#include "tm_shaping_utils_platform.h"


int found_L_K_N(double desired_factor,
                    uint16_t *L_value,
					uint16_t *K_value,
					uint16_t *N_value);


/*************************
* returned values:
* 0 - ok
* TM_BW_OUT_OF_RANGE						failed to configure
* TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE	desired cbs or ebs value is too small for desired cir/eir
*		in this case   values in  pcbs_kb/pebs_kb are updated to minimum possible value.
* TM_BW_UNDERFLOW				not enough accuracy 
*************************/ 
int calculate_node_shaping( double		cir_units,					/* input */
							double		eir_units,					/* input */
							uint32_t *	pcbs_kb,					/* IN/OUT */
							uint32_t *	pebs_kb,					/* IN/OUT */
							uint32_t	token_resolution_multiplyer,/* input */
							uint8_t *	pcir_divider_exp,			/* OUT */
							uint8_t *	peir_divider_exp,			/* OUT */
							uint8_t *	pcir_resolution,			/* OUT */
							uint8_t *	peir_resolution,			/* OUT */
							uint32_t *	pcir_token,					/* OUT */
							uint32_t *	peir_token					/* OUT */
							);



/*************************
* returned values:
* 0 - ok
* TM_BW_OUT_OF_RANGE			failed to configure
* TM_BW_UNDERFLOW				not enough accuracy 
* TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE	desired cbs or ebs value is too small for desired cir/eir
*		in this case   values in  pcbs_kb/pebs_kb are updated to minimum possible value. 
*************************/ 
int test_node_shaping( double		cir_units,					/* input */
						double		eir_units,					/* input */
						uint32_t	token_resolution_multiplyer,/* input */
						uint32_t *	pcbs_kb,					/* IN/OUT */
						uint32_t *	pebs_kb						/* IN/OUT */
						);



#define		PORT_PERIODS_MAX			4000

/****************************************************************
* returned values:
* returned values:
* 0 - ok
* TM_BW_OUT_OF_RANGE			failed to configure
* TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE	desired cbs or ebs value is too small for desired cir/eir
*		in this case   values in  pcbs_kb/pebs_kb are updated to minimum possible value. 
********************************************************************/ 

int calculate_port_shaping( double		cir_units,					/* input */
							double		eir_units,					/* input */
							uint32_t *	pcbs_kb,					/* input */
							uint32_t *	pebs_kb,					/* input */
							uint32_t	token_resolution_multiplyer,/* input */
							uint32_t *	pperiods,					/* OUT */
							uint8_t *	pcir_resolution,			/* OUT */
							uint8_t *	peir_resolution,			/* OUT */
							uint32_t *	pcir_token,					/* OUT */
							uint32_t *	peir_token					/* OUT */
							);


#ifdef PRINT_SHAPING_RESULTS

	#define PRINT_NODE_SHAPING_CALCULATION_RESULT(cir_bw, eir_bw)	\
	{\
		\
		double test_cir_bw;\
		double test_eir_bw;\
		{\
			if (cir_bw)\
			{\
				test_cir_bw=8.0*((double)ctl->level_data[level].level_update_frequency)/(double)ctl->level_data[level].per_interval*cir_token/(1<<min_div_exp);\
				if (cir_resolution) test_cir_bw*=(1<<ctl->level_data[level].token_res_exp);\
				tm_printf("desired cir=%13lu accuracy=%8.5f (token=%4ld, div_exp=%2d, res_bit=%d) ",cir_bw,(test_cir_bw - cir_bw*1000.0)/test_cir_bw,cir_token, min_div_exp, cir_resolution);\
			}\
			if (eir_bw)\
			{\
				test_eir_bw=8.0*((double)ctl->level_data[level].level_update_frequency)/(double)ctl->level_data[level].per_interval*eir_token/(1<<max_div_exp);\
				if (eir_resolution) test_eir_bw*=(1<<ctl->level_data[level].token_res_exp);\
				tm_printf("desired eir=%13lu accuracy=%8.5f (token=%4ld, div_exp=%2d, res_bit=%d) ",eir_bw,(test_eir_bw - eir_bw*1000.0)/test_eir_bw, eir_token, max_div_exp,eir_resolution );\
			}\
			tm_printf("\n");\
		}\
	}
	/*
	#define PRINT_NODE_SHAPING_CALCULATION_RESULT(cir_bw,eir_bw)\
		PRINT_SHAPING_CALCULATION_RESULT(cir_bw,eir_bw,level,div_exp,(1<<div_exp),cir_token,eir_token,cir_resolution, eir_resolution)
	#define PRINT_PORT_SHAPING_CALCULATION_RESULT(cir_bw,eir_bw)\
		PRINT_SHAPING_CALCULATION_RESULT(cir_bw,eir_bw,P_LEVEL,periods,periods,min_token,max_token,res_min_bw, res_max_bw)
	*/

	#define PRINT_PORT_SHAPING_CALCULATION_RESULT(cir_bw, eir_bw)	\
	{\
		\
		double test_cir_bw;\
		double test_eir_bw;\
		{\
			tm_printf("periods=%5u  ",periods);\
			if (cir_bw)\
			{\
				test_cir_bw=8.0*((double)ctl->level_data[P_LEVEL].level_update_frequency)/(double)ctl->level_data[P_LEVEL].per_interval*min_token/periods;\
				if (res_min_bw) test_cir_bw*=(1<<ctl->level_data[P_LEVEL].token_res_exp);\
				tm_printf("desired cir=%13lu accuracy=%8.5f (token=%4ld, res_bit=%d) ",cir_bw,(test_cir_bw - cir_bw*1000.0)/test_cir_bw,min_token,res_min_bw);\
			}\
			if (eir_bw)\
			{\
				test_eir_bw=8.0*((double)ctl->level_data[P_LEVEL].level_update_frequency)/(double)ctl->level_data[P_LEVEL].per_interval*max_token/periods;\
				if (res_max_bw) test_eir_bw*=(1<<ctl->level_data[P_LEVEL].token_res_exp);\
				tm_printf("desired eir=%13lu accuracy=%8.5f (token=%4ld,res_bit=%d) ",eir_bw,(test_eir_bw - eir_bw*1000.0)/test_eir_bw, max_token,res_max_bw );\
			}\
			tm_printf("\n");\
		}\
	}
#else
	/* do nothing */
	#define PRINT_NODE_SHAPING_CALCULATION_RESULT(cir_bw, eir_bw)
	#define PRINT_PORT_SHAPING_CALCULATION_RESULT(cir_bw, eir_bw)
#endif


#endif   /* TM_SHAPING_UTILS_H */
