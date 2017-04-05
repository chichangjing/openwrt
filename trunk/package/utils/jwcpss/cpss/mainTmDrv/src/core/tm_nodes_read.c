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
 * @brief tm_nodes_read implementation.
 *
 * @file tm_nodes_read.c
 *
 * $Revision: 2.0 $
 */

#include <stdlib.h>
#include "tm_nodes_read.h"
#include "tm_errcodes.h"
#include "tm_os_interface.h"
#include "tm_locking_interface.h"

#include "tm_nodes_ctl_internal.h"

/***************************************************************************
 * Read Configuration 
 ***************************************************************************/

/**
 */
int tm_read_queue_configuration(tm_handle hndl, uint32_t queue_index,
                                struct tm_queue_params *params)
{
    struct tm_queue *node = NULL;
    int rc;

    TM_CTL(ctl, hndl)

	CONVERT_TO_PHYSICAL(Q_LEVEL,queue_index) /** if supported - inside macro */
	
    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

    /* Check parameters validity */
	VALIDATE_NODE_ALLOCATED(queue,queue_index)
	if (rc) goto out;
	
    node = &(ctl->tm_queue_array[queue_index]);

    params->shaping_profile_ref = node->shaping_profile_ref;
    params->quantum = node->dwrr_quantum;
    params->wred_profile_ref = node->wred_profile_ref;
    params->elig_prio_func_ptr = node->elig_prio_func;
out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_read_a_node_configuration(tm_handle hndl, uint32_t node_index,
                                 struct tm_a_node_params *params)
{
    struct tm_a_node *node = NULL;
    int rc = 0;
    int i;

    TM_CTL(ctl, hndl);
	
	CONVERT_TO_PHYSICAL(A_LEVEL,node_index)    /** if supported - inside macro */

	rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

    /* Check parameters validity */
	VALIDATE_NODE_ALLOCATED(a_node,node_index)
	if (rc) goto out;
		
    node = &(ctl->tm_a_node_array[node_index]);
    
    params->shaping_profile_ref = node->shaping_profile_ref;
    params->quantum = node->dwrr_quantum;
    for (i=0; i<8; i++)
	{
        if ((node->dwrr_enable_per_priority_mask & (1 << i)) != 0)
            params->dwrr_priority[i] = TM_ENABLE;
        else
            params->dwrr_priority[i] = TM_DISABLE;
	}
    params->wred_profile_ref = node->wred_profile_ref;
    params->elig_prio_func_ptr = node->elig_prio_func;
    params->num_of_children = node->mapping.childHi  - node->mapping.childLo + 1;
out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_read_b_node_configuration(tm_handle hndl, uint32_t node_index,
                                 struct tm_b_node_params *params)
{
    struct tm_b_node *node = NULL;
    int rc = 0;
    int i;

    TM_CTL(ctl, hndl)

	CONVERT_TO_PHYSICAL(B_LEVEL,node_index) /** if supported - inside macro */
	
	rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

    /* Check parameters validity */
	VALIDATE_NODE_ALLOCATED(b_node,node_index)
	if (rc) goto out;
	
    node = &(ctl->tm_b_node_array[node_index]);
    
    params->shaping_profile_ref = node->shaping_profile_ref;
    params->quantum = node->dwrr_quantum;
    for (i=0; i<8; i++)
	{
        if ((node->dwrr_enable_per_priority_mask & (1 << i)) != 0)
            params->dwrr_priority[i] = TM_ENABLE;
        else
            params->dwrr_priority[i] = TM_DISABLE;
	}
    params->wred_profile_ref = node->wred_profile_ref;
    params->elig_prio_func_ptr = node->elig_prio_func;
    params->num_of_children = node->mapping.childHi - node->mapping.childLo + 1;
out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_read_c_node_configuration(tm_handle hndl, uint32_t node_index,
                                 struct tm_c_node_params *params)
{
    struct tm_c_node *node = NULL;
    int rc = 0;
    int i;

    TM_CTL(ctl, hndl)

	CONVERT_TO_PHYSICAL(C_LEVEL,node_index)   /** if supported - inside macro */
	
    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

