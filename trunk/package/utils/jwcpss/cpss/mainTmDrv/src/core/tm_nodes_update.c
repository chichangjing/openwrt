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
 * @brief tm_nodes_update implementation.
 *
 * @file tm_nodes_update.c
 *
 * $Revision: 2.0 $
 */

/* #include <assert.h> */
#include "tm_nodes_update.h"
#include "tm_errcodes.h"
#include "tm_locking_interface.h"
#include "tm_errcodes.h"
#include "set_hw_registers.h"
#include "tm_set_local_db_defaults.h"
#include "tm_hw_configuration_interface.h"
/* for validation shaping profiles during node creation */
#include "tm_shaping_internal.h"
#include "tm_shaping_utils.h"
#include "tm_nodes_ctl_internal.h"

#include "rm_status.h"

#include <errno.h>
#include <stdlib.h>

int tm_update_elig_fun(tm_handle hndl, uint8_t level, uint32_t index, uint8_t elig_func)
{
    int rc;

    TM_CTL(ctl, hndl);

    CONVERT_TO_PHYSICAL(level, index); /* If supported - inside macro. */

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc)
        return rc;

	VALIDATE_ELIG_FUNCTION(elig_func)

    switch (level) {
    case Q_LEVEL:
        if (index >= ctl->tm_total_queues) {
            rc = -ENODATA;
            goto out;
        }
        ctl->tm_queue_array[index].elig_prio_func = elig_func;
        break;
    case A_LEVEL:
        if (index >= ctl->tm_total_a_nodes) {
            rc = -ENODATA;
            goto out;
        }
        ctl->tm_a_node_array[index].elig_prio_func = elig_func;
        break;
    case B_LEVEL:
        if (index >= ctl->tm_total_b_nodes) {
            rc = -ENODATA;
            goto out;
        }
        ctl->tm_b_node_array[index].elig_prio_func = elig_func;
        break;
    case C_LEVEL:
        if (index >= ctl->tm_total_c_nodes) {
            rc = -ENODATA;
            goto out;
        }
        ctl->tm_c_node_array[index].elig_prio_func = elig_func;
        break;
    case P_LEVEL:
        if (index >= ctl->tm_total_ports) {
            rc = -ENODATA;
            goto out;
        }
        ctl->tm_port_array[index].elig_prio_func = elig_func;
        break;
    default:
        rc = -EADDRNOTAVAIL;
        goto out;
    }

    if (rc >= 0) rc = set_hw_node_elig_prio_function(hndl, level, index);
	if (rc) goto out;
	/* in some realizations PerEn  is implemented also per node 
	*  so changing eligible function can cause  to change  PerEn  value per current node
	*  It's possible to set node PerEn  there always enabled or update shaping parameters
	*  here  and node PerEn  will be updated there according to new eligible function type
	*/
	if (level==P_LEVEL)	rc=set_hw_port_shaping(hndl, (uint8_t)index);
	else				rc=set_hw_node_shaping(hndl, level, index);

out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

/***************************************************************************
 * Parameters Update
 ***************************************************************************/

/* 
UPDATE_ELIGIBLE_FUNCTION return OOR error in 2 cases: 
    1) out of range elig_func_index 
    2) shaping depended elig_func_index when shaping / periodic scheme is disabled 
       cause to traffic to stop.
*/
#define UPDATE_ELIGIBLE_FUNCTION(level , elig_func_table, elig_func_test, elig_func_index, elig_func_pointer , if_valid_action) \
    if (elig_func_index != (uint8_t)TM_INVAL)\
	{\
        switch (elig_func_test(elig_func_table, elig_func_index))\
        {\
			case -1:rc = TM_CONF_ELIG_PRIO_FUNC_ID_OOR; goto out;\
			case  1:\
			if (ctl->level_data[level].shaping_status == TM_DISABLE)\
			{\
				rc = TM_CONF_ELIG_PRIO_FUNC_ID_OOR;\
				goto out;\
			}\
			default:\
			elig_func_pointer = elig_func_index;\
			if_valid_action;\
		}\
    }
#define     UPDATE_QUEUE_ELIGIBLE_FUNCTION( if_valid_action ) \
	UPDATE_ELIGIBLE_FUNCTION(Q_LEVEL,ctl->tm_elig_prio_q_lvl_tbl, is_queue_elig_fun_uses_shaper , params->elig_prio_func_ptr, queue->elig_prio_func , if_valid_action )
