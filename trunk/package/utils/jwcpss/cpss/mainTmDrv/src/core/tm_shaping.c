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
 * @brief tm_shaping implementation.
 *
 * @file tm_shaping.c
 *
 * $Revision: 2.0 $
 */





#include "tm_shaping.h"

#include "tm_errcodes.h"
#include "tm_locking_interface.h"
#include "tm_set_local_db_defaults.h"
#include "set_hw_registers.h"
#include "tm_shaping_utils.h"
#include "tm_nodes_ctl_internal.h"

#include "rm_alloc.h"
#include "rm_status.h"
#include "rm_free.h"

#include <errno.h>
#include <stdlib.h>
#include <assert.h>


int init_infinite_shaping_profile(tm_handle hndl)
{
    int default_profile_index;
    TM_CTL(ctl, hndl)
    default_profile_index = rm_find_free_shaping_profile(ctl->rm);
    /* default_profile_index must be first in resource pool , otherwise function is called by wrong way */
    if (default_profile_index != TM_INF_SHP_PROFILE)
	{
        return  -ENOSPC;
    }
    return set_sw_shaping_profile_default(ctl->tm_shaping_profiles,TM_INF_SHP_PROFILE);
}

/**
*/
int tm_verify_shaping_profile(tm_handle hndl,
                              enum tm_level level,
                              struct tm_shaping_profile_params *profile)
{
    int rc;

    TM_CTL(ctl, hndl)

	if (!profile)			return -EACCES;
	if ((uint32_t)level > C_LEVEL)	return -EADDRNOTAVAIL;

    rc = tm_sched_lock(TM_ENV(ctl));
    if (rc) return rc;
    /* 0<cbs<=TM_4M_kB or 0<ebs<=TM_4M_kB */
    if ((profile->cbs> TM_4M_kB) || (profile->ebs > TM_4M_kB))
	{
        rc = -EFAULT;
        goto out;
    }
    
    /* check if shaping is not disabled/not yet set for the level */
    if (ctl->periodic_scheme_state != TM_ENABLE)
	{
        rc = TM_CONF_UPD_RATE_NOT_CONF_FOR_LEVEL;
        goto out;
    }

    rc = test_node_shaping(1000.0*profile->cir_bw/ctl->level_data[level].unit,
						   1000.0*profile->eir_bw/ctl->level_data[level].unit,
						   (1 << ctl->level_data[level].token_res_exp),
						   &profile->cbs,
						   &profile->ebs);
out:
    tm_sched_unlock(TM_ENV(ctl));
    return rc;
}

int tm_create_shaping_profile(tm_handle hndl,
                              enum tm_level level,
                              struct tm_shaping_profile_params *profile,
                              uint32_t * prof_index)
{
    int rc;
    struct tm_shaping_profile *sh_profile;
    int prof_ind = (int)TM_INVAL;
    
    uint32_t	cir_token;
    uint32_t	eir_token;
    uint8_t		cir_resolution;
    uint8_t		eir_resolution;
    uint8_t		min_div_exp;
    uint8_t		max_div_exp;
    uint32_t	cbs;
    uint32_t	ebs;

    TM_CTL(ctl, hndl)

	if (!profile)			return -EACCES;
	if (!prof_index)		return -EACCES;
	if ((uint32_t)level > C_LEVEL)	return -EADDRNOTAVAIL;

    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc) return rc;

    rc = tm_sched_lock(TM_ENV(ctl));
    if (rc)
	{
        tm_glob_unlock(TM_ENV(ctl));
        return rc;
    }
    /* 0<cbs<=TM_4M_kB or 0<ebs<=TM_4M_kB */
    if ((profile->cbs> TM_4M_kB) || (profile->ebs > TM_4M_kB))
	{
        rc = -EFAULT;
        goto out;
    }
    
    /* check if shaping is not disabled/not yet set for the level */
    if (ctl->periodic_scheme_state != TM_ENABLE)
	{
        rc = TM_CONF_UPD_RATE_NOT_CONF_FOR_LEVEL;
        goto out;
    }

    prof_ind = rm_find_free_shaping_profile(ctl->rm);
    if (prof_ind < 0)
	{
        rc = -ENOSPC;
        goto out;
    }
	else
	{
        *prof_index = prof_ind;
	}
	cbs = profile->cbs;
	ebs = profile->ebs;
    rc = calculate_node_shaping(1000.0*profile->cir_bw/ctl->level_data[level].unit,
								1000.0*profile->eir_bw/ctl->level_data[level].unit,
								&cbs,
								&ebs,
								(1 << ctl->level_data[level].token_res_exp),
								&min_div_exp,
								&max_div_exp,
								&cir_resolution,
								&eir_resolution,
								&cir_token, &eir_token);
    if (rc) goto out;