    /* Check parameters validity */
	VALIDATE_NODE_ALLOCATED(c_node,node_index)
	if (rc) goto out;
	
    node = &(ctl->tm_c_node_array[node_index]);
    
    params->shaping_profile_ref = node->shaping_profile_ref;
    params->quantum = node->dwrr_quantum;
    for (i=0; i<8; i++)
        if ((node->dwrr_enable_per_priority_mask & (1 << i)) != 0)
            params->dwrr_priority[i] = TM_ENABLE;
        else
            params->dwrr_priority[i] = TM_DISABLE;
    params->wred_cos = node->wred_cos;
    for (i=0; i<TM_WRED_COS; i++)
        params->wred_profile_ref[i] = node->wred_profile_ref[i];
    params->elig_prio_func_ptr = node->elig_prio_func;
    params->num_of_children = node->mapping.childHi - node->mapping.childLo + 1;
out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_read_port_configuration(tm_handle hndl, uint32_t port_index,
                               struct tm_port_params *params,
                               struct tm_port_drop_per_cos *cos_params)
								
{
    struct tm_port *port = NULL;
    int rc = 0;
    int i;

    TM_CTL(ctl, hndl)

	rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

    /* Check parameters validity */
    VALIDATE_NODE_ALLOCATED(port,port_index)
	if (rc) goto out;
	
    port = &(ctl->tm_port_array[port_index]);
    /*read from SW*/
    params->cir_bw = port->cir_sw;
    params->eir_bw = port->eir_sw;
	
    params->cbs = port->cir_burst_size ;
    params->ebs = port->eir_burst_size ;