#define     UPDATE_NODE_ELIGIBLE_FUNCTION(level, elig_func_table,  if_valid_action ) \
	UPDATE_ELIGIBLE_FUNCTION(level,elig_func_table, is_node_elig_fun_uses_shaper, params->elig_prio_func_ptr, node->elig_prio_func , if_valid_action )




#define	UPDATE_DWRR_ENABLE_PER_PRIORITY_MASK(dwrr_prio_flag_array, node, error, action) \
	for (i=0; i<8; i++)\
	{\
		if ( dwrr_prio_flag_array[i] != (uint8_t)TM_INVAL)\
		{\
			if ((dwrr_prio_flag_array[i] != TM_DISABLE) && (dwrr_prio_flag_array[i] != TM_ENABLE))	\
			{\
				rc = error;\
				goto out;\
			}\
			node->dwrr_enable_per_priority_mask = (node->dwrr_enable_per_priority_mask & (~(0x1 << i)) & 0xFF )  /* reset bit in bitmask */\
												 | (dwrr_prio_flag_array[i] << i); \
			action\
		}\
	}
	
#define	UPDATE_NODE_DWRR_ENABLE_PER_PRIORITY_MASK(error) 	UPDATE_DWRR_ENABLE_PER_PRIORITY_MASK(params->dwrr_priority , node, error, fl_changed = 1;)

#define	UPDATE_PORT_DWRR_ENABLE_PER_PRIORITY_MASK(error) 	UPDATE_DWRR_ENABLE_PER_PRIORITY_MASK( dwrr_priority , port , error,  )
/**
 */
int tm_update_queue(tm_handle hndl, uint32_t index,
                    struct tm_queue_params *params)
{
    int rc;
    uint8_t fl_changed = 0;
    uint8_t sh_changed = 0;
    struct tm_queue *queue = NULL;

    TM_CTL(ctl, hndl);

	CONVERT_TO_PHYSICAL(Q_LEVEL,index)   /** if supported - inside macro */
	
    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

    /* Check parameters validity */
	VALIDATE_NODE_ALLOCATED(queue,index)
	if (rc) goto out;
   
    queue = &(ctl->tm_queue_array[index]);
    
	UPDATE_QUEUE_ELIGIBLE_FUNCTION	(fl_changed = 1)

    if (params->shaping_profile_ref != TM_INVAL)
	{
        /* Check param */
		rc= check_shaping_profile_validity(hndl,params->shaping_profile_ref, Q_LEVEL);
		if (rc) goto out;

        /* Update SW DB */
		rc=remove_node_from_shaping_profile(hndl, queue->shaping_profile_ref, Q_LEVEL, index);
		if (rc) goto out;
 		rc=add_node_to_shaping_profile(hndl, params->shaping_profile_ref, Q_LEVEL, index);
		if (rc) goto out;
		
		sh_changed = 1;
    }

    if (params->quantum != (uint16_t)TM_INVAL)
	{
		VALIDATE_NODE_QUANTUM_VALUE(params->quantum, TM_CONF_Q_QUANTUM_OOR)
		if (rc) goto out; 
        /* Update SW DB */
        /* Update quantum even if DWRR is disabled */
        queue->dwrr_quantum = params->quantum;
        fl_changed = 1;
    }


    if (params->wred_profile_ref != queue->wred_profile_ref)
	{
        /* check if queue drop profile already exists */
		TEST_RESOURCE_EXISTS(rm_queue_drop_profile_status(ctl->rm, params->wred_profile_ref),/* out_of range error*/TM_CONF_Q_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_Q_WRED_PROF_REF_OOR)
		if (rc) goto out;
        /* Update SW DB */
        ctl->tm_q_lvl_drop_profiles[queue->wred_profile_ref].use_counter--;
/*        assert(ctl->tm_q_lvl_drop_profiles[queue->wred_profile_ref].use_counter >= 0); */
        queue->wred_profile_ref = params->wred_profile_ref;
        ctl->tm_q_lvl_drop_prof_ptr[index] = params->wred_profile_ref;
        ctl->tm_q_lvl_drop_profiles[queue->wred_profile_ref].use_counter++;
        fl_changed = 1;
    }