#ifdef DEBUG
    PRINT_NODE_SHAPING_CALCULATION_RESULT(profile->cir_bw,profile->eir_bw);
#endif

    /* update the SW image */
    sh_profile = &(ctl->tm_shaping_profiles[prof_ind]);
    sh_profile->use_counter = 0;
	sh_profile->level=level;  
	sh_profile->cir_bw=profile->cir_bw;
	sh_profile->eir_bw=profile->eir_bw;
    sh_profile->min_token = cir_token;
    sh_profile->min_burst_size = profile->cbs;
    sh_profile->max_token = eir_token;
    sh_profile->max_burst_size = profile->ebs;
	sh_profile->min_div_exp = min_div_exp;
	sh_profile->max_div_exp = max_div_exp;
    sh_profile->min_token_res = cir_resolution;
    sh_profile->max_token_res = eir_resolution;
    sh_profile->use_counter = 0;
out:
    if (rc)
	{
        /* free shaping profile */
        if (prof_ind != (int)TM_INVAL)
            rm_free_shaping_profile(ctl->rm, prof_ind);
    }
    tm_sched_unlock(TM_ENV(ctl));
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

/**
*/
int tm_delete_shaping_profile(tm_handle hndl, uint32_t prof_index)
{
    int rc;
	
    TM_CTL(ctl, hndl)

	rc = tm_glob_lock(TM_ENV(ctl));
    if (rc) return rc;

	if (prof_index == 0)
	{ /* reserved profile */
        rc = -EACCES;
        goto out;
    }   
 	/* validate shaping profile */
 	TEST_RESOURCE_EXISTS(rm_shaping_profile_status(ctl->rm,prof_index) , -EADDRNOTAVAIL, -EFAULT)
 	if (rc) goto out;
 	
    if (ctl->tm_shaping_profiles[prof_index].use_counter > 0)
    { /* profile in use */
        rc = -EPERM;
        goto out;
    }

    /* free profile's entry in the RM (status is checked inside) */
    rc = rm_free_shaping_profile(ctl->rm, prof_index);
    if (rc < 0)
	{
        rc = -EFAULT;
        goto out;
    }
    set_sw_shaping_profile_default(ctl->tm_shaping_profiles, prof_index);

out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}


/**
*/
int tm_read_shaping_profile(tm_handle hndl,
                            enum tm_level level,
                            uint32_t prof_index,
                            struct tm_shaping_profile_params *profile)
{
    int rc;
    struct tm_shaping_profile *sh_profile;

    TM_CTL(ctl, hndl)

	rc = tm_glob_lock(TM_ENV(ctl));
    if (rc) return rc;
    
    rc = tm_sched_lock(TM_ENV(ctl));
    if (rc)
	{
        tm_glob_unlock(TM_ENV(ctl));
        return rc;
    }
    
    if ((uint32_t)level > C_LEVEL)
	{
        rc = -EDOM;
        goto out;
    }
	/* validate shaping profile */
 	TEST_RESOURCE_EXISTS(rm_shaping_profile_status(ctl->rm,prof_index) ,/* out_of range error*/  -EADDRNOTAVAIL, /*profile not used error*/ -EFAULT)
 	if (rc) goto out;
 	
