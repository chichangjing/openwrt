/*
 * (c), Copyright 2009-2013, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief TM tm logical layer interface.
 *
 * @file tm_logical_layer_interface.h
 *
 * $Revision: 2.0 $
 */


#ifndef _TM_LOGICAL_LAYER_INTERFACE_H_
#define _TM_LOGICAL_LAYER_INTERFACE_H_

#include "tm_defs.h"

int				tm_get_node_number(tm_handle hndl, enum tm_level level, const char * logical_name);
const char *	tm_get_node_logical_name(tm_handle hndl,enum tm_level level, unsigned int node_number);


/***************************************************************************
 * Port Creation 
 ***************************************************************************/

/** Create Port , download its parameters to HW, and assign name to created port
 *
 *   @param[in]     hndl            TM lib handle. 
 *   @param[in]     port_index      Port index.       
 *   @param[in]     port_speed      Port physical bandwidth.
 *   @param[in]     params          Port parameters structure pointer.
 *   @param[in]     num_of_c_nodes  Number of C-nodes under port.
 *   @param[in]     num_of_b_nodes  Number of B-nodes under port.
 *   @param[in]     num_of_a_nodes  Number of A-nodes under port.
 *   @param[in]     num_of_queues   Number of Queues under port.
 *   @param[in]     port_name       string containing name to assign to new port.

 *   @note: to indicate that no shaping is needed for port, set
 *   params->cir_bw to TM_INVAL, in this case other shaping 
 *   parameters will not be considered.
 *   Valid number of nodes per level must be multiple of number nodes
 *   on upper level (parent nodes) in case of equal distribution. And
 *   in case of unequal distribution all parent nodes except the last
 *   one must have the same children range and the last parent node -
 *   number of children less that the range. In case of not valid
 *   number will be returned -EACCES error code.
 *   @note: if port will be used for transparent queues, set the number_of_c/b/a_nodes = 1.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -ENOMEM if memory allocation in RM fails.
 *   @retval -EACCES if number of nodes per level is not valid.
 *   @retval TM_CONF_PORT_IND_OOR if port_index is out of range.
 *   @retval TM_CONF_PORT_IND_USED if port_index is already used.
 *   @retval TM_CONF_PORT_SPEED_OOR if port_speed is out of range.
 *
 *   @retval TM_CONF_P_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_PORT_BW_OUT_OF_SPEED.
 *   @retval TM_CONF_PORT_QUANTUM_OOR.
 *   @retval TM_CONF_PORT_DWRR_PRIO_OOR.
 *
 *   @retval TM_CONF_INVALID_NUM_OF_C_NODES.
 *   @retval TM_CONF_INVALID_NUM_OF_B_NODES.
 *   @retval TM_CONF_INVALID_NUM_OF_A_NODES.
 *   @retval TM_CONF_INVALID_NUM_OF_QUEUES.
 *
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is
 *   oor
 *
 *   @retval TM_CONF_PORT_BW_OOR
 *   @retval TM_CONF_PORT_BS_OOR
 *   @retval TM_CONF_PORT_MIN_TOKEN_TOO_LARGE.
 *   @retval TM_CONF_PORT_MAX_TOKEN_TOO_LARGE.
 *   @retval TM_CONF_PORT_MIN_SHAP_NOT_INC_BW_MULT
 *   @retval TM_CONF_PORT_MAX_SHAP_NOT_INC_BW_MULT
 *   @retval TM_HW_PORT_CONFIG_FAIL if download to HW fails.
 */
int tm_create_logical_port (tm_handle hndl, uint8_t port_index,
							enum tm_port_bw port_speed,
							struct tm_port_params *params,
							uint16_t num_of_c_nodes,
							uint16_t num_of_b_nodes,
							uint16_t num_of_a_nodes,
							uint32_t num_of_queues,
							const char * port_name);

							/** Create Port with assymetric sub-tree and download its parameters to HW.
 *
 *   @param[in]     hndl            TM lib handle. 
 *   @param[in]     port_index      Port index.       
 *   @param[in]     port_speed      Port physical bandwidth.
 *   @param[in]     params          Port parameters structure pointer.
 *   @param[in]     port_name       string containing name to assign to new port.
 *
 *   @note: to indicate that no shaping is needed for port, set
 *   params->cir_bw to TM_INVAL, in this case other shaping
 *   parameters will not be considered.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -ENOMEM if memory allocation in RM fails.
 *   @retval -EACCES if number of children nodes is not valid.
 *   @retval TM_CONF_PORT_IND_OOR if port_index is out of range.
 *   @retval TM_CONF_PORT_IND_USED if port_index is already used.
 *   @retval TM_CONF_PORT_SPEED_OOR if port_speed is out of range.
 *
 *   @retval TM_CONF_P_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_PORT_BW_OUT_OF_SPEED.
 *   @retval TM_CONF_PORT_QUANTUM_OOR.
 *   @retval TM_CONF_PORT_DWRR_PRIO_OOR.
 *   @retval TM_CONF_INVALID_NUM_OF_C_NODES.
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is oor
 *
 *   @retval TM_CONF_PORT_BW_OOR
 *   @retval TM_CONF_PORT_BS_OOR
 *   @retval TM_CONF_PORT_MIN_TOKEN_TOO_LARGE.
 *   @retval TM_CONF_PORT_MAX_TOKEN_TOO_LARGE.
 *   @retval TM_CONF_PORT_MIN_SHAP_NOT_INC_BW_MULT
 *   @retval TM_CONF_PORT_MAX_SHAP_NOT_INC_BW_MULT
 *   @retval TM_HW_PORT_CONFIG_FAIL if download to HW fails.
 */
