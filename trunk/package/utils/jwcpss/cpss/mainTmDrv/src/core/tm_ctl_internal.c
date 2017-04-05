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
 * @brief tm_ctl implementation.
 *
 * @file tm_ctl.c
 *
 * $Revision: 2.0 $
 */

#include "tm_ctl_internal.h"

#include "tm_set_local_db_defaults.h"
#include <errno.h>
#include "tm_os_interface.h"
#include "tm_locking_interface.h"
#include "rm_ctl.h"
#include "tm_connection_management.h"
#include "tm_nodes_ctl.h"
#include "tm_nodes_ctl_internal.h"
#include "tm_hw_configuration_interface.h"
#include "tm_drop.h"
#include "tm_elig_prio_func.h"
#include "tm_errcodes.h"
#include "set_hw_registers.h"
#include "tm_get_gen_param_interface.h"
#include "tm_shaping_internal.h"
#include "tm_logical_layer_core.h"

#include <stdlib.h>
#include <stdio.h>


#define goto while(cpssOsPrintf("out: %s(%d)\n", __FILE__, __LINE__) || 1) goto

int tm_lib_hw_init(tm_handle hndl)
{
	int rc;
    TM_CTL(ctl, hndl);

   /* Get general parameters : LAD frequency,  dwrr params  - they are readed from H/W resources*/
    rc = tm_get_gen_params(ctl);
    if (rc) goto out;

	/* configure default drop registers */
	rc = _tm_config_default_drop_hw((tm_handle)ctl);
    if (rc)  goto out;
        
    rc = tm_config_elig_prio_func_table((tm_handle)ctl,1); /* with H/W update*/
	if (rc)  goto out;

    rc = set_hw_uninstall_default_queues((tm_handle)ctl);
    if (rc) goto out;

    rc = set_hw_disable_ports((tm_handle) ctl, get_tm_port_count(TM_ENV(ctl)));
    if (rc) goto out;
out:
  return rc;
}

static int tm_lib_close_nolock(struct tm_ctl *ctl);

/**
 */