    for (i = 0; i < 8; i++)
	{
        if ((port->dwrr_enable_per_priority_mask & (1 << i)) != 0)
            params->dwrr_priority[i] = TM_ENABLE;
        else
            params->dwrr_priority[i] = TM_DISABLE;
	}
    for (i=0; i<8; i++)
	{
        params->quantum[i] = port->dwrr_quantum_per_level[i].quantum;
		params->elig_prio_func_ptr = port->elig_prio_func;
		params->wred_profile_ref = port->wred_profile_ref;
	}
    cos_params->wred_cos = port->wred_cos;
    for (i=0; i<8; i++)
        cos_params->wred_profile_ref[i] = port->wred_profile_ref_cos[i];
    params->num_of_children = port->mapping.childHi - port->mapping.childLo + 1;

out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


#define FLAGGED_ELIG_FUN(node) node->elig_prio_func + 64*node->node_temporary_disabled

int tm_traverse_port_tree_sw(tm_handle hndl, unsigned int port_index, TM_PORT_TRAVERSE_TREE_FUN dump_fun, void * vpEnv,  void* vpUserData)
{
	int ic, ib, ia, iq;
	struct tm_port *		port_ptr ;
	struct tm_c_node *	c_node_ptr;
	struct tm_b_node *	b_node_ptr;
	struct tm_a_node *	a_node_ptr;
	struct tm_queue *		queue_ptr;
	int result;

    TM_CTL(ctl, hndl);

	port_ptr = &(ctl->tm_port_array[port_index]);
	result = dump_fun(1, vpEnv, P_LEVEL, port_index, port_ptr->mapping.nodeStatus, 0, FLAGGED_ELIG_FUN(port_ptr), vpUserData);
	if (result) return result;
	/* safety check - only USED node have valid childern range */
	if (port_ptr->mapping.nodeStatus == TM_NODE_USED)
	{
		for (ic = port_ptr->mapping.childLo ; ic <= port_ptr->mapping.childHi ; ic++)
		{
			c_node_ptr=&(ctl->tm_c_node_array[ic]);
			result = dump_fun(1, vpEnv, C_LEVEL, ic, c_node_ptr->mapping.nodeStatus, c_node_ptr->mapping.nodeParent, FLAGGED_ELIG_FUN(c_node_ptr), vpUserData);		
			if (result == -1 )  continue ; else if (result > 0 ) return result;
			/* safety check - only USED node have valid childern range */
			if (c_node_ptr->mapping.nodeStatus == TM_NODE_USED)
			{
				for (ib = c_node_ptr->mapping.childLo ; ib <= c_node_ptr->mapping.childHi ; ib++)
				{
					b_node_ptr=&(ctl->tm_b_node_array[ib]);
					result = dump_fun(1, vpEnv, B_LEVEL, ib, b_node_ptr->mapping.nodeStatus, b_node_ptr->mapping.nodeParent, FLAGGED_ELIG_FUN(b_node_ptr), vpUserData);
					if (result == -1 )  continue ; else if (result > 0 ) return result;
					/* safety check - only USED node have valid childern range */
					if (b_node_ptr->mapping.nodeStatus == TM_NODE_USED)
					{					
						for (ia = b_node_ptr->mapping.childLo ; ia <= b_node_ptr->mapping.childHi ; ia++)
						{
							a_node_ptr=&(ctl->tm_a_node_array[ia]);
							result = dump_fun(1, vpEnv, A_LEVEL, ia, a_node_ptr->mapping.nodeStatus, a_node_ptr->mapping.nodeParent, FLAGGED_ELIG_FUN(a_node_ptr),vpUserData);
							if (result == -1 )  continue ; else if (result > 0 ) return result;
							/* safety check - only USED node have valid childern range */
							if (a_node_ptr->mapping.nodeStatus == TM_NODE_USED)
							{
								for (iq = a_node_ptr->mapping.childLo ; iq <= a_node_ptr->mapping.childHi ; iq++)
								{
									queue_ptr=&(ctl->tm_queue_array[iq]);
									result = dump_fun(1, vpEnv, Q_LEVEL, iq, queue_ptr->mapping.nodeStatus, queue_ptr->mapping.nodeParent,  FLAGGED_ELIG_FUN(queue_ptr),vpUserData);
									if (result > 0 ) return result;
								}
							}
							result = dump_fun(0, vpEnv, A_LEVEL, ia, a_node_ptr->mapping.nodeStatus, a_node_ptr->mapping.nodeParent, FLAGGED_ELIG_FUN(a_node_ptr),vpUserData);
							if (result > 0 ) return result;
						}
					}
					result = dump_fun(0,  vpEnv, B_LEVEL, ib, b_node_ptr->mapping.nodeStatus, b_node_ptr->mapping.nodeParent, FLAGGED_ELIG_FUN(b_node_ptr), vpUserData);
					if (result == -1 )  continue ; else if (result > 0 ) return result;
				}
			}
			result = dump_fun(0, vpEnv, C_LEVEL, ic, c_node_ptr->mapping.nodeStatus, c_node_ptr->mapping.nodeParent, FLAGGED_ELIG_FUN(c_node_ptr), vpUserData);
			if (result > 0 ) return result;
		}
	}
	result = dump_fun(0, vpEnv, P_LEVEL, port_index, port_ptr->mapping.nodeStatus, 0, FLAGGED_ELIG_FUN(port_ptr) , vpUserData);
	if (result) return result;
	return 0;
}


#include "set_hw_registers_imp.h"


int tm_traverse_port_tree_hw(tm_handle hndl, unsigned int port_index, TM_PORT_TRAVERSE_TREE_FUN dump_fun, void * vpEnv,  void* vpUserData)
{
	
	int portChildLo=0,portChildHi=0, portEf=0;
	int cnodeChildLo=0,cnodeChildHi=0, cnodeParent=0, cnodeEf=0;
	int bnodeChildLo=0,bnodeChildHi=0, bnodeParent=0, bnodeEf=0;
	int anodeChildLo=0,anodeChildHi=0, anodeParent=0, anodeEf=0;
	int queueParent=0, queueEf=0;
	
	struct tm_port *		port_ptr ;
	struct tm_c_node *	c_node_ptr;
	struct tm_b_node *	b_node_ptr;
	struct tm_a_node *	a_node_ptr;
	struct tm_queue *		queue_ptr;
	
	int ic, ib, ia, iq;
	int result;
	int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_PortRangeMap)
    TM_REGISTER_VAR(TM_Sched_ClvltoPortAndBlvlRangeMap)
    TM_REGISTER_VAR(TM_Sched_BLvltoClvlAndAlvlRangeMap)
    TM_REGISTER_VAR(TM_Sched_ALvltoBlvlAndQueueRangeMap)
    TM_REGISTER_VAR(TM_Sched_QueueAMap)