    /* Download to HW */
    if (fl_changed)
	{
		if (set_hw_queue_params(hndl, index)
		 || set_hw_node_elig_prio_function(hndl, Q_LEVEL, index))
		{
			rc = TM_HW_QUEUE_CONFIG_FAIL;
			goto out;
		}
	}
	if (sh_changed)
	{
        if (set_hw_node_shaping(hndl, Q_LEVEL, index))
		{
            rc = TM_HW_SHAPING_PROF_FAILED;
            goto out;
        }		
	}

out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/** 
 */
int tm_update_a_node(tm_handle hndl, uint32_t index,
                     struct tm_a_node_params *params)
{
    int rc;
    int i;
    uint8_t fl_changed = 0;
    uint8_t sh_changed = 0;
    struct tm_a_node *node = NULL;

    TM_CTL(ctl, hndl);

	CONVERT_TO_PHYSICAL(A_LEVEL,index)   /** if supported - inside macro */

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc)  return rc;

    /* Check parameters validity */
	VALIDATE_NODE_ALLOCATED(a_node,index)
	if (rc) goto out;
   
    node = &(ctl->tm_a_node_array[index]);
    
	UPDATE_NODE_ELIGIBLE_FUNCTION(A_LEVEL,ctl->tm_elig_prio_a_lvl_tbl, fl_changed = 1)

    if (params->shaping_profile_ref != TM_INVAL)
	{
		rc= check_shaping_profile_validity(hndl,params->shaping_profile_ref, A_LEVEL);
		if (rc) goto out;

        /* Update SW DB */
		rc=remove_node_from_shaping_profile(hndl, node->shaping_profile_ref, A_LEVEL, index);
		if (rc) goto out;
		rc=add_node_to_shaping_profile(hndl, params->shaping_profile_ref, A_LEVEL, index);
		if (rc) goto out;

		sh_changed = 1;
    }

    /* DWRR for A-node */
    if (params->quantum != (uint16_t)TM_INVAL)
	{
		VALIDATE_NODE_QUANTUM_VALUE(params->quantum, TM_CONF_A_QUANTUM_OOR)
		if (rc) goto out; 
        /* Update SW DB */
        node->dwrr_quantum = params->quantum;
		fl_changed = TM_ENABLE;
    }

    /* DWRR for Queues in A-node's range */
	UPDATE_NODE_DWRR_ENABLE_PER_PRIORITY_MASK(TM_CONF_A_DWRR_PRIO_OOR )

	/* Drop profile */
    if (params->wred_profile_ref != node->wred_profile_ref)
	{
        /* check if a node drop profile already exists */
		TEST_RESOURCE_EXISTS(rm_a_node_drop_profile_status(ctl->rm, params->wred_profile_ref),/* out_of range error*/TM_CONF_A_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_A_WRED_PROF_REF_OOR)
		if (rc) goto out;

        /* Update SW DB */
        ctl->tm_a_lvl_drop_profiles[node->wred_profile_ref].use_counter--;
        node->wred_profile_ref = params->wred_profile_ref;
        ctl->tm_a_lvl_drop_prof_ptr[index] = params->wred_profile_ref;
        ctl->tm_a_lvl_drop_profiles[node->wred_profile_ref].use_counter++;
		fl_changed = TM_ENABLE;
    }

    /* Download to HW */
    if (fl_changed)
	{
		if (set_hw_a_node_params(hndl, index)
		 || set_hw_node_elig_prio_function(hndl, A_LEVEL, index))
		{
			rc = TM_HW_A_NODE_CONFIG_FAIL;
			goto out;
		}
	}
	if (sh_changed)
	{
        if (set_hw_node_shaping(hndl, A_LEVEL, index))
		{
            rc = TM_HW_SHAPING_PROF_FAILED;
            goto out;
        }		
	}
out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_update_b_node(tm_handle hndl, uint32_t index,
                     struct tm_b_node_params *params)
{
    int rc;
    int i;
    uint8_t fl_changed = 0;
    uint8_t sh_changed = 0;
    struct tm_b_node *node = NULL;

    TM_CTL(ctl, hndl);

	CONVERT_TO_PHYSICAL(B_LEVEL,index)   /** if supported - inside macro */
	
    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc)   return rc;

    /* Check parameters validity */
	VALIDATE_NODE_ALLOCATED(b_node,index)
	if (rc) goto out;
   
    node = &(ctl->tm_b_node_array[index]);
    