int tm_lib_sw_init(tm_handle hEnv, tm_handle *htm, struct tm_lib_init_params *init_params_PTR)
{
    struct tm_ctl *ctl = NULL;
    uint32_t total_shap_profiles;

    int rc = 0;
	unsigned int i;
	unsigned int j;

	
    if ((*htm) != NULL) return  -EINVAL;

    *htm = NULL;

	if (init_tm_hardware_configuration(hEnv))  return  TM_CONF_INVALID_PROD_NAME;

    /* Allocate handle */
    ctl = tm_malloc(sizeof(*ctl) * 1);
    if (!ctl) return -GT_OUT_OF_CPU_MEM;

	tm_memset(ctl, 0, sizeof(*ctl));
    ctl->magic = TM_MAGIC;
	ctl->hEnv=hEnv;
#ifdef VIRTUAL_LAYER
	ctl->hVirtualLayer=NULL;
#endif
    ctl->hLogicalLayer=NULL;

    ctl->tm_queue_array = NULL;
    ctl->tm_a_node_array = NULL;
    ctl->tm_b_node_array = NULL;
    ctl->tm_c_node_array = NULL;
    ctl->tm_port_array = NULL;
    ctl->tm_q_lvl_drop_prof_ptr = NULL;
    ctl->tm_a_lvl_drop_prof_ptr = NULL;
    ctl->tm_b_lvl_drop_prof_ptr = NULL;
    for (i=0; i<TM_WRED_COS; i++)
    {
        ctl->tm_c_lvl_drop_prof_ptr[i] = NULL;
        ctl->tm_p_lvl_drop_prof_ptr_cos[i] = NULL;
    }
    ctl->tm_p_lvl_drop_prof_ptr = NULL;

    ctl->tm_shaping_profiles = NULL;
    ctl->tm_q_lvl_drop_profiles = NULL;
    ctl->tm_a_lvl_drop_profiles = NULL;
    ctl->tm_b_lvl_drop_profiles = NULL;
    for (i=0; i<TM_WRED_COS; i++)
    {
        ctl->tm_c_lvl_drop_profiles[i] = NULL;
        ctl->tm_p_lvl_drop_profiles_cos[i] = NULL;
    }
    ctl->tm_p_lvl_drop_profiles = NULL;

    ctl->tm_wred_q_lvl_curves = NULL;
    ctl->tm_wred_a_lvl_curves = NULL;
    ctl->tm_wred_b_lvl_curves = NULL;
    for (i=0; i<TM_WRED_COS; i++)
    {
         ctl->tm_wred_c_lvl_curves[i] = NULL;
         ctl->tm_wred_ports_curves_cos[i] = NULL;
    }
    ctl->tm_wred_ports_curves = NULL;
    ctl->tm_port_sms_attr_pbase = NULL;
    ctl->tm_port_sms_attr_qmap_pars =NULL;

	/* tm lib init params */
	if (init_params_PTR)
		tm_memcpy(&ctl->init_params, init_params_PTR, sizeof(struct tm_lib_init_params)); 

	ctl->init_params.tm_mtu = MAX(TM_MTU, ctl->init_params.tm_mtu);
	/* calculate quantum limits */

   /* Get general parameters : LAD frequency,  dwrr params  - they are readed from H/W resources*/
    rc = get_hw_gen_params(ctl);
    if (rc) goto out;
 	
    /* Total number of nodes per level */
    ctl->tm_total_ports = 	get_tm_port_count(hEnv);
    ctl->tm_total_c_nodes = get_tm_c_nodes_count(hEnv);
    ctl->tm_total_b_nodes = get_tm_b_nodes_count(hEnv);
    ctl->tm_total_a_nodes = get_tm_a_nodes_count(hEnv);
    ctl->tm_total_queues = 	get_tm_queues_count(hEnv);
   
    total_shap_profiles = ctl->tm_total_queues + ctl->tm_total_a_nodes + ctl->tm_total_b_nodes + ctl->tm_total_c_nodes;
    
    ctl->rm = rm_open(ctl->tm_total_ports,ctl->tm_total_c_nodes, ctl->tm_total_b_nodes, ctl->tm_total_a_nodes, ctl->tm_total_queues);
    if (!ctl->rm) 
    {
		rc = -EINVAL;
        goto out;
	}

    /* allocate global arrays */
    ctl->tm_port_array = tm_malloc(sizeof(struct tm_port) * ctl->tm_total_ports);
    if (ctl->tm_port_array == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_c_node_array = tm_malloc(sizeof(struct tm_c_node) * ctl->tm_total_c_nodes);
    if (ctl->tm_c_node_array == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_b_node_array = tm_malloc(sizeof(struct tm_b_node) * ctl->tm_total_b_nodes);
    if (ctl->tm_b_node_array == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_a_node_array = tm_malloc(sizeof(struct tm_a_node) * ctl->tm_total_a_nodes);
    if (ctl->tm_a_node_array == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_queue_array = tm_malloc(sizeof(struct tm_queue) * ctl->tm_total_queues);
    if (ctl->tm_queue_array == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_shaping_profiles = tm_malloc(sizeof(struct tm_shaping_profile) * total_shap_profiles);
    if (ctl->tm_shaping_profiles == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_q_lvl_drop_profiles = tm_malloc(sizeof(struct tm_drop_profile) * TM_NUM_QUEUE_DROP_PROF);
    if (ctl->tm_q_lvl_drop_profiles == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_a_lvl_drop_profiles = tm_malloc(sizeof(struct tm_drop_profile) * TM_NUM_A_NODE_DROP_PROF);
    if (ctl->tm_a_lvl_drop_profiles == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_b_lvl_drop_profiles = tm_malloc(sizeof(struct tm_drop_profile) * TM_NUM_B_NODE_DROP_PROF);
    if (ctl->tm_b_lvl_drop_profiles == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    for (i=0; i<TM_WRED_COS; i++)
	{
        ctl->tm_c_lvl_drop_profiles[i] = tm_malloc(sizeof(struct tm_drop_profile) * TM_NUM_C_NODE_DROP_PROF);
        if (ctl->tm_c_lvl_drop_profiles[i] == NULL)
		{
            rc = -GT_OUT_OF_CPU_MEM;
            goto out;
        }
    }    
	/* for ports  drop_profile & drop_profile_cos are per port  so drop profiles array sizes are equial to ports count */
    ctl->tm_p_lvl_drop_profiles = tm_malloc(sizeof(struct tm_drop_profile) * ctl->tm_total_ports);
    if (ctl->tm_p_lvl_drop_profiles == NULL) {
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    for (i=0; i<TM_WRED_COS; i++)
    {
        ctl->tm_p_lvl_drop_profiles_cos[i] = tm_malloc(sizeof(struct tm_drop_profile) * ctl->tm_total_ports);
        if (ctl->tm_p_lvl_drop_profiles_cos[i] == NULL)
        {
            rc = -GT_OUT_OF_CPU_MEM;
            goto out;
        }
    }
    ctl->tm_wred_q_lvl_curves = tm_malloc(sizeof(struct tm_wred_curve) * TM_NUM_WRED_QUEUE_CURVES);
    if (ctl->tm_wred_q_lvl_curves == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_wred_a_lvl_curves = tm_malloc(sizeof(struct tm_wred_curve) * TM_NUM_WRED_A_NODE_CURVES);
    if (ctl->tm_wred_a_lvl_curves == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_wred_b_lvl_curves = tm_malloc(sizeof(struct tm_wred_curve) * TM_NUM_WRED_B_NODE_CURVES);
    if (ctl->tm_wred_b_lvl_curves == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    for (i=0; i<TM_WRED_COS; i++)
	{
        ctl->tm_wred_c_lvl_curves[i] = tm_malloc(sizeof(struct tm_wred_curve) * TM_NUM_WRED_C_NODE_CURVES);
        if (ctl->tm_wred_c_lvl_curves[i] == NULL)
		{
            rc = -GT_OUT_OF_CPU_MEM;
            goto out;
        }
    }
    ctl->tm_wred_ports_curves = tm_malloc(sizeof(struct tm_wred_curve) * TM_NUM_WRED_PORT_CURVES);
    if (ctl->tm_wred_ports_curves == NULL)
    {
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    for (i=0; i<TM_WRED_COS; i++)
    {
        ctl->tm_wred_ports_curves_cos[i] = tm_malloc(sizeof(struct tm_wred_curve) * TM_NUM_WRED_PORT_CURVES);
        if (ctl->tm_wred_ports_curves_cos[i] == NULL)
        {
            rc = -GT_OUT_OF_CPU_MEM;
            goto out;
        }
    }
    ctl->tm_q_lvl_drop_prof_ptr = tm_malloc(sizeof(uint16_t) * ctl->tm_total_queues);
    if (ctl->tm_q_lvl_drop_prof_ptr == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_a_lvl_drop_prof_ptr = tm_malloc(sizeof(uint16_t) * ctl->tm_total_a_nodes);
    if (ctl->tm_a_lvl_drop_prof_ptr == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_b_lvl_drop_prof_ptr = tm_malloc(sizeof(uint16_t) * ctl->tm_total_b_nodes);
    if (ctl->tm_b_lvl_drop_prof_ptr == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    for (i=0; i<TM_WRED_COS; i++)
	{
        ctl->tm_c_lvl_drop_prof_ptr[i] = tm_malloc(sizeof(uint8_t) * ctl->tm_total_c_nodes);
        if (ctl->tm_c_lvl_drop_prof_ptr[i] == NULL)
		{
            rc = -GT_OUT_OF_CPU_MEM;
            goto out;
        }
    }
    ctl->tm_p_lvl_drop_prof_ptr = tm_malloc(sizeof(uint8_t) * ctl->tm_total_ports);
    if (ctl->tm_p_lvl_drop_prof_ptr == NULL)
    {
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    for (i=0; i<TM_WRED_COS; i++)
    {
        ctl->tm_p_lvl_drop_prof_ptr_cos[i] = tm_malloc(sizeof(uint8_t) * ctl->tm_total_ports);
        if (ctl->tm_p_lvl_drop_prof_ptr_cos[i] == NULL)
        {
            rc = -GT_OUT_OF_CPU_MEM;
            goto out;
        }
    }
    ctl->tm_port_sms_attr_pbase = tm_malloc(sizeof(struct port_sms_attr_pbase) * ctl->tm_total_ports);
    if (ctl->tm_port_sms_attr_pbase == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }
    ctl->tm_port_sms_attr_qmap_pars = tm_malloc(sizeof(struct port_sms_attr_qmap_parsing) * ctl->tm_total_ports);
    if (ctl->tm_port_sms_attr_qmap_pars == NULL)
	{
        rc = -GT_OUT_OF_CPU_MEM;
        goto out;
    }

    /* set to default all the arrays fields */
    tm_memset(ctl->tm_port_sms_attr_pbase, 0, sizeof(struct port_sms_attr_pbase) * ctl->tm_total_ports);
    for (i=0; i<ctl->tm_total_ports; i++)   ctl->tm_port_sms_attr_pbase[i].pbase = 0x4;
    
    tm_memset(ctl->tm_port_sms_attr_qmap_pars, 0, sizeof(struct port_sms_attr_qmap_parsing) * ctl->tm_total_ports);

	/* set default values for tree nodes INCLUDING DEFAULT MAPPING INITIALIZATION (second parameter is 1 ) */
    for (i=0 ; i < ctl->tm_total_ports; i++)			
        set_sw_port_default(&(ctl->tm_port_array[i]),1);
    for (i=0 ; i < ctl->tm_total_c_nodes; i++)			
        set_sw_c_node_default(&(ctl->tm_c_node_array[i]),1);
    for (i=0 ; i < ctl->tm_total_b_nodes; i++)			
        set_sw_b_node_default(&(ctl->tm_b_node_array[i]),1);
    for (i=0 ; i < ctl->tm_total_a_nodes; i++)			
        set_sw_a_node_default(&(ctl->tm_a_node_array[i]),1);
    for (i=0 ; i < ctl->tm_total_queues; i++)			
        set_sw_queue_default(&(ctl->tm_queue_array[i]),1);

    for (i=0; i < TM_NUM_QUEUE_DROP_PROF; i++)			
        set_sw_drop_profile_default(ctl->tm_q_lvl_drop_profiles, i);
    for (i=0; i < TM_NUM_A_NODE_DROP_PROF; i++)			
        set_sw_drop_profile_default(ctl->tm_a_lvl_drop_profiles, i);
    for (i=0; i < TM_NUM_B_NODE_DROP_PROF; i++)			
        set_sw_drop_profile_default(ctl->tm_b_lvl_drop_profiles, i);
    for (j=0; j < TM_WRED_COS; j++)
	{
        for (i=0; i < TM_NUM_C_NODE_DROP_PROF; i++)		
            set_sw_drop_profile_default(ctl->tm_c_lvl_drop_profiles[j], i);
    }
    for (i=0; i < ctl->tm_total_ports; i++)				
        set_sw_drop_profile_default(ctl->tm_p_lvl_drop_profiles, i);
    for (j=0; j < TM_WRED_COS; j++)
    {
        for (i=0; i < ctl->tm_total_ports; i++) 		
            set_sw_drop_profile_default(ctl->tm_p_lvl_drop_profiles_cos[j], i);
    }
    
    for (i=0; i < TM_NUM_WRED_QUEUE_CURVES; i++) 		
        set_sw_wred_curve_default(ctl->tm_wred_q_lvl_curves, (uint16_t)i);
    for (i=0; i < TM_NUM_WRED_A_NODE_CURVES; i++)		
        set_sw_wred_curve_default(ctl->tm_wred_a_lvl_curves, (uint16_t)i);
    for (i=0; i < TM_NUM_WRED_B_NODE_CURVES; i++)		
        set_sw_wred_curve_default(ctl->tm_wred_b_lvl_curves, (uint16_t)i);
    for (j=0; j < TM_WRED_COS; j++)
	{
        for (i=0; i < TM_NUM_WRED_C_NODE_CURVES; i++)	
            set_sw_wred_curve_default(ctl->tm_wred_c_lvl_curves[j], (uint16_t)i);
    }
    for (i=0; i < TM_NUM_WRED_PORT_CURVES; i++)			
        set_sw_wred_curve_default(ctl->tm_wred_ports_curves, (uint16_t)i);
    for (j=0; j < TM_WRED_COS; j++)
    {
        for (i=0; i < TM_NUM_WRED_PORT_CURVES; i++) 	
            set_sw_wred_curve_default(ctl->tm_wred_ports_curves_cos[j], (uint16_t)i);
    }
    
    for (i=0; i < ctl->tm_total_queues; i++)        
        ctl->tm_q_lvl_drop_prof_ptr[i] = TM_NO_DROP_PROFILE;
    for (i=0; i < ctl->tm_total_a_nodes; i++)       
        ctl->tm_a_lvl_drop_prof_ptr[i] = TM_NO_DROP_PROFILE;
    for (i=0; i < ctl->tm_total_b_nodes; i++)       
        ctl->tm_b_lvl_drop_prof_ptr[i] = TM_NO_DROP_PROFILE;
    for (i=0; i < TM_WRED_COS; i++)
    {
        for (j=0; j < ctl->tm_total_c_nodes; j++)   
            ctl->tm_c_lvl_drop_prof_ptr[i][j] = TM_NO_DROP_PROFILE;
    }
            
    for (i=0; i < ctl->tm_total_ports; i++)         
        ctl->tm_p_lvl_drop_prof_ptr[i] = TM_NO_DROP_PROFILE;
    for (i=0; i < TM_WRED_COS; i++)
    {
        for (j=0; j < ctl->tm_total_ports; j++)     
            ctl->tm_p_lvl_drop_prof_ptr_cos[i][j] = TM_NO_DROP_PROFILE;
    }

    /* Set TM2TM flags */
    ctl->port_ch.configured = TM_DISABLE;
    ctl->node_ch.configured = TM_DISABLE;
    
    /* Shaping Profile #0 is reserved in the library and represents no
     * shaping. If the user application doesn't want to apply shaping
     * for a node the TM_INF_SHP_PROFILE must be used for
     * shaping profile referece */
	rc=init_infinite_shaping_profile((tm_handle)ctl);
    if (rc) goto out;

#ifdef USE_DEFAULT_CURVES
    /* Drop Profile #0 & WRED Curve #0 are reserved in the library and
     * represent no drop. If the user application doesn't want to
     * apply drop for a node the TM_NO_DROP_PROFILE must be used for
     * drop profile referece */
    /* Create default curves & drop profiles for each level */
#else
   /* Drop Profile #0 is reserved in the library and
     * represent no drop. If the user application doesn't want to
     * apply drop for a node the TM_NO_DROP_PROFILE must be used for
     * drop profile referece */
    /* Create default drop profiles for each level */
#endif    
    
    
    rc = _tm_config_default_drop_sw((tm_handle)ctl);
    if (rc) goto out;

    /* scheduling configuration */
    rc = set_sw_sched_conf_default((tm_handle)ctl);
    if (rc) goto out;

    /* other scheduler registers */
    rc = set_sw_gen_conf_default((tm_handle)ctl);
    if (rc) goto out;

    rc = tm_config_elig_prio_func_table((tm_handle)ctl,0); /* without H/W update*/
    if (rc) goto out;

    ctl->node_quantum_chunk_size_bytes = get_node_quantum_chunk_size();
    ctl->port_quantum_chunk_size_bytes = get_port_quantum_chunk_size();

    /*
    min_port_quant = 4*ctl->port_ch_emit*ctl->dwrr_bytes_burst_limit
    max_pkg_len_bursts = (ctl->mtu + ctl->min_pkg_size)/16
    min_port_quantum_bytes= MAX (min_port_quant, max_pkg_len_bursts)
    min_port_quant_bursts = min_port_quantum_bytes / 0x40
    */

    /* 
    ctl->min_pkg_size has updated at get_hw_gen_params
    ctl->dwrr_bytes_burst_limit has updated at set_sw_sched_conf_default 
    */ 

    /* port quantum limits */ 
    ctl->min_port_quantum_bytes = MAX((ctl->min_pkg_size + ctl->init_params.tm_mtu)/16 ,(uint32_t)(4*ctl->port_ch_emit*ctl->dwrr_bytes_burst_limit));

    /* aligned port quantum limits */ 
    j = (ctl->min_port_quantum_bytes + ctl->port_quantum_chunk_size_bytes - 1) /  ctl->port_quantum_chunk_size_bytes;
    ctl->min_port_quantum_bytes = (j * ctl->port_quantum_chunk_size_bytes);

    ctl->max_port_quantum_bytes = get_port_max_quantum_chunks()*get_port_quantum_chunk_size();

    /* node quantum limits */
    ctl->min_node_quantum_bytes = ctl->init_params.tm_mtu + ctl->min_pkg_size;

    /* aligned node quantum limits */
    j = (ctl->min_node_quantum_bytes + ctl->node_quantum_chunk_size_bytes - 1) /  ctl->node_quantum_chunk_size_bytes;
    ctl->min_node_quantum_bytes = (j * ctl->node_quantum_chunk_size_bytes);

    ctl->max_node_quantum_bytes = ctl->min_node_quantum_bytes * get_node_quantum_range();
    if (ctl->max_node_quantum_bytes > get_node_max_quantum_chunks()*ctl->node_quantum_chunk_size_bytes) 
        ctl->max_node_quantum_bytes = get_node_max_quantum_chunks()*ctl->node_quantum_chunk_size_bytes;


#ifdef VIRTUAL_LAYER
    rc = allocate_virtual_layer(&ctl->hVirtualLayer);
    if (rc < 0) goto out;
#else
    /* Reshuffling changes list */
    ctl->list.next = NULL;
#endif

    /* set the handle */
    *htm = (tm_handle)ctl;

    return rc;

out:
    tm_lib_close_nolock(ctl);
    return rc;
}


int tm_lib_close_sw(tm_handle hndl)
{
    int rc;
 	void *hEnv;

	TM_CTL(ctl,hndl)

	hEnv = TM_ENV(ctl);

    /* Is nodes followed by glob the right lock order? */
    rc = tm_nodes_lock(hEnv);
    if (rc)
        return rc;

    rc = tm_glob_lock(hEnv);
    if (rc)
    {
        tm_nodes_unlock(hEnv);
        return rc;
    }

    rc = tm_lib_close_nolock(ctl);

    tm_nodes_unlock(hEnv);
    tm_glob_unlock(hEnv);

    return rc;
}

/**
 */

#define TM_FREE(var)  if (var) { tm_free(var) ; var = NULL ;}

static int tm_lib_close_nolock(struct tm_ctl *ctl)
{
    unsigned int i;

    rm_close(ctl->rm);

    TM_FREE(ctl->tm_queue_array);
    TM_FREE(ctl->tm_a_node_array);
    TM_FREE(ctl->tm_b_node_array);
    TM_FREE(ctl->tm_c_node_array);
    TM_FREE(ctl->tm_port_array);
    TM_FREE(ctl->tm_q_lvl_drop_prof_ptr);
    TM_FREE(ctl->tm_a_lvl_drop_prof_ptr);
    TM_FREE(ctl->tm_b_lvl_drop_prof_ptr);
    for (i=0; i<TM_WRED_COS; i++)
    {
        TM_FREE(ctl->tm_c_lvl_drop_prof_ptr[i]);
        TM_FREE(ctl->tm_p_lvl_drop_prof_ptr_cos[i]);
    }
    TM_FREE(ctl->tm_p_lvl_drop_prof_ptr);

    TM_FREE(ctl->tm_shaping_profiles);
    TM_FREE(ctl->tm_q_lvl_drop_profiles);
    TM_FREE(ctl->tm_a_lvl_drop_profiles);
    TM_FREE(ctl->tm_b_lvl_drop_profiles);
    for (i=0; i<TM_WRED_COS; i++)
    {
        TM_FREE(ctl->tm_c_lvl_drop_profiles[i]);
        TM_FREE(ctl->tm_p_lvl_drop_profiles_cos[i]);
    }
    TM_FREE(ctl->tm_p_lvl_drop_profiles);

    TM_FREE(ctl->tm_wred_q_lvl_curves);
    TM_FREE(ctl->tm_wred_a_lvl_curves);
    TM_FREE(ctl->tm_wred_b_lvl_curves);
    for (i=0; i<TM_WRED_COS; i++)
    {
         TM_FREE(ctl->tm_wred_c_lvl_curves[i]);
         TM_FREE(ctl->tm_wred_ports_curves_cos[i]);
    }
    TM_FREE(ctl->tm_wred_ports_curves);
    TM_FREE(ctl->tm_port_sms_attr_pbase);
    TM_FREE(ctl->tm_port_sms_attr_qmap_pars);

#ifdef VIRTUAL_LAYER
	release_virtual_layer(ctl->hVirtualLayer);
#else
    /* Clean reshuffling list */
    tm_clean_list_nolock(ctl);
#endif
	_tm_logical_clear_dictionary(ctl->hLogicalLayer);
   /* release TM lib handle */
    tm_free(ctl);

    return 0;
}

/**
 */
int tm_lib_restart_hw(tm_handle hndl)
{
    uint32_t 	i;
    uint8_t 	j;
    int rc;

    TM_CTL(ctl,hndl)

    rc = set_hw_max_dp_mode(hndl);
    if (rc)	goto out;

    rc = set_hw_drop_aqm_mode(hndl);
    if (rc)	goto out;

    for (i=0; i<TM_NUM_WRED_QUEUE_CURVES; i++)
    {
        rc = set_hw_queues_wred_curve(hndl, (uint8_t)i);
        if (rc)	goto out;
    }

    for (i=0; i<TM_NUM_WRED_A_NODE_CURVES; i++)
	{
        rc = set_hw_a_nodes_wred_curve(hndl, (uint8_t)i);
        if (rc) goto out;
    }

    for (i=0; i<TM_NUM_WRED_B_NODE_CURVES; i++)
	{
        rc = set_hw_b_nodes_wred_curve(hndl, (uint8_t)i);
        if (rc) goto out;
    }

    for (i=0; i<TM_NUM_WRED_C_NODE_CURVES; i++)
	{
        for (j=0; j<TM_WRED_COS; j++)
		{
            rc = set_hw_c_nodes_wred_curve(hndl, j, (uint8_t)i);
            if (rc)  goto out;
        }
    }

    for (i=0; i<TM_NUM_WRED_PORT_CURVES; i++)
	{
        rc = set_hw_ports_wred_curve(hndl, (uint8_t)i);
        if (rc) goto out;
        for (j=0; j<TM_WRED_COS; j++)
        {
            rc = set_hw_ports_wred_curve_cos(hndl, j, (uint8_t)i);
            if (rc) goto out;
        }
    }

    for (i=0; i<TM_NUM_QUEUE_DROP_PROF; i++)
	{
        rc = set_hw_queue_drop_profile(hndl, i);
        if (rc) goto out;
    }

    for (i=0; i<TM_NUM_A_NODE_DROP_PROF; i++)
	{
        rc = set_hw_a_nodes_drop_profile(hndl, i);
        if (rc) goto out;
    }

    for (i=0; i<TM_NUM_B_NODE_DROP_PROF; i++)
	{
        rc = set_hw_b_nodes_drop_profile(hndl, i);
        if (rc) goto out;
    }

    for (i=0; i<TM_NUM_C_NODE_DROP_PROF; i++)
	{
        for (j=0; j<TM_WRED_COS; j++)
		{
            rc = set_hw_c_nodes_drop_profile(hndl, j, i);
            if (rc) goto out;
        }
    }

    /* No need to set Port Drop porfiles, it's done inside
     * set_hw_port later */

    rc = set_hw_aging_status(hndl);
    if (rc < 0) goto out;

    rc = set_hw_periodic_scheme(hndl);
    if (rc) goto out;

    rc = set_hw_tree_deq_status(hndl);
    if (rc < 0) goto out;

    rc = set_hw_tree_dwrr_priority(hndl);
    if (rc < 0) goto out;

    /* other registers */
    rc = set_hw_gen_conf(hndl);
    if (rc) goto out;

    for (i=0; i<P_LEVEL; i++)
	{
        rc = set_hw_shaping_status(hndl, i);
        if (rc) goto out;
    }

    rc = set_hw_dwrr_limit(hndl);
    if (rc) goto out;

    /* Download Eligable Prio Table */
    rc = set_hw_elig_prio_func_tbl_q_level(hndl);
    if (rc) goto out;

    rc = set_hw_elig_prio_func_tbl_a_level(hndl);
    if (rc) goto out;

    rc = set_hw_elig_prio_func_tbl_b_level(hndl);
    if (rc) goto out;

    rc = set_hw_elig_prio_func_tbl_c_level(hndl);
    if (rc) goto out;

    rc = set_hw_elig_prio_func_tbl_p_level(hndl);
    if (rc) goto out;

    rc = set_hw_uninstall_default_queues(hndl);
    if (rc) goto out;

    /* Download Nodes */
    for (i=0; i < ctl->tm_total_ports; i++)
	{
		if (ctl->tm_port_array[i].mapping.nodeStatus != TM_NODE_FREE)
		{
			rc = set_hw_port(hndl, (uint8_t)i);
			if (rc)  goto out;
			rc = set_hw_node_elig_prio_function(hndl, P_LEVEL, i);
			if (rc)  goto out;
		}
    }

    for (i=0; i < ctl->tm_total_c_nodes; i++)
	{
		if (ctl->tm_c_node_array[i].mapping.nodeStatus != TM_NODE_FREE)
		{
			rc = set_hw_node_mapping(hndl, C_LEVEL, i);
			if (rc)		goto out;
			rc = set_hw_b_node_params(hndl, i);
			if (rc)		goto out;
			rc = set_hw_node_elig_prio_function(hndl, C_LEVEL, i);
			if (rc)		goto out;
			/* sets the c-node shaping params base on the shaping profile ptr */
			rc = set_hw_node_shaping(hndl, C_LEVEL, i);
			if (rc)		goto out;
		}
    }

    for (i=0; i < ctl->tm_total_b_nodes; i++)
	{
		if (ctl->tm_b_node_array[i].mapping.nodeStatus != TM_NODE_FREE)
		{
			rc = set_hw_node_mapping(hndl, B_LEVEL, i);
			if (rc)		goto out;
			rc = set_hw_b_node_params(hndl, i);
			if (rc)		goto out;
			rc = set_hw_node_elig_prio_function(hndl, B_LEVEL, i);
			if (rc)		goto out;
			/* sets the b-node shaping params base on the shaping profile ptr */
			rc = set_hw_node_shaping(hndl, B_LEVEL, i);
			if (rc)		goto out;
		}
    }

    for (i=0; i < ctl->tm_total_a_nodes; i++)
	{
		if (ctl->tm_a_node_array[i].mapping.nodeStatus != TM_NODE_FREE)
		{
			rc = set_hw_node_mapping(hndl, A_LEVEL, i);
			if (rc)		goto out;
			rc = set_hw_a_node_params(hndl, i);
			if (rc)		goto out;
			rc = set_hw_node_elig_prio_function(hndl, A_LEVEL, i);
			if (rc)		goto out;
			/* sets the a-node shaping params base on the shaping profile ptr */
			rc = set_hw_node_shaping(hndl, A_LEVEL, i);
			if (rc)		goto out;
		}
	}
    for (i=0; i < ctl->tm_total_queues; i++)
	{
		if (ctl->tm_queue_array[i].mapping.nodeStatus != TM_NODE_FREE)
		{
			rc = set_hw_node_mapping(hndl, Q_LEVEL, i);
			if (rc)		goto out;
			rc = set_hw_queue_params(hndl, i);
			if (rc)		goto out;
			rc = set_hw_node_elig_prio_function(hndl, Q_LEVEL, i);
			if (rc)		goto out;
			/* sets the queue shaping params base on the shaping profile
			 * ptr */
			rc = set_hw_node_shaping(hndl, Q_LEVEL, i);
			if (rc)		goto out;
			if (ctl->tm_queue_array[i].installed == TM_ENABLE)	rc = set_hw_install_queue(hndl, i);
			else 												rc = set_hw_uninstall_queue(hndl, i);
			if (rc)		goto out;
		}
    }

  out:
    if (rc)	tm_lib_close_sw(hndl);
    return rc;
}
