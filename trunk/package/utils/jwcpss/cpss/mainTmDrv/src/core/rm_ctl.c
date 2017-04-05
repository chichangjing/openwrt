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
 * @brief Resource Manager control functions implementation.
 *
 * @file rm_ctl.c
 *
 * $Revision: 2.0 $
 */

#include <stdlib.h>
#include <errno.h>

#include "rm_internal_types.h"
#include "rm_ctl.h"
#include "rm_chunk.h"

#include "tm_os_interface.h"
#include "tm_core_types.h"
/* for TM_NUM_PORT_DROP_PROF definition */
/*#include "tm_hw_configuration_interface.h" */


/**
 */
 
#define	CHECK_RM_CREATION(fun)   if (!fun) { rm_close((rmctl_t)ctl); return NULL;}
 
 
rmctl_t * rm_open(	uint16_t total_ports,
					uint16_t total_c_nodes,
					uint16_t total_b_nodes,
					uint16_t total_a_nodes,
					uint32_t total_queues)
{

 	struct rmctl *   ctl = NULL;

    /* Create rmctl instance */
    ctl = tm_malloc(sizeof(struct rmctl));
    if (ctl) 
    {
		tm_memset(ctl, 0, sizeof(*ctl));

		/* Fill in ctl structure */
		ctl->magic = RM_MAGIC;
		
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_SHAPING_PRF]), total_queues + total_a_nodes + total_b_nodes + total_c_nodes))

		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_Q_CURVE]), TM_NUM_WRED_QUEUE_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_A_CURVE]), TM_NUM_WRED_A_NODE_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_B_CURVE]), TM_NUM_WRED_B_NODE_CURVES))
		
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_C_CURVE_COS_0]), TM_NUM_WRED_C_NODE_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_C_CURVE_COS_1]), TM_NUM_WRED_C_NODE_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_C_CURVE_COS_2]), TM_NUM_WRED_C_NODE_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_C_CURVE_COS_3]), TM_NUM_WRED_C_NODE_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_C_CURVE_COS_4]), TM_NUM_WRED_C_NODE_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_C_CURVE_COS_5]), TM_NUM_WRED_C_NODE_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_C_CURVE_COS_6]), TM_NUM_WRED_C_NODE_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_C_CURVE_COS_7]), TM_NUM_WRED_C_NODE_CURVES))
		
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_P_CURVE]), TM_NUM_WRED_PORT_CURVES))

		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_P_CURVE_COS_0]), TM_NUM_WRED_PORT_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_P_CURVE_COS_1]), TM_NUM_WRED_PORT_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_P_CURVE_COS_2]), TM_NUM_WRED_PORT_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_P_CURVE_COS_3]), TM_NUM_WRED_PORT_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_P_CURVE_COS_4]), TM_NUM_WRED_PORT_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_P_CURVE_COS_5]), TM_NUM_WRED_PORT_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_P_CURVE_COS_6]), TM_NUM_WRED_PORT_CURVES))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_WRED_P_CURVE_COS_7]), TM_NUM_WRED_PORT_CURVES))

		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_Q_DROP_PRF]), TM_NUM_QUEUE_DROP_PROF))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_A_DROP_PRF]), TM_NUM_A_NODE_DROP_PROF))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_B_DROP_PRF]), TM_NUM_B_NODE_DROP_PROF))
		
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_C_DROP_PRF_COS_0]), TM_NUM_C_NODE_DROP_PROF))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_C_DROP_PRF_COS_1]), TM_NUM_C_NODE_DROP_PROF))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_C_DROP_PRF_COS_2]), TM_NUM_C_NODE_DROP_PROF))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_C_DROP_PRF_COS_3]), TM_NUM_C_NODE_DROP_PROF))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_C_DROP_PRF_COS_4]), TM_NUM_C_NODE_DROP_PROF))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_C_DROP_PRF_COS_5]), TM_NUM_C_NODE_DROP_PROF))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_C_DROP_PRF_COS_6]), TM_NUM_C_NODE_DROP_PROF))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_C_DROP_PRF_COS_7]), TM_NUM_C_NODE_DROP_PROF))
		
		/* for ports  drop profiles are per port -  drop profile count is equial to ports number */
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_P_DROP_PRF]), total_ports))
		
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_P_DROP_PRF_COS_0]), total_ports))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_P_DROP_PRF_COS_1]), total_ports))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_P_DROP_PRF_COS_2]), total_ports))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_P_DROP_PRF_COS_3]), total_ports))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_P_DROP_PRF_COS_4]), total_ports))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_P_DROP_PRF_COS_5]), total_ports))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_P_DROP_PRF_COS_6]), total_ports))
		CHECK_RM_CREATION(init_resource_manager(&(ctl->rm_profiles[RM_P_DROP_PRF_COS_7]), total_ports))



		ctl->rm_free_nodes[A_LEVEL]=rm_new_chunk(0,total_a_nodes,NULL);
		ctl->rm_free_nodes[B_LEVEL]=rm_new_chunk(0,total_b_nodes,NULL);
		ctl->rm_free_nodes[C_LEVEL]=rm_new_chunk(0,total_c_nodes,NULL);
		/* special case - patching H/W bug  - exclude queue 0  from configuration*/
		ctl->rm_free_nodes[Q_LEVEL]=rm_new_chunk(1,total_queues-1,NULL);
	
	}
	return (rmctl_t *)ctl;
}


