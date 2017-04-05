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
 * @brief  functions for set/get  platform specific registers
 *
 * @file set_hw_registers_platform.c
 *
 * $Revision: 2.0 $
 */

#include "tm_core_types.h"
#include "set_hw_registers_imp.h"

#include "set_hw_registers.h"
#include "tm_set_local_db_defaults.h"


int set_hw_shaping_status(tm_handle hndl, enum tm_level level)
{
    int rc = -ERANGE;

    TM_REGISTER_VAR(TM_Sched_PortPerConf)
    TM_REGISTER_VAR(TM_Sched_ClvlPerConf)
    TM_REGISTER_VAR(TM_Sched_BlvlPerConf)
    TM_REGISTER_VAR(TM_Sched_AlvlPerConf)
    TM_REGISTER_VAR(TM_Sched_QueuePerConf)

    TM_CTL(ctl, hndl)

	switch (level)
	{
    case Q_LEVEL:
		/* reset register reserved fields */
		TM_RESET_REGISTER(TM.Sched.QueuePerConf, TM_Sched_QueuePerConf)
		if (rc)	break;
		/* assign register fields */
        TM_REGISTER_SET(TM_Sched_QueuePerConf, DecEn,			ctl->level_data[Q_LEVEL].shaper_dec)
        TM_REGISTER_SET(TM_Sched_QueuePerConf, PerInterval ,	ctl->level_data[Q_LEVEL].per_interval)
        TM_REGISTER_SET(TM_Sched_QueuePerConf, PerEn ,			ctl->level_data[Q_LEVEL].shaping_status)
        TM_REGISTER_SET(TM_Sched_QueuePerConf, TokenResExp ,	ctl->level_data[Q_LEVEL].token_res_exp)
		/* write register */
        TM_WRITE_REGISTER(TM.Sched.QueuePerConf, TM_Sched_QueuePerConf)
		break;
    case A_LEVEL:
		/* reset register reserved fields */
		TM_RESET_REGISTER(TM.Sched.AlvlPerConf, TM_Sched_AlvlPerConf)
		if (rc)	break;
		/* assign register fields */		
        TM_REGISTER_SET(TM_Sched_AlvlPerConf, DecEn ,		ctl->level_data[A_LEVEL].shaper_dec)
        TM_REGISTER_SET(TM_Sched_AlvlPerConf, PerInterval , ctl->level_data[A_LEVEL].per_interval)
        TM_REGISTER_SET(TM_Sched_AlvlPerConf, PerEn ,		ctl->level_data[A_LEVEL].shaping_status)
        TM_REGISTER_SET(TM_Sched_AlvlPerConf, TokenResExp , ctl->level_data[A_LEVEL].token_res_exp);
		/* write register */
        TM_WRITE_REGISTER(TM.Sched.AlvlPerConf, TM_Sched_AlvlPerConf)
        break;
    case B_LEVEL:
		/* reset register reserved fields */
		TM_RESET_REGISTER(TM.Sched.BlvlPerConf, TM_Sched_BlvlPerConf)
		if (rc)	break;
		/* assign register fields */		
        TM_REGISTER_SET(TM_Sched_BlvlPerConf, DecEn ,		ctl->level_data[B_LEVEL].shaper_dec)
        TM_REGISTER_SET(TM_Sched_BlvlPerConf, PerInterval , ctl->level_data[B_LEVEL].per_interval)
        TM_REGISTER_SET(TM_Sched_BlvlPerConf, PerEn ,		ctl->level_data[B_LEVEL].shaping_status)
        TM_REGISTER_SET(TM_Sched_BlvlPerConf, TokenResExp , ctl->level_data[B_LEVEL].token_res_exp);
		/* write register */
        TM_WRITE_REGISTER(TM.Sched.BlvlPerConf, TM_Sched_BlvlPerConf)
        break;
    case C_LEVEL:
		/* reset register reserved fields */
		TM_RESET_REGISTER(TM.Sched.ClvlPerConf, TM_Sched_ClvlPerConf)
		/* assign register fields */		
        TM_REGISTER_SET(TM_Sched_ClvlPerConf, DecEn ,		ctl->level_data[C_LEVEL].shaper_dec)
        TM_REGISTER_SET(TM_Sched_ClvlPerConf, PerInterval , ctl->level_data[C_LEVEL].per_interval)
        TM_REGISTER_SET(TM_Sched_ClvlPerConf ,PerEn ,		ctl->level_data[C_LEVEL].shaping_status)
        TM_REGISTER_SET(TM_Sched_ClvlPerConf, TokenResExp , ctl->level_data[C_LEVEL].token_res_exp);
		/* write register */
        TM_WRITE_REGISTER(TM.Sched.ClvlPerConf, TM_Sched_ClvlPerConf)
        break;
    case P_LEVEL:
		/* reset register reserved fields */
		TM_RESET_REGISTER(TM.Sched.PortPerConf, TM_Sched_PortPerConf)
		if (rc)	break;
		/* assign register fields */		
        TM_REGISTER_SET(TM_Sched_PortPerConf, DecEn ,		ctl->level_data[P_LEVEL].shaper_dec)
        TM_REGISTER_SET(TM_Sched_PortPerConf, PerInterval , ctl->level_data[P_LEVEL].per_interval)
        TM_REGISTER_SET(TM_Sched_PortPerConf, PerEn ,		ctl->level_data[P_LEVEL].shaping_status);
        TM_REGISTER_SET(TM_Sched_PortPerConf, TokenResExp , ctl->level_data[P_LEVEL].token_res_exp);
		/* write register */
        TM_WRITE_REGISTER(TM.Sched.PortPerConf, TM_Sched_PortPerConf)
        break;
	default:
		break;
    }
    COMPLETE_HW_WRITE
	return rc;
}