    sh_profile = &(ctl->tm_shaping_profiles[prof_index]);
	/*check if profile index match level*/
	if(sh_profile->level!=level)
	{
		rc = -EDOM;
		goto out;
	}
	
	/* here profile is valid */
	profile->cir_bw = sh_profile->cir_bw;
	profile->cbs = sh_profile->min_burst_size;
	profile->eir_bw = sh_profile->eir_bw;
	profile->ebs = sh_profile->max_burst_size;
        
out:
    tm_sched_unlock(TM_ENV(ctl));
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

/* MACRO for update all layer nodes referenced to updated shaping profile */

#define	UPDATE_PROFILE_CLIENTS(layer, layer_size) \
	i=0;\
	node_counter=0;\
	while ( (node_counter < sh_profile->use_counter) && (i < layer_size)) \
	{\
		if (prof_index==ctl->layer[i].shaping_profile_ref) \
		{\
			/* update node */\
			if (set_hw_node_shaping(hndl, level, i) < 0)\
			{\
				rc = TM_HW_SHAPING_PROF_FAILED;\
				goto out;\
			}\
			node_counter++;\
		}\
		i++;\
	}\
/* fprintf(stderr,">>>>>>>>>>>>>>>>>>>>>>>>>>> %s        level=%d,  use_counter=%d ,  node_counter=%d,  index=%d\n",__FUNCTION__, level, sh_profile->use_counter , node_counter, i) 	; */




/**
 */
int tm_update_shaping_profile(tm_handle hndl,
                              enum tm_level level,
                              uint32_t prof_index,
                              struct tm_shaping_profile_params *profile)
{
   struct tm_shaping_profile *sh_profile;

	int			rc;

    uint32_t	cir_token;
    uint32_t	eir_token;
    uint8_t	cir_resolution;
    uint8_t	eir_resolution;
    uint8_t	min_div_exp;
    uint8_t	max_div_exp;

    uint32_t 	min_bw;
    uint32_t 	max_bw;
    
	uint32_t 	min_burst_size;
    uint32_t 	max_burst_size;

    uint32_t 	node_counter;
    uint32_t  	i;

    TM_CTL(ctl, hndl)

	rc = tm_glob_lock(TM_ENV(ctl));
    if (rc) return rc;
    
    rc = tm_sched_lock(TM_ENV(ctl));
    if (rc)
    {
        tm_glob_unlock(TM_ENV(ctl));
        return rc;
    }

    if (prof_index == TM_INF_SHP_PROFILE) { /* reserved profile */
        rc = -EACCES;
        goto out;
    }
    
	/* validate shaping profile */
 	TEST_RESOURCE_EXISTS(rm_shaping_profile_status(ctl->rm,prof_index) ,/* out_of range error*/  -EACCES, /*profile not used error*/ -EPERM)
 	if (rc) goto out;

    sh_profile = &(ctl->tm_shaping_profiles[prof_index]);
	/*check if profile index match level*/
	if(sh_profile->level!=level)
	{
		rc = -EADDRNOTAVAIL;
		goto out;
	}
   
    if ((profile->cbs != TM_INVAL) && (profile->cbs > TM_4M_kB))
	{
        rc = -EFAULT;
        goto out;
    }
	if ((profile->ebs != TM_INVAL) && (profile->ebs > TM_4M_kB))
	{
		rc = -EFAULT;
		goto out;
	} 

    if (profile->cir_bw != TM_INVAL)	min_bw = profile->cir_bw;
    else								min_bw = sh_profile->cir_bw;
    
    if (profile->eir_bw != TM_INVAL)    max_bw = profile->eir_bw;
    else								max_bw = sh_profile->eir_bw;