/**
 */
int rm_close(rmctl_t hndl)
{
    int i;

    RM_HANDLE(ctl, hndl);

    ctl->magic = 0;

	clear_chunk_list(ctl->rm_free_nodes[Q_LEVEL]);
	clear_chunk_list(ctl->rm_free_nodes[A_LEVEL]);
	clear_chunk_list(ctl->rm_free_nodes[B_LEVEL]);
	clear_chunk_list(ctl->rm_free_nodes[C_LEVEL]);


	for (i=0 ; i < RM_MAX_RESOURCE_TYPES ; i++)
	{
		free_resource_manager(&(ctl->rm_profiles[i]));
	}

    /* free rm handle */
    tm_free(ctl);
    return 0;
}


/****************************************************************************
 * resource manager implementation
 ****************************************************************************/ 




int init_resource_manager(struct rm_resource_manager * mgr, uint32_t size)
{
    mgr->used = tm_malloc(size*sizeof(uint8_t));
 	/* set all resources free */
 	if (mgr->used)
 	{
		mgr->size = size;
		tm_memset(mgr->used, 0, size);
		mgr->counter = size;    
		mgr->index= 0;
	}
	return (mgr->used !=NULL);
}

void free_resource_manager(struct rm_resource_manager * mgr)
{
	if (mgr->used)	tm_free(mgr->used);
}


/* to get first allocated index -1 is used */
int get_next_allocated_resource(struct rm_resource_manager * mgr, int index)
{
    int next_index = index + 1; 
    int ret_next_index = -1;

	if (mgr->counter == mgr->size)  return -1;

	while(next_index < (int)mgr->size)
    {
        if (mgr->used[next_index] == RESOURCE_ALLOCATED)
        {
            ret_next_index = next_index;
            break;
        }
        next_index++;
    }

	
	return ret_next_index;
}


int  get_free_resource(struct rm_resource_manager * mgr)
{
	if (mgr->counter == 0)  return -1;
	while(mgr->used[mgr->index] != RESOURCE_FREE)
	{
		mgr->index++;
		if (mgr->index == mgr->size) mgr->index=0;
	}
	
	mgr->used[mgr->index] = RESOURCE_ALLOCATED; 
	mgr->counter--;
	return mgr->index;
}

int  reserve_resource(struct rm_resource_manager * mgr, uint32_t resource_id)
{
	if (resource_id >= mgr->size)
		return -1; /* out of range */
	if (mgr->used[resource_id] != RESOURCE_FREE)
		return -1; /* busy */
	mgr->used[resource_id] = RESOURCE_ALLOCATED;
	mgr->counter--;
	return 0;
}