/* assume here that shaping profile is valid */
int set_hw_node_shaping_ex(tm_handle hndl, enum tm_level level, uint32_t node_ind, struct tm_shaping_profile *profile)
{
    int rc = 0;
 
    TM_REGISTER_VAR(TM_Sched_ClvlTokenBucketTokenEnDiv)
    TM_REGISTER_VAR(TM_Sched_BlvlTokenBucketTokenEnDiv)
    TM_REGISTER_VAR(TM_Sched_AlvlTokenBucketTokenEnDiv)
    TM_REGISTER_VAR(TM_Sched_QueueTokenBucketTokenEnDiv)
    TM_REGISTER_VAR(TM_Sched_ClvlTokenBucketBurstSize)
    TM_REGISTER_VAR(TM_Sched_BlvlTokenBucketBurstSize)
    TM_REGISTER_VAR(TM_Sched_AlvlTokenBucketBurstSize)
    TM_REGISTER_VAR(TM_Sched_QueueTokenBucketBurstSize)
 
    TM_CTL(ctl, hndl)
	
	/*  profile <--> level conformance test */
	if ((profile->level !=level) && (profile->level != ALL_LEVELS))
	{
		rc = -EFAULT;
		goto out;
	}

	switch (level)
	{
	case Q_LEVEL:
        NODE_VALIDATION(ctl->tm_total_queues)
		if (rc) goto out;
		/* profile assignment */
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.QueueTokenBucketTokenEnDiv, node_ind, TM_Sched_QueueTokenBucketTokenEnDiv)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_QueueTokenBucketTokenEnDiv , MinDivExp , 	profile->min_div_exp);
		TM_REGISTER_SET(TM_Sched_QueueTokenBucketTokenEnDiv , MaxDivExp , 	profile->max_div_exp);
		TM_REGISTER_SET(TM_Sched_QueueTokenBucketTokenEnDiv , MinToken , 	profile->min_token | ((uint16_t)profile->min_token_res << 11))
		TM_REGISTER_SET(TM_Sched_QueueTokenBucketTokenEnDiv , MaxToken ,	profile->max_token | ((uint16_t)profile->max_token_res << 11))
		TM_REGISTER_SET(TM_Sched_QueueTokenBucketTokenEnDiv , PerEn ,		is_queue_elig_fun_uses_shaper(ctl->tm_elig_prio_q_lvl_tbl,ctl->tm_queue_array[node_ind].elig_prio_func))
/* 
		let prevent case that  eligible function core is changed (in elig_function table)   ,but clients will not be updated
		set PerEn  always enabled for nodes
		TM_REGISTER_SET(TM_Sched_QueueTokenBucketTokenEnDiv , PerEn ,  		1)
*/
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.QueueTokenBucketTokenEnDiv, node_ind, TM_Sched_QueueTokenBucketTokenEnDiv)
		if (rc)	goto out;

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.QueueTokenBucketBurstSize, node_ind,  TM_Sched_QueueTokenBucketBurstSize)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_QueueTokenBucketBurstSize , MaxBurstSz , profile->max_burst_size);
		TM_REGISTER_SET(TM_Sched_QueueTokenBucketBurstSize , MinBurstSz , profile->min_burst_size);
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.QueueTokenBucketBurstSize, node_ind,  TM_Sched_QueueTokenBucketBurstSize)
		if (rc)	goto out;
		break;
		
	case A_LEVEL:
        NODE_VALIDATION(ctl->tm_total_a_nodes)
		if (rc) goto out;
		/* profile assignment */
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.AlvlTokenBucketTokenEnDiv, node_ind, TM_Sched_AlvlTokenBucketTokenEnDiv)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_AlvlTokenBucketTokenEnDiv , MinDivExp , 	profile->min_div_exp)
		TM_REGISTER_SET(TM_Sched_AlvlTokenBucketTokenEnDiv , MaxDivExp , 	profile->max_div_exp)
		TM_REGISTER_SET(TM_Sched_AlvlTokenBucketTokenEnDiv , MinToken , 	profile->min_token | ((uint16_t)profile->min_token_res << 11))
		TM_REGISTER_SET(TM_Sched_AlvlTokenBucketTokenEnDiv , MaxToken ,		profile->max_token | ((uint16_t)profile->max_token_res << 11))
		TM_REGISTER_SET(TM_Sched_AlvlTokenBucketTokenEnDiv , PerEn ,		is_node_elig_fun_uses_shaper(ctl->tm_elig_prio_a_lvl_tbl,ctl->tm_a_node_array[node_ind].elig_prio_func))
