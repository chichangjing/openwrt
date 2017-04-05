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
 * @brief utilities for calculation shaping parameters
 *
 * @file tm_shaping_utils.c
 *
 * $Revision: 2.0 $
 */



#include "tm_shaping_utils.h"
#include "tm_os_interface.h"
#include "tm_errcodes.h"
#include <stdlib.h>

#define	NODE_SHAPER_ACCURACY			0.005

#define	MINIMAL_PORT_SHAPER_ACCURACY	0.005
#define	OPTIMAL_PORT_SHAPER_ACCURACY	0.0001


int __calculate_node_shaping( 	double		bw_in_tokens,
								uint32_t *	pburst_size_bytes,
								uint32_t	token_resolution_multiplyer,
								uint8_t *	pdivider_exp,
								uint8_t *	presolution,
								uint32_t *	ptoken
							)
{
	uint16_t	token=0;

	double		next_bw_in_tokens;
	double		accuracy;
	uint32_t	effective_token;
	uint8_t		divider_exp;

	*presolution=0;
	/* check if necessary to use token resolution for max cir/eir */
	if ( (bw_in_tokens / ( 1 << MAX_DIVIDER_EXP)) > MAX_POSSIBLE_TOKEN )
	{
		*presolution=1;
		bw_in_tokens /= token_resolution_multiplyer;
	}

	/* find maximum accuracy solution ( regardless of burst) */
	for ( divider_exp = MAX_DIVIDER_EXP;  divider_exp > 0 ; divider_exp--)
	{
		if (bw_in_tokens < MAX_POSSIBLE_TOKEN) break;
		bw_in_tokens /=2.0;
	}
	if ( bw_in_tokens > MAX_POSSIBLE_TOKEN ) return 1; /* bandwidth error - out of range  , failed to configure */

	/* try to decrease burst sise - trading off accuracy  */
	while( divider_exp > 0 )
	{
		/* calculate accuracy in case of decreasing divider exponent */
		next_bw_in_tokens=bw_in_tokens /2.0;
		accuracy=(1.0*((int)(next_bw_in_tokens + 0.5))-next_bw_in_tokens)/next_bw_in_tokens;
		if (tm_fabs_db(accuracy) > NODE_SHAPER_ACCURACY) break;
		bw_in_tokens = next_bw_in_tokens;
		divider_exp--;
	}
	/* check if */
	token=(int)(bw_in_tokens + 0.5);
	/* check if it enough accurate*/
	accuracy=(1.0*token-bw_in_tokens)/bw_in_tokens;
	if (tm_fabs_db(accuracy) > NODE_SHAPER_ACCURACY) return 3;
	
	
	/* calculate effective token size in bytes and compare it to desired burst size */
	effective_token = token *  ((*presolution) ? token_resolution_multiplyer : 1);
	if (effective_token > *pburst_size_bytes)
	{
		*pburst_size_bytes = effective_token;
		return 2;
	}

	/* here successfully configured */
	*pdivider_exp=divider_exp;
	*ptoken	= token;
	return 0;
}

int calculate_node_shaping( double		cir_units,					
							double		eir_units,					
							uint32_t *	pcbs_kb,						
							uint32_t *	pebs_kb,						
							uint32_t	token_resolution_multiplyer,
							uint8_t *	pcir_divider_exp,			
							uint8_t *	peir_divider_exp,			
							uint8_t *	pcir_resolution,			
							uint8_t *	peir_resolution,			
							uint32_t *	pcir_token,					
							uint32_t *	peir_token					
							)
{
	uint32_t	burst;
	burst = *pcbs_kb * 1024;
	switch (__calculate_node_shaping(cir_units, &burst, token_resolution_multiplyer, pcir_divider_exp, pcir_resolution,pcir_token))
	{
		case 1: return 	TM_BW_OUT_OF_RANGE;
		case 2:
			{
				*pcbs_kb = (burst+512) /1024;
				burst = *pebs_kb * 1024;
				__calculate_node_shaping(eir_units, &burst, token_resolution_multiplyer, peir_divider_exp, peir_resolution, peir_token);
				*pebs_kb = (burst+512) /1024;
				return	TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE;
			}
		case 3:	return	TM_BW_UNDERFLOW;
		default: break;
	}
	burst = *pebs_kb * 1024;
	switch (__calculate_node_shaping(eir_units, &burst, token_resolution_multiplyer, peir_divider_exp, peir_resolution, peir_token))
	{
		case 1: return TM_BW_OUT_OF_RANGE;
		case 2:
			{
				*pebs_kb = (burst+512) /1024;
				return	TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE;
			}
		case 3:	return TM_BW_UNDERFLOW;
		default: break;
	}
	return 0;
}

