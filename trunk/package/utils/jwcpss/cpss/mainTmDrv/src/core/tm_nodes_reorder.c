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
 * @brief TM node reordering implementation.
 *
 * @file tm_nodes_reorder.c
 *
 * $Revision: 2.0 $
 */

#include "tm_nodes_reorder.h"
#include "tm_errcodes.h"
#include "set_hw_registers.h"
#include "tm_locking_interface.h"
#include "tm_nodes_ctl_internal.h"

#include <errno.h>



#define	SWITCH_NODES_PROC(LEVEL,node_type,child_node_type, node_range_error, hw_error) \
{\
	struct tm_##node_type * node1=&ctl->tm_##node_type##_array[node1_index];\
	struct tm_##node_type * node2=&ctl->tm_##node_type##_array[node2_index];\
	VALIDATE_NODE_ALLOCATED_EX(node_type,node1_index,node_range_error,node_range_error)\
	if (rc) goto out;\
	VALIDATE_NODE_ALLOCATED_EX(node_type,node2_index,node_range_error,node_range_error)\
	if (rc) goto out;\
	/* --------------------------------------disable all nodes  ???? -------------------------------------*/\
	/* disable nodes */\
	if (set_hw_node_disable(hndl,level,node1_index)) {rc = hw_error; goto out;}\
	if (set_hw_node_disable(hndl,level,node2_index)) {rc = hw_error; goto out;}\
	/* disable children */\
	for (index=node1->mapping.childLo ; index < node1->mapping.childHi ; index++)	 { if (set_hw_node_disable(hndl,CHILD_LEVEL(level),index)) {rc = hw_error; goto out;}}\
	for (index=node2->mapping.childLo ; index < node2->mapping.childHi ; index++)	 { if (set_hw_node_disable(hndl,CHILD_LEVEL(level),index)) {rc = hw_error; goto out;}}\
	/* --------------------------------------update S/W database-----------------------------------------*/\
	/* switch parents of node children */\
	for (index=node1->mapping.childLo ; index < node1->mapping.childHi ; index++)	 { ctl->tm_##child_node_type##_array[index].mapping.nodeParent = node2_index; }\
	for (index=node2->mapping.childLo ; index < node2->mapping.childHi ; index++)	 { ctl->tm_##child_node_type##_array[index].mapping.nodeParent = node1_index; }\
	/* swap  ranges */\
	index = node1->mapping.childLo;\
	node1->mapping.childLo = node2->mapping.childLo;\
	node2->mapping.childLo = index;\
	index = node1->mapping.childHi;\
	node1->mapping.childHi = node2->mapping.childHi;\
	node2->mapping.childHi = index;\
	/* --------------------------------------update H/W mapping-----------------------------------------*/\
	/* update childen mapping*/\
	for (index=node1->mapping.childLo ; index < node1->mapping.childHi ; index++)	  { if (set_hw_node_mapping(hndl,CHILD_LEVEL(level), index)) {rc = hw_error; goto out;}}\
	for (index=node2->mapping.childLo ; index < node2->mapping.childHi ; index++)	  { if (set_hw_node_mapping(hndl,CHILD_LEVEL(level), index)) {rc = hw_error; goto out;}}\
	/* update nodes mapping */\
	if (set_hw_node_mapping(hndl,level,node1_index)) {rc = hw_error; goto out;}\
	if (set_hw_node_mapping(hndl,level,node2_index)) {rc = hw_error; goto out;}\
	/* --------------------------------------enable all nodes  ???? -------------------------------------*/\
	/* enable children */\
	for (index=node1->mapping.childLo ; index < node1->mapping.childHi ; index++)	  { if (set_hw_node_elig_prio_function(hndl,CHILD_LEVEL(level),index)) {rc = hw_error; goto out;}}\
	for (index=node2->mapping.childLo ; index < node2->mapping.childHi ; index++)	  { if (set_hw_node_elig_prio_function(hndl,CHILD_LEVEL(level),index)) {rc = hw_error; goto out;}}\
	/* enable nodes */\
	if (set_hw_node_elig_prio_function(hndl,level,node1_index)) {rc = hw_error; goto out;}\
	if (set_hw_node_elig_prio_function(hndl,level,node2_index)) {rc = hw_error; goto out;}\
}

/**
 */
int tm_nodes_switch(tm_handle hndl,
                    enum tm_level level,
                    uint16_t node1_index,
                    uint16_t node2_index)
{
    int rc;
    int index;

    TM_CTL(ctl, hndl)
    
	CONVERT_TO_PHYSICAL(level,node1_index)
	CONVERT_TO_PHYSICAL(level,node2_index)

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;
    
    switch(level)
	{
		case A_LEVEL:
			SWITCH_NODES_PROC(A_LEVEL, a_node, queue, TM_CONF_A_NODE_IND_OOR, TM_HW_A_NODE_CONFIG_FAIL)
			break;
		case B_LEVEL:
			SWITCH_NODES_PROC(B_LEVEL, b_node, a_node, TM_CONF_B_NODE_IND_OOR, TM_HW_B_NODE_CONFIG_FAIL)
			break;            
		case C_LEVEL:
			SWITCH_NODES_PROC(C_LEVEL, c_node, b_node, TM_CONF_C_NODE_IND_OOR, TM_HW_B_NODE_CONFIG_FAIL)
			break;       
		case P_LEVEL:
			SWITCH_NODES_PROC(P_LEVEL, port, c_node, TM_CONF_PORT_IND_OOR, TM_HW_PORT_CONFIG_FAIL)
			break;   
		default:
			rc= -ERANGE;   
    }

out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