/* 
		let prevent case that  eligible function core is changed (in elig_function table)   ,but clients will not be updated
		set PerEn  always enabled for nodes
		TM_REGISTER_SET(TM_Sched_AlvlTokenBucketTokenEnDiv , PerEn ,  		1)
*/
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.AlvlTokenBucketTokenEnDiv, node_ind, TM_Sched_AlvlTokenBucketTokenEnDiv)
		if (rc)	goto out;

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.AlvlTokenBucketBurstSize, node_ind, TM_Sched_AlvlTokenBucketBurstSize)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_AlvlTokenBucketBurstSize , MaxBurstSz , profile->max_burst_size)
		TM_REGISTER_SET(TM_Sched_AlvlTokenBucketBurstSize , MinBurstSz , profile->min_burst_size)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.AlvlTokenBucketBurstSize, node_ind, TM_Sched_AlvlTokenBucketBurstSize)
		if (rc)	goto out;
		break;
	case B_LEVEL:
        NODE_VALIDATION(ctl->tm_total_b_nodes)
		if (rc) goto out;
		/* profile assignment */
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.BlvlTokenBucketTokenEnDiv, node_ind, TM_Sched_BlvlTokenBucketTokenEnDiv)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_BlvlTokenBucketTokenEnDiv , MinDivExp , 	profile->min_div_exp)
		TM_REGISTER_SET(TM_Sched_BlvlTokenBucketTokenEnDiv , MaxDivExp , 	profile->max_div_exp)
		TM_REGISTER_SET(TM_Sched_BlvlTokenBucketTokenEnDiv , MinToken , 	profile->min_token | ((uint16_t)profile->min_token_res << 11))
		TM_REGISTER_SET(TM_Sched_BlvlTokenBucketTokenEnDiv , MaxToken ,		profile->max_token | ((uint16_t)profile->max_token_res << 11))
		TM_REGISTER_SET(TM_Sched_BlvlTokenBucketTokenEnDiv , PerEn ,		is_node_elig_fun_uses_shaper(ctl->tm_elig_prio_b_lvl_tbl,ctl->tm_b_node_array[node_ind].elig_prio_func))
/* 
		let prevent case that  eligible function core is changed (in elig_function table)   ,but clients will not be updated
		set PerEn  always enabled for nodes
		TM_REGISTER_SET(TM_Sched_BlvlTokenBucketTokenEnDiv , PerEn ,  		1)
*/
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.BlvlTokenBucketTokenEnDiv, node_ind, TM_Sched_BlvlTokenBucketTokenEnDiv)
		if (rc)	goto out;
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.BlvlTokenBucketBurstSize, node_ind,  TM_Sched_BlvlTokenBucketBurstSize)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_BlvlTokenBucketBurstSize , MaxBurstSz , profile->max_burst_size)
		TM_REGISTER_SET(TM_Sched_BlvlTokenBucketBurstSize , MinBurstSz , profile->min_burst_size)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.BlvlTokenBucketBurstSize, node_ind,  TM_Sched_BlvlTokenBucketBurstSize)
		if (rc)	goto out;
		break;
	case C_LEVEL:
        NODE_VALIDATION(ctl->tm_total_c_nodes)
		if (rc) goto out;
		/* profile assignment */
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.ClvlTokenBucketTokenEnDiv, node_ind, TM_Sched_ClvlTokenBucketTokenEnDiv)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_ClvlTokenBucketTokenEnDiv , MinDivExp , 	profile->min_div_exp)
		TM_REGISTER_SET(TM_Sched_ClvlTokenBucketTokenEnDiv , MaxDivExp , 	profile->max_div_exp)
		TM_REGISTER_SET(TM_Sched_ClvlTokenBucketTokenEnDiv , MinToken , 	profile->min_token | ((uint16_t)profile->min_token_res << 11))
		TM_REGISTER_SET(TM_Sched_ClvlTokenBucketTokenEnDiv , MaxToken ,		profile->max_token | ((uint16_t)profile->max_token_res << 11))
		TM_REGISTER_SET(TM_Sched_ClvlTokenBucketTokenEnDiv , PerEn ,		is_node_elig_fun_uses_shaper(ctl->tm_elig_prio_c_lvl_tbl,ctl->tm_c_node_array[node_ind].elig_prio_func))