int  release_resource(struct rm_resource_manager * mgr, uint32_t resource_id)
{
	if (resource_id >= mgr->size) return -1; /* out of range */
	if (mgr->used[resource_id] != RESOURCE_ALLOCATED)  return -1; /* not allocated  or range allocated*/
	mgr->used[resource_id] = RESOURCE_FREE;
	mgr->counter++;
	/* hole processing */
	if (mgr->index > resource_id) mgr->index = resource_id;
	return 0;
}

int  get_free_count(struct rm_resource_manager * mgr)
{
	return mgr->counter;
}

int  resource_status(struct rm_resource_manager * mgr, uint32_t resource_id)
{
	if (resource_id >= mgr->size)  return  -1;
	return mgr->used[resource_id] ;
}

int  get_free_resource_range(struct rm_resource_manager * mgr,uint32_t range)
{
	uint32_t i;
	uint32_t sum;
	if (mgr->counter < range )  return -1;
	/* let use that free resource value is 0 
	 * so sum of all resource values  in free range  must be 0 
	 */
	sum=0; 
	for (i = mgr->index ; i < mgr->index+range-1 ; i++)  sum+= mgr->used[i] ;
	/* calculate "floating  window" */
	for (i = mgr->index ; i <= mgr->size - range ; i++)
	{
		sum+= mgr->used[i+range-1] ;	
		if (sum==0) /* range found */
		{ 
			mgr->counter-=range;
			/* mark range as used */
			for (  ; range ; )
			{				
				range--;
				mgr->used[i+range] = RESOURCE_RANGE_ALLOCATED+range;  /* mark as used in range*/
			}
			if (mgr->index == i) mgr->index+=range;
			return i;
		}
		sum -= mgr->used[i] ;
	}
	/* not found , return fault value*/
	return -1;
}

int  get_free_resource_range_slot(struct rm_resource_manager * mgr, uint32_t range)
{
	uint32_t i;
	uint32_t slot_pos;
	if (mgr->counter < range )  return -1;
	/*  find first potentially free slot */
	slot_pos = mgr->index / range ;
	if (mgr->index % range) slot_pos +=1;
	slot_pos *= range;
	/* search free slot */
	while(1)
	{
		for (i = slot_pos ; i < slot_pos+range  ; i++)
		{
			if (i == mgr->size) return -1; /* last position in the pool achieved , free range not found */
			if (mgr->used[i])/* some resource occupied - jump to next range */
			{
				slot_pos += range;
				break;
			}
		}
		if (i==slot_pos+range) /*  all  resources in range are free*/
		{
			mgr->counter-=range;
			/* mark range as used */
			for (  ; range ; )
			{				
				range--;
				mgr->used[slot_pos+range] = RESOURCE_RANGE_ALLOCATED+range;  /* mark as used in range*/
			}
			return slot_pos;
		}
	}
}


int  validate_resource_range(struct rm_resource_manager * mgr, uint32_t resource_id, uint32_t range)
{
	uint32_t i;
	if (resource_id >= mgr->size) return 1; /* invalid */
	for (i =0 ; i < range ; i++) 
	{
		if (mgr->used[i+resource_id] != RESOURCE_RANGE_ALLOCATED+i ) return 1;
	}
	return 0;
}

int  release_resource_range(struct rm_resource_manager * mgr, uint32_t resource_id, uint32_t range)
{
	uint32_t i;
	if (validate_resource_range(mgr,resource_id,range) ) return -1;
	/* range looks valid */
	for (i =0 ; i < range ; i++)
	{
		mgr->used[i+resource_id] = RESOURCE_FREE;
	}
	mgr->counter+=range;
	/* hole processing */
	if (mgr->index > resource_id) mgr->index = resource_id;
	return 0;
}
