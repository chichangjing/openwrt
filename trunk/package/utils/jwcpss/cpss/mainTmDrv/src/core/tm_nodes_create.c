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
 * @brief TM nodes creation implementation.
 *
 * @file tm_nodes_create.c
 *
 * $Revision: 2.0 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

/*#define RESHUFFLING_SHIELD */

#include "tm_nodes_create.h"

#include "tm_core_types.h"

#include "tm_virtual_layer_interface.h"
#include "tm_locking_interface.h"
#include "tm_errcodes.h"
#include "tm_set_local_db_defaults.h"
#include "set_hw_registers.h"
#include "tm_os_interface.h"

#include "tm_nodes_ctl_internal.h"
#include "tm_hw_configuration_interface.h"
#include "tm_shaping_utils.h"
/* for validation shaping profiles during node creation */
#include "tm_shaping_internal.h"

#include "rm_status.h"
#include "rm_chunk.h"


/**
 * Reallocate all A nodes under a B node so that more A nodes can be
 * allocated.
 */
int tm_reallocate_a_nodes(tm_handle hndl,
                          uint32_t b_node_ind,
                          uint32_t num_of_children,
                          uint32_t *a_node_ind)
{
    int rc = 0;
    struct tm_b_node * b_node ;
    uint32_t a_old ;
    uint32_t a_old_hi ;
    uint32_t len ;
	uint32_t a_new_hi ;
    uint32_t a_new;
    uint32_t a, an;
    struct tm_a_node *ap;
	int q;
    struct node_mapping_t *a_map;

    TM_CTL(ctl, hndl)

    CONVERT_TO_PHYSICAL(B_LEVEL, b_node_ind)

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    if (b_node_ind >= ctl->tm_total_b_nodes) {
        rc = TM_CONF_B_NODE_IND_OOR;
        goto out;
    }

    b_node = &(ctl->tm_b_node_array[b_node_ind]);
    a_old = b_node->mapping.childLo;
    a_old_hi = b_node->mapping.childHi;
    len = a_old_hi - a_old + 1;

    if (b_node->mapping.nodeStatus != TM_NODE_USED) {
        rc = TM_CONF_B_NODE_IND_NOT_EXIST;
        goto out;
    }

    /** Find a free a-node range. */
    if (len >= num_of_children) {
        rc = 0;
        goto out;  /* Enough space for requested A nodes! */
    }

    rc = rm_allocate_chunk(ctl->rm, A_LEVEL, num_of_children, &a_new);
    if (rc < 0)
        goto out;
    a_new_hi = a_new + num_of_children - 1;

    /* First copy existing A nodes to their new position. Update the Q
     * node parent pointers and the B node children pointers. Update
     * references and reference counting.
     *
     * mapping.nodeStatus (FREE,USED,RESERVED,DELETED) is also copied.
     *
     * NB: The use_counters are invariant during reallocation.
     * Therefore we skip the ++ here and the -- during de-allocation.
     */
    for (a = a_old, an = a_new;
         a <= a_old_hi;
         a++, an++) {
        ap = &ctl->tm_a_node_array[a];
        ctl->tm_a_node_array[an] = *ap;  /* Almost all node state! */

        /* Beware, the AlvlDropProfPtr is duplicated. */
        ctl->tm_a_lvl_drop_prof_ptr[an] = ap->wred_profile_ref; /* Dupl.! */

        if (ap->mapping.nodeStatus == TM_NODE_USED) { /* Has children. */
            for (q = ap->mapping.childLo; q <= ap->mapping.childHi; q++) {
                ctl->tm_queue_array[q].mapping.nodeParent = an;
            }
        }
    }
    b_node->mapping.childLo = a_new;
    b_node->mapping.childHi = a_new_hi;

    /* Disable original tree top down before writing any other
     * registers. */
    rc = set_hw_node_disable(hndl, B_LEVEL, b_node_ind);
    assert(rc == 0);
    for (a = a_old; a <= a_old_hi; a++) {
        rc = set_hw_node_disable(hndl, A_LEVEL, a);
        assert(rc == 0);
    }

    /* Part from the eligibility functions; write all HW A node
     * registers, all Q node parent pointers, and finally the B node
     * child pointers.
     */
    for (a = a_new; a <= a_new_hi; a++) {
        rc = set_hw_a_node_params(hndl, a); /* Also AlvlDropProfPtr. */
        assert(rc == 0);
        rc = set_hw_node_shaping(hndl, A_LEVEL, a); /* From ShpProf DB. */
        assert(rc == 0);
        rc = set_hw_node_mapping(hndl, A_LEVEL, a);  /* Lo, Hi, Parent. */
        assert(rc == 0);

        a_map = &ctl->tm_a_node_array[a].mapping;
        if (a_map->nodeStatus == TM_NODE_USED) {  /* Has children. */
            for (q = a_map->childLo; q <= a_map->childHi; q++) {
                rc = set_hw_node_mapping(hndl, Q_LEVEL, q);
                assert(rc == 0);
            }
        }
        else if (a_map->nodeStatus == TM_NODE_FREE) {
            a_map->nodeStatus = TM_NODE_RESERVED;  /* SW only. */
        }
    }
    rc = set_hw_node_mapping(hndl, B_LEVEL, b_node_ind);
    assert(rc == 0);

    /* Enable the new tree bottom up. */
    for (a = a_new; a <= a_new_hi; a++) {
        rc = set_hw_node_elig_prio_function(hndl, A_LEVEL, a);
        assert(rc == 0);
    }
    rc = set_hw_node_elig_prio_function(hndl, B_LEVEL, b_node_ind);
    assert(rc == 0);

    /* Free the old A nodes and put them back in the free pool. */
    for (a = a_old; a <= a_old_hi; a++) {
        /* NB: use_counters are invariant. */
        set_sw_a_node_default(&ctl->tm_a_node_array[a], 1);
        ctl->tm_a_lvl_drop_prof_ptr[a] = TM_NO_DROP_PROFILE;  /* Dupl.! */
    }
    rc = rm_release_chunk(ctl->rm, A_LEVEL, len, a_old);

    if (rc == 0)
	{
#ifdef VIRTUAL_LAYER
        for (a = a_old, an = a_new; a <= a_old_hi; a++, an++) {
            if (ctl->tm_a_node_array[an].mapping.nodeStatus == TM_NODE_USED) {
                rc = change_physical_id_for_virtual(ctl->hVirtualLayer, A_LEVEL, a, an);
                assert(rc == 0);
            }
        }
        rc = tm_get_node_virtual_id(hndl, A_LEVEL, a_new, a_node_ind);
        assert(rc == 0);
#else
        *a_node_ind = a_new;
#endif
    }

out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

/**
 */

static int port_update_sw_image(tm_handle hndl,
                                enum tm_port_bw port_speed,
                                struct tm_port_params *params,
                                uint32_t min_token,
                                uint32_t max_token,
                                uint32_t divider,
                                uint8_t res_min_bw,
                                uint8_t res_max_bw,
                                uint8_t port_index)
{

    int i;
    struct tm_port *port = NULL;

    TM_CTL(ctl, hndl)

    port = &(ctl->tm_port_array[port_index]);

    /* Update Port SW DB */
    port->port_speed = port_speed;

    ctl->tm_p_lvl_drop_profiles[params->wred_profile_ref].use_counter++;
    ctl->tm_p_lvl_drop_prof_ptr[port_index] = params->wred_profile_ref;
    port->wred_profile_ref = params->wred_profile_ref;

    /* update shaping parameters */
    port->cir_token = min_token;
    port->eir_token = max_token;
    port->periods = divider;
    port->min_token_res = res_min_bw;
    port->max_token_res = res_max_bw;

	port->cir_sw = params->cir_bw;
	port->eir_sw = params->eir_bw;
    if (params->cir_bw < TM_INVAL)
	{
        port->cir_burst_size = params->cbs;
        port->eir_burst_size = params->ebs;
    }
	else
	{
		/* no shaping for port */
        port->cir_burst_size = 0x1FFFF;
        port->eir_burst_size = 0x1FFFF;
    }

    /* DWRR for Port */
    for (i=0; i<8; i++)
        port->dwrr_quantum_per_level[i].quantum = params->quantum[i];


    /* DWRR for C-nodes in Port's range */
    port->dwrr_enable_per_priority_mask = 0;
    for (i=0; i<8; i++)
        port->dwrr_enable_per_priority_mask =
            port->dwrr_enable_per_priority_mask | (params->dwrr_priority[i] << i);

    /* Update sw image with eligible priority function pointer */
    port->elig_prio_func = params->elig_prio_func_ptr;
    return 0;
}


/**
 */
static int port_calc_ranges(uint16_t parents,
			    uint32_t children,
			    uint32_t *norm_range,
			    uint32_t *last_range)
{
  uint32_t range;
  uint32_t remainder;

  if (parents == 1) {
    *norm_range = 0;
    *last_range = children;
    return 0;
  }

  range = tm_ceil((float)children /(float)parents);

  if (range * parents != children)
    remainder = children - range * (parents - 1);
  else
    remainder = range;

  if (remainder > range)
    return -ENOMEM;

  *norm_range = range;
  *last_range = remainder;
  return 0;
}


/**
 */
static void set_a_node_params_default(struct tm_a_node_params *params)
{
    int i;

    if (params)
	{
		params->shaping_profile_ref = TM_INF_SHP_PROFILE;
		params->quantum = 0x40;
		params->wred_profile_ref = 0;
		for (i=0; i<8; i++)
			params->dwrr_priority[i] = 0;
		params->elig_prio_func_ptr = TM_NODE_DISABLED_FUN;
	}
}


/**
 */
static void set_b_node_params_default(struct tm_b_node_params *params)
{
    int i;

    if (params)
	{
		params->shaping_profile_ref = TM_INF_SHP_PROFILE;
		params->quantum = 0x40;
		params->wred_profile_ref = 0;
		for (i=0; i<8; i++)
			params->dwrr_priority[i] = 0;
		params->elig_prio_func_ptr = TM_NODE_DISABLED_FUN;
	}
}


/**
 */
static void set_c_node_params_default(struct tm_c_node_params *params)
{
    int i;

    if(params)
	{
    params->shaping_profile_ref = TM_INF_SHP_PROFILE;
    params->quantum = 0x40;
    params->wred_cos = 0;
		for (i=0; i<8; i++)
		{
        params->dwrr_priority[i] = 0;
        params->wred_profile_ref[i] = 0;
    }
    params->elig_prio_func_ptr = TM_NODE_DISABLED_FUN;
	}
}

/**
 */
int get_tm_port_bw_by_port_type(enum tm_port_bw port_type, uint32_t *cir_bw_ptr, uint32_t *eir_bw_ptr)
{
    uint32_t    port_speed = 0;
    int         rc = 0;

    switch (port_type)
    {
        case TM_1G_PORT:
            port_speed = TM_1G_SPEED;
            break;
        case TM_2HG_PORT:
            port_speed = TM_2HG_SPEED;
            break;
        case TM_10G_PORT:
            port_speed = TM_10G_SPEED;
            break;
        case TM_40G_PORT:
            port_speed = TM_40G_SPEED;
            break;
        case TM_50G_PORT:
            port_speed = TM_50G_SPEED;
            break;
        case TM_100G_PORT:
            port_speed = TM_100G_SPEED;
            break;
        default:
            rc = TM_CONF_PORT_SPEED_OOR;
            goto out;
    }

    if (*cir_bw_ptr == TM_INF_SHP_BW)
    {
        *cir_bw_ptr = TM_MAX_BW;
    }
    else if(*cir_bw_ptr > port_speed)
    {
        rc = TM_CONF_PORT_BW_OUT_OF_SPEED;
        goto out;
    }

    if (*eir_bw_ptr == TM_INF_SHP_BW)
    {
        *eir_bw_ptr = TM_MAX_BW;
    }
    else if(*eir_bw_ptr > port_speed)
    {
        rc = TM_CONF_PORT_BW_OUT_OF_SPEED;
        goto out;
    }

out:
    return rc;
}

/***************************************************************************
 * Port Creation 
 ***************************************************************************/

/**
 */
int tm_create_asym_port_ex(tm_handle hndl, uint8_t port_index,
                        enum tm_port_bw port_speed,
                        struct tm_port_params *params,
						uint32_t first_child_in_range)
{
    struct tm_port *port = NULL;
	uint32_t    min_token;
	uint32_t	max_token;
	uint8_t	    res_min_bw;
	uint8_t	    res_max_bw;
	uint32_t	periods;
    uint32_t	cir_bw, eir_bw;
    uint32_t	cbs;
    uint32_t	ebs;

    int rc = 0;
    uint32_t i;

    TM_CTL(ctl, hndl)

	if (params==NULL) return -EINVAL;

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    /* Check parameters validity */

    VALIDATE_NODE_FREE_EX(port,port_index, TM_CONF_PORT_IND_OOR, TM_CONF_PORT_IND_USED)
    if (rc) goto out;

    port = &(ctl->tm_port_array[port_index]);

    /* check port drop profile reference */
  	TEST_RESOURCE_EXISTS(rm_port_drop_profile_status(ctl->rm, params->wred_profile_ref) ,/* out_of range error*/TM_CONF_P_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_P_WRED_PROF_REF_OOR)
 	if (rc) goto out;

    /* Port params */
	/* DWRR for Port */
    for (i=0; i<8; i++)
    {
		VALIDATE_PORT_QUANTUM_VALUE(params->quantum[i],TM_CONF_PORT_QUANTUM_OOR)
		if(rc) goto out;
	}

    /* DWRR for C-nodes in Port's range */
    for (i=0; i<8; i++)
    {
        if ((params->dwrr_priority[i] != TM_DISABLE) &&
            (params->dwrr_priority[i] != TM_ENABLE))
		{
            rc = TM_CONF_PORT_DWRR_PRIO_OOR;
            goto out;
        }
    }

	VALIDATE_ELIG_FUNCTION(params->elig_prio_func_ptr)

    if ((params->num_of_children > rm_get_max_chunk_size(ctl->rm,C_LEVEL)) || (params->num_of_children == 0))
	{
        rc = TM_CONF_INVALID_NUM_OF_C_NODES;
        goto out;
    }

   /* if shaping is defined for a port */
    if (ctl->level_data[P_LEVEL].shaping_status==TM_ENABLE)
	{
        cir_bw = params->cir_bw; 
        eir_bw = params->eir_bw; 
        /* check that it fits the max physical port speed */
        rc = get_tm_port_bw_by_port_type(port_speed, &cir_bw, &eir_bw);
        if (rc)
        {
            goto out;
        }
      
        if ((params->cbs > TM_128M_kB ) || (params->ebs > TM_128M_kB ))
        {
            rc = TM_CONF_PORT_BS_OOR;
            goto out;
        }

        if (cir_bw == TM_MAX_BW)
            cbs = TM_MAX_BURST;
        else
            cbs = params->cbs;

        if (eir_bw == TM_MAX_BW)
            ebs = TM_MAX_BURST;
        else
            ebs = params->ebs;
		
		rc = calculate_port_shaping(1000.0*cir_bw/ctl->level_data[P_LEVEL].unit,
									1000.0*eir_bw/ctl->level_data[P_LEVEL].unit,
									&cbs,
									&ebs,
									(1 << ctl->level_data[P_LEVEL].token_res_exp),
									&periods,
									&res_min_bw,
									&res_max_bw,
									&min_token,
									&max_token);
        if (rc)
            goto out;

#if DEBUG
        PRINT_PORT_SHAPING_CALCULATION_RESULT(cir_bw, eir_bw)
#endif
    }
	else
	{   /* no shaping for port */
        periods = 1;     /* Small divider (shared). */
        res_min_bw = 7;  /* Large CIR exponent. */
        res_max_bw = 7;  /* Large EIR exponent. */
        min_token = 0x7FF;
        max_token = 0x7FF;
    }

    port->sym_mode = TM_DISABLE;

	rc=allocate_asym_port(ctl,port_index,params->num_of_children, first_child_in_range);
	if (rc) goto out;

    /* on params->cir_bw / params->eir_bw = TM_INF_SHP_BW
       TM_MAX_BW is used for shaping calculations and
       TM_INF_SHP_BW is kept in sw image */
    port_update_sw_image(hndl, port_speed, params,
                         min_token, max_token, periods,
                         res_min_bw, res_max_bw, port_index);
    /* Download to HW */
    rc = set_hw_port(hndl, port_index);
    if (rc < 0)
        rc = TM_HW_PORT_CONFIG_FAIL;

out:
    if (rc)
	{
        if (rc == TM_HW_PORT_CONFIG_FAIL)
		{
			free_asym_port(ctl,port_index);
			set_sw_port_default(port,0);
			ctl->tm_p_lvl_drop_prof_ptr[port_index] = 0;
			ctl->tm_p_lvl_drop_profiles[params->wred_profile_ref].use_counter--;
        }
    }
    tm_sched_unlock(TM_ENV(ctl));
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

int tm_create_asym_port(tm_handle hndl, uint8_t port_index,
                        enum tm_port_bw port_speed,
                        struct tm_port_params *params)
{
	return 	tm_create_asym_port_ex(hndl, port_index, port_speed, params, -1 /* no preallocated range */);
}

int tm_create_asym_port_with_c_node_range(tm_handle hndl,
											uint8_t port_index,
											enum tm_port_bw port_speed,
											struct tm_port_params *params,
											uint32_t first_c_node_in_range, uint32_t c_nodes_range)
{
	params->num_of_children = c_nodes_range;
	CONVERT_TO_PHYSICAL(C_LEVEL,first_c_node_in_range)
	return tm_create_asym_port_ex(hndl, port_index, port_speed, params, first_c_node_in_range);
}

/**
 */
int tm_create_port(tm_handle hndl, uint8_t port_index,
                   enum tm_port_bw port_speed,
                   struct tm_port_params *params,
                   uint16_t num_of_c_nodes,
                   uint16_t num_of_b_nodes,
                   uint16_t num_of_a_nodes,
                   uint32_t num_of_queues)
{
    struct tm_port *port = NULL;
    uint16_t parents;
    uint32_t children;
    uint32_t norm_range;
    uint32_t last_range;
    int rc = 0;

    TM_CTL(ctl, hndl)

	if (params==NULL) return -EINVAL;

    /* Check parameters validity */
    if (port_index >= ctl->tm_total_ports)
	{
        rc = TM_CONF_PORT_IND_OOR;
        goto out;
    }

    port = &(ctl->tm_port_array[port_index]);
	if (port->mapping.nodeStatus != TM_NODE_FREE)
	{
        rc = TM_CONF_PORT_IND_USED;
        goto out;
	}

    if ((num_of_c_nodes > rm_get_max_chunk_size(ctl->rm,C_LEVEL)) || (num_of_c_nodes == 0))
	{
        rc = TM_CONF_INVALID_NUM_OF_C_NODES;
        goto out;
    }

    if ((num_of_b_nodes > rm_get_max_chunk_size(ctl->rm,B_LEVEL)) || (num_of_c_nodes > num_of_b_nodes))
	{
        rc = TM_CONF_INVALID_NUM_OF_B_NODES;
        goto out;
    }

    if ((num_of_a_nodes > rm_get_max_chunk_size(ctl->rm,A_LEVEL)) || (num_of_b_nodes > num_of_a_nodes))
	{
        rc = TM_CONF_INVALID_NUM_OF_A_NODES;
        goto out;
    }

    if ((num_of_queues > rm_get_max_chunk_size(ctl->rm,Q_LEVEL)) || (num_of_a_nodes > num_of_queues))
	{
        rc = TM_CONF_INVALID_NUM_OF_QUEUES;
        goto out;
    }


    params->num_of_children = num_of_c_nodes;

    rc = tm_create_asym_port(ctl, port_index, port_speed, params);
    if (rc)
        goto out;

    /* Calculate ranges */
    port->children_range.norm_range[C_LEVEL] = num_of_c_nodes;

    /* C to B levels ranges */
    parents = num_of_c_nodes;
    children = num_of_b_nodes;

    rc = port_calc_ranges(parents, children, &norm_range, &last_range);
    if (rc < 0)
      goto out;

    port->children_range.norm_range[B_LEVEL] = norm_range;
    port->children_range.last_range[B_LEVEL] = last_range;

    /* B to A levels ranges */
    parents = num_of_b_nodes;
    children = num_of_a_nodes;

    rc = port_calc_ranges(parents, children, &norm_range, &last_range);
    if (rc < 0)
      goto out;

    port->children_range.norm_range[A_LEVEL] = norm_range;
    port->children_range.last_range[A_LEVEL] = last_range;

    /* A to Q levels ranges */
    parents = num_of_a_nodes;
    children = num_of_queues;

    rc = port_calc_ranges(parents, children, &norm_range, &last_range);
    if (rc < 0)
      goto out;

    port->children_range.norm_range[Q_LEVEL] = norm_range;
    port->children_range.last_range[Q_LEVEL] = last_range;


    /* Set sym mode */
    port->sym_mode = TM_ENABLE;

out:
    return rc;
}


/**
 */
int tm_config_port_drop_per_cos(tm_handle hndl, uint8_t port_index,
                   struct tm_port_drop_per_cos *params)
{
    struct tm_port *port = NULL;
    int rc = 0;
    int i;

    TM_CTL(ctl, hndl)

	if (params==NULL) return -EINVAL;

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

	/* Check if port exists */
 	VALIDATE_NODE_ALLOCATED_EX(port, port_index, TM_CONF_PORT_IND_OOR, TM_CONF_PORT_IND_NOT_EXIST)
	if (rc) goto out;

    port = &(ctl->tm_port_array[port_index]);

    /* check port drop profile reference */
    for (i=0; i<TM_WRED_COS; i++)
    {
        if (params->wred_cos & (1<<i))
        {
			TEST_RESOURCE_EXISTS(rm_port_drop_profile_status_cos(ctl->rm, (uint8_t)i, params->wred_profile_ref[i]) ,/* out_of range error*/TM_CONF_P_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_P_WRED_PROF_REF_OOR)
			if (rc) goto out;
        }
    }

    for (i=0; i<TM_WRED_COS; i++)
    {
        /* Update Drop profile pointer */
        if (params->wred_cos & (1<<i))
        {
            ctl->tm_p_lvl_drop_prof_ptr_cos[i][port_index] = params->wred_profile_ref[i];
            ctl->tm_p_lvl_drop_profiles_cos[i][params->wred_profile_ref[i]].use_counter++;
            port->wred_profile_ref_cos[i] = params->wred_profile_ref[i];
        }
        else
        {
            ctl->tm_p_lvl_drop_prof_ptr_cos[i][port_index] = 0;
            ctl->tm_p_lvl_drop_profiles_cos[i][0].use_counter++;
            port->wred_profile_ref_cos[i] = 0;
        }
    }
    port->wred_cos = params->wred_cos;

    /* Download to HW */
	rc = set_hw_port_drop_cos(hndl, port_index);
	if (rc < 0)
	{
		rc = TM_HW_PORT_CONFIG_FAIL;
		goto out;
    }

out:
    if (rc)
    {
        if (rc == TM_HW_PORT_CONFIG_FAIL)
        {
            for (i=0; i<TM_WRED_COS; i++)
            {
                if (params->wred_cos & (1<<i))
                {
                    ctl->tm_p_lvl_drop_prof_ptr_cos[i][port_index] = 0;
                    ctl->tm_p_lvl_drop_profiles_cos[i][params->wred_profile_ref[i]].use_counter--;
                    port->wred_profile_ref_cos[i] = 0;
                }
            }
            port->wred_cos = 0; /* bit map */
        }
    }
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/***************************************************************************
 * Queue Creation
 ***************************************************************************/

/**
 */
int tm_create_queue_to_port(tm_handle hndl, uint8_t port_index,
                            struct tm_queue_params *q_params,
                            struct tm_a_node_params *a_params,
                            struct tm_b_node_params *b_params,
                            struct tm_c_node_params *c_params,
                            uint32_t *queue_index_ptr,
                            uint32_t *a_node_index_ptr,
                            uint32_t *b_node_index_ptr,
                            uint32_t *c_node_index_ptr)
{
    int rc = 0;
	TM_CTL(ctl,hndl)

    rc = tm_create_a_node_to_port(hndl, port_index, a_params, b_params, c_params,
                                  a_node_index_ptr, b_node_index_ptr, c_node_index_ptr);
    if (rc)
        return rc;

    rc = tm_create_queue_to_a_node(hndl, *a_node_index_ptr, q_params, queue_index_ptr);
    if (rc)
	{
        delete_a_node_proc(hndl, *a_node_index_ptr);
        delete_b_node_proc(hndl, *b_node_index_ptr);
        delete_c_node_proc(hndl, *c_node_index_ptr);
    }
    return rc;
}


/**
 */
int tm_create_trans_queue_to_port(tm_handle hndl,
                                  uint8_t port_index,
                                  struct tm_queue_params *q_params,
                                  uint32_t * queue_index_ptr)
{
    struct tm_port *port = NULL;
    struct tm_c_node_params c_params;
    struct tm_b_node_params b_params;
    struct tm_a_node_params a_params;
    uint32_t c_index;
    uint32_t b_index;
    uint32_t a_index;
    int rc = 0;
    int p_flag = TM_DISABLE;

    TM_CTL(ctl, hndl)

	if (q_params==NULL) return -EINVAL;
	if (queue_index_ptr==NULL) return -EINVAL;

    /* Check parameters validity */
	VALIDATE_NODE_ALLOCATED_EX(port, port_index, TM_CONF_PORT_IND_OOR, TM_CONF_PORT_IND_NOT_EXIST)
	if (rc) goto out;

    port = &(ctl->tm_port_array[port_index]);

    if (ctl->tm_port_array[port_index].sym_mode != TM_ENABLE)
	{
      rc = -EFAULT;
      goto out;
    }

	c_index = port->mapping.childLo;

    /* Check that there is only one possible child in path to queues */
    if (c_index != (uint32_t)port->mapping.childHi)
	{
      rc = -EFAULT;
      goto out;
    }

    if ((port->children_range.norm_range[B_LEVEL] != 0) ||
	   (port->children_range.last_range[B_LEVEL] != 1))
	{
      rc = -EFAULT;
      goto out;
    }

    if ((port->children_range.norm_range[A_LEVEL] != 0) ||
	(port->children_range.last_range[A_LEVEL] != 1))
	{
      rc = -EFAULT;
      goto out;
    }

    /* q_params are checked inside one of the internal calls */

	/* Find free queue */
	if (ctl->tm_c_node_array[c_index].mapping.nodeStatus == TM_NODE_USED)
	{
		/* Transparent path C-B-A-nodes exists */
		b_index = ctl->tm_c_node_array[c_index].mapping.childLo;
		a_index = ctl->tm_b_node_array[b_index].mapping.childLo;
		/*convert to virtualID (if necessary  , in order to use api call below */
		CONVERT_TO_VIRTUAL(A_LEVEL,a_index)
	}
	else
	{
		/* Transparent path C-B-A-nodes doesn't exist */
		/* Create transparent path , use transparent settings for a,b,c node parameters */
		set_a_node_params_default(&a_params);
		set_b_node_params_default(&b_params);
		set_c_node_params_default(&c_params);
		/* set eligible function "always eligible" for transparent nodes */
		a_params.elig_prio_func_ptr = TM_ELIG_N_FP0;
		b_params.elig_prio_func_ptr = TM_ELIG_N_FP0;
		c_params.elig_prio_func_ptr = TM_ELIG_N_FP0;
		rc = tm_create_a_node_to_port(ctl, port_index,
					&a_params, &b_params, &c_params,
					&a_index, &b_index, &c_index);
		if (rc)
			goto out;
		p_flag = TM_ENABLE;
    }
    rc = tm_create_queue_to_a_node(hndl, a_index, q_params, queue_index_ptr);

out:
    if (rc)
	{
        if (p_flag == TM_ENABLE)
		{
            delete_a_node_proc(ctl,  a_index);
            delete_b_node_proc(ctl,  b_index);
            delete_c_node_proc(ctl,  c_index);
        }
    }
    return rc;
}


/**
 */
int tm_create_queue_to_c_node(tm_handle hndl, uint32_t c_node_index,
                              struct tm_queue_params *q_params,
                              struct tm_a_node_params *a_params,
                              struct tm_b_node_params *b_params,
                              uint32_t *queue_index_ptr,
                              uint32_t *a_node_index_ptr,
                              uint32_t *b_node_index_ptr)
{
    int rc = 0;

    rc = tm_create_a_node_to_c_node(hndl, c_node_index, a_params, b_params,
                                    a_node_index_ptr, b_node_index_ptr);
    if (rc)
        return rc;

    rc = tm_create_queue_to_a_node(hndl, *a_node_index_ptr, q_params, queue_index_ptr);
    if (rc)
	{
        delete_a_node_proc(hndl, *a_node_index_ptr);
        delete_b_node_proc(hndl, *b_node_index_ptr);
    }
    return rc;
}


/**
 */
int tm_create_queue_to_b_node(tm_handle hndl, uint32_t b_node_index,
                              struct tm_queue_params *q_params,
                              struct tm_a_node_params *a_params,
                              uint32_t *queue_index_ptr,
                              uint32_t *a_node_index_ptr)
{
    int rc = 0;

    rc = tm_create_a_node_to_b_node(hndl, b_node_index, a_params, a_node_index_ptr);
    if (rc)
        return rc;

    rc = tm_create_queue_to_a_node(hndl, *a_node_index_ptr, q_params, queue_index_ptr);
    if (rc)
	{
        delete_a_node_proc(hndl, *a_node_index_ptr);
    }
    return rc;
}


/**
 */
int tm_create_queue_to_a_node_ex(tm_handle hndl, uint32_t parent_a_node_index,
                              struct tm_queue_params *q_params,
                              uint32_t * queue_index_ptr)
{
    struct tm_a_node *parent_a_node = NULL;
    struct tm_queue *queue = NULL;
    int rc = 0;
	int	i;
	tm_node_status_t old_node_status;

    TM_CTL(ctl, hndl)

	if (q_params==NULL) return -EINVAL;
	if (queue_index_ptr==NULL) return -EINVAL;

	CONVERT_TO_PHYSICAL(A_LEVEL,parent_a_node_index)

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc)  return rc;
	/*******************************************************************************/
	/** Check parameters validity                                                  */
	/*******************************************************************************/
    VALIDATE_NODE_ALLOCATED_EX(a_node, parent_a_node_index, TM_CONF_A_NODE_IND_OOR, TM_CONF_A_NODE_IND_NOT_EXIST)
	if (rc) goto out;

    parent_a_node = &(ctl->tm_a_node_array[parent_a_node_index]);

    /* Queue params */
	rc= check_shaping_profile_validity(hndl,q_params->shaping_profile_ref, Q_LEVEL);
	if (rc) goto out;

	VALIDATE_ELIG_FUNCTION(q_params->elig_prio_func_ptr)

	/* check if queue drop profile already exists */
 	TEST_RESOURCE_EXISTS(rm_queue_drop_profile_status(ctl->rm, q_params->wred_profile_ref),/* out_of range error*/TM_CONF_Q_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_Q_WRED_PROF_REF_OOR)
 	if (rc) goto out;
    /* check quantum validity*/
	VALIDATE_NODE_QUANTUM_VALUE(q_params->quantum, TM_CONF_Q_QUANTUM_OOR)
	if (rc) goto out;

	/*******************************************************************************/
	/** all input parameters are valid, select queue                         */
	/*******************************************************************************/

	if (*queue_index_ptr !=(uint32_t)-1)
	{
		/*******************************************************************************/
		/**  queue is selected by user - test if itis valid & free                     */
		/*******************************************************************************/
		i = *queue_index_ptr;
 		if ((i <  parent_a_node->mapping.childLo)  || (i >  parent_a_node->mapping.childHi))
		{
			/* nothing to do in caseof queues , unrecoverable state */
			rc = -ENOBUFS;
			goto out;
		}
		if (ctl->tm_queue_array[i].mapping.nodeStatus <= TM_NODE_USED)
		{
			/* queue is already allocated,  error state */
			rc = -ENOBUFS;
			goto out;
		}
	}
	else
	{
		/*******************************************************************************/
		/**  queis nor preselected- Find free Queue                            */
		/*******************************************************************************/
		/* looking for reserved or previously deleted nodes in the range */
 		for (i= parent_a_node->mapping.childLo ;  i <= parent_a_node->mapping.childHi ; i++)
		{
		if (ctl->tm_queue_array[i].mapping.nodeStatus > TM_NODE_USED) break;
		}
		if (i > parent_a_node->mapping.childHi )   /* all nodes are already used */
		{
			/* nothing to do in caseof queues , unrecoverable state */
			rc = -ENOBUFS;
			goto out;
		}
	}

	/**************************************************************************/
	/** queue selected                                                          */
	/**************************************************************************/
    /* for queues allocate_reserved_queue(..) always errorless */
    allocate_reserved_queue(ctl,(uint32_t)i, parent_a_node_index, & old_node_status);
    *queue_index_ptr = i;
    queue = &(ctl->tm_queue_array[*queue_index_ptr]);
	/*******************************************************************************/
	/** update queue node parameters                                               */
	/*******************************************************************************/
    /* Update Queue SW DB */
	rc=add_node_to_shaping_profile(ctl,q_params->shaping_profile_ref, Q_LEVEL, *queue_index_ptr);
	if (rc < 0) goto out;

    queue->wred_profile_ref = q_params->wred_profile_ref;
    ctl->tm_q_lvl_drop_prof_ptr[*queue_index_ptr] = q_params->wred_profile_ref;
    ctl->tm_q_lvl_drop_profiles[q_params->wred_profile_ref].use_counter++;
    queue->elig_prio_func = q_params->elig_prio_func_ptr;

    /* DWRR for Queue - update even if disabled */
    queue->dwrr_quantum = q_params->quantum;

	/**************************************************************************/
	/** queue parameters are set , let update H/W                             */
	/**************************************************************************/
    /* Download Queue to HW */
    if (set_hw_node_mapping(hndl, Q_LEVEL, *queue_index_ptr)
	 || set_hw_queue_params(hndl, *queue_index_ptr)
	 || set_hw_node_elig_prio_function(hndl, Q_LEVEL, *queue_index_ptr))
	{
        rc = TM_HW_QUEUE_CONFIG_FAIL;
        goto out;
    }
    /* Download Queue shaping parameters (from profile) to HW */
    if (set_hw_node_shaping(hndl, Q_LEVEL, *queue_index_ptr))
    {
		rc = TM_HW_SHAPING_PROF_FAILED;
		goto out;
	}

	/*******************************************************************************/
	/** here  node successfully created.                                           */
	/*******************************************************************************/
	/** if  virtual numbers on queues should require   - create virtual ID & add entry to dictionary
	 GENERATE_VIRTUAL_ID (Q_LEVEL, *queue_index);
	 ************************************************/

out:
    if (rc)
	{
        if (rc == TM_HW_QUEUE_CONFIG_FAIL)
		{
			remove_node_from_shaping_profile(hndl, queue->shaping_profile_ref, Q_LEVEL, *queue_index_ptr);
            ctl->tm_q_lvl_drop_prof_ptr[*queue_index_ptr] = 0;
            ctl->tm_q_lvl_drop_profiles[q_params->wred_profile_ref].use_counter--;
            set_sw_queue_default(queue,0); /* reset queue parameters without mapping */
            free_allocated_queue(ctl,*queue_index_ptr,old_node_status);		  /* restore queue mapping */
        }
    }
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


int tm_create_queue_to_a_node(tm_handle hndl, uint32_t parent_a_node_index,
                              struct tm_queue_params *q_params,
                              uint32_t * queue_index_ptr)
{
	*queue_index_ptr = (uint32_t) -1;
	return tm_create_queue_to_a_node_ex(hndl, parent_a_node_index, q_params, queue_index_ptr);
}

int tm_init_selected_queue_of_a_node(tm_handle hndl, uint32_t parent_a_node_index,
                              struct tm_queue_params *q_params,
                              uint32_t queue_index)
{
	/* test if queue_index!= -1 */
	return tm_create_queue_to_a_node_ex(hndl, parent_a_node_index, q_params, &queue_index);
}



/***************************************************************************
 * A-node Creation
 ***************************************************************************/

/**
 */
int tm_create_a_node_to_port(tm_handle hndl, uint8_t port_index,
                             struct tm_a_node_params *a_params,
                             struct tm_b_node_params *b_params,
                             struct tm_c_node_params *c_params,
                             uint32_t *a_node_index_ptr,
                             uint32_t *b_node_index_ptr,
                             uint32_t *c_node_index_ptr)
{
    int rc = 0;

    rc = tm_create_b_node_to_port(hndl, port_index, b_params, c_params,
                                  b_node_index_ptr, c_node_index_ptr);
    if (rc)
        return rc;

    rc = tm_create_a_node_to_b_node(hndl, *b_node_index_ptr, a_params, a_node_index_ptr);
    if (rc)
	{
        delete_b_node_proc(hndl, *b_node_index_ptr);
        delete_c_node_proc(hndl, *c_node_index_ptr);
    }
    return rc;
}


/**
 */
int tm_create_a_node_to_c_node(tm_handle hndl, uint32_t c_node_index,
                               struct tm_a_node_params *a_params,
                               struct tm_b_node_params *b_params,
                               uint32_t *a_node_index_ptr,
                               uint32_t *b_node_index_ptr)
{
    int rc = 0;

    rc = tm_create_b_node_to_c_node(hndl, c_node_index, b_params, b_node_index_ptr);
    if (rc)
        return rc;

    rc = tm_create_a_node_to_b_node(hndl, *b_node_index_ptr, a_params, a_node_index_ptr);
    if (rc)
	{
        delete_b_node_proc(hndl, *b_node_index_ptr);
    }
    return rc;
}


/**
 */
/***********************************************************************************************************************/

/* because code for advanced node allocation for A & B layer is identical  except  layer names  (a_node / b_node  , queue/a_node  A_LEVEL/B_LEVEL)
 * writing subroutine is not impossible (a_node and b_node are different types with different sizes, we deal witkh arrays of them ),
 * copy/paste is error prone
 * I developed  some MACRO below ,which are instaniated by node types */



/* utilizing neghborhoods */

#define UTILIZE_NEGHBORHOOD_DECLARATIONS(node_type,parent_node_type)	\
		struct tm_##node_type 			*neighbor_node = NULL;\
		struct tm_##parent_node_type 	*neighbor_parent = NULL;\
		int	left_parent_neighborhood_index = -1;\
		int	right_parent_neighborhood_index = -1;\
		int	parent_update = 0;



#define UTILIZE_NEGHBORHOOD( LEVEL,node_type, parent_node_type, parent_node)	\
		if ((!found) && ((uint32_t)parent_node->mapping.childLo >0))\
		{\
			/* left neighbor node */\
			neighbor_node=&(ctl->tm_##node_type##_array[parent_node->mapping.childLo - 1]);\
			switch (neighbor_node->mapping.nodeStatus)\
			{\
				case TM_NODE_DELETED:\
				case TM_NODE_RESERVED:\
					/* find left neighborhood of the parent node */\
					left_parent_neighborhood_index=neighbor_node->mapping.nodeParent;\
					/* remove neighbor_node from it : update it's  child range */\
					neighbor_parent=&(ctl->tm_##parent_node_type##_array[left_parent_neighborhood_index]);\
					if (ENABLE_REDUCE_NODE_RANGE( neighbor_parent))\
					{\
						neighbor_parent->mapping.childHi--; /* what about zero range ??? */\
						found=1;\
					}\
					break;\
				case TM_NODE_FREE:\
					/* extract one position from free resources pool - must be chunk left of the node*/\
					rm_expand_chunk_left(ctl->rm,LEVEL,parent_node->mapping.childLo - 1);\
					found=1;\
					break;\
				default: break; /*failed , need to continue search*/\
			}\
			if (found)\
			{\
				/* expand parent range left */\
				parent_node->mapping.childLo--;\
				/* assign index of node found */\
				i=parent_node->mapping.childLo;\
				/* set flag to H/W update of the parent node */\
				parent_update = -1;\
			}\
		}\
		/* test right neighbor*/\
		if ((!found) && ((uint32_t)(parent_node->mapping.childHi) < ctl->tm_total_##parent_node_type##s-1))\
		{\
			/* right neighbor node */\
			neighbor_node=&(ctl->tm_##node_type##_array[parent_node->mapping.childHi +1]);\
			switch (neighbor_node->mapping.nodeStatus)\
			{\
				case TM_NODE_DELETED:\
				case TM_NODE_RESERVED:\
					/* find left neighborhood of the parent node */\
					right_parent_neighborhood_index=neighbor_node->mapping.nodeParent;\
					/* remove neighbor_node from it : update it's  child range */\
					neighbor_parent=&(ctl->tm_##parent_node_type##_array[right_parent_neighborhood_index]);\
					if (ENABLE_REDUCE_NODE_RANGE( neighbor_parent))\
					{\
						neighbor_parent->mapping.childLo++; \
						found=1;\
					}\
				case TM_NODE_FREE:\
					rm_expand_chunk_right(ctl->rm,LEVEL,parent_node->mapping.childHi +1);\
					found=1;\
					break;\
				default: break;  /*failed , need to continue search*/\
			}\
			if (found)\
			{\
				/* expand parent range right */\
				parent_node->mapping.childHi++;\
				/* assign index of node found */\
				i=parent_node->mapping.childHi;	\
				/* set flag to H/W update of the parent node */\
				parent_update = 1;\
			}\
		}


#define UPDATE_CHILDREN_MAPPING(level, ERROR)
/*
#define UPDATE_CHILDREN_MAPPING(level, ERROR)	\
	for (i = node->mapping.childLo ; i <= node->mapping.childHi; i++)\
	{\
		if (set_hw_node_mapping(hndl,CHILD_LEVEL(level),i))\
		{\
			rc = ERROR;\
			goto out;\
		}\
		if (set_hw_node_elig_prio_function(hndl,CHILD_LEVEL(level),i))\
		{\
			rc = ERROR;\
			goto out;\
		}\
	}
*/

#define	UPDATE_PARENTS_MAPPING(LEVEL, parent_index , ERROR)	\
	if (right_parent_neighborhood_index>= 0)\
	{\
		if (set_hw_node_mapping(hndl,PARENT_LEVEL(LEVEL),right_parent_neighborhood_index))\
		{\
			rc = ERROR;\
			goto out;\
		}\
	}\
	if (left_parent_neighborhood_index >= 0)\
	{\
		if (set_hw_node_mapping(hndl,PARENT_LEVEL(LEVEL),left_parent_neighborhood_index))\
		{\
			rc = ERROR;\
			goto out;\
		}\
	}\
	if (parent_update)\
	{\
		if (set_hw_node_mapping(hndl,PARENT_LEVEL(LEVEL),parent_index))\
		{\
			rc = ERROR;\
			goto out;\
		}\
	}


#define	RESTORE_PARENTS_MAPPING(parent_node)	\
	/* restore parent neighborhood mapping - if any */\
	if (right_parent_neighborhood_index >= 0)	neighbor_parent->mapping.childLo--; \
	if (left_parent_neighborhood_index  >= 0) 	neighbor_parent->mapping.childHi++; \
	/* restore parent mapping - if any */\
	if (parent_update > 0) 						parent_node->mapping.childHi--; \
	if (parent_update < 0) 						parent_node->mapping.childLo++; \


/* minimum impact reshuffling */


#ifdef VIRTUAL_LAYER
	#define	UPDATE_INDEXES_DECLARATIONS
	#define UPDATE_INDEXES_IMPLEMENTATION(LEVEL,parent_index)	rc= change_physical_id_for_virtual(ctl->hVirtualLayer, LEVEL, /*old index */level_index+oldChildLo, /*new index*/ level_index+newChildLo);
#else
	#define	UPDATE_INDEXES_DECLARATIONS		struct tm_tree_change *change = NULL;
	#define UPDATE_INDEXES_IMPLEMENTATION(LEVEL,parent_index)	\
			change = (struct tm_tree_change *)tm_malloc(sizeof(struct tm_tree_change));\
			if (change == NULL)\
			{\
				rc = -ENOMEM;\
				goto out;\
			}\
			else\
			{\
				change->index = parent_index;\
				change->old_index = level_index+oldChildLo;\
				change->new_index = level_index+newChildLo;\
				change->next = ctl->list.next;\
				ctl->list.next = change;\
			}
#endif


#define MIN_IMPACT_RESHUFFLING(parent_node,parent_index,LEVEL,node_type,child_node_type,drop_profile_ptr_array,ERROR)	\
	{\
			int level_index;\
			int child_index;\
			int oldChildLo;\
			int newChildLo;\
			uint32_t ui;\
			UPDATE_INDEXES_DECLARATIONS\
			/* current range is : parent_node->mapping.childHi - parent_node->mapping.childLo+1;*/\
			range = parent_node->mapping.childHi - parent_node->mapping.childLo+1;\
			/* desired range is : parent_node->mapping.childHi - parent_node->mapping.childLo +2 */\
			if (!rm_allocate_chunk(ctl->rm, LEVEL, range+1, &ui))\
			{\
				/* update  S/W db */\
				oldChildLo=parent_node->mapping.childLo;\
				newChildLo=ui;\
				parent_node->mapping.childLo = newChildLo;\
				parent_node->mapping.childHi = newChildLo+range;\
				for (level_index = 0; level_index < (int)range ; level_index++)\
				{\
					/* copy old node content to the new one */\
					tm_memcpy(&(ctl->tm_##node_type##_array[level_index+newChildLo]), &(ctl->tm_##node_type##_array[level_index+oldChildLo]), sizeof(struct tm_##node_type));\
					/* Beware, the lvlDropProfPtr is duplicated. */\
					drop_profile_ptr_array[level_index+newChildLo] = drop_profile_ptr_array[level_index+oldChildLo];\
					/* switch chilren to new node*/\
					for (child_index =ctl->tm_##node_type##_array[level_index+ newChildLo].mapping.childLo ;  child_index  <= ctl->tm_##node_type##_array[level_index+ newChildLo].mapping.childHi ; child_index++)\
					{\
						ctl->tm_##child_node_type##_array[child_index].mapping.nodeParent=level_index+newChildLo;\
					}\
					/* reset old node related parameters*/ \
					drop_profile_ptr_array[level_index+oldChildLo] = TM_NO_DROP_PROFILE;\
				}\
				/* update H/W */\
				/* 1. disable all necessary nodes */\
				/* all new nodes are disabled */\
				/* to be sure (currently new nodes have valid default elig function) */\
				for (level_index= 0;  level_index <= (int)range ;  level_index++)\
				{\
					set_hw_node_disable(hndl,LEVEL,level_index+newChildLo);\
				}\
				/* disable old nodes */\
				for (level_index= 0;  level_index < (int)range ;  level_index++)\
				{\
					set_hw_node_disable(hndl,LEVEL,level_index+oldChildLo);\
				}\
				/* 2.  update nodes content*/\
				/* optionally : disable parent_node */\
				set_hw_node_disable(hndl,PARENT_LEVEL(LEVEL),parent_index);\
				/* 2.0   update parent node  children range*/\
				set_hw_node_mapping(hndl, PARENT_LEVEL(LEVEL), parent_index);\
				/* 2.1    update new nodes */\
				for (level_index = 0; level_index < (int)range ; level_index++)\
				{\
					/* 2.1.0 -  update grandchildren parents for children attached to nodes*/\
					for (child_index =ctl->tm_##node_type##_array[level_index+ newChildLo].mapping.childLo ;  child_index <= ctl->tm_##node_type##_array[level_index+ newChildLo].mapping.childHi ; child_index++)\
					{\
						/* disable child nodee ? */\
						/*set_hw_node_disable(hndl,CHILD_LEVEL(LEVEL),child_index);*/\
						if (set_hw_node_mapping(hndl, CHILD_LEVEL(LEVEL), child_index)) \
						{\
							rc = ERROR;\
							/* rollback TBD */\
							goto out;\
						}\
						/* enable child node ?*/\
						/*set_hw_node_elig_prio_function(hndl, CHILD_LEVEL(LEVEL), child_index)) */\
						/* now  child node in H/W is mapped to new node */\
					}\
					/* 2.1.1 -  update node parameters & enable it */\
					/*  (Optionally : it is still attached to  disabled parent_node) */\
					if (set_hw_node_mapping(hndl, LEVEL, level_index+newChildLo)\
					 || set_hw_##node_type##_params(hndl,level_index+newChildLo)\
					 || set_hw_node_shaping(hndl, LEVEL, level_index+newChildLo)\
					 || set_hw_node_elig_prio_function(hndl, LEVEL, level_index+newChildLo))\
					{\
						rc = ERROR;\
						/* rollback TBD */\
						goto out;\
					}\
					/* old node is already disabled in H/W so all node params we don't need to update */\
					/* ony thing we need is to reset in H/W drop profile ptr*/\
					if (set_hw_##node_type##_drop_profile_ptr(hndl,level_index+oldChildLo))\
					{\
						rc = ERROR;\
						/* rollback TBD */\
						goto out;\
					}\
				}\
				/* Optionally ; enable parent node*/\
				set_hw_node_elig_prio_function(hndl, PARENT_LEVEL(LEVEL), parent_index);\
				/* post update S/W */\
				/* 1. release old nodes pool */\
				rm_release_chunk(ctl->rm,LEVEL,range,oldChildLo);\
				/* mark old nodes as free */\
				/* & make changes list or update virtual ID */\
				for (level_index = 0; level_index < (int)range ; level_index++)\
				{\
					set_sw_##node_type##_default(&(ctl->tm_##node_type##_array[level_index+oldChildLo]),1);\
					UPDATE_INDEXES_IMPLEMENTATION(LEVEL,parent_index)\
				}\
			}\
			found = 1;\
			i=parent_node->mapping.childHi;\
	}

/***********************************************************************************************************************/


int tm_create_a_node_to_b_node_ex(tm_handle hndl, uint32_t parent_b_node_index,
                               struct tm_a_node_params *a_params, uint32_t first_queue_in_range,
                               uint32_t *a_node_index_ptr)
{
    struct tm_port 	*parent_port = NULL;
    struct tm_c_node 	*parent_c_node = NULL;
	struct tm_b_node 	*parent_b_node = NULL;
    struct tm_a_node 	*node = NULL;
    uint32_t range;
    int rc = 0;
    int i;
#if 0
    int reshuffling = 0;
#endif
	tm_node_status_t old_node_status;
	int found= 0;

	UTILIZE_NEGHBORHOOD_DECLARATIONS(a_node,b_node)

    TM_CTL(ctl, hndl)

	if (a_params==NULL) return -EINVAL;
	if (a_node_index_ptr==NULL) return -EINVAL;

	CONVERT_TO_PHYSICAL(B_LEVEL, parent_b_node_index)

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc)  return rc;
	/*******************************************************************************/
	/** Check parameters validity                                                  */
	/*******************************************************************************/
    VALIDATE_NODE_ALLOCATED_EX(b_node, parent_b_node_index, TM_CONF_B_NODE_IND_OOR, TM_CONF_B_NODE_IND_NOT_EXIST)
	if (rc) goto out;

    parent_b_node = &(ctl->tm_b_node_array[parent_b_node_index]);
    parent_c_node = &(ctl->tm_c_node_array[parent_b_node->mapping.nodeParent]);
	parent_port = &(ctl->tm_port_array[parent_c_node->mapping.nodeParent]);
    /* check A-node params */
	rc= check_shaping_profile_validity(hndl,a_params->shaping_profile_ref, A_LEVEL);
	if (rc) goto out;

	VALIDATE_NODE_QUANTUM_VALUE(a_params->quantum, TM_CONF_A_QUANTUM_OOR)
	if (rc) goto out;

	/* DWRR for Queues in A-node's range */
    for (i=0; i<8; i++)
	{
        if ((a_params->dwrr_priority[i] != TM_DISABLE) &&
            (a_params->dwrr_priority[i] != TM_ENABLE)) {
            rc = TM_CONF_A_DWRR_PRIO_OOR;
            goto out;
        }
    }

	VALIDATE_ELIG_FUNCTION(a_params->elig_prio_func_ptr)

	/* check if the referenced a-node drop profile exists */
  	TEST_RESOURCE_EXISTS(rm_a_node_drop_profile_status(ctl->rm, a_params->wred_profile_ref),/* out_of range error*/TM_CONF_A_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_A_WRED_PROF_REF_OOR)
 	if (rc) goto out;

 	/* in case of asymmetric port :
 	 * 1. the children range  is already defined here
 	 * 2. some H/W changes can be made during search free node (reshuffling , etc), which  should be rolled back if
 	 * no enought free pool.
 	 * testing for enought range can prevent this
 	 * For symmetric node range is defined when free node is known, but in this case no reshuffling -> no rollback
 	 */
	if ((parent_port->sym_mode == TM_DISABLE) /* asymetric port */ && (rm_get_max_chunk_size(ctl->rm, Q_LEVEL) < a_params->num_of_children))
	{
		rc = TM_CONF_INVALID_NUM_OF_QUEUES;
		goto out;
	}

	/**************************************************************************/
	/** Find free a-node                                                      */
	/**************************************************************************/
  	/* looking for reserved or previously deleted nodes in the range */
  	for (i= parent_b_node->mapping.childLo ;  i <= parent_b_node->mapping.childHi ; i++)
	{
		if (ctl->tm_a_node_array[i].mapping.nodeStatus > TM_NODE_USED)
		{
			found=1;
			break;
		}
	}
	/* if (found)  i= number of node founded  */
	if ((!found) && (parent_port->sym_mode == TM_ENABLE))	/* for symmetric pors it's unrecoverable case */
	{
	}
	else /* for asymmetric ports let try additional capabilities */
	{
		if (!found)
		{
			/*try to utilize free/reserved node from right/left side */
			UTILIZE_NEGHBORHOOD(A_LEVEL, a_node, b_node, parent_b_node)
		}
/*  SN only  - #ifdef VIRTUAL_LAYER */
                /*try to reshuffling here */
                if (!found)
				{
					/* for SN - delayed reshuffling
					rc = TM_CONF_NEED_REALLOC;
                    goto out;
					*/
                    /* for future use */
                    MIN_IMPACT_RESHUFFLING(parent_b_node, parent_b_node_index, A_LEVEL, a_node, queue, ctl->tm_a_lvl_drop_prof_ptr, TM_HW_A_NODE_CONFIG_FAIL);
#if 0
                    reshuffling = 1;
#endif
                }
/* #endif */
	}
	if ( !found)  /* not found anyway */
	{
		rc = -ENOBUFS;
		goto out;
	}
	/**************************************************************************/
	/** free node found , try to allocate children range and set node as used */
	/**************************************************************************/
	/* calculate range */
	if (parent_port->sym_mode == TM_ENABLE) /* symetric mode under the port */
	{
		/* predefined range is not compatible with symmetric port tree creation */
		if (first_queue_in_range != (uint32_t)(NO_PREDEFINED_RANGE))
		{
			rc = TM_CONF_INVALID_NUM_OF_QUEUES;
			goto out;
		}
		if ( i == parent_b_node->mapping.childHi) 	range = parent_port->children_range.last_range[Q_LEVEL];/* last child in range */
		else 										range = parent_port->children_range.norm_range[Q_LEVEL];
	}
	else range = a_params->num_of_children; /* asymmetric mode under the port */
	/* allocate chidrens & nset node as used*/
	if (allocate_reserved_a_node(ctl,(uint32_t)i, parent_b_node_index, range, first_queue_in_range, &old_node_status)) 
	{
        rc = TM_CONF_INVALID_NUM_OF_QUEUES;
        goto out;
    }
	/**************************************************************************/
	/** node allocated ,let update node params                                */
	/**************************************************************************/
    *a_node_index_ptr = i;
    node = &(ctl->tm_a_node_array[i]);
    /* Update A-Node SW DB */
	rc=add_node_to_shaping_profile(ctl,a_params->shaping_profile_ref, A_LEVEL, *a_node_index_ptr);
	if (rc < 0) goto out;

    /* Update Drop profile pointer */
    ctl->tm_a_lvl_drop_prof_ptr[*a_node_index_ptr] = a_params->wred_profile_ref;
    node->wred_profile_ref = a_params->wred_profile_ref;
    ctl->tm_a_lvl_drop_profiles[a_params->wred_profile_ref].use_counter++;
    node->elig_prio_func = a_params->elig_prio_func_ptr;

    /* DWRR for A-node - update even if disabled */
    node->dwrr_quantum = a_params->quantum;

    /* DWRR for Queues in A-node's range */
    node->dwrr_enable_per_priority_mask = 0;
    for (i=0; i<8; i++)
        node->dwrr_enable_per_priority_mask =
            node->dwrr_enable_per_priority_mask | (a_params->dwrr_priority[i] << i);

	/**************************************************************************/
	/** node parameters are set , let update H/W                              */
	/**************************************************************************/
#ifdef RESHUFFLING_SHIELD
	{
		int iLo;
		int iHi;
		iLo = parent_b_node->mapping.childLo ;
		iHi = parent_b_node->mapping.childHi ;
		/* reduce childen range to old one */
		if (parent_update > 0 ) iHi--;
		if (parent_update < 0 ) iLo--;
		set_hw_node_disable(hndl, B_LEVEL,parent_b_node_index);
		for (i=iLo ;  i <=iHi ; i++)
		{
			set_hw_node_disable(hndl, A_LEVEL,i);
		}
	}
#endif
	/* here  parent value in registers of allocated children is inconsistent with children range of new node
	 * It will be updated during creation child nodes ,  but in order to be consistent we can update mapping here
	 * Tradeof :  writing  range* 2 registers
	*/
	UPDATE_CHILDREN_MAPPING(A_LEVEL, TM_HW_A_NODE_CONFIG_FAIL);

	/*update parents  mapping - if were changed*/
	/* this action is possible also under traffic - it atomically removes unused child */
	UPDATE_PARENTS_MAPPING(A_LEVEL, parent_b_node_index, TM_HW_A_NODE_CONFIG_FAIL);
    /* Download A-Node to HW */
    if (set_hw_node_mapping(hndl, A_LEVEL, *a_node_index_ptr)
	 || set_hw_a_node_params(hndl, *a_node_index_ptr)
	 || set_hw_node_elig_prio_function(hndl, A_LEVEL, *a_node_index_ptr))
	{
        rc = TM_HW_A_NODE_CONFIG_FAIL;
        goto out;
    }
	/* Download A-node shaping parameters (from profile) to HW */
    if (set_hw_node_shaping(hndl, A_LEVEL, *a_node_index_ptr))
    {
		rc = TM_HW_SHAPING_PROF_FAILED;
		goto out;
    }
	/*******************************************************************************/
	/** here  node successfully created.                                           */
	/*******************************************************************************/
#ifdef RESHUFFLING_SHIELD
	{
		set_hw_node_elig_prio_function(hndl,B_LEVEL,parent_b_node_index);
		for (i=parent_b_node->mapping.childLo ;  i <=parent_b_node->mapping.childHi ; i++)
		{
			set_hw_node_elig_prio_function(hndl, A_LEVEL,i);
		}
	}
#endif
#if 0
        if (reshuffling) {
            int a;
            for (a = parent_b_node->mapping.childLo;
                 a < parent_b_node->mapping.childHi; /* Skip added node! */
                 a++) {
                int q;
                for (q = ctl->tm_a_node_array[a].mapping.childLo;
                     q <= ctl->tm_a_node_array[a].mapping.childHi;
                     q++) {
                    printf("Q%d\n", q);
                    rc = set_hw_node_elig_prio_function(hndl, Q_LEVEL, q);
                    assert(rc == 0);
                }
                rc = set_hw_node_elig_prio_function(hndl, A_LEVEL, a);
                assert(rc == 0);
            }
        }
#endif
	/* create virtual ID & add entry to dictionary */
	GENERATE_VIRTUAL_ID (A_LEVEL, *a_node_index_ptr);

out:
    if (rc)
	{
        if (rc == TM_HW_A_NODE_CONFIG_FAIL)
		{
			remove_node_from_shaping_profile(hndl, a_params->shaping_profile_ref, A_LEVEL, *a_node_index_ptr);
            ctl->tm_a_lvl_drop_prof_ptr[*a_node_index_ptr] = 0;
            ctl->tm_a_lvl_drop_profiles[a_params->wred_profile_ref].use_counter--;
            set_sw_a_node_default(node ,0); /* reset a_node parameters without mapping */
            free_allocated_a_node(ctl,*a_node_index_ptr,old_node_status); /* restore node mapping & release children */
			RESTORE_PARENTS_MAPPING(parent_b_node)	            /* restore parents  mapping - if any */
        }
    }
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


int tm_create_a_node_to_b_node(tm_handle hndl, uint32_t parent_b_node_index,
                               struct tm_a_node_params *a_params,
                               uint32_t *a_node_index_ptr)
{
	return tm_create_a_node_to_b_node_ex(hndl,parent_b_node_index, a_params, NO_PREDEFINED_RANGE ,a_node_index_ptr);
}
int tm_create_a_node_with_queue_range_to_b_node(tm_handle hndl, uint32_t parent_b_node_index,
												struct tm_a_node_params *a_params,
												uint32_t first_queue_in_range, uint32_t queue_range,
												uint32_t *a_node_index_ptr)
{
	a_params->num_of_children = queue_range;
	CONVERT_TO_PHYSICAL(Q_LEVEL,first_queue_in_range)
	return tm_create_a_node_to_b_node_ex(hndl,parent_b_node_index, a_params, first_queue_in_range ,a_node_index_ptr);
}

/***************************************************************************
 * B-node Creation
 ***************************************************************************/

/**
 */
int tm_create_b_node_to_port(tm_handle hndl, uint8_t port_index,
                             struct tm_b_node_params *b_params,
                             struct tm_c_node_params *c_params,
                             uint32_t *b_node_index_ptr,
                             uint32_t *c_node_index_ptr)
{
    int rc = 0;

    rc = tm_create_c_node_to_port(hndl, port_index, c_params, c_node_index_ptr);
    if (rc) return rc;

    rc = tm_create_b_node_to_c_node(hndl, *c_node_index_ptr, b_params, b_node_index_ptr);
    if (rc)
	{
        delete_c_node_proc(hndl, *c_node_index_ptr);
    }
    return rc;
}


/**
 */
int tm_create_b_node_to_c_node_ex(tm_handle hndl, uint32_t parent_c_node_index,
                               struct tm_b_node_params *b_params, uint32_t first_child_in_range,
                               uint32_t *b_node_index_ptr)
{
    struct tm_port *parent_port = NULL;
    struct tm_c_node *parent_c_node = NULL;
    struct tm_b_node *node = NULL;
    uint32_t range;
    int rc = 0;
    int i;
	tm_node_status_t old_node_status;
	int found= 0;

 	UTILIZE_NEGHBORHOOD_DECLARATIONS(b_node, c_node)

    TM_CTL(ctl, hndl)

	if (b_params==NULL) return -EINVAL;
	if (b_node_index_ptr==NULL) return -EINVAL;

	CONVERT_TO_PHYSICAL(C_LEVEL,parent_c_node_index)

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;
	/*******************************************************************************/
	/** Check parameters validity                                                  */
	/*******************************************************************************/
    VALIDATE_NODE_ALLOCATED_EX(c_node, parent_c_node_index, TM_CONF_C_NODE_IND_OOR, TM_CONF_C_NODE_IND_NOT_EXIST)
	if (rc) goto out;

    parent_c_node = &(ctl->tm_c_node_array[parent_c_node_index]);
	parent_port = &(ctl->tm_port_array[parent_c_node->mapping.nodeParent]);
    /* check B-node params */
	rc= check_shaping_profile_validity(hndl,b_params->shaping_profile_ref, B_LEVEL);
	if (rc) goto out;

	VALIDATE_NODE_QUANTUM_VALUE(b_params->quantum, TM_CONF_B_QUANTUM_OOR)
	if (rc) goto out;

	/* DWRR for A-nodes in B-node's range */
    for (i=0; i<8; i++)
	{
        if ((b_params->dwrr_priority[i] != TM_DISABLE) &&
            (b_params->dwrr_priority[i] != TM_ENABLE))
		{
            rc = TM_CONF_B_DWRR_PRIO_OOR;
            goto out;
        }
    }
    /* check drop profile validity */
    if (b_params->wred_profile_ref >= TM_NUM_B_NODE_DROP_PROF)
	{
        rc = TM_CONF_B_WRED_PROF_REF_OOR;
        goto out;
    }
    /* check eligible function validity */
	VALIDATE_ELIG_FUNCTION(b_params->elig_prio_func_ptr)

	/* check if the referenced b-node drop profile exists */
 	TEST_RESOURCE_EXISTS(rm_b_node_drop_profile_status(ctl->rm, b_params->wred_profile_ref),/* out_of range error*/TM_CONF_B_WRED_PROF_REF_OOR,/*profile not used error*/TM_CONF_B_WRED_PROF_REF_OOR)
 	if (rc) goto out;

 	/* in case of asymmetric port :
 	 * 1. the children range  is already defined here
 	 * 2. some H/W changes can be made during search free node (reshuffling , etc), which  should be rolled back if
 	 * no enought free pool.
 	 * testing for enought range can prevent this
 	 * For symmetric node range is defined when free node is known, but in this case no reshuffling -> no rollback
 	 */
	if ((parent_port->sym_mode == TM_DISABLE) /* asymetric port */ && (rm_get_max_chunk_size(ctl->rm, A_LEVEL) < b_params->num_of_children))
	{
		rc = TM_CONF_INVALID_NUM_OF_A_NODES;
		goto out;
	}

	/*******************************************************************************/
	/** Find free B node                                                           */
	/*******************************************************************************/
    /* looking for reserved or previously deleted nodes in the range */
  	for (i= parent_c_node->mapping.childLo ;  i <= parent_c_node->mapping.childHi ; i++)
	{
		if (ctl->tm_b_node_array[i].mapping.nodeStatus > TM_NODE_USED)
		{
			found=1;
			break;
		}
	}
	/* if (found)  i= number of node founded  */
	if ((!found) && (parent_port->sym_mode == TM_ENABLE))	/* for symmetric pors it's unrecoverable case */
	{
	}
	else /* for asymmetric ports let try additional capabilities */
	{
		if (!found)
		{
			/*try to utilize free/reserved node from right/left side */
			UTILIZE_NEGHBORHOOD(B_LEVEL, b_node, c_node, parent_c_node)
		}
/* SN only - #ifdef VIRTUAL_LAYER */
                /*try to reshuffling here */
                if (!found)
				{
                    /*for SN - delayed reshuffling
					rc = TM_CONF_NEED_REALLOC;
                    goto out;
					*/
                    /* for future use */
                    MIN_IMPACT_RESHUFFLING(parent_c_node, parent_c_node_index, B_LEVEL, b_node, a_node, ctl->tm_b_lvl_drop_prof_ptr, TM_HW_B_NODE_CONFIG_FAIL);
                }
/* #endif */
	}
	if ( !found)  /* not found anyway*/
	{
		rc = -ENOBUFS;
		goto out;
	}
	/**************************************************************************/
	/** free node found , try to allocate children range and set node as used */
	/**************************************************************************/
	/* calculate range */
	if (parent_port->sym_mode == TM_ENABLE) /* symetric mode under the port */
	{
		if ( i == parent_c_node->mapping.childHi) 	range = parent_port->children_range.last_range[A_LEVEL];/* last child in range */
		else 										range = parent_port->children_range.norm_range[A_LEVEL];
	}
	else range = b_params->num_of_children; /* asymmetric mode under the port */
	/* allocate chidrens & set node as used*/
	if (allocate_reserved_b_node(ctl, (uint32_t)i, parent_c_node_index, range, first_child_in_range, &old_node_status)) 
	{
        rc = TM_CONF_INVALID_NUM_OF_A_NODES;
        goto out;
    }
	/**************************************************************************/
	/** node allocated ,let update node params                                */
	/**************************************************************************/
	*b_node_index_ptr = i;
	node = &(ctl->tm_b_node_array[i]);
    /* Update B-Node SW DB */
 	rc=add_node_to_shaping_profile(ctl,b_params->shaping_profile_ref, B_LEVEL, *b_node_index_ptr);
	if (rc < 0) goto out;

    node->elig_prio_func = b_params->elig_prio_func_ptr;

    /* Update Drop profile pointer */
    ctl->tm_b_lvl_drop_prof_ptr[*b_node_index_ptr] = b_params->wred_profile_ref;
    node->wred_profile_ref = b_params->wred_profile_ref;
    ctl->tm_b_lvl_drop_profiles[b_params->wred_profile_ref].use_counter++;

    /* DWRR for B-node - update even if disabled */
    node->dwrr_quantum = b_params->quantum;

    /* DWRR for A-nodes in B-node's range */
    node->dwrr_enable_per_priority_mask = 0;
    for (i=0; i<8; i++)
        node->dwrr_enable_per_priority_mask =
            node->dwrr_enable_per_priority_mask | (b_params->dwrr_priority[i] << i);
	/**************************************************************************/
	/** node parameters are set , let update H/W                              */
	/**************************************************************************/
	/* here  parent value in registers of allocated children is inconsistent with children range of new node
	 * It will be updated during creation child nodes ,  but in order to be consistent we can update mapping here
	 * Tradeof :  writing  range* 2 registers
	*/
    UPDATE_CHILDREN_MAPPING(B_LEVEL, TM_HW_B_NODE_CONFIG_FAIL);

	/*update parent neighborhood mapping - if were changed*/
	/* this action is possible also under traffic - it atomically removes unused child */
    UPDATE_PARENTS_MAPPING(B_LEVEL, parent_c_node_index, TM_HW_B_NODE_CONFIG_FAIL);
    /* Download B-Node to HW */
    if (set_hw_node_mapping(hndl, B_LEVEL, *b_node_index_ptr)
	 || set_hw_b_node_params(hndl, *b_node_index_ptr)
	 || set_hw_node_elig_prio_function(hndl, B_LEVEL, *b_node_index_ptr))
	{
        rc = TM_HW_B_NODE_CONFIG_FAIL;
        goto out;
    }
    /* Download B-node shaping parameters (from profile) to HW */
    if (set_hw_node_shaping(hndl, B_LEVEL, *b_node_index_ptr))
    {
		rc = TM_HW_SHAPING_PROF_FAILED;
		goto out;
	}
	/*******************************************************************************/
	/** here  node successfully created.                                           */
	/*******************************************************************************/
	/* create virtual ID & add entry to dictionary */
	GENERATE_VIRTUAL_ID (B_LEVEL, *b_node_index_ptr)

out:
    if (rc)
	{
        if (rc == TM_HW_B_NODE_CONFIG_FAIL)
		{
			remove_node_from_shaping_profile(hndl, b_params->shaping_profile_ref, B_LEVEL, *b_node_index_ptr);
            ctl->tm_b_lvl_drop_prof_ptr[*b_node_index_ptr] = 0;
            ctl->tm_b_lvl_drop_profiles[b_params->wred_profile_ref].use_counter--;
            /* reset node parameters without mapping */
            set_sw_b_node_default(node,0);
            /* restore node mapping & release children */
            free_allocated_b_node(ctl,*b_node_index_ptr,old_node_status);
 			RESTORE_PARENTS_MAPPING(parent_c_node)	            /* restore parent level  mapping - if any */
      }
    }
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

int tm_create_b_node_to_c_node(tm_handle hndl, uint32_t parent_c_node_index,
                               struct tm_b_node_params *b_params,
                               uint32_t *b_node_index_ptr)
{
	return	tm_create_b_node_to_c_node_ex(hndl, parent_c_node_index, b_params, NO_PREDEFINED_RANGE,b_node_index_ptr);
}

#ifdef  SOLVE_PHYSICAL_LOGICAL_PROBLEM
	int tm_create_b_node_to_c_node_with_a_node_range(tm_handle hndl, uint32_t parent_c_node_index,
													struct tm_b_node_params *b_params,
													uint32_t first_a_node_in_range, uint32_t a_nodes_range,
													uint32_t *b_node_index_ptr)
	{
		b_params->num_of_children = a_nodes_range;
		CONVERT_TO_PHYSICAL(A_LEVEL,first_a_node_in_range)
		return tm_create_b_node_to_c_node_ex(hndl, parent_c_node_index, b_params, first_a_node_in_range, b_node_index_ptr);
	}
#endif
/***************************************************************************
 * C-node Creation
 ***************************************************************************/

/**
 */
int tm_create_c_node_to_port_ex(tm_handle hndl, uint8_t parent_port_index,
                             struct tm_c_node_params *c_params, uint32_t first_child_in_range,
                             uint32_t *c_node_index_ptr)
{
    struct tm_port 	*parent_port = NULL;
    struct tm_c_node 	*node = NULL;
    uint16_t range;
    int rc = 0;
    int i;
	tm_node_status_t old_node_status;

    TM_CTL(ctl, hndl)

	if (c_params==NULL) return -EINVAL;
	if (c_node_index_ptr==NULL) return -EINVAL;

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;
	/*******************************************************************************/
	/** Check parameters validity                                                  */
	/*******************************************************************************/
    /* Check port validity */
    VALIDATE_NODE_ALLOCATED_EX(port, parent_port_index, TM_CONF_PORT_IND_OOR, TM_CONF_PORT_IND_NOT_EXIST)
	if (rc) goto out;

    parent_port = &(ctl->tm_port_array[parent_port_index]);

    /* C-node params */
	rc= check_shaping_profile_validity(hndl,c_params->shaping_profile_ref, C_LEVEL);
	if (rc) goto out;
	/* check validity of the Quantum value*/
	VALIDATE_NODE_QUANTUM_VALUE(c_params->quantum, TM_CONF_C_QUANTUM_OOR)
	if (rc) goto out;

	/* check validity of eligible function number */
    VALIDATE_ELIG_FUNCTION(c_params->elig_prio_func_ptr)

	/* check DWRR for B-nodes in C-node's range */
    for (i=0; i<8; i++)
	{
        if ((c_params->dwrr_priority[i] != TM_DISABLE) &&
            (c_params->dwrr_priority[i] != TM_ENABLE))
		{
            rc = TM_CONF_C_DWRR_PRIO_OOR;
            goto out;
        }
    }
#if 0 /* TBD - do we need it? */
    if (c_params->wred_cos == 0) {  /* at least one drop profile
                                     * should be applied */
        rc = TM_CONF_C_WRED_PROF_REF_OOR;
        goto out;
    }
#endif
    for (i=0; i<TM_WRED_COS; i++)
	{
        if (c_params->wred_cos & (1<<i))
		{
			 /* check if the referenced c-node drop profile exists */
			TEST_RESOURCE_EXISTS(rm_c_node_drop_profile_status_cos(ctl->rm, (uint8_t)i, c_params->wred_profile_ref[i]),/* out_of range error*/TM_CONF_C_WRED_PROF_REF_OOR, /*profile not used error*/TM_CONF_C_WRED_PROF_REF_OOR)
			if (rc) goto out;
        }
    }

    /* in case of creation  of C-nodes resuffling is not implemented , so it is not necessary to preliminary validate children range
     * But if "reshuffling" will be expanded to this level, it's recommended to  validate range here , as done for A & B levels
     */

	/*******************************************************************************/
	/** Find free C-node                                                           */
	/*******************************************************************************/
    /* looking for reserved or previously deleted nodes in the range */
	for (i= parent_port->mapping.childLo ;  i <= parent_port->mapping.childHi ; i++)
	{
		if (ctl->tm_c_node_array[i].mapping.nodeStatus > TM_NODE_USED) break;
	}
	if (i > parent_port->mapping.childHi )   /* all nodes are already used */
	{
		rc = -ENOBUFS;
		goto out;
	}
	/**************************************************************************/
	/** free node found , try to allocate children range and set node as used */
	/**************************************************************************/
	/* calculate range */
	if (parent_port->sym_mode == TM_ENABLE) /* symetric mode under the port */
	{
		if ( i == parent_port->mapping.childHi) 	range = parent_port->children_range.last_range[B_LEVEL];/* last child in range */
		else 										range = parent_port->children_range.norm_range[B_LEVEL];
	}
	else /* asymmetric mode under the port */
	{
		range = c_params->num_of_children;
	}
	/* allocate chidrens & set node as used*/
	if (allocate_reserved_c_node(ctl, (uint32_t)i, parent_port_index, range, first_child_in_range, &old_node_status))
	{
        rc = TM_CONF_INVALID_NUM_OF_B_NODES;
        goto out;
    }
	/**************************************************************************/
	/** node allocated ,let update node params                                */
	/**************************************************************************/
	*c_node_index_ptr = i;
    node = &(ctl->tm_c_node_array[i]);
    /* Update C-node SW DB */
	rc=add_node_to_shaping_profile(ctl,c_params->shaping_profile_ref, C_LEVEL, *c_node_index_ptr);
	if (rc < 0) goto out;

    node->elig_prio_func = c_params->elig_prio_func_ptr;

    for (i=0; i<TM_WRED_COS; i++)
	{
        /* Update Drop profile pointer */
        if (c_params->wred_cos & (1<<i))
		{
            ctl->tm_c_lvl_drop_prof_ptr[i][*c_node_index_ptr] =c_params->wred_profile_ref[i];
            ctl->tm_c_lvl_drop_profiles[i][c_params->wred_profile_ref[i]].use_counter++;
            node->wred_profile_ref[i] = c_params->wred_profile_ref[i];
        }
		else
		{
            ctl->tm_c_lvl_drop_prof_ptr[i][*c_node_index_ptr] = 0;
            ctl->tm_c_lvl_drop_profiles[i][0].use_counter++;
            node->wred_profile_ref[i] = 0;
        }
    }
    /* update Wred CoS for future usage within the HW update */
    node->wred_cos = c_params->wred_cos;
    /* DWRR for C-node - update even if disabled */
    node->dwrr_quantum = c_params->quantum;
    /* DWRR for B-nodes in C-node's range */
    node->dwrr_enable_per_priority_mask = 0;
    for (i=0; i<8; i++)
        node->dwrr_enable_per_priority_mask =
            node->dwrr_enable_per_priority_mask | (c_params->dwrr_priority[i] << i);

 	/**************************************************************************/
	/** node parameters are set , let update H/W                              */
	/**************************************************************************/
	/* here  parent value in registers of allocated children is inconsistent with children range of new node
	 * It will be updated during creation child nodes ,  but in order to be consistent we can update mapping here
	 * Tradeof :  writing  range* 2 registers
	*/
    UPDATE_CHILDREN_MAPPING(C_LEVEL, TM_HW_C_NODE_CONFIG_FAIL);

    /* Download C-node to HW */
	/* update node H/W */
    if (set_hw_node_mapping(hndl, C_LEVEL, *c_node_index_ptr)
	 || set_hw_c_node_params(hndl, *c_node_index_ptr)
	 || set_hw_node_elig_prio_function(hndl, C_LEVEL, *c_node_index_ptr))
	{
        rc = TM_HW_C_NODE_CONFIG_FAIL;
        goto out;
    }
    /* Download C-node shaping parameters (from profile) to HW */
    if (set_hw_node_shaping(hndl, C_LEVEL, *c_node_index_ptr))
    {
		rc = TM_HW_SHAPING_PROF_FAILED;
		goto out;
	}
	/*******************************************************************************/
	/** here  node successfully created.                                           */
	/*******************************************************************************/
	/* create virtual ID & add entry to dictionary */
	/*** currently not supported ********************
	 GENERATE_VIRTUAL_ID (C_LEVEL, *c_node_index);
	 ************************************************/

out:
    if (rc)
	{
        if (rc == TM_HW_C_NODE_CONFIG_FAIL)
		{
			remove_node_from_shaping_profile(hndl, c_params->shaping_profile_ref, C_LEVEL, *c_node_index_ptr);
            for (i=0; i<TM_WRED_COS; i++)
            {
                if (c_params->wred_cos & (1<<i))
                {
                    ctl->tm_c_lvl_drop_prof_ptr[i][*c_node_index_ptr] = 0;
                    ctl->tm_c_lvl_drop_profiles[i][c_params->wred_profile_ref[i]].use_counter--;
                }
            }
            set_sw_c_node_default(node,0);/* reset a_node parameters without mapping */
            free_allocated_c_node(ctl, *c_node_index_ptr,old_node_status); /* restore node mapping & release children */
       }
    }
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

int tm_create_c_node_to_port(tm_handle hndl, uint8_t parent_port_index,
                             struct tm_c_node_params *c_params,
                             uint32_t *c_node_index_ptr)
{
	return tm_create_c_node_to_port_ex(hndl, parent_port_index, c_params, NO_PREDEFINED_RANGE,c_node_index_ptr);
}

#ifdef  SOLVE_PHYSICAL_LOGICAL_PROBLEM
	int tm_create_c_node_to_port_with_b_node_range(	tm_handle hndl,
													uint8_t parent_port_index,
													struct tm_c_node_params *c_params,
													uint32_t first_b_node_in_range, uint32_t b_nodes_range,
													uint32_t *c_node_index_ptr)
	{
		c_params->num_of_children = b_nodes_range;
		CONVERT_TO_PHYSICAL(B_LEVEL,first_b_node_in_range)
		return tm_create_c_node_to_port_ex(hndl, parent_port_index, c_params, first_b_node_in_range ,c_node_index_ptr);
	}
#endif