	if (profile->cbs != TM_INVAL)		min_burst_size = profile->cbs;
	else								min_burst_size = sh_profile->min_burst_size;

	if (profile->ebs != TM_INVAL)		max_burst_size = profile->ebs;
	else								max_burst_size = sh_profile->max_burst_size;

    rc = calculate_node_shaping(1000.0 * min_bw/ctl->level_data[level].unit, 
								1000.0 * max_bw/ctl->level_data[level].unit, 
								&min_burst_size,
								&max_burst_size,
								(1 << ctl->level_data[level].token_res_exp),
								&min_div_exp,
								&max_div_exp,
								&cir_resolution,
								&eir_resolution,
								&cir_token,
								&eir_token);
    if (rc)
        goto out;

#ifdef DEBUG
    PRINT_NODE_SHAPING_CALCULATION_RESULT(min_bw , max_bw);
#endif

    /* update the SW image */
	sh_profile->cir_bw=min_bw;
	sh_profile->eir_bw=max_bw;
    sh_profile->min_token = cir_token;
    sh_profile->min_burst_size = min_burst_size;
    sh_profile->max_token = eir_token;
    sh_profile->max_burst_size = max_burst_size;
    sh_profile->min_div_exp = min_div_exp;
    sh_profile->max_div_exp = max_div_exp;
    sh_profile->min_token_res = cir_resolution;
    sh_profile->max_token_res = eir_resolution;
     