/* 
		let prevent case that  eligible function core is changed (in elig_function table)   ,but clients will not be updated
		set PerEn  always enabled for nodes
		TM_REGISTER_SET(TM_Sched_ClvlTokenBucketTokenEnDiv , PerEn ,  		1)
*/
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.ClvlTokenBucketTokenEnDiv, node_ind, TM_Sched_ClvlTokenBucketTokenEnDiv)
		if (rc)	goto out;
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.ClvlTokenBucketBurstSize, node_ind, TM_Sched_ClvlTokenBucketBurstSize)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_ClvlTokenBucketBurstSize , MaxBurstSz , profile->max_burst_size)
		TM_REGISTER_SET(TM_Sched_ClvlTokenBucketBurstSize , MinBurstSz , profile->min_burst_size)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.ClvlTokenBucketBurstSize, node_ind, TM_Sched_ClvlTokenBucketBurstSize)
		if (rc)	goto out;
		break;
	default:
		rc = -EFAULT;
		break;
	}
out:
    COMPLETE_HW_WRITE;
	return rc;
}



int set_hw_port_shaping(tm_handle hndl, uint8_t port_ind)
{
    int rc =  -EFAULT;
    struct tm_port *port = NULL;

    TM_REGISTER_VAR(TM_Sched_PortTokenBucketTokenEnDiv)
    TM_REGISTER_VAR(TM_Sched_PortTokenBucketBurstSize)

    TM_CTL(ctl, hndl)

	if (port_ind < ctl->tm_total_ports)
	{
		port = &(ctl->tm_port_array[port_ind]);

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.PortTokenBucketTokenEnDiv , port_ind , TM_Sched_PortTokenBucketTokenEnDiv)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_PortTokenBucketTokenEnDiv , Periods ,  	port->periods)
		TM_REGISTER_SET(TM_Sched_PortTokenBucketTokenEnDiv , MinToken ,		port->cir_token | ((uint16_t)port->min_token_res << 11))
		TM_REGISTER_SET(TM_Sched_PortTokenBucketTokenEnDiv , MaxToken ,		port->eir_token | ((uint16_t)port->max_token_res << 11))
		TM_REGISTER_SET(TM_Sched_PortTokenBucketTokenEnDiv , PerEn ,  		is_node_elig_fun_uses_shaper(ctl->tm_elig_prio_p_lvl_tbl,port->elig_prio_func))
/* 
		let prevent case that  eligible function core is changed (in elig_function table)   ,but clients will not be updated
		set PerEn  always enabled for nodes
*/
		TM_REGISTER_SET(TM_Sched_PortTokenBucketTokenEnDiv , PerEn ,  		1)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.PortTokenBucketTokenEnDiv , port_ind , TM_Sched_PortTokenBucketTokenEnDiv)
		if (rc) goto out;

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.PortTokenBucketBurstSize , port_ind , TM_Sched_PortTokenBucketBurstSize)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_PortTokenBucketBurstSize , MaxBurstSz , port->eir_burst_size)
		TM_REGISTER_SET(TM_Sched_PortTokenBucketBurstSize , MinBurstSz , port->cir_burst_size)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.PortTokenBucketBurstSize , port_ind , TM_Sched_PortTokenBucketBurstSize)
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

/**
 * default tree configuration for BC :
 *   each C node has 32  queues attached to it.
 *   ports 0:190 have  2 C nodes attached to each these port 
 *   the port 191 has a rest of Cnodes (242) attached to it,
 *   so "regular port" has 64 queues attached for it
 *    4 first queues of each port are installed , 
*/
/*
#define DEFAULT_QUEUES_PER_REGULAR_PORT		64
#define INSTALLED_QUEUES_PER_PORT			4
*/


int set_hw_uninstall_default_queues(tm_handle hndl)
{
    int rc =  -EFAULT;
    int i;
    int j;
	int queuesPerPort;

	DECLARE_CPSS_ENV(env)
    TM_CTL(ctl, hndl);
	
	ASSIGN_CPSS_ENV(env,TM_ENV(ctl))

	queuesPerPort = env->tree_structure.queuesToAnode 
					* env->tree_structure.aNodesToBnode
					* env->tree_structure.bNodesToCnode
					* env->tree_structure.cNodesToPort;
	

    /* Uninstall active Queues */
    for (i = 0; i < (int)ctl->tm_total_ports; i++)
	{
        for (j = 0; j < env->tree_structure.installedQueuesPerPort; j++)
		{
			rc= set_hw_uninstall_queue(hndl, queuesPerPort*i+j);
            if (rc) goto out;
        }
    }
out:
    COMPLETE_HW_WRITE;
	return rc;
}
