/*
 * (c), Copyright 2009-2014, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief tm_drop implementation.
 *
 * @file tm_drop.c
 *
 * $Revision: 2.0 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "tm_drop.h"
#include "tm_errcodes.h"

#include "rm_alloc.h"
#include "rm_free.h"
#include "rm_status.h"

#include "set_hw_registers.h"
#include "tm_set_local_db_defaults.h"
#include "tm_os_interface.h"
#include "tm_locking_interface.h"
#include "tm_hw_configuration_interface.h"
#include "tm_nodes_ctl_internal.h"

#define div_roundup(a, b) ((a + (b - 1))/(b))

/* #define goto while(printf("out: %s(%d)\n", __FILE__, __LINE__) || 1) goto */
/**
 */


#define UPDATE_CURVE(curve_array_element)	\
{\
    curve = &(curve_array_element);\
    for (i=0; i<32; i++)\
        curve->prob[i] = (uint8_t)tm_roundf((float)curve->prob[i] / scaling_coeff);\
}

/*
* If  scaling for the new curve is greater than scaling for all previuos curves,
* all register values for old curves must be scaled down according to new scaling.
* example : if first curve was calculated according to scaling 25% & new one have scaling 100%
* the previous curve values should be divided by 4 (100 /25)
* function below does this.
*/
int prv_scaling_down_curves_at_level(tm_handle hndl,
									   enum tm_level level,
									   uint8_t last_curve_ind,
									   float scaling_coeff)
{
    int i;
    uint8_t j;
    uint8_t k;
    struct tm_wred_curve *curve = NULL;
    int rc = 0;

    TM_CTL(ctl, hndl)

    switch (level)
	{
    case Q_LEVEL:
        for (j=0; j<TM_NUM_WRED_QUEUE_CURVES; j++)
		{
            rc = rm_wred_queue_curve_status(ctl->rm, j);
            if (rc < 0) return rc;
            if ((rc==0) || (j == last_curve_ind)) break; /* end */

			UPDATE_CURVE(ctl->tm_wred_q_lvl_curves[j])
            /* Download to HW */
            rc = set_hw_queues_wred_curve(ctl, j);
            if (rc) return rc;
        }
        break;
    case A_LEVEL:
        for (j=0; j<TM_NUM_WRED_A_NODE_CURVES; j++)
		{
            rc = rm_wred_a_node_curve_status(ctl->rm, j);
            if (rc < 0) return rc;
            if ((rc==0) || (j == last_curve_ind)) break; /* end */

			UPDATE_CURVE(ctl->tm_wred_a_lvl_curves[j])
            /* Download to HW */
            rc = set_hw_a_nodes_wred_curve(ctl, j);
            if (rc) return rc;
        }
        break;
    case B_LEVEL:
        for (j=0; j<TM_NUM_WRED_B_NODE_CURVES; j++)
		{
            rc = rm_wred_b_node_curve_status(ctl->rm, j);
            if (rc < 0) return rc;
            if ((rc==0) || (j == last_curve_ind)) break; /* end */

			UPDATE_CURVE(ctl->tm_wred_b_lvl_curves[j])

            /* Download to HW */
            rc = set_hw_b_nodes_wred_curve(ctl, j);
            if (rc) return rc;
        }
        break;
    case C_LEVEL:
        for (k=0; k<TM_WRED_COS; k++)
		{
            for (j=0; j<TM_NUM_WRED_C_NODE_CURVES; j++)
			{
				rc = rm_wred_c_node_curve_status_cos(ctl->rm, k, j);
				if (rc < 0) return rc;
				if ((rc==0) || (j == last_curve_ind)) break; /* end */

				UPDATE_CURVE(ctl->tm_wred_c_lvl_curves[k][j])
                /* Download to HW */
                rc = set_hw_c_nodes_wred_curve(ctl, k, j);
                if (rc) return rc;
            }
        }
        break;
    case P_LEVEL:
        for (j=0; j<TM_NUM_WRED_PORT_CURVES; j++)
		{
			rc = rm_wred_port_curve_status(ctl->rm, j);
			if (rc < 0) return rc;
			if ((rc==0) || (j == last_curve_ind)) break; /* end */

			UPDATE_CURVE(ctl->tm_wred_ports_curves[j])
            /* Download to HW */
            rc = set_hw_ports_wred_curve(ctl, j);
            if (rc) return rc;
        }
        for (k=0; k<TM_WRED_COS; k++)
		{
            for (j=0; j<TM_NUM_WRED_PORT_CURVES; j++) {
                rc = rm_wred_port_curve_status_cos(ctl->rm, k, j);
				if (rc < 0) return rc;
				if ((rc==0) || (j == last_curve_ind)) break; /* end */

				UPDATE_CURVE(ctl->tm_wred_ports_curves_cos[k][j])
                /* Download to HW */
                rc = set_hw_ports_wred_curve_cos(ctl, k, j);
                if (rc)  return rc;
            }
        }
        break;
    }
    return 0;
}


/**
 */

#define CALCULATE_CURVE(curve_array_element)\
{\
    curve = &(curve_array_element);\
    for (i=0; i<32; i++)   curve->prob[i] = (uint8_t)tm_roundf(probability_points_percents[i]/current_probability_scaling*exp);\
}

static float scaling_probability[TM_MAX_PROB_12H+1] = { 100.0, 50.0, 25.0, 12.5 };