    /* update shaping for all profile clients */
    switch(level)
    {
		case Q_LEVEL:
		{
                  UPDATE_PROFILE_CLIENTS(tm_queue_array,ctl->tm_total_queues);
			break;
		}
		case A_LEVEL:
		{
                  UPDATE_PROFILE_CLIENTS(tm_a_node_array,ctl->tm_total_a_nodes);
			break;
		}
		case B_LEVEL:
		{
                  UPDATE_PROFILE_CLIENTS(tm_b_node_array,ctl->tm_total_b_nodes);
			break;
		}
		case C_LEVEL:
		{
                  UPDATE_PROFILE_CLIENTS(tm_c_node_array,ctl->tm_total_c_nodes);
			break;
		}
		default: break;
	}	
out:
    tm_sched_unlock(TM_ENV(ctl));
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}


#define __RETURN_PROF_REF_OOR_ERROR_CODE \
{\
	switch (level)\
	{\
			case Q_LEVEL: return TM_CONF_Q_SHAPING_PROF_REF_OOR; \
			case A_LEVEL: return TM_CONF_A_SHAPING_PROF_REF_OOR;\
			case B_LEVEL: return TM_CONF_B_SHAPING_PROF_REF_OOR;\
			case C_LEVEL: return TM_CONF_C_SHAPING_PROF_REF_OOR;\
			default:  return TM_CONF_SHAPING_PROF_REF_OOR; /* not happend */\
	}\
}

int check_shaping_profile_validity(	tm_handle hndl, uint32_t prof_index, enum tm_level level)
{
    TM_CTL(ctl, hndl);

#ifdef PER_NODE_SHAPING
	/* direct shaping can't be input parameter - so profile is invalid for  create/update purposes */
    if (prof_index ==TM_DIRECT_NODE_SHAPING) __RETURN_PROF_REF_OOR_ERROR_CODE;
#endif
    if (rm_shaping_profile_status(ctl->rm, prof_index) != 1 )  __RETURN_PROF_REF_OOR_ERROR_CODE;
	if ((prof_index) && (ctl->tm_shaping_profiles[prof_index].level != level)) return TM_WRONG_SHP_PROFILE_LEVEL;
	return 0;
}


#define SET_SHAPING_PROFILE_TO_NODE(profile) \
    switch(level) \
    {\
		case Q_LEVEL:ctl->tm_queue_array[node_index].shaping_profile_ref=profile; break;\
		case A_LEVEL:ctl->tm_a_node_array[node_index].shaping_profile_ref=profile; break;\
		case B_LEVEL:ctl->tm_b_node_array[node_index].shaping_profile_ref=profile; break;\
		case C_LEVEL:ctl->tm_c_node_array[node_index].shaping_profile_ref=profile; break;\
		default: break;\
	}

#define GET_NODE_SHAPING_PROFILE(profile) \
    switch(level) \
    {\
		case Q_LEVEL:profile = ctl->tm_queue_array[node_index].shaping_profile_ref; break;\
		case A_LEVEL:profile = ctl->tm_a_node_array[node_index].shaping_profile_ref; break;\
		case B_LEVEL:profile = ctl->tm_b_node_array[node_index].shaping_profile_ref; break;\
		case C_LEVEL:profile = ctl->tm_c_node_array[node_index].shaping_profile_ref; break;\
		default:assert(0);profile=0xFFFF; break;				\
	}





int add_node_to_shaping_profile(tm_handle hndl,
								uint32_t prof_index,
								enum tm_level level,
								uint32_t node_index)
{
    int rc;
    TM_CTL(ctl, hndl);
	(void)node_index;
#ifdef PER_NODE_SHAPING
	/* this can't happend  */
	if (prof_index==TM_DIRECT_NODE_SHAPING)  return 0; /* direct shaping  -  node is not client  */
#endif 
	if (prof_index==TM_INF_SHP_PROFILE)  return 0; /* default profile not updated, so it doesn't need clients */
	rc = check_shaping_profile_validity(hndl,prof_index, level);
	if (rc) return rc;
	SET_SHAPING_PROFILE_TO_NODE(prof_index);
	ctl->tm_shaping_profiles[prof_index].use_counter++;	
	return 0;
}

int remove_node_from_shaping_profile(tm_handle hndl,
								uint32_t prof_index,
								enum tm_level level,
								uint32_t node_index)
{
    int rc;
    TM_CTL(ctl, hndl);
	(void)node_index;
#ifdef PER_NODE_SHAPING
	if (prof_index==TM_DIRECT_NODE_SHAPING)  return 0; /* direct shaping  -  node is not client  */
#endif 
	if (prof_index==TM_INF_SHP_PROFILE)  return 0; /* default profile not updated, so it doesn't need clients */
	rc = check_shaping_profile_validity(hndl,prof_index, level);
	if (rc) return rc;
	/* reset node shaping profile ref  to  default (unupdated) profile*/  
	SET_SHAPING_PROFILE_TO_NODE(TM_INF_SHP_PROFILE);
	ctl->tm_shaping_profiles[prof_index].use_counter--;	
	return rc;
}



#ifdef PER_NODE_SHAPING

	#define COPY_NODE_SHAPING_PARAMS_TO_PTR(source_node, target_ptr) \
		target_ptr->cir_bw=source_node.cir_bw;\
		target_ptr->eir_bw=source_node.eir_bw;\
		target_ptr->cbs=source_node.cbs;\
		target_ptr->ebs=source_node.ebs;

	#define COPY_NODE_SHAPING_PARAMS_FROM_PTR(source_ptr, target_node) \
		target_node.cir_bw=source_ptr->cir_bw;\
		target_node.eir_bw=source_ptr->eir_bw;\
		target_node.cbs=source_ptr->cbs;\
		target_node.ebs=source_ptr->ebs;


	int update_node_shaping_proc(tm_handle hndl,
									  enum tm_level level,
									  uint32_t node_index,
									  struct tm_shaping_profile_params * node_shaping_params)
	{
		int rc;
		uint16_t	node_shaping_profile;
		
		uint32_t	cir_token;
		uint32_t	eir_token;
		uint8_t	cir_resolution;
		uint8_t	eir_resolution;
		uint8_t	min_div_exp;
		uint8_t	max_div_exp;
		struct 	tm_shaping_profile tmp_shp_profile;
		uint32_t		cbs;
		uint32_t		ebs;