int test_node_shaping( double		cir_units,					/* input */
						double		eir_units,					/* input */
						uint32_t	token_resolution_multiplyer,/* input */
						uint32_t *	pcbs_kb,					/* IN/OUT */
						uint32_t *	pebs_kb						/* IN/OUT */
						)
{
	uint8_t		divider_exp;
	uint8_t		resolution;
	uint32_t	token;
	uint32_t	burst;
	int result_1;
	int result_2;

	burst = *pcbs_kb * 1024;
	result_1 = __calculate_node_shaping(cir_units, &burst, token_resolution_multiplyer, &divider_exp, &resolution,&token);
	if (result_1 == 2) 
	{
		/* update committed burst size */
		*pcbs_kb = (burst+512) /1024;
	}
	burst = *pebs_kb * 1024;
	result_2 = __calculate_node_shaping(eir_units, &burst, token_resolution_multiplyer, &divider_exp, &resolution,&token);
	if (result_2 == 2) 
	{
		/* update extra burst size */
		*pebs_kb = (burst+512) /1024;
	}
	if ((result_1 == 1) || (result_2 == 1)) return TM_BW_OUT_OF_RANGE;
	if ((result_1 == 3) || (result_2 == 3)) return TM_BW_UNDERFLOW;
	if ((result_1 == 2) || (result_2 == 2)) return TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE;
	return 0;
}



/* following constants are used for separation cir/cbs  & eir/ebs related errors in case of processing  max of them */
#define	EXT_ERROR_SHIFT				10
#define	SET_CIR_MAX					error_shift=0;
#define	SET_EIR_MAX					error_shift=EXT_ERROR_SHIFT;

#define	MAX_ERROR_SHIFT(error)		error_shift+error
#define	MIN_ERROR_SHIFT(error)		(error+EXT_ERROR_SHIFT - error_shift)
									