int tm_create_logical_asym_port(tm_handle hndl, uint8_t port_index,
								enum tm_port_bw port_speed,
								struct tm_port_params *params,
								const char * port_name);

/***************************************************************************
 * C-node Creation 
 ***************************************************************************/

/** Create path from C-node to Port and assign logical name to new C-node
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     port_index      Port index to connect queue.      
 *   @param[in]     c_params        C-node parameters structure pointer.
 *   @param[in]     node_name       string containing name to assign to new C-node
 *   @param[out]    c_node_index    C-node index pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -ENOBUFS if no free C-nodes under the Port can be added.
 *   @retval TM_CONF_PORT_IND_OOR if port_index is out of range.
 *   @retval TM_CONF_PORT_IND_NOT_EXIST if port_index is not in use.
 *
 *   @retval TM_CONF_C_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_C_PRIORITY_OOR.
 *   @retval TM_CONF_C_QUANTUM_OOR.
 *   @retval TM_CONF_C_DWRR_PRIO_OOR.
 *   @retval TM_CONF_B_WRED_PROF_REF_OOR.
 *
 *   @retval TM_CONF_INVALID_NUM_OF_B_NODES.
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is
 *   oor
 *
 *   @retval TM_HW_C_NODE_CONFIG_FAIL if C-node download to HW fails.
 */
int tm_create_logical_c_node_to_port (	tm_handle hndl, uint8_t port_index,
										struct tm_c_node_params *c_params,
										const char * node_name,
										uint32_t *c_node_index);

/** Create path from B-node to C-node, assign "persistent" logical name to created A-node.
 *  "logical" node stores it's parent regardless of reshuffling process, which can change it's physical address.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     c_node_index    C-node index to connect queue.      
 *   @param[in]     b_params        B-node parameters structure pointer.      
 *   @param[in]     node_name       string containing "persistent" logical name of created B-node.      
 *   @param[out]    b_node_index    B-node index pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -ENOBUFS if no free B-nodes under the C-node can be added.
 *   @retval TM_CONF_C_NODE_IND_OOR if c_node_index is out of range.
 *   @retval TM_CONF_C_NODE_IND_NOT_EXIST if c_node_index is not in use.
 *
 *   @retval TM_CONF_B_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_B_PRIORITY_OOR.
 *   @retval TM_CONF_B_QUANTUM_OOR.
 *   @retval TM_CONF_B_DWRR_PRIO_OOR.
 *   @retval TM_CONF_B_WRED_PROF_REF_OOR.
 *
 *   @retval TM_CONF_INVALID_NUM_OF_A_NODES.
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is oor
 *
 *   @retval TM_HW_B_NODE_CONFIG_FAIL if B-node download to HW fails.
 */
int tm_create_logical_b_node_to_c_node (	tm_handle hndl, uint32_t parent_index,
											struct tm_b_node_params *b_params,
											const char * node_name,
											uint32_t * node_index);
											
/** Create path from A-node to B-node, assign "persistent" logical name to created A-node.
 *  "logical" node stores it's parent regardless of reshuffling process, which can change it's physical address.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     b_node_index    B-node index to connect queue.      
 *   @param[in]     a_params        A-node parameters structure pointer.      
 *   @param[in]     node_name       string containing "persistent" logical name of created A-node.      
 *   @param[out]    a_node_index    A-node index pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -ENOBUFS if no free A-nodes under the B-node can be added.
 *   @retval TM_CONF_B_NODE_IND_OOR if b_node_index is out of range.
 *   @retval TM_CONF_B_NODE_IND_NOT_EXIST if b_node_index is not in use.
 *
 *   @retval TM_CONF_A_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_A_PRIORITY_OOR.
 *   @retval TM_CONF_A_QUANTUM_OOR.
 *   @retval TM_CONF_A_DWRR_PRIO_OOR.
 *   @retval TM_CONF_A_WRED_PROF_REF_OOR.
 *
 *   @retval TM_CONF_INVALID_NUM_OF_QUEUES.
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is oor
 *
 *   @retval TM_HW_A_NODE_CONFIG_FAIL if A-node download to HW fails.
 */
int tm_create_logical_a_node_to_b_node (	tm_handle hndl, uint32_t parent_index,
											struct tm_a_node_params *a_params,
											const char * node_name,
											uint32_t * node_index);
											
/** Create path from Queue to A-node.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     a_node_index    A-node index to connect queue.      
 *   @param[in]     q_params        Queue parameters structure pointer.        
 *   @param[in]     node_name       string containing logical name of created Queue.        
 *   @param[out]    queue_index     Queue index pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -ENOBUFS if no free Queues under the A-node can be added.
 *   @retval TM_CONF_A_NODE_IND_OOR if a_node_index is out of range.
 *   @retval TM_CONF_A_NODE_IND_NOT_EXIST if a_node_index is not in use.
 *
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is oor
 *
 *   @retval TM_CONF_Q_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_Q_PRIORITY_OOR.
 *   @retval TM_CONF_Q_QUANTUM_OOR.
 *   @retval TM_CONF_Q_WRED_PROF_REF_OOR.
 *
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if queue download to HW fails.
 */int tm_create_logical_queue_to_a_node (	tm_handle hndl, uint32_t parent_index,
											struct tm_queue_params *q_params,
											const char * node_name,
											uint32_t * queue_index);


#endif /* _TM_LOGICAL_LAYER_INTERFACE_H_ */