		TM_CTL(ctl, hndl)

		rc = tm_nodes_lock(TM_ENV(ctl));
		if (rc) return rc;

		/* Check node  validity */
		switch (level)
		{
			case Q_LEVEL:
				VALIDATE_NODE_ALLOCATED(queue,node_index)
				break;
			case A_LEVEL:
				VALIDATE_NODE_ALLOCATED(a_node,node_index)
				break;
			case B_LEVEL:
				VALIDATE_NODE_ALLOCATED(b_node,node_index)
				break;
			case C_LEVEL:
				VALIDATE_NODE_ALLOCATED(c_node,node_index)
			   break;
			case P_LEVEL:
				VALIDATE_NODE_ALLOCATED(port,node_index)
				break;
			default:
				rc = -ERANGE;
		}
		if (rc) goto out;
		
		/* Check shaping params validity */
		if ((node_shaping_params->cbs > TM_4M_kB) || (node_shaping_params->ebs > TM_4M_kB))
		{
			rc = -EFAULT;
			goto out;
		}
		/* calculate shaping  */
		cbs = node_shaping_params->cbs;
		ebs = node_shaping_params->ebs;

		rc = calculate_node_shaping(1000.0 * node_shaping_params->cir_bw/ctl->level_data[level].unit, 
									1000.0 * node_shaping_params->eir_bw/ctl->level_data[level].unit, 
									&cbs,
									&ebs,
									(1 << ctl->level_data[level].token_res_exp),
									&min_div_exp,
									&max_div_exp,
									&cir_resolution,
									&eir_resolution,
									&cir_token,
									&eir_token);
        if (rc)
            goto out;

		PRINT_NODE_SHAPING_CALCULATION_RESULT(node_shaping_params->cir_bw ,node_shaping_params->eir_bw)
		/* if valid : */
		GET_NODE_SHAPING_PROFILE(node_shaping_profile);
		rc=remove_node_from_shaping_profile(hndl, node_shaping_profile, level, node_index);
		if (rc) goto out;
		tmp_shp_profile.level=level;  
		tmp_shp_profile.cir_bw=node_shaping_params->cir_bw;
		tmp_shp_profile.eir_bw=node_shaping_params->eir_bw;
		tmp_shp_profile.min_token = cir_token;
		tmp_shp_profile.min_burst_size = node_shaping_params->cbs;
		tmp_shp_profile.max_token = eir_token;
		tmp_shp_profile.max_burst_size = node_shaping_params->ebs;
		tmp_shp_profile.min_div_exp = min_div_exp;
		tmp_shp_profile.max_div_exp = max_div_exp;
		tmp_shp_profile.min_token_res = cir_resolution;
		tmp_shp_profile.max_token_res = eir_resolution;    
		rc = set_hw_node_shaping_ex(hndl, level, node_index, &tmp_shp_profile);
		/* Update SW DB */
		if (rc)
		{
			rc = TM_HW_SHAPING_PROF_FAILED;
			/* rollback  -return node to clients set   ??? */
			add_node_to_shaping_profile(hndl, node_shaping_profile, level, node_index);
			goto out;
		}
		SET_SHAPING_PROFILE_TO_NODE(TM_DIRECT_NODE_SHAPING);
		/* store  node shaping parameters */  
		switch(level) 
		{
			case Q_LEVEL:COPY_NODE_SHAPING_PARAMS_FROM_PTR(node_shaping_params ,ctl->tm_queue_array[node_index] ); break;
			case A_LEVEL:COPY_NODE_SHAPING_PARAMS_FROM_PTR(node_shaping_params ,ctl->tm_a_node_array[node_index]); break;
			case B_LEVEL:COPY_NODE_SHAPING_PARAMS_FROM_PTR(node_shaping_params ,ctl->tm_b_node_array[node_index]); break;
			case C_LEVEL:COPY_NODE_SHAPING_PARAMS_FROM_PTR(node_shaping_params ,ctl->tm_c_node_array[node_index]); break;
			default: break;
		}
	out:
		tm_nodes_unlock(TM_ENV(ctl));
		return rc;
	}