    TM_REGISTER_VAR(TM_Sched_QueueEligPrioFuncPtr)
    TM_REGISTER_VAR(TM_Sched_AlvlEligPrioFuncPtr)
    TM_REGISTER_VAR(TM_Sched_BlvlEligPrioFuncPtr)
    TM_REGISTER_VAR(TM_Sched_ClvlEligPrioFuncPtr)
    TM_REGISTER_VAR(TM_Sched_PortEligPrioFuncPtr)

    TM_CTL(ctl, hndl)

	TM_READ_TABLE_REGISTER(TM.Sched.PortRangeMap , port_index, TM_Sched_PortRangeMap)
	if (rc) return -1;
	TM_REGISTER_GET(TM_Sched_PortRangeMap , Lo , portChildLo , (int) )
	TM_REGISTER_GET(TM_Sched_PortRangeMap , Hi , portChildHi, (int) )
	
	TM_READ_TABLE_REGISTER(TM.Sched.PortEligPrioFuncPtr , port_index , TM_Sched_PortEligPrioFuncPtr)
	if (rc) return -1;
	TM_REGISTER_GET(TM_Sched_PortEligPrioFuncPtr , Ptr , portEf, (uint8_t))
	
	port_ptr = &(ctl->tm_port_array[port_index]);
	result = dump_fun(1, vpEnv, P_LEVEL, port_index, port_ptr->mapping.nodeStatus, 0, portEf , vpUserData);
	if (result) return result;
	/* safety check - continue only if node is enabled*/
	if (port_ptr->mapping.nodeStatus == TM_NODE_USED)
	{
		for (ic = portChildLo ; ic <= portChildHi ; ic++)
		{
			TM_READ_TABLE_REGISTER(TM.Sched.ClvltoPortAndBlvlRangeMap , ic , TM_Sched_ClvltoPortAndBlvlRangeMap)
			if (rc) return -1;
			TM_REGISTER_GET(TM_Sched_ClvltoPortAndBlvlRangeMap , Port , cnodeParent, (int) )
			TM_REGISTER_GET(TM_Sched_ClvltoPortAndBlvlRangeMap , BlvlLo , cnodeChildLo, (int)  )
			TM_REGISTER_GET(TM_Sched_ClvltoPortAndBlvlRangeMap , BlvlHi , cnodeChildHi, (int) )									

			TM_READ_TABLE_REGISTER(TM.Sched.ClvlEligPrioFuncPtr , ic , TM_Sched_ClvlEligPrioFuncPtr)
			if (rc) return -1;
			TM_REGISTER_GET(TM_Sched_ClvlEligPrioFuncPtr , Ptr , cnodeEf, (uint8_t))

			c_node_ptr=&(ctl->tm_c_node_array[ic]);
			
			result = dump_fun(1, vpEnv, C_LEVEL, ic, c_node_ptr->mapping.nodeStatus, cnodeParent, cnodeEf, vpUserData);
			if (result == -1 )  continue ; else if (result > 0 ) return result;
			/* safety check - only USED node have valid childern range */
			if (cnodeEf != TM_NODE_DISABLED_FUN) 
			{

				for (ib = cnodeChildLo ; ib <= cnodeChildHi ; ib++)
				{
					TM_READ_TABLE_REGISTER(TM.Sched.BLvltoClvlAndAlvlRangeMap , ib , TM_Sched_BLvltoClvlAndAlvlRangeMap)
					if (rc) return -1;
					TM_REGISTER_GET(TM_Sched_BLvltoClvlAndAlvlRangeMap , Clvl , bnodeParent ,(int))
					TM_REGISTER_GET(TM_Sched_BLvltoClvlAndAlvlRangeMap , AlvlLo , bnodeChildLo,(int))
					TM_REGISTER_GET(TM_Sched_BLvltoClvlAndAlvlRangeMap , AlvlHi , bnodeChildHi,(int))
					
					TM_READ_TABLE_REGISTER(TM.Sched.BlvlEligPrioFuncPtr , ib , TM_Sched_BlvlEligPrioFuncPtr)
					if (rc) return -1;
					TM_REGISTER_GET(TM_Sched_BlvlEligPrioFuncPtr , Ptr , bnodeEf,(uint8_t))

					b_node_ptr=&(ctl->tm_b_node_array[ib]);

					result = dump_fun(1, vpEnv, B_LEVEL, ib, b_node_ptr->mapping.nodeStatus, bnodeParent, bnodeEf, vpUserData);
					if (result == -1 )  continue ; else if (result > 0 ) return result;

					/* safety check - only USED node have valid childern range */
					if (bnodeEf != TM_NODE_DISABLED_FUN) 
					{					
						for (ia = bnodeChildLo ; ia <= bnodeChildHi ; ia++)
						{
							TM_READ_TABLE_REGISTER(TM.Sched.ALvltoBlvlAndQueueRangeMap , ia , TM_Sched_ALvltoBlvlAndQueueRangeMap)
							if (rc) return -1;
							TM_REGISTER_GET(TM_Sched_ALvltoBlvlAndQueueRangeMap , Blvl , anodeParent,(int))
							TM_REGISTER_GET(TM_Sched_ALvltoBlvlAndQueueRangeMap , QueueLo , anodeChildLo,(int))
							TM_REGISTER_GET(TM_Sched_ALvltoBlvlAndQueueRangeMap , QueueHi , anodeChildHi,(int))
							
							TM_READ_TABLE_REGISTER(TM.Sched.AlvlEligPrioFuncPtr , ia , TM_Sched_AlvlEligPrioFuncPtr)
							if (rc) return -1;
							TM_REGISTER_GET(TM_Sched_AlvlEligPrioFuncPtr , Ptr , anodeEf, (uint8_t))			

							a_node_ptr=&(ctl->tm_a_node_array[ia]);
							
							result = dump_fun(1, vpEnv, A_LEVEL, ia, a_node_ptr->mapping.nodeStatus, anodeParent, anodeEf,vpUserData);
							if (result == -1 )  continue ; else if (result > 0 ) return result;
							/* safety check - only USED node have valid childern range */
							if (anodeEf != TM_NODE_DISABLED_FUN) 
							{
								for (iq = anodeChildLo ; iq <= anodeChildHi ; iq++)
								{
									TM_READ_TABLE_REGISTER(TM.Sched.QueueAMap , iq , TM_Sched_QueueAMap)
									if (rc) return -1;
									TM_REGISTER_GET(TM_Sched_QueueAMap , Alvl , queueParent,(int))
									
									TM_READ_TABLE_REGISTER(TM.Sched.QueueEligPrioFuncPtr , iq , TM_Sched_QueueEligPrioFuncPtr)
									if (rc) return -1;
									TM_REGISTER_GET(TM_Sched_QueueEligPrioFuncPtr , Ptr , queueEf, (uint8_t))

									queue_ptr=&(ctl->tm_queue_array[iq]);
									
									result = dump_fun(1, vpEnv, Q_LEVEL, iq, queue_ptr->mapping.nodeStatus, queueParent,  queueEf,vpUserData);
									if (result > 0 ) return result;
								}
							}
							result = dump_fun(0, vpEnv, A_LEVEL, ia, a_node_ptr->mapping.nodeStatus, anodeParent, anodeEf, vpUserData);
							if (result > 0 ) return result;
						}
					}
					result = dump_fun(0,  vpEnv, B_LEVEL, ib, b_node_ptr->mapping.nodeStatus, bnodeParent, bnodeEf, vpUserData);
					if (result == -1 )  continue ; else if (result > 0 ) return result;
				}
			}
			result = dump_fun(0, vpEnv, C_LEVEL, ic, c_node_ptr->mapping.nodeStatus, cnodeParent, cnodeEf, vpUserData);
			if (result > 0 ) return result;
		}
	}
	result = dump_fun(0, vpEnv, P_LEVEL, port_index, port_ptr->mapping.nodeStatus, 0, portEf , vpUserData);
	if (result) return result;
	return 0;
}


