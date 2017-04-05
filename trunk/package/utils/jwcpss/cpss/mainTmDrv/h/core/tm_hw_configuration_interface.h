#ifndef _HARDWARE_CONFIGURATION_INTERFACE_H_
#define _HARDWARE_CONFIGURATION_INTERFACE_H_

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
 * @brief declaration of interface to H/W configuration.
 *
 * @file tm_hw_configuration_interfase.h
 *
 * $Revision: 2.0 $
 */

#include "tm_defs.h"

/*
 interface for access to all hardware resources - interface is platform independent, 
 implementation is platform dependent, different files for different platforms
*/

/*
 initialization of all hardware resources
 returning result - 0 if success
                    >0 if failed
*/



unsigned int	init_tm_hardware_configuration(void * hEnv);

/*
 following function returns 1 if hardware resources were successfully initialized 
 otherwise 0 (finitialization failed or not performed)
*/

/* following functions return appropriate hardware value  or 0 if hardware initialization missed or failed */
unsigned int	get_tm_port_count(void * hEnv);
unsigned int	get_tm_c_nodes_count(void * hEnv);
unsigned int	get_tm_b_nodes_count(void * hEnv);
unsigned int	get_tm_a_nodes_count(void * hEnv);
unsigned int	get_tm_queues_count(void * hEnv);


unsigned int	get_node_quantum_chunk_size();
unsigned int	get_port_quantum_chunk_size();
unsigned int	get_node_quantum_range();
unsigned int	get_node_max_quantum_chunks();
unsigned int	get_port_max_quantum_chunks();

#endif /* _HARDWARE_CONFIGURATION_INTERFACE_H_ */