	UPDATE_NODE_ELIGIBLE_FUNCTION(B_LEVEL,ctl->tm_elig_prio_b_lvl_tbl,  fl_changed = 1)

    if (params->shaping_profile_ref != TM_INVAL)
	{
 		rc= check_shaping_profile_validity(hndl,params->shaping_profile_ref, B_LEVEL);
		if (rc) goto out;

        /* Update SW DB */
 		rc=remove_node_from_shaping_profile(hndl, node->shaping_profile_ref, B_LEVEL, index);
		if (rc) goto out;
		rc=add_node_to_shaping_profile(hndl, params->shaping_profile_ref, B_LEVEL, index);
		if (rc) goto out;
		sh_changed=1;
    }

    /* DWRR for B-node */
    if (params->quantum != (uint16_t)TM_INVAL)
	{
		VALIDATE_NODE_QUANTUM_VALUE(params->quantum, TM_CONF_B_QUANTUM_OOR)
		if (rc) goto out; 
        /* Update SW DB */
        node->dwrr_quantum = params->quantum;
        fl_changed = 1;
    }

    /* DWRR for A-nodes in B-node's range */
	UPDATE_NODE_DWRR_ENABLE_PER_PRIORITY_MASK(TM_CONF_B_DWRR_PRIO_OOR )
	
	/* Drop profile */
    if (params->wred_profile_ref != (uint16_t)TM_INVAL)
	{
        /* check if b node drop profile already exists */
		TEST_RESOURCE_EXISTS(rm_b_node_drop_profile_status(ctl->rm, params->wred_profile_ref),/* out_of range error*/TM_CONF_B_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_B_WRED_PROF_REF_OOR)
		if (rc) goto out;
        /* Update SW DB */
        ctl->tm_b_lvl_drop_profiles[node->wred_profile_ref].use_counter--;
/*        assert(ctl->tm_b_lvl_drop_profiles[node->wred_profile_ref].use_counter >= 0); */
        node->wred_profile_ref = params->wred_profile_ref;
        ctl->tm_b_lvl_drop_prof_ptr[index] = params->wred_profile_ref;
        ctl->tm_b_lvl_drop_profiles[node->wred_profile_ref].use_counter++;
        fl_changed = 1;
    }
    
    /* Download to HW */
    if (fl_changed)
	{
		if (set_hw_b_node_params(hndl, index)
		 || set_hw_node_elig_prio_function(hndl, B_LEVEL, index))
		{
			rc = TM_HW_B_NODE_CONFIG_FAIL;
			goto out;
		}
	}
	if (sh_changed)
	{
        if (set_hw_node_shaping(hndl, B_LEVEL, index))
		{
            rc = TM_HW_SHAPING_PROF_FAILED;
            goto out;
        }		
	}
out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/** 
 */
int tm_update_c_node(tm_handle hndl, uint32_t index,
                     struct tm_c_node_params *params)
{
    uint8_t cos_map;
    uint8_t old_wred_profile_ref;
    int rc = 0;
    int i;
    uint8_t fl_changed = 0;
    uint8_t sh_changed = 0;
	struct tm_c_node *node = NULL;

    TM_CTL(ctl, hndl);

	CONVERT_TO_PHYSICAL(Q_LEVEL,index)   /** if supported - inside macro */

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

    /* Check parameters validity */
	VALIDATE_NODE_ALLOCATED(c_node,index)
	if (rc) goto out;
   
    node = &(ctl->tm_c_node_array[index]);

	UPDATE_NODE_ELIGIBLE_FUNCTION(C_LEVEL,ctl->tm_elig_prio_c_lvl_tbl, fl_changed = 1)

    if (params->shaping_profile_ref != TM_INVAL)
	{
 		rc= check_shaping_profile_validity(hndl,params->shaping_profile_ref, C_LEVEL);
		if (rc) goto out;
		
        /* Update SW DB */
		rc=remove_node_from_shaping_profile(hndl, node->shaping_profile_ref, C_LEVEL, index);
		if (rc) goto out;
		rc=add_node_to_shaping_profile(hndl, params->shaping_profile_ref, C_LEVEL, index);
		if (rc) goto out;
		sh_changed=1;
    }