int calculate_port_shaping( double		cir_units,
							double		eir_units,
							uint32_t *	pcbs_kb,
							uint32_t *	pebs_kb,
							uint32_t	token_resolution_multiplyer,
							uint32_t *	pperiods,
							uint8_t *	pcir_resolution,
							uint8_t *	peir_resolution,
							uint32_t *	pcir_token,
							uint32_t *	peir_token
						   )
{
	uint8_t		max_resolution=0;
	uint8_t		min_resolution=0;
	uint16_t	max_token=0;
	uint16_t	min_token=0;

	double		max_bw_in_tokens;
	double		min_bw_in_tokens;
	uint32_t	max_burst_size_bytes;
	uint32_t	min_burst_size_bytes;
	uint16_t	periods;

	double		exact_ratio;
	double		actual_ratio;
	double		accuracy;
	uint16_t	token;
	uint16_t	divider;
	uint32_t	effective_token;
	int			error_shift;
	
	double 		best_accuracy;
	uint16_t	best_token;
	uint16_t	best_periods;
	uint32_t	updated_cbs;
	uint32_t	updated_ebs;
	int			rc;
	
	updated_cbs = 0;
	updated_ebs = 0;
	rc = 0;

	if ( cir_units >= eir_units)
	{
		max_bw_in_tokens=cir_units;
		max_burst_size_bytes=*pcbs_kb*1024;
		min_bw_in_tokens=eir_units;
		min_burst_size_bytes=*pebs_kb*1024;
		SET_CIR_MAX;
	}
	else
	{
		max_bw_in_tokens=eir_units;
		min_bw_in_tokens=cir_units;
		max_burst_size_bytes=*pebs_kb*1024;
		min_burst_size_bytes=*pcbs_kb*1024;
		SET_EIR_MAX
	}

	/* check if nesessary to use token resolution for max cir/eir */
	if (max_bw_in_tokens > ((double)(MAX_POSSIBLE_TOKEN) * PORT_PERIODS_MAX))
	{
		max_resolution=1;
		max_bw_in_tokens /= token_resolution_multiplyer;
	}

	/*  find maximum accuracy solution ( regardless of burst)
	  using periods value try to find maximal token in  range  1::MAX_POSSIBLE_TOKEN, so rounding error will be at most 0.5/token  */
	periods = PORT_PERIODS_MAX;
	if (max_bw_in_tokens > MAX_POSSIBLE_TOKEN)
	{
		/* calculate desired periods - */
		periods=(uint32_t)(((double)MAX_POSSIBLE_TOKEN)/max_bw_in_tokens*(double)PORT_PERIODS_MAX);
		if (periods==0) periods=1; 
		/* calculate token */
		max_bw_in_tokens=max_bw_in_tokens*(double)(periods)/(double)PORT_PERIODS_MAX;
	}

/*	if (max_bw_in_tokens > MAX_POSSIBLE_TOKEN) return MAX_ERROR_SHIFT(1); */ /* bandwidth error - overflow out of range  , failed to configure */
	if (max_bw_in_tokens > MAX_POSSIBLE_TOKEN) return TM_BW_OUT_OF_RANGE;

	/* here we found "exact" solution   :  ratio  max_bw_in_tokens/periods  gives us exact desired banwidth  (before rounding  of token value) */
	exact_ratio=max_bw_in_tokens/periods;
	/* rounded token value for best solution */
	max_token=(int)(max_bw_in_tokens + 0.5);

	/* now we will try to found  pair of integers <token,periods>  with minimal value of periods &   token/periods ratio  differs from exact ratio 
	 less then required accuracy */
	 
	/*
	  try to iterate between  pairs from 1..max_token  & 1..periods  to achieve  ratio with required accuracy
	 in worst case  procedure below will return pair  <token,periods> unchanged
	*/
	token=1;
	divider=1;
	best_accuracy=1.0;
	
	/* not necessary , prevent compilation warning */
	
	best_token=1;
	best_periods=1;
	
	while(1)
	{
		if ((divider>PORT_PERIODS_MAX) || (token>max_token+1))  break;
		actual_ratio=(1.0*token)/divider;
		accuracy=(actual_ratio-exact_ratio)/exact_ratio;
		/* check if it is solution */
		if (tm_fabs_db(accuracy) < best_accuracy)
		{
			best_token=token;
			best_periods=divider;
			best_accuracy=tm_fabs_db(accuracy);
			if (best_accuracy < OPTIMAL_PORT_SHAPER_ACCURACY) break; /* achieve better accuracy causes bigger bursness */
		}
		/* iterate to next pair */
		if (actual_ratio > exact_ratio ) divider++;
		else token++;
	}

/*	if (best_accuracy > MINIMAL_PORT_SHAPER_ACCURACY) return MAX_ERROR_SHIFT(3); */ /* not enough accuracy , failed to configure */
	if (best_accuracy > MINIMAL_PORT_SHAPER_ACCURACY) return TM_BW_OUT_OF_RANGE;
	
	max_token=best_token;
	periods=best_periods;
	
	/* 	here we have "optimal burstness" solution  */
	effective_token = max_token *  (max_resolution ? token_resolution_multiplyer : 1);
	if (effective_token > max_burst_size_bytes)
	{
/*			return MAX_ERROR_SHIFT(2); */
		if (error_shift)	updated_ebs = effective_token;
		else				updated_cbs = effective_token;
		rc = 1;
	}

	/* here successfully configured max of cir/eir  */
	if (min_bw_in_tokens) /* if necessary to configure min */
	{
		min_bw_in_tokens = min_bw_in_tokens*periods/PORT_PERIODS_MAX;
		if ( min_bw_in_tokens > MAX_POSSIBLE_TOKEN )
		{
			min_resolution=1;
			min_bw_in_tokens /= token_resolution_multiplyer;
		}
		min_token=(int)(min_bw_in_tokens + 0.5);
/*		if (min_token==0)  return MAX_ERROR_SHIFT(4); */ /* failed to configure both cir/eir underflow occurred - too wide difference */
		if (min_token==0)	return TM_BW_UNDERFLOW;
		effective_token = min_token *  (min_resolution ? token_resolution_multiplyer : 1);
/*		if (effective_token > min_burst_size_bytes) return MIN_ERROR_SHIFT(2); */
		if (effective_token > min_burst_size_bytes)
		{
			if (error_shift)	updated_cbs = effective_token;
			else				updated_ebs = effective_token;
			rc = 1;
		}
	}

	if (rc)
	{
		if (updated_cbs) *pcbs_kb = updated_cbs/1024 + 1;
		if (updated_ebs) *pebs_kb = updated_ebs/1024 + 1;
		return TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE;
	}


	/* 
	here we successfully configure  tokens 
	let assign them
	*/

	*pperiods=periods;
	if(cir_units > eir_units)
	{
		*pcir_token	= max_token;
		*peir_token	= min_token;
		*pcir_resolution=max_resolution;
		*peir_resolution=min_resolution;
	}
	else
	{
		*pcir_token	= min_token;
		*peir_token	= max_token;
		*pcir_resolution=min_resolution;
		*peir_resolution=max_resolution;
	}
	return 0;
}



#define	K_L_N_ACCURACY	0.00001
#define	K_L_N_BIT_WIDTH	14

/** NEW RevB: Calculate Periodic Rate Shaping L,K,N parameters for a given update
 * bank_upd_rate_shaped for a SINGLE level. If no values found, return an error code */
int found_L_K_N(double desired_factor,
                    uint16_t *L_value,
					uint16_t *K_value,
					uint16_t *N_value)
{
    int L;
    int K;
    int N;
    double factor;
    double accuracy;
    int found = 0;
    /* sanity check */
    if (desired_factor >= 1.0 ) return 0;
	N=(int)(1.0/(1.0-desired_factor))-1;
	if (N > (1<< K_L_N_BIT_WIDTH)) return 0;
    /***/	
	K=1;
	L=0;
	while(1)
	{
		factor = 1.0-(double)(K + L)/(double)((N+1)*K+L*((N+1)+1));
		accuracy = (factor - desired_factor)/desired_factor;
		if (tm_fabs_db(accuracy) <= K_L_N_ACCURACY)
		{
			found = 1;
			break;
		}
		if (factor > desired_factor)	K++;
		else							L++;
		if (K > (1<<K_L_N_BIT_WIDTH) || (L > (1<<K_L_N_BIT_WIDTH))) break;
	}

	if (found)
	{
		*L_value = L;
		*K_value = K;
		*N_value = N;
	}
	return found;	
}