	int read_node_shaping_proc(tm_handle hndl,
								  enum tm_level level,
								  uint32_t node_index,
								  uint16_t * shaping_profile,
								  struct tm_shaping_profile_params * node_shaping_params)
	{
		int rc;

		TM_CTL(ctl, hndl);
		
		rc = tm_nodes_lock(TM_ENV(ctl));
		if (rc) return rc;

		/* Check node  validity */
		switch (level)
		{
		case Q_LEVEL:
			if (node_index >= ctl->tm_total_queues) rc = -EFAULT;
			else if (ctl->tm_queue_array[node_index].mapping.nodeStatus != TM_NODE_USED)  rc = -ENODATA;
			else rc = 0;
			break;
		case A_LEVEL:
			if (node_index >= ctl->tm_total_a_nodes) rc = -EFAULT;
			else if (ctl->tm_a_node_array[node_index].mapping.nodeStatus != TM_NODE_USED)  rc = -ENODATA;
			else rc = 0;
			break;
		case B_LEVEL:
			if (node_index >= ctl->tm_total_b_nodes) rc = -EFAULT;
			else if (ctl->tm_b_node_array[node_index].mapping.nodeStatus != TM_NODE_USED)  rc = -ENODATA;
			else rc = 0;
			break;
		case C_LEVEL:
			if (node_index >= ctl->tm_total_c_nodes) rc = -EFAULT;
			else if (ctl->tm_c_node_array[node_index].mapping.nodeStatus != TM_NODE_USED)  rc = -ENODATA;
			else rc = 0;
		   break;
		default:
			rc = -ERANGE;
		}
		if (rc) goto out;

		GET_NODE_SHAPING_PROFILE(* shaping_profile);
		
		if (* shaping_profile == TM_DIRECT_NODE_SHAPING)
		{
			/* fill  structure by shaping parameters */
			switch(level) 
			{
				case Q_LEVEL:COPY_NODE_SHAPING_PARAMS_TO_PTR(ctl->tm_queue_array[node_index] ,node_shaping_params); break;
				case A_LEVEL:COPY_NODE_SHAPING_PARAMS_TO_PTR(ctl->tm_a_node_array[node_index],node_shaping_params); break;
				case B_LEVEL:COPY_NODE_SHAPING_PARAMS_TO_PTR(ctl->tm_b_node_array[node_index],node_shaping_params); break;
				case C_LEVEL:COPY_NODE_SHAPING_PARAMS_TO_PTR(ctl->tm_c_node_array[node_index],node_shaping_params); break;
				default: break;
			}
		}

	out:
		tm_nodes_unlock(TM_ENV(ctl));
		return rc;
	}

	int tm_update_node_shaping(tm_handle hndl,
								  enum tm_level level,
								  uint32_t node_index,
								  struct tm_shaping_profile_params * node_shaping_params)
	{		
		DECLARE_TM_CTL_PTR(ctl, hndl);
		(void)ctl; /* prevent warning if virtual layer is not used */
		CONVERT_TO_PHYSICAL(level,node_index);
		return update_node_shaping_proc( hndl, level, node_index, node_shaping_params);
	}


	int tm_read_node_shaping(tm_handle hndl,
								  enum tm_level level,
								  uint32_t node_index,
								  uint16_t * shaping_profile,
								  struct tm_shaping_profile_params * node_shaping_params)
	{
		DECLARE_TM_CTL_PTR(ctl, hndl);
		(void)ctl;/* prevent warning if virtual layer is not used */
		CONVERT_TO_PHYSICAL(level,node_index);
		return read_node_shaping_proc( hndl, level, node_index, shaping_profile, node_shaping_params);
	}


#endif /* PER_NODE_SHAPING */