    /* DWRR for C-node */
    if (params->quantum != (uint16_t)TM_INVAL)
	{
		VALIDATE_NODE_QUANTUM_VALUE(params->quantum, TM_CONF_C_QUANTUM_OOR)
		if (rc) goto out; 
        /* Update SW DB */
        node->dwrr_quantum = params->quantum;
        fl_changed = 1;
    }

    /* DWRR for B-nodes in C-node's range */
	UPDATE_NODE_DWRR_ENABLE_PER_PRIORITY_MASK(TM_CONF_C_DWRR_PRIO_OOR )

	/* Drop profile */
    cos_map = node->wred_cos;
    for (i=0; i<TM_WRED_COS; i++)
	{
        if (params->wred_profile_ref[i] != (uint8_t)TM_INVAL)
		{
            if (params->wred_cos & (1<<i))
			{
                /* apply new drop profile for this cos */
                /* check if c node drop profile already exists */
				TEST_RESOURCE_EXISTS(rm_c_node_drop_profile_status_cos(ctl->rm, (uint8_t)i, params->wred_profile_ref[i]),/* out_of range error*/TM_CONF_C_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_C_WRED_PROF_REF_OOR)
				if (rc) goto out;

                if (params->wred_profile_ref[i] != node->wred_profile_ref[i])
				{
                    /* Update SW DB */
                    old_wred_profile_ref = ctl->tm_c_lvl_drop_prof_ptr[i][index];
                    ctl->tm_c_lvl_drop_profiles[i][old_wred_profile_ref].use_counter--;
/*                    assert(ctl->tm_c_lvl_drop_profiles[i][old_wred_profile_ref].use_counter >= 0); */
                    ctl->tm_c_lvl_drop_prof_ptr[i][index] = params->wred_profile_ref[i];
                    ctl->tm_c_lvl_drop_profiles[i][params->wred_profile_ref[i]].use_counter++;
                    node->wred_profile_ref[i] = params->wred_profile_ref[i];

                    /* update Wred CoS for future usage within the HW update */
                    cos_map =
                        (cos_map & ~(0x1<<i)) | (params->wred_cos & (0x1<<i));
                    fl_changed = TM_ENABLE;
                }
				else
				{
                    /* set wred_cos bit is the same */
                    cos_map =
                        (cos_map & ~(0x1<<i)) | (params->wred_cos & (0x1<<i));
                }
            }
			else
			{   
				/* remove the use in old drop profile */
                if (node->wred_cos & (0x1<<i))
				{
                    /* the profile was in use */
                    /* Update SW DB */
                    old_wred_profile_ref = ctl->tm_c_lvl_drop_prof_ptr[i][index];
                    ctl->tm_c_lvl_drop_profiles[i][old_wred_profile_ref].use_counter--;
/*                    assert(ctl->tm_c_lvl_drop_profiles[i][old_wred_profile_ref].use_counter >= 0); */
                    ctl->tm_c_lvl_drop_prof_ptr[i][index] = TM_NO_DROP_PROFILE;
                    ctl->tm_c_lvl_drop_profiles[i][params->wred_profile_ref[TM_NO_DROP_PROFILE]].use_counter++;
                    node->wred_profile_ref[i] = TM_NO_DROP_PROFILE;

                    /* update Wred CoS for future usage within the HW update */
                    cos_map = (cos_map & ~(0x1 << i));
                    fl_changed = 1;
                }
            }
        }
    }
    node->wred_cos = cos_map;
 