static int prv_tm_create_wred_curve_proc(tm_handle hndl,
                         enum tm_level level,
                         uint8_t cos,
                         float * probability_points_percents,
						 float max_probability,
                         uint8_t *curve_index)
{

    struct tm_wred_curve *curve;

	float current_probability_scaling;
	float old_probability_scaling;
	enum tm_max_prob_mode current_probability_scaling_enum;

	uint8_t res;
    float exp;
    int i;
    int rc;
    int curve_ind = (int)TM_INVAL;

    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    if ((uint32_t)level > P_LEVEL)
	{
        rc = -EADDRNOTAVAIL;
        goto out;
    }

    /* find free curve */
    switch (level)
	{
    case Q_LEVEL:
        curve_ind = rm_find_free_wred_queue_curve(ctl->rm);
        break;
    case A_LEVEL:
        curve_ind = rm_find_free_wred_a_node_curve(ctl->rm);
        break;
    case B_LEVEL:
        curve_ind = rm_find_free_wred_b_node_curve(ctl->rm);
        break;
    case C_LEVEL:
        if (cos >= TM_WRED_COS) {
            rc = -EDOM;
            goto out;
        }
        curve_ind = rm_find_free_wred_c_node_curve(ctl->rm, cos);
        break;
    case P_LEVEL:
        if(cos == (uint8_t)TM_INVAL)
		{
            curve_ind = rm_find_free_wred_port_curve(ctl->rm);
        }
        else
		{
            if (cos >= TM_WRED_COS) {
                rc = -EDOM;
                goto out;
            }
            curve_ind = rm_find_free_wred_port_curve_cos(ctl->rm, cos);
        }
        break;
    default:
        break;
    }

    if (curve_ind < 0)
	{
        rc = -ENOSPC;
        goto out;
    } else
        *curve_index = (uint8_t)curve_ind;


    /* Check that max_p doesn't exceed dp_max mode. If - yes, update
     * all existing curves at this level and for this color according to the new mode.
	 */
	if (max_probability > 50.0)			{ current_probability_scaling = 100.0;	current_probability_scaling_enum = TM_MAX_PROB_100; }
	else if (max_probability > 25.0) 	{ current_probability_scaling = 50.0;	current_probability_scaling_enum = TM_MAX_PROB_50;  }
	else if (max_probability > 12.5)	{ current_probability_scaling = 25.0;	current_probability_scaling_enum = TM_MAX_PROB_25;  }
	else								{ current_probability_scaling = 12.5;	current_probability_scaling_enum = TM_MAX_PROB_12H; }

	old_probability_scaling = scaling_probability[ctl->dp_unit.local[level].max_p_mode[0]];
	if (old_probability_scaling < current_probability_scaling)
	{
		/* need to rescale previous curves */

		/* update system scaling value */
        for (i=0; i<TM_MAX_PACKET_COLORS; i++)
            ctl->dp_unit.local[level].max_p_mode[i] = current_probability_scaling_enum;
        rc = set_hw_max_dp_mode(ctl);
        if (rc)
		{
            rc = TM_HW_WRED_CURVE_FAILED;
            goto out;
        }
       /* scale down all the rest of curves at this level */
        rc = prv_scaling_down_curves_at_level(ctl, level, (uint8_t)curve_ind, /* scaling_coeff = */ current_probability_scaling / old_probability_scaling);
        if (rc)
		{
            rc = TM_HW_WRED_CURVE_FAILED;
            goto out;
        }
	}
	else
	{
		/*
		 * use already calcuated probability scaling from D/B
		 * update current_scaling if necessary.
		 */
		current_probability_scaling = old_probability_scaling;
	}


    /* Calculate prob values of current curve */
    res = ctl->dp_unit.local[level].resolution;
    exp = (float)((1 << res) - 1);

    switch (level)
	{
    case Q_LEVEL:
        /* update SW image */
		CALCULATE_CURVE(ctl->tm_wred_q_lvl_curves[*curve_index])
        /* update HW */
        rc = set_hw_queues_wred_curve(hndl, *curve_index);
        break;
    case A_LEVEL:
        /* update SW image */
 		CALCULATE_CURVE(ctl->tm_wred_a_lvl_curves[*curve_index])
        /* update HW */
        rc = set_hw_a_nodes_wred_curve(hndl, *curve_index);
        break;
    case B_LEVEL:
        /* update SW image */
 		CALCULATE_CURVE(ctl->tm_wred_b_lvl_curves[*curve_index])
        /* update HW */
        rc = set_hw_b_nodes_wred_curve(hndl, *curve_index);
        break;
    case C_LEVEL:
        /* update SW image */
 		CALCULATE_CURVE(ctl->tm_wred_c_lvl_curves[cos][*curve_index])
        /* update HW */
        rc = set_hw_c_nodes_wred_curve(hndl, cos, *curve_index);
        break;
    case P_LEVEL:
         if(cos == (uint8_t)TM_INVAL)/* Global Port */
		{
	       /* update SW image */
 			CALCULATE_CURVE(ctl->tm_wred_ports_curves[*curve_index])
			/* update HW */
            rc = set_hw_ports_wred_curve(hndl, *curve_index);
		}
        else
		{
	       /* update SW image */
 			CALCULATE_CURVE(ctl->tm_wred_ports_curves_cos[cos][*curve_index])
			/* update HW */
            rc = set_hw_ports_wred_curve_cos(hndl, cos, *curve_index);
		}
    default:
        break;
    }
    if (rc)
        rc = TM_HW_WRED_CURVE_FAILED;
out:
    if (rc)
	{
        switch (level)
		{
        case Q_LEVEL:
            if (curve_ind >= 0) rm_free_wred_queue_curve(ctl->rm, *curve_index);
            if (rc == TM_HW_WRED_CURVE_FAILED) set_sw_wred_curve_default(ctl->tm_wred_a_lvl_curves,*curve_index);
            break;
        case A_LEVEL:
            if (curve_ind >= 0)	rm_free_wred_a_node_curve(ctl->rm, *curve_index);
            if (rc == TM_HW_WRED_CURVE_FAILED)	set_sw_wred_curve_default(ctl->tm_wred_a_lvl_curves,*curve_index);
            break;
        case B_LEVEL:
            if (curve_ind >= 0)	rm_free_wred_b_node_curve(ctl->rm, *curve_index);
            if (rc == TM_HW_WRED_CURVE_FAILED)	set_sw_wred_curve_default(ctl->tm_wred_b_lvl_curves,*curve_index);
            break;
        case C_LEVEL:
            if (curve_ind >= 0)	rm_free_wred_c_node_curve(ctl->rm, cos, *curve_index);
            if (rc == TM_HW_WRED_CURVE_FAILED)	set_sw_wred_curve_default(ctl->tm_wred_c_lvl_curves[cos],*curve_index);
            break;
        case P_LEVEL:
            if (curve_ind >= 0)
			{
                if(cos == (uint8_t)TM_INVAL) /* Global Port */
                    rm_free_wred_port_curve(ctl->rm, *curve_index);
                else
                    rm_free_wred_port_curve_cos(ctl->rm, cos, *curve_index);
            }
            if (rc == TM_HW_WRED_CURVE_FAILED)
			{
                if(cos == (uint8_t)TM_INVAL) /* Global Port */
                    set_sw_wred_curve_default(ctl->tm_wred_ports_curves, *curve_index);
                else
                    set_sw_wred_curve_default(ctl->tm_wred_ports_curves_cos[cos], *curve_index);
            }
            break;
        default:
            break;
        }
    }
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

int tm_create_wred_curve(tm_handle hndl,
                         enum tm_level level,
                         uint8_t cos,
                         uint8_t *prob,
                         uint8_t *curve_index_ptr)
{
	float probability_points_ps[32];
	float max_curve_probability;
	int i;
    max_curve_probability = prob[0];
    for (i=0; i<32; i++)
	{
        if (prob[i] > 100) return -EPERM;
		probability_points_ps[i]= (float)prob[i];
        if (probability_points_ps[i] > max_curve_probability)
		{
            max_curve_probability = probability_points_ps[i];
		}
    }
	return prv_tm_create_wred_curve_proc(hndl,level, cos, probability_points_ps, max_curve_probability, curve_index_ptr);
}

int tm_create_wred_traditional_curve(tm_handle hndl,
                                     enum tm_level level,
                                     uint8_t cos,
                                     float max_curve_probability,
                                     uint8_t *curve_index_ptr)
{
	float probability_points_ps[32];
	int i;
    if ((max_curve_probability == 0.0) || (max_curve_probability > 100.0))  return -EPERM;
    for (i=0; i<32; i++)
	{
		probability_points_ps[i]= max_curve_probability * (i+1) /32;
    }
	return prv_tm_create_wred_curve_proc(hndl,level, cos, probability_points_ps, max_curve_probability, curve_index_ptr);
}



/**
*/

#define	DROP_MAX_THRESHOLD_BURSTS	DROP_TD_THRESHOLD_FIELD_MAX_VALUE*DROP_TD_RESOLUTION_MULTIPLYER


int __validate_drop_profile_params (struct tm_drop_profile_params * drop_profile_params,    uint8_t color_num)
{
	int i;
	if ( (drop_profile_params->cbtdBytes / DROP_MEMORY_BURST_SIZE) > DROP_MAX_THRESHOLD_BURSTS )  return -EACCES;
	switch (drop_profile_params->drop_mode)
	{
		case REGULAR_TD_ONLY:
			break;
		case CATD:
			for (i=0; i<color_num; i++)
			{
				if ( (drop_profile_params->caTdDp.catdBytes[i] / DROP_MEMORY_BURST_SIZE) > DROP_MAX_THRESHOLD_BURSTS ) return -EACCES;
			}
			break;
		case WRED:
			{
				if (drop_profile_params->wredDp.aqlExponent > 0xF)		return -EFAULT;
				for (i=0; i<color_num; i++)
				{
					if (drop_profile_params->wredDp.caWredTdMaxThreshold[i] < drop_profile_params->wredDp.caWredTdMinThreshold[i]) return -ERANGE;
					if (3 == color_num)
					{
						if (drop_profile_params->wredDp.dpCurveScale[i] > 15) /* 4 bits */ return -ERANGE;
					}
					else
					{
						if (drop_profile_params->wredDp.dpCurveScale[i] > 63) /* 6 bits */ return -ERANGE;
					}
				}
				break;
			}

		default : return -ENODATA;
	}
	return 0;
}


#define SETUP_UNUSED_COLORS(first_unused_color) \
    for (i=first_unused_color; i<3; i++) \
	{\
        drop_profile->curve_id[i] = 0;\
        drop_profile->scale_exp[i] = 22;\
        drop_profile->scale_ratio[i] = 0;\
        drop_profile->min_threshold[i] = 1023;\
        drop_profile->dp_ratio[i] = 0;\
    }\

#define SET_TD_THRESHOLD(threshold_value_bursts)	\
{\
	if (threshold_value_bursts  == DROP_MAX_THRESHOLD_BURSTS)\
	{\
		drop_profile->td_thresh_res = TM_ENABLE;\
		drop_profile->td_threshold_bursts = DROP_TD_THRESHOLD_FIELD_MAX_VALUE;\
	}\
	else\
	{\
		if (threshold_value_bursts > DROP_TD_THRESHOLD_FIELD_MAX_VALUE )\
		{\
			drop_profile->td_thresh_res = TM_ENABLE;\
			drop_profile->td_threshold_bursts = threshold_value_bursts / DROP_TD_RESOLUTION_MULTIPLYER ;\
		}\
		else\
		{\
			drop_profile->td_thresh_res = TM_DISABLE;\
			drop_profile->td_threshold_bursts = threshold_value_bursts;\
		}\
	}\
}

void __init_drop_profile(struct tm_drop_profile_params *profile_params, uint8_t color_num, struct tm_drop_profile *drop_profile)
{
	int i;
	uint8_t exp;
    double max_thresh_scaled = 0;
    double min_thresh_scaled = 0;

	SET_TD_THRESHOLD(profile_params->cbtdBytes / DROP_MEMORY_BURST_SIZE)
	switch (profile_params->drop_mode)
	{
		case REGULAR_TD_ONLY:
			drop_profile->color_td_en = TM_DISABLE;
			drop_profile->aql_exp = 0;
            /* Disable WRED for all colors */
			SETUP_UNUSED_COLORS(0)
			break;
		case CATD:
            drop_profile->color_td_en = TM_ENABLE;

            for (i=0; i<color_num; i++)
			{
                for (exp=0; exp<22; exp++)
				{
                    min_thresh_scaled = 1.0 * profile_params->caTdDp.catdBytes[i] / DROP_MEMORY_BURST_SIZE / (1<<exp);
                    if (min_thresh_scaled < 1024)
                        break;
                }
                drop_profile->min_threshold[i] = (uint16_t)min_thresh_scaled;

                drop_profile->curve_id[i] = 0;
                drop_profile->scale_exp[i] = exp;
                drop_profile->scale_ratio[i] = 0;
                drop_profile->dp_ratio[i] = 0;
            }
            /* Disable WRED for not used colors */
			SETUP_UNUSED_COLORS(color_num)
			break;
		case WRED:
            drop_profile->color_td_en = TM_DISABLE;
            /* calculate min_th, scale_exp and scale_ratio for each color */
            for (i=0; i<color_num; i++)
			{
                drop_profile->curve_id[i] = profile_params->wredDp.curveIndex[i];

                for (exp=0; exp<22; exp++) {
					max_thresh_scaled = 1.0 * profile_params->wredDp.caWredTdMaxThreshold[i] / DROP_MEMORY_BURST_SIZE / (1<<exp);
                    if (max_thresh_scaled < 1024)
                        break;
                }
				min_thresh_scaled = 1.0 * profile_params->wredDp.caWredTdMinThreshold[i] / DROP_MEMORY_BURST_SIZE / (1<<exp);
                drop_profile->scale_exp[i] = exp;

              /*  ScaleRatioColor = 1024 * 32 / (MaxTH - MinTH). */
                if (max_thresh_scaled == min_thresh_scaled)
                    drop_profile->scale_ratio[i] = 1023;
                else
				{
                    uint32_t ratio;
                    ratio = (uint32_t)tm_round(1024*32/(max_thresh_scaled - min_thresh_scaled+1));
                    if (ratio > 1023)
                        drop_profile->scale_ratio[i] = 1023;
                    else
                        drop_profile->scale_ratio[i] = ratio;
                }
                drop_profile->min_threshold[i] = (uint16_t)min_thresh_scaled;
                drop_profile->dp_ratio[i] = profile_params->wredDp.dpCurveScale[i];
            }
            /* Disable WRED for not used colors */
			SETUP_UNUSED_COLORS(color_num)
			break;
		/* can't happen,  drop mode already validated */
		default : break;
	}
}

void __update_drop_profile_cache(struct tm_drop_profile_params *profile_params, uint8_t color_num, struct tm_drop_profile *drop_profile)
{
	(void)color_num;
/*
	int i;
    int first_unused_color = color_num;

	if(profile_params->drop_mode == REGULAR_TD_ONLY)
	    first_unused_color = 0;
*/
    /* cache user input values */
    tm_memcpy(&(drop_profile->drop_profile_source),profile_params,sizeof(struct tm_drop_profile_params));

/* clear unused colors cached params
    for (i = first_unused_color; i < 3; i++)
    {
        drop_profile->drop_profile_source.wredDp.curveIndex[i] = 0;
        drop_profile->drop_profile_source.wredDp.dpCurveScale[i] = 0;
        drop_profile->drop_profile_source.wredDp.caWredTdMinThreshold[i] = 0;
        drop_profile->drop_profile_source.wredDp.caWredTdMaxThreshold[i] = 0;
        drop_profile->drop_profile_source.caTdDp.catdBytes[i] = 0;
    }
*/
}

int tm_create_drop_profile(tm_handle hndl,
                           enum tm_level level,
                           uint8_t cos,
                           struct tm_drop_profile_params *profile_params,
                           uint16_t *prof_index)
{

    struct tm_drop_profile *drop_profile;
    int i;
    int rc;
    int prof_ind = (int)TM_INVAL;

    TM_CTL(ctl, hndl)


    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    if ((uint32_t)level > P_LEVEL)
	{
        rc = -EPERM;
        goto out;
    }

#if 0 /* TBD for TM2TM? */
    if (ctl->dp_unit.local[level].configured == TM_DISABLE) {
        rc = -EBADMSG;
        goto out;
    }
#endif
	rc = __validate_drop_profile_params (profile_params, ctl->dp_unit.local[level].color_num);
	if (rc) goto out;

	if (profile_params->drop_mode == WRED)
	{
		/* check if the assigned WRED curve exists */
		for (i=0; i < ctl->dp_unit.local[level].color_num ; i++)
		{

			switch (level)
			{
			case Q_LEVEL:
				TEST_RESOURCE_EXISTS(rm_wred_queue_curve_status(ctl->rm, profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
				if (rc) goto out;
				break;
			case A_LEVEL:
				TEST_RESOURCE_EXISTS(rm_wred_a_node_curve_status(ctl->rm, profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
				if (rc) goto out;
				break;
			case B_LEVEL:
				TEST_RESOURCE_EXISTS(rm_wred_b_node_curve_status(ctl->rm, profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
				if (rc) goto out;
				break;
			case C_LEVEL:
				TEST_RESOURCE_EXISTS(rm_wred_c_node_curve_status_cos(ctl->rm, cos, profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
				if (rc) goto out;
				break;
			case P_LEVEL:
				if(cos == (uint8_t)TM_INVAL) /* Global Port */
				{
					TEST_RESOURCE_EXISTS(rm_wred_port_curve_status(ctl->rm, profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
					if (rc) goto out;
				}
				else
				{
					TEST_RESOURCE_EXISTS(rm_wred_port_curve_status_cos(ctl->rm, cos,profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
					if (rc) goto out;
				}
				break;
			default:
				break;
			}
		}
    }

    /* find free profile */
    switch (level)
	{
    case Q_LEVEL:
        prof_ind = rm_find_free_queue_drop_profile(ctl->rm);
        break;
    case A_LEVEL:
        prof_ind = rm_find_free_a_node_drop_profile(ctl->rm);
        break;
    case B_LEVEL:
        prof_ind = rm_find_free_b_node_drop_profile(ctl->rm);
        break;
    case C_LEVEL:
        prof_ind = rm_find_free_c_node_drop_profile(ctl->rm, cos);
        break;
    case P_LEVEL:
        if(cos == (uint8_t)TM_INVAL) /* Global Port */
            prof_ind = rm_find_free_port_drop_profile(ctl->rm);
        else
            prof_ind = rm_find_free_port_drop_profile_cos(ctl->rm, cos);
        break;
    default:
        break;
    }
    if (prof_ind < 0)
    {
        rc = -ENOSPC;
        goto out;
    }

    *prof_index = prof_ind;

    switch (level)
	{
    case Q_LEVEL:
        drop_profile = &(ctl->tm_q_lvl_drop_profiles[*prof_index]);
        break;
    case A_LEVEL:
        drop_profile = &(ctl->tm_a_lvl_drop_profiles[*prof_index]);
        break;
    case B_LEVEL:
        drop_profile = &(ctl->tm_b_lvl_drop_profiles[*prof_index]);
        break;
    case C_LEVEL:
        drop_profile = &(ctl->tm_c_lvl_drop_profiles[cos][*prof_index]);
        break;
    case P_LEVEL:
        if(cos == (uint8_t)TM_INVAL) /* Global Port */
            drop_profile = &(ctl->tm_p_lvl_drop_profiles[*prof_index]);
        else
            drop_profile = &(ctl->tm_p_lvl_drop_profiles_cos[cos][*prof_index]);
        break;
    default:
        rc = -EPERM;
        goto out;
        break;
    }

	__init_drop_profile(profile_params, ctl->dp_unit.local[level].color_num, drop_profile);

	/* update HW */
    switch (level)
	{
    case Q_LEVEL:
        rc = set_hw_queue_drop_profile(hndl, *prof_index);
        break;
    case A_LEVEL:
        rc = set_hw_a_nodes_drop_profile(hndl, *prof_index);
        break;
    case B_LEVEL:
        rc = set_hw_b_nodes_drop_profile(hndl, *prof_index);
        break;
    case C_LEVEL:
        rc = set_hw_c_nodes_drop_profile(hndl, cos, *prof_index);
        break;
    case P_LEVEL:
        /* Port Drop profile should be set to hw later in time of port's
         * creation */
        break;
    default:
        break;
    }
    if (rc) {
        rc = TM_HW_DROP_PROFILE_FAILED;
        goto out;
    }

    __update_drop_profile_cache(profile_params, ctl->dp_unit.local[level].color_num, drop_profile);

out:
    if (rc)
	{
        switch (level)
		{
        case Q_LEVEL:
            if (prof_ind >= 0)
                rm_free_queue_drop_profile(ctl->rm, *prof_index);
            if (rc == TM_HW_DROP_PROFILE_FAILED)
                set_sw_drop_profile_default(ctl->tm_q_lvl_drop_profiles, *prof_index);
            break;
        case A_LEVEL:
            if (prof_ind >= 0)
                rm_free_a_node_drop_profile(ctl->rm, *prof_index);
            if (rc == TM_HW_DROP_PROFILE_FAILED)
                set_sw_drop_profile_default(ctl->tm_a_lvl_drop_profiles, *prof_index);
            break;
        case B_LEVEL:
            if (prof_ind >= 0)
                rm_free_b_node_drop_profile(ctl->rm, *prof_index);
            if (rc == TM_HW_DROP_PROFILE_FAILED)
                set_sw_drop_profile_default(ctl->tm_b_lvl_drop_profiles, *prof_index);
            break;
        case C_LEVEL:
            if (prof_ind >= 0)
                rm_free_c_node_drop_profile(ctl->rm, cos, *prof_index);
            if (rc == TM_HW_DROP_PROFILE_FAILED)
                set_sw_drop_profile_default(ctl->tm_c_lvl_drop_profiles[cos], *prof_index);
            break;
        case P_LEVEL:
            if (prof_ind >= 0)
            {
                if(cos == (uint8_t)TM_INVAL) /* Global Port */
                    rm_free_port_drop_profile(ctl->rm, *prof_index);
                else
                    rm_free_port_drop_profile_cos(ctl->rm, cos, *prof_index);
            }
            if (rc == TM_HW_DROP_PROFILE_FAILED)
			{
				if(cos == (uint8_t)TM_INVAL) /* Global Port */
					set_sw_drop_profile_default(ctl->tm_p_lvl_drop_profiles, *prof_index);
				else
					set_sw_drop_profile_default(ctl->tm_p_lvl_drop_profiles_cos[cos], *prof_index);
            }
            break;
        default:
            break;
        }
    }
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}


int tm_update_drop_profile(tm_handle hndl,
                           enum tm_level level,
                           uint8_t cos,
						   uint16_t prof_index,
                           struct tm_drop_profile_params *profile_params)
{

    struct tm_drop_profile *drop_profile;
    int i;
    int rc;
    uint8_t color_num;

    TM_CTL(ctl, hndl)


    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    if ((uint32_t)level > P_LEVEL)
	{
        rc = -EPERM;
        goto out;
    }

#if 0 /* TBD for TM2TM? */
    if (ctl->dp_unit.local[level].configured == TM_DISABLE) {
        rc = -EBADMSG;
        goto out;
    }
#endif

    color_num = ctl->dp_unit.local[level].color_num;

	rc = __validate_drop_profile_params (profile_params, color_num);
	if (rc) goto out;

	if (profile_params->drop_mode == WRED)
	{
		for (i=0; i < color_num ; i++)
		{
			/* check if the assigned WRED curve exists */
			switch (level)
			{
			case Q_LEVEL:
				TEST_RESOURCE_EXISTS(rm_wred_queue_curve_status(ctl->rm, profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
				if (rc) goto out;
				break;
			case A_LEVEL:
				TEST_RESOURCE_EXISTS(rm_wred_a_node_curve_status(ctl->rm, profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
				if (rc) goto out;
				break;
			case B_LEVEL:
				TEST_RESOURCE_EXISTS(rm_wred_b_node_curve_status(ctl->rm, profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
				if (rc) goto out;
				break;
			case C_LEVEL:
				TEST_RESOURCE_EXISTS(rm_wred_c_node_curve_status_cos(ctl->rm, cos, profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
				if (rc) goto out;
				break;
			case P_LEVEL:
				if(cos == (uint8_t)TM_INVAL)
				{
					TEST_RESOURCE_EXISTS(rm_wred_port_curve_status(ctl->rm, profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
					if (rc) goto out;
				}
				else
				{
					TEST_RESOURCE_EXISTS(rm_wred_port_curve_status_cos(ctl->rm, cos, profile_params->wredDp.curveIndex[i]),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
					if (rc) goto out;
				}
				break;
			default:
				break;
			}
		}
    }

    /* test drop profile existence */
    switch (level)
	{
    case Q_LEVEL:
		TEST_RESOURCE_EXISTS(rm_queue_drop_profile_status(ctl->rm, prof_index),/* out_of range error*/TM_CONF_Q_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_Q_WRED_PROF_REF_OOR)
		if (rc) goto out;
		drop_profile = &(ctl->tm_q_lvl_drop_profiles[prof_index]);
		break;
    case A_LEVEL:
 		TEST_RESOURCE_EXISTS(rm_a_node_drop_profile_status(ctl->rm, prof_index),/* out_of range error*/TM_CONF_A_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_A_WRED_PROF_REF_OOR)
		if (rc) goto out;
        drop_profile = &(ctl->tm_a_lvl_drop_profiles[prof_index]);
        break;
    case B_LEVEL:
 		TEST_RESOURCE_EXISTS(rm_b_node_drop_profile_status(ctl->rm, prof_index),/* out_of range error*/TM_CONF_B_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_B_WRED_PROF_REF_OOR)
		if (rc) goto out;
        drop_profile = &(ctl->tm_b_lvl_drop_profiles[prof_index]);
        break;
    case C_LEVEL:
		TEST_RESOURCE_EXISTS(rm_c_node_drop_profile_status_cos(ctl->rm, cos, prof_index),/* out_of range error*/TM_CONF_C_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_C_WRED_PROF_REF_OOR)
		if (rc) goto out;
        drop_profile = &(ctl->tm_c_lvl_drop_profiles[cos][prof_index]);
        break;
	case P_LEVEL:
        if(cos == (uint8_t)TM_INVAL) /* Global Port */
		{
 			TEST_RESOURCE_EXISTS(rm_port_drop_profile_status(ctl->rm,  prof_index),/* out_of range error*/TM_CONF_P_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_P_WRED_PROF_REF_OOR)
			if (rc) goto out;
            drop_profile = &(ctl->tm_p_lvl_drop_profiles[prof_index]);
		}
        else
		{
			TEST_RESOURCE_EXISTS(rm_port_drop_profile_status_cos(ctl->rm, cos, prof_index),/* out_of range error*/TM_CONF_P_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_P_WRED_PROF_REF_OOR)
			if (rc) goto out;
            drop_profile = &(ctl->tm_p_lvl_drop_profiles_cos[cos][prof_index]);
		}
        break;
    default:
        rc = -EPERM;
        goto out;
        break;
    }
 	if (rc) goto out;

	__init_drop_profile(profile_params, color_num, drop_profile);

	/* update HW */
    switch (level)
	{
    case Q_LEVEL:
        rc = set_hw_queue_drop_profile(hndl, prof_index);
        break;
    case A_LEVEL:
        rc = set_hw_a_nodes_drop_profile(hndl, prof_index);
        break;
    case B_LEVEL:
        rc = set_hw_b_nodes_drop_profile(hndl, prof_index);
        break;
    case C_LEVEL:
        rc = set_hw_c_nodes_drop_profile(hndl, cos, prof_index);
        break;
    case P_LEVEL:
		/* profile can be updated only if it hasn't clients */
		if (drop_profile->use_counter)
		{
			rc = -EPERM;
			goto out;
		}
        break;
    default:
        break;
    }
    if (rc) {
        rc = TM_HW_DROP_PROFILE_FAILED;
        goto out;
    }

    __update_drop_profile_cache(profile_params, ctl->dp_unit.local[level].color_num, drop_profile);

out:
    if (rc)
	{
/*       nothing to do  */
    }
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}


/**
*/
int tm_delete_drop_profile(tm_handle hndl,
                           enum tm_level level,
                           uint8_t cos,
                           uint16_t prof_index)
{
    int rc;
    TM_CTL(ctl ,hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    if ((uint32_t)level > P_LEVEL)
	{
        rc = -EDOM;
        goto out;
    }

    if (prof_index == TM_NO_DROP_PROFILE)
	{ /* reserved profile */
        rc = -EPERM;
        goto out;
    }

    switch (level)
	{
    case Q_LEVEL:
        if (prof_index >= TM_NUM_QUEUE_DROP_PROF)
		{
            rc = -EADDRNOTAVAIL;
            goto out;
        }

        if (ctl->tm_q_lvl_drop_profiles[prof_index].use_counter > 0)
		{
            rc = -EPERM;
            goto out;
        }

        /* free profile's entry in the RM (status is checked inside) */
        rc = rm_free_queue_drop_profile(ctl->rm, prof_index);
        if (rc < 0)
		{
            rc = -EFAULT;
            goto out;
        }

        /* set SW image entry to default values */
        set_sw_drop_profile_default(ctl->tm_q_lvl_drop_profiles, prof_index);
        break;
    case A_LEVEL:
        if (prof_index >= TM_NUM_A_NODE_DROP_PROF)
		{
            rc = -EADDRNOTAVAIL;
            goto out;
        }

        if (ctl->tm_a_lvl_drop_profiles[prof_index].use_counter > 0)
		{
            rc = -EPERM;
            goto out;
        }
        /* free profile's entry in the RM (status is checked inside) */
        rc = rm_free_a_node_drop_profile(ctl->rm, prof_index);
        if (rc < 0)
		{
            rc = -EFAULT;
            goto out;
        }

        /* set SW image entry to default values */
        set_sw_drop_profile_default(ctl->tm_a_lvl_drop_profiles, prof_index);
        break;
    case B_LEVEL:
        if (prof_index >= TM_NUM_B_NODE_DROP_PROF)
		{
            rc = -EADDRNOTAVAIL;
            goto out;
        }

        if (ctl->tm_b_lvl_drop_profiles[prof_index].use_counter > 0)
		{
            rc = -EPERM;
            goto out;
        }
        /* free profile's entry in the RM (status is checked inside) */
        rc = rm_free_b_node_drop_profile(ctl->rm, prof_index);
        if (rc < 0)
		{
            rc = -EFAULT;
            goto out;
        }

        /* set SW image entry to default values */
        set_sw_drop_profile_default(ctl->tm_b_lvl_drop_profiles, prof_index);
        break;
    case C_LEVEL:
        if (prof_index >= TM_NUM_C_NODE_DROP_PROF)
		{
            rc = -EADDRNOTAVAIL;
            goto out;
        }

        if (cos >= TM_WRED_COS)
		{
            rc = -ENODEV;
            goto out;
        }

        if (ctl->tm_c_lvl_drop_profiles[cos][prof_index].use_counter > 0) {
            rc = -EPERM;
            goto out;
        }
        /* free profile's entry in the RM (status is checked inside) */
        rc = rm_free_c_node_drop_profile(ctl->rm, cos, prof_index);
        if (rc < 0)
		{
            rc = -EFAULT;
            goto out;
        }

        /* set SW image entry to default values */
        set_sw_drop_profile_default(ctl->tm_c_lvl_drop_profiles[cos],
                                    prof_index);
        break;
    case P_LEVEL:
        if (prof_index >= ctl->tm_total_ports)
		{
            rc = -EADDRNOTAVAIL;
            goto out;
        }

        if(cos == (uint8_t)TM_INVAL){ /* Global Port */
            if (ctl->tm_p_lvl_drop_profiles[prof_index].use_counter > 0) {
                rc = -EPERM;
                goto out;
            }

            /* free profile's entry in the RM (status is checked inside) */
            rc = rm_free_port_drop_profile(ctl->rm, prof_index);
            if (rc < 0) {
                rc = -EFAULT;
                goto out;
            }

            /* set SW image entry to default values */
            set_sw_drop_profile_default(ctl->tm_p_lvl_drop_profiles, prof_index);
        } else {
            if (cos >= TM_WRED_COS) {
                rc = -ENODEV;
                goto out;
            }

            if (ctl->tm_p_lvl_drop_profiles_cos[cos][prof_index].use_counter > 0) {
                rc = -EPERM;
                goto out;
            }
            /* free profile's entry in the RM (status is checked inside) */
            rc = rm_free_port_drop_profile_cos(ctl->rm, cos, prof_index);
            if (rc < 0) {
                rc = -EFAULT;
                goto out;
            }

            /* set SW image entry to default values */
            set_sw_drop_profile_default(ctl->tm_p_lvl_drop_profiles_cos[cos], prof_index);
        }
        break;
    default:
        break;
    }

    /* set HW to default */
    switch (level)
	{
    case Q_LEVEL:
        rc = set_hw_queue_drop_profile(hndl, prof_index);
        break;
    case A_LEVEL:
        rc = set_hw_a_nodes_drop_profile(hndl, prof_index);
        break;
    case B_LEVEL:
        rc = set_hw_b_nodes_drop_profile(hndl, prof_index);
        break;
    case C_LEVEL:
        rc = set_hw_c_nodes_drop_profile(hndl, cos, prof_index);
        break;
    default:
        break;
    }
    if (rc)
        rc = TM_HW_DROP_PROFILE_FAILED;
out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}


/**
*/

int tm_read_drop_profile(tm_handle hndl,
                         enum tm_level level,
                         uint8_t cos,
                         uint16_t prof_index,
                         struct tm_drop_profile_params *profile)
{
    int rc;
    struct tm_drop_profile *drop_profile = NULL;
    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc) return rc;

    if ((uint32_t)level > P_LEVEL)
	{
        rc = -EPERM;
        goto out;
    }

    switch (level)
	{
    case Q_LEVEL:
		TEST_RESOURCE_EXISTS(rm_queue_drop_profile_status(ctl->rm, prof_index),/* out_of range error*/-EADDRNOTAVAIL,/*profile not used error*/-EFAULT)
		if (rc) goto out;
		drop_profile = &(ctl->tm_q_lvl_drop_profiles[prof_index]);
        break;
    case A_LEVEL:
		TEST_RESOURCE_EXISTS(rm_a_node_drop_profile_status(ctl->rm, prof_index),/* out_of range error*/-EADDRNOTAVAIL,/*profile not used error*/-EFAULT)
		if (rc) goto out;
		drop_profile = &(ctl->tm_a_lvl_drop_profiles[prof_index]);
		break;
    case B_LEVEL:
		TEST_RESOURCE_EXISTS(rm_b_node_drop_profile_status(ctl->rm, prof_index),/* out_of range error*/-EADDRNOTAVAIL,/*profile not used error*/-EFAULT)
		if (rc) goto out;
		drop_profile = &(ctl->tm_b_lvl_drop_profiles[prof_index]);
        break;
    case C_LEVEL:
		TEST_RESOURCE_EXISTS(rm_c_node_drop_profile_status_cos(ctl->rm, cos, prof_index),/* out_of range error*/-EADDRNOTAVAIL,/*profile not used error*/-EFAULT)
		if (rc) goto out;
		drop_profile = &(ctl->tm_c_lvl_drop_profiles[cos][prof_index]);
        break;
    case P_LEVEL:
        if(cos == (uint8_t)TM_INVAL)
        { /* Global Port */
			TEST_RESOURCE_EXISTS(rm_port_drop_profile_status(ctl->rm, prof_index),/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/ -EFAULT)
			if (rc) goto out;
			drop_profile = &(ctl->tm_p_lvl_drop_profiles[prof_index]);
        }
        else
        {
			TEST_RESOURCE_EXISTS(rm_port_drop_profile_status_cos(ctl->rm,cos,prof_index) ,/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/ -EFAULT)
			if (rc) goto out;
			drop_profile = &(ctl->tm_p_lvl_drop_profiles_cos[cos][prof_index]);break;
        }
        break;
    default:
        rc = -EPERM;
        goto out;
        break;
    }

	tm_memcpy(profile,&(drop_profile->drop_profile_source),sizeof(struct tm_drop_profile_params));


out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_set_drop_color_num(tm_handle hndl,
                          enum tm_level lvl,
                          enum tm_color_num num)
{
    int rc;

    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;
    /* check parameters validity */
    if ((uint32_t)lvl > P_LEVEL)
	{
        rc = -EPERM;
        goto out;
    }

    if ((uint32_t)num > TM_3_COLORS)
	{
        rc = -EFAULT;
        goto out;
    }

    switch (num)
	{
    case TM_1_COLORS:
        ctl->dp_unit.local[lvl].color_num = 1;
        ctl->dp_unit.local[lvl].resolution = 6;
        break;
    case TM_2_COLORS:
        ctl->dp_unit.local[lvl].color_num = 2;
        ctl->dp_unit.local[lvl].resolution = 6;
        break;
    case TM_3_COLORS:
        ctl->dp_unit.local[lvl].color_num = 3;
        ctl->dp_unit.local[lvl].resolution = 4;
        break;
    }

    /* TBD: how to reduce the num of these calls? */
    rc = set_hw_drop_color_num(hndl);
    if (rc)
        rc = TM_HW_COLOR_NUM_CONFIG_FAIL;

out:
   tm_glob_unlock(TM_ENV(ctl));
    return rc;
}


int tm_get_drop_color_num_and_resolution(tm_handle hndl,
										  enum tm_level lvl,
										  enum tm_color_num  * num,
										  uint8_t * resolution)
{
    TM_CTL(ctl, hndl)

    if ((uint32_t)lvl > P_LEVEL)      return -EPERM;

	switch(ctl->dp_unit.local[lvl].color_num)
	{
		case 3:
			*num = TM_3_COLORS;
			*resolution = 4;
			break;
		case 2:
			*num = TM_2_COLORS;
			*resolution = 6;
			break;
		case 1:
		default:
			*num = TM_1_COLORS;
			*resolution = 6;
			break;
	}
	return 0;
}






 /**
 */
int tm_aging_change_status(tm_handle hndl, uint8_t status)
{
    int rc;
    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    /* check parameters validity */
    if ((status != TM_ENABLE) && (status != TM_DISABLE)) {
        rc = -EFAULT;
        goto out;
    }

    ctl->aging_status = status; /* TM_ENABLE/TM_DISABLE */

    rc = set_hw_aging_status(ctl);
    if (rc < 0)
        rc = TM_HW_AGING_CONFIG_FAIL;
out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

/**
 */
int tm_aging_status_get(tm_handle hndl, uint8_t * status)
{
    int rc = 0;
    TM_CTL(ctl, hndl)

    rc = set_hw_aging_status(ctl);
    if (rc < 0)
        rc = TM_HW_AGING_CONFIG_FAIL;
	else 
		*status = ctl->aging_status ;
    return rc;
}


/**
 */
int tm_dp_source_set(tm_handle hndl,
                     enum tm_level lvl,
                     uint8_t color,
                     enum tm_dp_source src)
{
    enum tm_dp_source src_old;
    int rc;
    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

	/* check parameters validity */
    if ((uint32_t)lvl > P_LEVEL ) {
        rc = -EPERM;
        goto out;
    }

    if (color > 2) {
        rc = -ENODEV;
        goto out;
    }

    if ((uint32_t)src > TM_QL) {
        rc = -EFAULT;
        goto out;
    }

    src_old = ctl->dp_unit.local[lvl].dp_src[color];
    if(src_old != src)
	{
        ctl->dp_unit.local[lvl].dp_src[color] = src;
        rc = set_hw_dp_source(ctl);
        if (rc < 0)
            rc = TM_HW_AQM_CONFIG_FAIL;
    }
out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

int tm_dp_source_get(tm_handle hndl,
                        enum tm_level lvl,
                        uint8_t color,
                        enum tm_dp_source *source)
{
    int rc;
    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

	/* check parameters validity */
    if ((uint32_t)lvl > P_LEVEL ) {
        rc = -EPERM;
        goto out;
    }

    if (color > 2) {
        rc = -ENODEV;
        goto out;
    }

    rc = get_hw_dp_source(ctl);
    if (rc < 0)
        rc = TM_HW_AQM_CONFIG_FAIL;
    *source = ctl->dp_unit.local[lvl].dp_src[color];

out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_set_drop_query_responce(tm_handle hndl,
                               uint8_t port_dp,
                               enum tm_level lvl)
{
    int rc;

    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    /* check parameters validity */
    if ((uint32_t)lvl > P_LEVEL) {
        rc = -EPERM;
        goto out;
    }

    if ((port_dp != TM_ENABLE) && (port_dp != TM_DISABLE)) {
        rc = -EFAULT;
        goto out;
    }

    rc = set_hw_dp_local_resp(ctl, port_dp, lvl);
    if (rc)
        rc = TM_HW_DP_QUERY_RESP_CONF_FAILED;
out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

/**
 */
int tm_get_drop_query_responce(tm_handle hndl,
                               uint8_t * port_dp,
                               enum tm_level * lvl)
{
    int rc;

    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    rc = get_hw_dp_local_resp(ctl, port_dp, lvl);
    if (rc)
        rc = TM_HW_DP_QUERY_RESP_CONF_FAILED;
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

/**
 */
int tm_set_drop_queue_cos(tm_handle hndl,
                          uint32_t index,
                          uint8_t cos)
{
    int rc;

    TM_CTL(ctl, hndl);

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)  return rc;

    /* check parameters validity */
    if (index >= ctl->tm_total_queues) {
        rc = -EFAULT;
        goto out;
    }

    if (cos >= TM_WRED_COS)
    {
        rc = -EFAULT;
        goto out;
    }

    rc = set_hw_queue_cos(ctl, index, cos);
    if (rc)
        rc = TM_HW_QUEUE_COS_CONF_FAILED;
out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

/**
 */
int tm_get_drop_queue_cos(tm_handle hndl,
                          uint32_t index,
                          uint8_t * cos)
{
    int rc;

    TM_CTL(ctl, hndl);

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)  return rc;

    /* check parameters validity */
    if (index >= ctl->tm_total_queues) {
        rc = -EFAULT;
        goto out;
    }

    rc = get_hw_queue_cos(ctl, index, cos);
    if (rc)
        rc = TM_HW_QUEUE_COS_CONF_FAILED;
out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}


/**
*/
int tm_create_aging_profile(tm_handle hndl,
                           struct tm_drop_profile_params profile_arr[TM_AGING_PROFILES],
                           uint16_t *prof_index)
{
    struct tm_drop_profile* node_profile_arr[TM_AGING_PROFILES];
    int i;
    int j;
    int rc;
    uint8_t color_num;
    int prof_ind = (int)TM_INVAL;
    uint16_t  prof_index_arr[TM_AGING_PROFILES];

    TM_CTL(ctl, hndl)


    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc) return rc;

#if 0 /* TBD for TM2TM? */
    if (ctl->dp_unit.local[Q_LEVEL].configured == TM_DISABLE) {
        rc = -EBADMSG;
        goto out;
    }
#endif

	color_num = ctl->dp_unit.local[Q_LEVEL].color_num;

    for (j = 0; j < TM_AGING_PROFILES; j++)
    {
		rc = __validate_drop_profile_params (&(profile_arr[j]), color_num);
		if (rc) goto out;
    }

	for (j = 0; j < TM_AGING_PROFILES; j++)
    {
		if (profile_arr[j].drop_mode == WRED)
		{
			for (i = 0; i < 3; i++)
			{
				TEST_RESOURCE_EXISTS(rm_wred_queue_curve_status(ctl->rm, profile_arr[j].wredDp.curveIndex[i]),/* out_of range error*/-EADDRNOTAVAIL, /*profile not used error*/-ENODEV)
				if (rc) goto out;
			}
		}
    }

	/* find free profile */
    prof_ind = rm_find_free_queue_aging_profile(ctl->rm);
    if (prof_ind < 0)
    {
        rc = -ENOSPC;
        goto out;
    }

    *prof_index = prof_ind;

    for (j = 0; j < TM_AGING_PROFILES; j++)
    {
        prof_index_arr[j] = prof_ind+j;
        node_profile_arr[j] = &(ctl->tm_q_lvl_drop_profiles[prof_index_arr[j]]);
                /* update SW image */
		__init_drop_profile(&(profile_arr[j]) , color_num, node_profile_arr[j]);
	}

	/* update HW */
    for (j = 0; j < TM_AGING_PROFILES; j++)
    {
        rc = set_hw_queue_drop_profile(hndl, prof_index_arr[j]);
        if (rc)
            rc = TM_HW_DROP_PROFILE_FAILED;
    }
    /* On success, cache user input values. */
    for (j = 0; j < TM_AGING_PROFILES; j++)
    {
		tm_memcpy(&(node_profile_arr[j]->drop_profile_source),&(profile_arr[j]), sizeof(struct tm_drop_profile_params));
	}

out:
    if (rc)
    {
        for (j = 0; j < TM_AGING_PROFILES; j++)
        {
            if (prof_ind >= 0)
                rm_free_queue_drop_profile(ctl->rm, prof_index_arr[j]);
            if (rc == TM_HW_DROP_PROFILE_FAILED)
                set_sw_drop_profile_default(ctl->tm_q_lvl_drop_profiles, prof_index_arr[j]);
        }
    }

    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

int tm_update_aging_profile(tm_handle hndl,
							uint16_t prof_index,
							uint16_t offset , /* 0 :: 3 */
                           struct tm_drop_profile_params  * profile_params)
{

    TM_CTL(ctl, hndl)

	/* validate that prof_index  points to aging profile */
	if (rm_validate_queue_aging_profile(ctl->rm, prof_index)) return -ENOSPC;
	if (offset >3) return -ENOSPC;
	/* profile index is valid aging profile */
	return  tm_update_drop_profile(hndl, Q_LEVEL, 0, prof_index+offset, profile_params);
}


int tm_read_node_drop_profile(tm_handle hndl, enum tm_level lvl ,  uint8_t cos, uint32_t node_index, uint16_t * prof_index)
{
    int rc;
    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc) return rc;

    switch (lvl)
	{
    case Q_LEVEL:
		VALIDATE_NODE_ALLOCATED(queue, node_index)
		if(rc) goto out;
		rc = tm_get_hw_queue_drop_profile_ptr(hndl, node_index, prof_index);
        break;
    case A_LEVEL:
		VALIDATE_NODE_ALLOCATED(a_node, node_index)
		rc = tm_get_hw_a_node_drop_profile_ptr(hndl, node_index, prof_index);
		if(rc) goto out;
		break;
    case B_LEVEL:
		VALIDATE_NODE_ALLOCATED(b_node, node_index)
		rc = tm_get_hw_b_node_drop_profile_ptr(hndl, node_index, prof_index);
		if(rc) goto out;
        break;
    case C_LEVEL:
		VALIDATE_NODE_ALLOCATED(c_node, node_index)
		if (cos > 7)
		{
			rc  = -ENODEV;
			goto out;
		}
		rc = tm_get_hw_c_node_drop_profile_ptr(hndl, node_index, cos, prof_index);
        break;
    case P_LEVEL:
		VALIDATE_NODE_ALLOCATED(port, node_index)
		if(rc) goto out;
		/* ports haven't H/W drop profile register , so returned value  is extracted from S/W database */
		if (cos == (uint8_t)TM_INVAL) *prof_index = ( uint8_t)ctl->tm_port_array[node_index].wred_profile_ref;
		else
		{
			if (cos > 7)
			{
				rc  = -ENODEV;
				goto out;
			}
			if (ctl->tm_port_array[node_index].wred_cos && (1 << cos) )	*prof_index = ( uint16_t)ctl->tm_port_array[node_index].wred_profile_ref_cos[cos];
			else rc = -ENODEV;
		}
		break;
	default:
		rc = -EPERM ;
	}
out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}



/**
*/
int tm_delete_aging_profile(tm_handle hndl,
                           uint16_t prof_index)
{
    int j;
    int rc;

    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    if (prof_index == 0) { /* reserved profile */
        rc = -EPERM;
        goto out;
    }

    if (prof_index >= TM_NUM_QUEUE_DROP_PROF) {
        rc = -EADDRNOTAVAIL;
        goto out;
    }

    if (ctl->tm_q_lvl_drop_profiles[prof_index].use_counter > 0) {
        rc = -EPERM;
        goto out;
    }

    rc = rm_free_queue_aging_profile(ctl->rm, (uint16_t)(prof_index));
	if (rc < 0) {
		rc = -EFAULT;
		goto out;
	}
    /* free profile's entry in the RM (status is checked inside) */
    for (j=TM_AGING_PROFILES-1; j>=0; j--)
    {
        /* set SW image entry to default values */
        set_sw_drop_profile_default(ctl->tm_q_lvl_drop_profiles, (uint16_t)(prof_index+j));
    }

    /* set HW to default */
    for (j=0; j<TM_AGING_PROFILES; j++) {
            rc = set_hw_queue_drop_profile(hndl, prof_index+j);
        if (rc)
            rc = TM_HW_DROP_PROFILE_FAILED;
     }
out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}


/** Calculate Bandwidth Delay Product (BDP) given bw in kilo bits/s
 * and delay in seconds.
 *
 * @param[in]    bw      bandwidth in kilo bits per second (not bits/s)
 * @param[in]    delay   delay in seconds
 *
 * @return Calculated bandwidth delay product in bytes
 */

/*
uint64_t tm_bdp(uint32_t bw, double delay)
{
    if (delay <= 0)
        return 0;

    return bw * (1000LL / 8) * delay;
}
*/
/*  in formulae above : 1000~kbits, 8~bytes */



int _tm_config_default_dp_mode(tm_handle hndl, int write_to_hw)
{
    int i;
    int j;
    int rc;
    TM_CTL(ctl, hndl)

	rc=0;
    for (i=Q_LEVEL; i<=P_LEVEL; i++)
	{
        ctl->dp_unit.local[i].resolution = 6;
        ctl->dp_unit.local[i].color_num = 2;
        for (j=0; j<TM_MAX_PACKET_COLORS; j++) {
            ctl->dp_unit.local[i].max_p_mode[j] = TM_MAX_PROB_12H;
            ctl->dp_unit.local[i].dp_src[j] = TM_AQL;
        }
    }
	if (write_to_hw)
	{
		/* Download to HW */
		/* Only Max Probability Mode should be downloaded, because all
		 * the rest of parameters configured in HW by default */
		rc = set_hw_max_dp_mode(ctl);
		if (rc) rc = TM_HW_WRED_CURVE_FAILED;
	}
	return rc;
}

int _tm_config_default_aging_status(tm_handle hndl, int write_to_hw)
{
    int rc;
    TM_CTL(ctl, hndl);

	rc=0;
    ctl->aging_status = TM_DISABLE;
    if (write_to_hw)
    {
		rc = set_hw_aging_status(ctl);
		if (rc) rc = TM_HW_AGING_CONFIG_FAIL;
    }
	return rc;
}
/**
*/

#ifdef USE_DEFAULT_CURVES

	#define	DEFAULT_CURVE	0

	int _tm_config_default_curves_sw(tm_handle hndl)
	{
		uint8_t cos;
		int curve_ind;

		TM_CTL(ctl, hndl)

		curve_ind = rm_find_free_wred_queue_curve(ctl->rm);
		if (curve_ind |= DEFAULT_CURVE) return 1;
		set_sw_wred_curve_default(ctl->tm_wred_q_lvl_curves ,DEFAULT_CURVE);

		curve_ind = rm_find_free_wred_a_node_curve(ctl->rm);
		if (curve_ind |= DEFAULT_CURVE) return 1;
		set_sw_wred_curve_default(ctl->tm_wred_a_lvl_curves ,DEFAULT_CURVE);

		curve_ind = rm_find_free_wred_b_node_curve(ctl->rm);
		if (curve_ind |= DEFAULT_CURVE) return 1;
		set_sw_wred_curve_default(ctl->tm_wred_b_lvl_curves,DEFAULT_CURVE);

		for (cos=0; cos<TM_WRED_COS; cos++)
		{
			curve_ind = rm_find_free_wred_c_node_curve(ctl->rm ,cos);
			if (curve_ind |= DEFAULT_CURVE) return 1;
			set_sw_wred_curve_default(ctl->tm_wred_c_lvl_curves[cos],DEFAULT_CURVE);
		}

		curve_ind = rm_find_free_wred_port_curve(ctl->rm);
		if (curve_ind |= DEFAULT_CURVE) return 1;
		set_sw_wred_curve_default(ctl->tm_wred_ports_curves,DEFAULT_CURVE);
		for (cos=0; cos<TM_WRED_COS; cos++)
		{
			curve_ind = rm_find_free_wred_port_curve_cos(ctl->rm ,cos);
			if (curve_ind |= DEFAULT_CURVE) return 1;
			set_sw_wred_curve_default(ctl->tm_wred_ports_curves_cos[cos],DEFAULT_CURVE);
		}
		return 0;
	}

	int _tm_config_default_curves_hw(tm_handle hndl)
	{
		struct tm_wred_curve curve;
		uint8_t cos;
		int j;
		int rc = TM_HW_WRED_CURVE_FAILED;
		TM_CTL(ctl, hndl)

		for (j=0; j<32; j++)  curve.prob[j] = 0;

		rc = set_hw_queues_default_wred_curve(ctl, curve.prob);
		if (rc) return rc;
		rc = set_hw_a_nodes_default_wred_curve(ctl, curve.prob);
		if (rc) return rc;
		rc = set_hw_b_nodes_default_wred_curve(ctl, curve.prob);
		if (rc) return rc;

		for (cos=0; cos<TM_WRED_COS; cos++)
		{
			rc=set_hw_c_nodes_default_wred_curve(ctl, cos, curve.prob);
			if (rc) return rc;
		}

		rc = set_hw_ports_default_wred_curve(ctl, curve.prob);
		if (rc) return rc;
		for (cos=0; cos<TM_WRED_COS; cos++)
		{
			rc=set_hw_ports_default_wred_curve_cos(ctl, cos, curve.prob);
			if (rc) return rc;
		}

		return 0;
	}
#endif

/* each default resource (profile) must be first in resource pool (index=0) and can't be updated /deleted */
/* the high-level API calls are responsible for default resources persistence*/
/* The TM_NO_DROP_PROFILE is alias for default drop profile index */

void __set_no_drop_profile(struct tm_drop_profile * drop_profile)
{
	int i;
	drop_profile->aql_exp = 0;
	drop_profile->td_thresh_res = TM_ENABLE;
	drop_profile->td_threshold_bursts = DROP_TD_THRESHOLD_FIELD_MAX_VALUE;
	drop_profile->color_td_en = TM_DISABLE;
	SETUP_UNUSED_COLORS(0)
	/* set cashed  values for read proposes */
	tm_memset(&(drop_profile->drop_profile_source), 0, sizeof(struct tm_drop_profile_params));
	drop_profile->drop_profile_source.drop_mode = REGULAR_TD_ONLY;
	drop_profile->drop_profile_source.cbtdBytes = (uint32_t)-1;
}


int _tm_config_default_profiles_sw(tm_handle hndl)
{
    struct tm_drop_profile *profile = NULL;
    uint8_t cos;
    int default_profile_index;

    TM_CTL(ctl, hndl)
 /*
   indexes for all default drop profiles must be 0 , otherwise  set default profiles is called in the wrong way
 */
	/* Queue default drop profile set */
	default_profile_index = rm_find_free_queue_drop_profile(ctl->rm);
    if (default_profile_index |= TM_NO_DROP_PROFILE) return 1;
	profile = &(ctl->tm_q_lvl_drop_profiles[default_profile_index]);
	__set_no_drop_profile(profile);

	/* A-node default drop profile set */
	default_profile_index = rm_find_free_a_node_drop_profile(ctl->rm);
    if (default_profile_index |= TM_NO_DROP_PROFILE) return 1;
	profile = &(ctl->tm_a_lvl_drop_profiles[default_profile_index]);
	__set_no_drop_profile(profile);

	/* B-node default drop profile set */
	default_profile_index = rm_find_free_b_node_drop_profile(ctl->rm);
    if (default_profile_index |= TM_NO_DROP_PROFILE) return 1;
	profile = &(ctl->tm_b_lvl_drop_profiles[default_profile_index]);
	__set_no_drop_profile(profile);

	/* C-node default drop profile set */
    for (cos=0; cos<TM_WRED_COS; cos++)
    {
		default_profile_index = rm_find_free_c_node_drop_profile(ctl->rm, cos);
		if (default_profile_index |= TM_NO_DROP_PROFILE) return 1;
		profile = &(ctl->tm_c_lvl_drop_profiles[cos][default_profile_index]);
		__set_no_drop_profile(profile);
	}

	/* Port default drop profile set */
	default_profile_index = rm_find_free_port_drop_profile(ctl->rm);
	if (default_profile_index |= TM_NO_DROP_PROFILE) return 1;
	profile = &(ctl->tm_p_lvl_drop_profiles[default_profile_index]);
	__set_no_drop_profile(profile);
	for (cos=0; cos<TM_WRED_COS; cos++)
	{
		default_profile_index = rm_find_free_port_drop_profile_cos(ctl->rm, cos);
		if (default_profile_index |= TM_NO_DROP_PROFILE) return 1;
		profile = &(ctl->tm_p_lvl_drop_profiles_cos[cos][default_profile_index]);
		__set_no_drop_profile(profile);
	}
	return 0;
}

int _tm_config_default_profiles_hw(tm_handle hndl)
{
    struct tm_drop_profile profile;
    uint8_t cos;
    int rc = TM_HW_WRED_CURVE_FAILED;
	uint32_t portNo;
    TM_CTL(ctl, hndl);

	__set_no_drop_profile(&profile);

	if (set_hw_queue_default_drop_profile(ctl, &profile) < 0)  return rc;
	if (set_hw_a_nodes_default_drop_profile(ctl, &profile) < 0)  return rc;
	if (set_hw_b_nodes_default_drop_profile(ctl, &profile) < 0)  return rc;
	for (cos=0; cos<TM_WRED_COS; cos++)
	{
		if (set_hw_c_nodes_default_drop_profile(ctl,  &profile , cos) < 0)  return rc;
	}
	for (portNo=0; portNo < ctl->tm_total_ports ; portNo++)
	{
		if (set_hw_ports_default_drop_profile(ctl,  &profile, portNo) < 0)  return rc;
		for (cos=0; cos<TM_WRED_COS; cos++)
		{
			if (set_hw_ports_default_drop_profile_cos(ctl,  &profile , cos, portNo) < 0)  return rc;
		}
	}
	return 0;
}



int _tm_config_default_drop_sw(tm_handle hndl)
{
    int rc;

    TM_CTL(ctl, hndl)

	rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

	rc =_tm_config_default_dp_mode(hndl,0);
    if (rc)  goto out;

	rc = _tm_config_default_aging_status(hndl,0);
    if (rc) goto out;

#ifdef USE_DEFAULT_CURVES
	rc = _tm_config_default_curves_sw(hndl);
    if (rc) goto out;
#endif

	rc = _tm_config_default_profiles_sw(hndl);
    if (rc) goto out;

  out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

int _tm_config_default_drop_hw(tm_handle hndl)
{
	int rc;

	rc = _tm_config_default_dp_mode(hndl, 1);
    if (rc)  return rc;

	rc = _tm_config_default_aging_status(hndl, 1);
    if (rc) return rc;

#ifdef USE_DEFAULT_CURVES
	rc = _tm_config_default_curves_hw(hndl);
     if (rc) return rc;
#endif

	rc = _tm_config_default_profiles_hw(hndl);
    if (rc) return rc;
    /* successful */
	return 0;
}


int tm_get_next_drop_profile(tm_handle hndl, enum tm_level level, uint8_t cos, uint16_t *prof_index_ptr)
{
    uint16_t prof_index = (*prof_index_ptr);
    int next_prof_index = prof_index;

    int ret_val = 0;

    TM_CTL(ctl, hndl)

    next_prof_index = rm_get_next_allocated_drop_resource(ctl->rm, level, cos, next_prof_index);

    if (next_prof_index == -1)
        ret_val = -EINVAL;

    *prof_index_ptr = next_prof_index;

    return ret_val;
}

/* */
#if 0

void tm_convert_hw_to_sw_drop_profile(struct tm_drop_profile *hw_drop_params_ptr, uint8_t color_num, struct tm_drop_profile_params *sw_drop_params_ptr)
{
    int i;
    uint32_t max_thresh_scaled = 0;
    uint32_t min_thresh_scaled = 0;

    tm_memset(sw_drop_params_ptr, 0, sizeof(struct tm_drop_profile_params));

    /* translate HW drop profile params to SW drop profile params*/
    /* Calculate TD bytes threshold */
    if (hw_drop_params_ptr->td_thresh_res == TM_ENABLE && hw_drop_params_ptr->td_threshold_bursts == DROP_MAX_THRESHOLD_BURSTS)
    {
        sw_drop_params_ptr->cbtdBytes = TM_MAX_DROP_THRESHOLD;
    }
    else
    {
        sw_drop_params_ptr->cbtdBytes = hw_drop_params_ptr->td_threshold_bursts * DROP_MEMORY_BURST_SIZE;
        if (hw_drop_params_ptr->td_thresh_res)
            sw_drop_params_ptr->cbtdBytes *= DROP_TD_RESOLUTION_MULTIPLYER;
    }

    /* conver drop mode check drop mode */
    if (hw_drop_params_ptr->color_td_en)
        sw_drop_params_ptr->drop_mode = CATD;
    else if (hw_drop_params_ptr->scale_ratio > 0)
        sw_drop_params_ptr->drop_mode = WRED;
    else
        sw_drop_params_ptr->drop_mode = REGULAR_TD_ONLY;

    if (sw_drop_params_ptr->drop_mode == REGULAR_TD_ONLY)
        return;

    /* convert CATD / WRED params */

     for (i=0; i < color_num; i++)
     {
         min_thresh_scaled = hw_drop_params_ptr->min_threshold[i];

         if (sw_drop_params_ptr->drop_mode == CATD)
         {
             sw_drop_params_ptr->caTdDp.catdBytes[i] = min_thresh_scaled * DROP_MEMORY_BURST_SIZE *
                                                       (1 << hw_drop_params_ptr->scale_exp[i]);
         }
         else
         {
             sw_drop_params_ptr->wredDp.caWredTdMinThreshold[i] = min_thresh_scaled * DROP_MEMORY_BURST_SIZE *
                                                                 (1 << hw_drop_params_ptr->scale_exp[i]);
         }
     }

     if (sw_drop_params_ptr->drop_mode == CATD)
         return;


     /* convert WRED params */

     for (i=0; i<color_num; i++)
     {
         /* curve index  */
         sw_drop_params_ptr->wredDp.curveIndex[i] = hw_drop_params_ptr->curve_id[i] ;
         /* cuve scaling */
         sw_drop_params_ptr->wredDp.dpCurveScale[i] = hw_drop_params_ptr->dp_ratio[i];

         /* wredMinThreshold, wredMaxThreshold */
         min_thresh_scaled = hw_drop_params_ptr->min_threshold[i]; 

         max_thresh_scaled = ((1024*32) / hw_drop_params_ptr->scale_ratio[i]) + min_thresh_scaled;

         sw_drop_params_ptr->wredDp.caWredTdMinThreshold[i] = min_thresh_scaled * DROP_MEMORY_BURST_SIZE *
                                                              (1 << hw_drop_params_ptr->scale_exp[i]);

         sw_drop_params_ptr->wredDp.caWredTdMaxThreshold[i] = max_thresh_scaled * DROP_MEMORY_BURST_SIZE *
                                                              (1 << hw_drop_params_ptr->scale_exp[i]);

     }
     /* aql exp */
     sw_drop_params_ptr->wredDp.aqlExponent = hw_drop_params_ptr->aql_exp;
}
  
	int tm_get_wred_drop_probability_by_queue_size(tm_handle     hndl,
												   enum tm_level level,                                               
												   struct tm_drop_profile *hw_drop_params_ptr, 
																	/* from tm_read_drop_profile_hw */
												   uint32_t queue_size,                                                
												   uint8_t  *hw_drop_probabilities_arr, 
																	/* array 0..31 of hw dps got from tm_get_hw_wred_curve */ 
												   int      *dp_per_color_arr)     
																	/* output array 0..2 of drop probability per color     */
	{
		int i;
		uint32_t queue_size_scaled = 0;
		uint32_t min_thresh_scaled = 0;
		uint32_t delta;
		float    max_probaility, percents, percents_resolution;
		float    exp;
		uint8_t  res;
		uint8_t  curve_index;

		TM_CTL(ctl, hndl)

		/* verify drop mode is wred */
		if (hw_drop_params_ptr->color_td_en)
			return -EPERM;

		if (hw_drop_params_ptr->scale_ratio == 0)
			return -EPERM;

		switch (level)
		{
		case Q_LEVEL:
			break;
		case A_LEVEL:
			break;
		case B_LEVEL:
			break;
		case C_LEVEL:
			break;
		case P_LEVEL:
			break;
		default:
			return -EPERM;
		}

		max_probaility = scaling_probability[ctl->dp_unit.local[level].max_p_mode[0]];

		/* Calculate prob values of current curve */
		res = ctl->dp_unit.local[level].resolution; /* 4, 6*/
		exp = (float)((1 << res) - 1); /* 15, 63*/
		/*max_probarobability: 100.0, 50.0, 25.0, 12.5 */

		queue_size_scaled = queue_size / DROP_MEMORY_BURST_SIZE / (1 << (uint32_t)hw_drop_params_ptr->scale_exp[0]);

		delta = queue_size_scaled - min_thresh_scaled;
     
		 for (i = 0; i < ctl->dp_unit.local[level].color_num; i++)
		 {
			 curve_index = delta * hw_drop_params_ptr->scale_ratio[i];

			 if (curve_index < 0)
			 {
				 dp_per_color_arr[i] = -1;
			 }
			 else if (curve_index > 31)
			 {
				 dp_per_color_arr[i] = 1;
			 }
			 else
			 {
				 percents_resolution = max_probaility / exp;

				 percents = (hw_drop_probabilities_arr[i] * percents_resolution) / max_probaility; 

				 percents /= 100.0;

				 /* check whether dp is scaled down */
				 if (hw_drop_params_ptr->dp_ratio[i] > 0)
				 {
					 percents = percents * (hw_drop_params_ptr->dp_ratio[i])  / exp;
				 }

				 /* convert to percents */
				 dp_per_color_arr[i] = (int)(percents + 0.9);
			 }
		 }

		 return 0;
	}
#endif

int tm_read_drop_profile_hw(tm_handle     hndl,
                            enum tm_level level,
                            uint8_t       cos,
                            uint32_t      prof_index,
                            struct        tm_drop_profile *dp_hw_params_ptr)
{
    int rc;

    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    if ((uint32_t)level > P_LEVEL)
    {
        rc = -EADDRNOTAVAIL;
        goto out;
    }

    if (dp_hw_params_ptr == NULL)
    {
        rc = -EPERM;
        goto out;
    }

 
	switch (level) 
    {
    case Q_LEVEL:
		TEST_RESOURCE_EXISTS(rm_queue_drop_profile_status(ctl->rm, prof_index),/* out_of range error*/TM_CONF_Q_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_Q_WRED_PROF_REF_OOR)
		if (rc) goto out;
		tm_memcpy(dp_hw_params_ptr, &(ctl->tm_q_lvl_drop_profiles[prof_index]), sizeof(struct tm_drop_profile));
        rc = tm_get_hw_queue_drop_profile(hndl, prof_index, dp_hw_params_ptr);
		break;
    case A_LEVEL:
 		TEST_RESOURCE_EXISTS(rm_a_node_drop_profile_status(ctl->rm, prof_index),/* out_of range error*/TM_CONF_A_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_A_WRED_PROF_REF_OOR)
		if (rc) goto out;
		tm_memcpy(dp_hw_params_ptr, &(ctl->tm_a_lvl_drop_profiles[prof_index]), sizeof(struct tm_drop_profile));
        rc = tm_get_hw_a_nodes_drop_profile(hndl, prof_index, dp_hw_params_ptr);
        break;
    case B_LEVEL:
 		TEST_RESOURCE_EXISTS(rm_b_node_drop_profile_status(ctl->rm, prof_index),/* out_of range error*/TM_CONF_B_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_B_WRED_PROF_REF_OOR)
		if (rc) goto out;
		tm_memcpy(dp_hw_params_ptr, &(ctl->tm_b_lvl_drop_profiles[prof_index]), sizeof(struct tm_drop_profile));
        rc = tm_get_hw_b_nodes_drop_profile(hndl, prof_index, dp_hw_params_ptr);
        break;
    case C_LEVEL:
		TEST_RESOURCE_EXISTS(rm_c_node_drop_profile_status_cos(ctl->rm, cos, prof_index),/* out_of range error*/TM_CONF_C_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_C_WRED_PROF_REF_OOR)
		if (rc) goto out;
		tm_memcpy(dp_hw_params_ptr,  &(ctl->tm_c_lvl_drop_profiles[cos][prof_index]), sizeof(struct tm_drop_profile));
        rc = tm_get_hw_c_nodes_drop_profile(hndl, cos, prof_index, dp_hw_params_ptr);
        break;
	case P_LEVEL:
        if(cos == (uint8_t)TM_INVAL) /* Global Port */
		{
 			TEST_RESOURCE_EXISTS(rm_port_drop_profile_status(ctl->rm,  prof_index),/* out_of range error*/TM_CONF_P_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_P_WRED_PROF_REF_OOR)
			if (rc) goto out;
		    tm_memcpy(dp_hw_params_ptr, &(ctl->tm_p_lvl_drop_profiles[prof_index]), sizeof(struct tm_drop_profile));
            rc = tm_get_hw_ports_drop_profile(hndl, prof_index, dp_hw_params_ptr);
		}
        else
		{
			TEST_RESOURCE_EXISTS(rm_port_drop_profile_status_cos(ctl->rm, cos, prof_index),/* out_of range error*/TM_CONF_P_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_P_WRED_PROF_REF_OOR)
			if (rc) goto out;
			tm_memcpy(dp_hw_params_ptr, &(ctl->tm_p_lvl_drop_profiles_cos[cos][prof_index]), sizeof(struct tm_drop_profile));
            rc = tm_get_hw_ports_drop_profile_cos(hndl, cos, prof_index, dp_hw_params_ptr);
		}
        break;
	}

out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

