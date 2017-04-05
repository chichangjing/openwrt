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
 * @brief basic H/W configuration functions implementation
 *
 * @file cpss_hw_configuration.c
 *
 * $Revision: 2.0 $
 */

#include <string.h>
#include "tm_hw_configuration_interface.h"

#include "tm_platform_implementation_definitions.h"

#include "tm_os_interface.h"



/***********************************************************************************/

void*	new_cpss_tm_environment_handle(GT_U8	devNumber)
{
	struct cpss_tm_environment * hEnv;
	hEnv = (struct cpss_tm_environment *)tm_malloc(sizeof(struct cpss_tm_environment));
	hEnv->magicNumber= HENV_MAGIC;
	hEnv->devNumber = devNumber;
	return (void *) hEnv;
}



unsigned int init_tm_hardware_configuration(void * hEnv)
{
	CPSS_ENV(env, hEnv, 1, 1)

	return 0;
}



unsigned int	get_tm_port_count(void * hEnv)
{
	CPSS_ENV(env,hEnv, -1, -1)
	return env->tree_structure.numOfPorts;
}
unsigned int	get_tm_a_nodes_count(void * hEnv)
{
	CPSS_ENV(env,hEnv, -1, -1)
	return env->tree_structure.numOfAnodes;
}
unsigned int	get_tm_b_nodes_count(void * hEnv)
{
	CPSS_ENV(env,hEnv, -1, -1)
	return env->tree_structure.numOfBnodes;
}
unsigned int	get_tm_c_nodes_count(void * hEnv)
{
	CPSS_ENV(env,hEnv, -1, -1)
	return env->tree_structure.numOfCnodes;
}
unsigned int	get_tm_queues_count(void * hEnv)
{
	CPSS_ENV(env,hEnv, -1, -1)
	return env->tree_structure.numOfQueues;
}



/****************************************************************************************/

#define TM_MAX_PORT_QUANTUM  0x1ff  /* Field is  9 bits. */
#define TM_MAX_NODE_QUANTUM 0x3fff  /* Field is 14 bits. */

#define NODE_QUANTUM_CHUNK_SIZE		256
#define NODE_QUANTUM_RANGE			256

#define PORT_QUANTUM_CHUNK_SIZE		64

unsigned int	get_node_quantum_chunk_size() { return NODE_QUANTUM_CHUNK_SIZE; }
unsigned int	get_node_quantum_range()	  { return NODE_QUANTUM_RANGE; }
unsigned int	get_port_quantum_chunk_size() { return PORT_QUANTUM_CHUNK_SIZE; }

unsigned int	get_node_max_quantum_chunks()	  { return TM_MAX_NODE_QUANTUM; }
unsigned int	get_port_max_quantum_chunks()	  { return TM_MAX_PORT_QUANTUM; }