    /* Download to HW */
    if (fl_changed)
	{
 		if (set_hw_c_node_params(hndl, index)
		 || set_hw_node_elig_prio_function(hndl, C_LEVEL, index))
		{
			rc = TM_HW_C_NODE_CONFIG_FAIL;
			goto out;
		}
	}
	if (sh_changed)
	{
        if (set_hw_node_shaping(hndl, C_LEVEL, index))
		{
            rc = TM_HW_SHAPING_PROF_FAILED;
            goto out;
        }		
	}
out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

/**
 */
int tm_update_port_shaping(tm_handle hndl, uint8_t index,
                           uint32_t cir_bw,
                           uint32_t eir_bw,
                           uint32_t cbs,
                           uint32_t ebs)
{
    uint32_t 	port_cir_bw;
    uint32_t 	port_eir_bw;
    uint32_t 	port_cbs;
    uint32_t	port_ebs;
    uint32_t 	min_token;
    uint32_t 	max_token;
    uint8_t 	res_min_bw;
    uint8_t 	res_max_bw;
    uint32_t 	periods;
    int 		rc = 0;
    struct tm_port *port = NULL;

    TM_CTL(ctl, hndl);

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

	VALIDATE_NODE_ALLOCATED(port,index)
	if (rc) goto out;
   
    
    port = &(ctl->tm_port_array[index]);

	/* check shaping parameters */
    if ((cbs != TM_INVAL) && (cbs > TM_128M_kB))
	{
    	rc = TM_CONF_PORT_BS_OOR;
        goto out;
    }
    if ((ebs != TM_INVAL) && (ebs > TM_128M_kB))
	{
    	rc = TM_CONF_PORT_BS_OOR;
        goto out;
    }

    port_cir_bw = cir_bw; 
    port_eir_bw = eir_bw; 
    /* check that it fits the max physical port speed */
    rc = get_tm_port_bw_by_port_type(port->port_speed, &port_cir_bw, &port_eir_bw);
    if (rc)
    {
        goto out;
    }

#if 0
	if (cir_bw != TM_INVAL) port_cir_bw = cir_bw;
	else					port_cir_bw = ctl->tm_port_array[index].cir_sw;
	if (eir_bw != TM_INVAL)	port_eir_bw = eir_bw;
	else					port_eir_bw = ctl->tm_port_array[index].eir_sw;

	if (cbs != TM_INVAL) port_cbs = cbs;
	else				 port_cbs = ctl->tm_port_array[index].cir_burst_size;

	if (ebs != TM_INVAL) port_ebs = ebs;
	else				 port_ebs = ctl->tm_port_array[index].eir_burst_size;
#endif

	if (port_cir_bw == TM_MAX_BW)
		port_cbs = TM_MAX_BURST;
	else
		port_cbs = cbs;

	if (port_eir_bw == TM_MAX_BW)
		port_ebs = TM_MAX_BURST;
	else
		port_ebs = ebs;

	rc = calculate_port_shaping(1000.0*port_cir_bw/ctl->level_data[P_LEVEL].unit,
								1000.0*port_eir_bw/ctl->level_data[P_LEVEL].unit,
								&port_cbs,
								&port_ebs,
								(1 << ctl->level_data[P_LEVEL].token_res_exp),
								&periods,
								&res_min_bw, &res_max_bw,
								&min_token, &max_token);
	if (rc)
		goto out;

#ifdef DEBUG
PRINT_PORT_SHAPING_CALCULATION_RESULT(port_cir_bw,port_eir_bw)
#endif

	/* update shaping parameters in SW */
	port->cir_sw = port_cir_bw;
	port->eir_sw = port_eir_bw;
	port->cir_burst_size = port_cbs;
	port->eir_burst_size = port_ebs;
	port->cir_token = min_token;
	port->eir_token = max_token;
	port->periods = periods;
	port->min_token_res = res_min_bw;
	port->max_token_res = res_max_bw;

    rc = set_hw_port_shaping(hndl, index);
    if (rc)
        rc = TM_HW_PORT_CONFIG_FAIL;
out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

int tm_verify_port_shaping_configuration(tm_handle hndl,
											enum tm_port_bw port_type,
											uint32_t cir_bw,
											uint32_t eir_bw,
											uint32_t * pcbs,
											uint32_t * pebs)
{
    uint32_t 	port_cir_bw;
    uint32_t 	port_eir_bw;
    uint32_t 	token;
    uint8_t 	res_bw;
    uint32_t 	periods;
    int 		rc = 0;

    TM_CTL(ctl, hndl);

    if (ctl->periodic_scheme_state != TM_ENABLE)
    {
        rc = TM_CONF_UPD_RATE_NOT_CONF_FOR_LEVEL;
        goto out;
    }

    /* check shaping parameters */
    if ((*pcbs > TM_128M_kB) || (*pebs > TM_128M_kB))
    {
    	rc = TM_CONF_PORT_BS_OOR;
        goto out;
    }
    rc = tm_glob_lock(TM_ENV(ctl));
    if (rc)  
        return rc;

    port_cir_bw = cir_bw;
    port_eir_bw = eir_bw;
     
    /* check that it fits the max physical port speed */
    rc = get_tm_port_bw_by_port_type(port_type, &port_cir_bw, &port_eir_bw);
    if (rc)
    {
        goto out;
    }

    if (port_cir_bw == TM_MAX_BW)
        *pcbs = TM_MAX_BURST;

    if (port_eir_bw == TM_MAX_BW)
        *pebs = TM_MAX_BURST;

	rc = calculate_port_shaping(1000.0*(port_cir_bw)/ctl->level_data[P_LEVEL].unit,
								1000.0*(port_eir_bw)/ctl->level_data[P_LEVEL].unit,
								pcbs,
								pebs,
								(1 << ctl->level_data[P_LEVEL].token_res_exp),
								&periods,
								&res_bw, &res_bw,
								&token, &token);
out:
    tm_glob_unlock(TM_ENV(ctl));
    return rc;
}

/**
 */
int tm_update_port_scheduling(tm_handle hndl,
			      uint8_t index,
			      uint8_t elig_prio_func_ptr,
			      uint16_t *quantum, /* 8 cells array */
                  uint8_t *dwrr_priority)
{
    struct tm_port *port = NULL;
    int rc = 0;
    int i;

    TM_CTL(ctl, hndl);

	rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc)  return rc;
    rc = tm_sched_lock(TM_ENV(ctl));
    if (rc)
	{
        tm_nodes_unlock(TM_ENV(ctl));
        return rc;
    }

	VALIDATE_NODE_ALLOCATED(port,index)
	if (rc) goto out;
   
    port = &(ctl->tm_port_array[index]);

	UPDATE_ELIGIBLE_FUNCTION(P_LEVEL,ctl->tm_elig_prio_p_lvl_tbl, is_node_elig_fun_uses_shaper, elig_prio_func_ptr, port->elig_prio_func, /*nothing to addition actions for port */)

    /* DWRR for Port */
    if ((quantum[0] != (uint16_t)TM_INVAL) ||
        (quantum[2] != (uint16_t)TM_INVAL) ||
        (quantum[3] != (uint16_t)TM_INVAL) ||
        (quantum[4] != (uint16_t)TM_INVAL) ||
        (quantum[5] != (uint16_t)TM_INVAL) ||
        (quantum[6] != (uint16_t)TM_INVAL) ||
        (quantum[7] != (uint16_t)TM_INVAL))
	{
        /* Check quantum */
        for (i=0; i<8; i++)
		{
            if(quantum[i] != (uint16_t)TM_INVAL)
			{
				VALIDATE_PORT_QUANTUM_VALUE(quantum[i] , TM_CONF_PORT_QUANTUM_OOR)
				if (rc) goto out; 
                   /* Update SW DB */
                port->dwrr_quantum_per_level[i].quantum = quantum[i];
			}
        }
    }

    /* DWRR for C-nodes in Port's range */
	UPDATE_PORT_DWRR_ENABLE_PER_PRIORITY_MASK(TM_CONF_PORT_DWRR_PRIO_OOR)

    rc = set_hw_port_scheduling(hndl, index);
    if (rc)
	{
        rc = TM_HW_PORT_CONFIG_FAIL;
        goto out;
    }

    /* Download port eligible function pointer (from profile) to HW */
    rc = set_hw_node_elig_prio_function(hndl, P_LEVEL, index);
    if (rc) 
        rc = TM_HW_ELIG_PRIO_FUNC_FAILED;

out:
    tm_sched_unlock(TM_ENV(ctl));
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_update_port_drop(tm_handle hndl, uint8_t index, uint8_t wred_profile_ref)
{
    struct tm_port *port = NULL;
    struct tm_drop_profile *profile = NULL;
    int rc = 0;

    TM_CTL(ctl, hndl);

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

	VALIDATE_NODE_ALLOCATED(port,index)
	if (rc) goto out;
   
    port = &(ctl->tm_port_array[index]);

    if ((wred_profile_ref != (uint8_t)TM_INVAL) &&
        (wred_profile_ref != port->wred_profile_ref))
	{
        /* Check param */
        /* check if port drop profile already exists */
		TEST_RESOURCE_EXISTS(rm_port_drop_profile_status(ctl->rm, wred_profile_ref),/* out_of range error*/TM_CONF_P_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_P_WRED_PROF_REF_OOR)
		if (rc) goto out;

        /* Update SW DB */
        profile = &(ctl->tm_p_lvl_drop_profiles[port->wred_profile_ref]);
        profile->use_counter--;
/*        assert(profile->use_counter >= 0); */
 
        port->wred_profile_ref = wred_profile_ref;
        /* add node to the list of new drop profile */
        profile = &(ctl->tm_p_lvl_drop_profiles[wred_profile_ref]);
        ctl->tm_p_lvl_drop_prof_ptr[index] = wred_profile_ref;
        profile->use_counter++;

        /* Download to HW the values of new drop profile to the port */
        rc = set_hw_port_drop_global(hndl, index);
        if (rc)
            rc = TM_HW_PORT_CONFIG_FAIL;
    }
out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_update_port_drop_cos(tm_handle hndl,
                            uint8_t index,
                            struct tm_port_drop_per_cos *params)
{
    struct tm_port *port = NULL;
    struct tm_drop_profile *profile = NULL;
    uint8_t new_cos_status;
    uint8_t old_cos_status;
    int rc = 0;
    uint8_t i;
    
    TM_CTL(ctl, hndl);

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;
 
	VALIDATE_NODE_ALLOCATED(port,index)
	if (rc) goto out;
   
    port = &(ctl->tm_port_array[index]);

    for (i=0; i<TM_WRED_COS; i++)
    {
        if (params->wred_profile_ref[i] != (uint8_t)TM_INVAL)
        {
            new_cos_status = params->wred_cos & (1<<i);
            old_cos_status = port->wred_cos & (1<<i);
            
            /* Old Drop Profile reference should be replaced by new */
            if((old_cos_status == new_cos_status) &&
               (old_cos_status == 1) &&
               (params->wred_profile_ref[i] != params->wred_profile_ref[i]))
            {
				TEST_RESOURCE_EXISTS(rm_port_drop_profile_status_cos(ctl->rm, i, params->wred_profile_ref[i]),/* out_of range error*/ TM_CONF_P_WRED_PROF_REF_OOR , /*profile not used error*/ TM_CONF_P_WRED_PROF_REF_OOR)
				if (rc) goto out;

              /* Remove old Drop Profile reference */
              profile = &(ctl->tm_p_lvl_drop_profiles_cos[i][port->wred_profile_ref_cos[i]]);
              profile->use_counter--;
            
              /* Add new Drop Profile reference */
              port->wred_profile_ref_cos[i] = params->wred_profile_ref[i];

              /* add node to the list of new drop profile */
              profile = &(ctl->tm_p_lvl_drop_profiles_cos[i][port->wred_profile_ref_cos[i]]);
              ctl->tm_p_lvl_drop_prof_ptr_cos[index][i] = params->wred_profile_ref[i];
              profile->use_counter++;            
            }
            else
            {
                /* Add new Drop Profile reference (wred_cos bit 0->1) */
                if(old_cos_status == 0)
                {
                    TEST_RESOURCE_EXISTS(rm_port_drop_profile_status_cos(ctl->rm, i, params->wred_profile_ref[i]),/* out_of range error*/TM_CONF_P_WRED_PROF_REF_OOR , /*profile not used error*/TM_CONF_P_WRED_PROF_REF_OOR)
					if (rc) goto out;

                    /* Add new Drop Profile reference */
                    port->wred_profile_ref_cos[i] = params->wred_profile_ref[i];

                    /* add node to the list of new drop profile */
                    profile = &(ctl->tm_p_lvl_drop_profiles_cos[i][port->wred_profile_ref_cos[i]]);
                    ctl->tm_p_lvl_drop_prof_ptr_cos[index][i] = params->wred_profile_ref[i];
                    profile->use_counter++;

                }
                /* Remove old Drop Profile reference (wred_cos bit 1->0) */
                else
                {
                    profile = &(ctl->tm_p_lvl_drop_profiles_cos[i][port->wred_profile_ref_cos[i]]);
                    profile->use_counter--;
                    /* set default values */
                    port->wred_profile_ref_cos[i] = TM_NO_DROP_PROFILE;

                    /* add node to the list of default drop profile */
                    profile = &(ctl->tm_p_lvl_drop_profiles_cos[i][TM_NO_DROP_PROFILE]);
                    ctl->tm_p_lvl_drop_prof_ptr_cos[index][i] = TM_NO_DROP_PROFILE;
                    profile->use_counter++;
            
                }
            }
        }
    }
	/* Download to HW the default values to the port */
	rc = set_hw_port_drop_cos(hndl, index);
	if (rc)
		rc = TM_HW_PORT_CONFIG_FAIL;
out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

