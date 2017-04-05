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
 * @brief TM nodes creation interface.
 *
 * @file tm_nodes_create.h
 *
 * $Revision: 2.0 $
 */

#ifndef   	TM_NODES_CREATE_H
#define   	TM_NODES_CREATE_H

#include "tm_defs.h"


/***************************************************************************
 * Port Creation 
 ***************************************************************************/

/** Create Port and download its parameters to HW.
 *
 *   @param[in]     hndl            TM lib handle. 
 *   @param[in]     port_index      Port index.       
 *   @param[in]     port_speed      Port physical bandwidth.
 *   @param[in]     params          Port parameters structure pointer.
 *   @param[in]     num_of_c_nodes  Number of C-nodes under port.
 *   @param[in]     num_of_b_nodes  Number of B-nodes under port.
 *   @param[in]     num_of_a_nodes  Number of A-nodes under port.
 *   @param[in]     num_of_queues   Number of Queues under port.

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
 *   @retval TM_BW_OUT_OF_RANGE  if  failed to configure desired b/w
 *   @retval * TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE	desired cbs or ebs value is too small for desired cir/eir
 *   @retval TM_BW_UNDERFLOW - if cir & eir are too different to configure together
 *   @retval TM_HW_PORT_CONFIG_FAIL if download to HW fails.
 */
int tm_create_port (tm_handle hndl, uint8_t port_index,
                    enum tm_port_bw port_speed,
                    struct tm_port_params *params,
                    uint16_t num_of_c_nodes,
                    uint16_t num_of_b_nodes,
                    uint16_t num_of_a_nodes,
                    uint32_t num_of_queues);


/** Create Port with assymetric sub-tree and download its parameters to HW.
 *
 *   @param[in]     hndl            TM lib handle. 
 *   @param[in]     port_index      Port index.       
 *   @param[in]     port_speed      Port physical bandwidth.
 *   @param[in]     params          Port parameters structure pointer.
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
 *
 *   @retval TM_CONF_P_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_PORT_QUANTUM_OOR.
 *   @retval TM_CONF_PORT_DWRR_PRIO_OOR.
 *   @retval TM_CONF_INVALID_NUM_OF_C_NODES.
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is oor
 *
 *   @retval TM_CONF_PORT_BS_OOR   if cbs or ebs is greater than H/W limit    
 *   @retval TM_BW_OUT_OF_RANGE  if  failed to configure desired b/w
 *   @retval TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE	desired cbs or ebs value is too small for desired cir/eir
 *   @retval TM_BW_UNDERFLOW - if cir & eir are too different to configure together
 *   @retval TM_HW_PORT_CONFIG_FAIL if download to HW fails.
 */
int tm_create_asym_port(tm_handle hndl, uint8_t port_index,
                        enum tm_port_bw port_speed,
                        struct tm_port_params *params);


/** Configure Port's Drop per Cos and download its parameters to HW.
 *
 *   @param[in]     hndl            TM lib handle. 
 *   @param[in]     port_index      Port index.       
 *   @param[in]     params          Port Drop parameters per Cos structure pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval TM_CONF_PORT_IND_OOR if port_index is out of range.
 *   
 *   @retval TM_CONF_P_WRED_PROF_REF_OOR.
 *
 *   @retval TM_HW_PORT_CONFIG_FAIL if download to HW fails.
 */
int tm_config_port_drop_per_cos(tm_handle hndl, uint8_t port_index,
                   struct tm_port_drop_per_cos *params);


/***************************************************************************
 * Queue Creation 
 ***************************************************************************/

/** Create path from Queue to Port.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node. 
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     port_index      Port index to connect queue.      
 *   @param[in]     q_params        Queue parameters structure pointer.        
 *   @param[in]     a_params        A-node parameters structure pointer.      
 *   @param[in]     b_params        B-node parameters structure pointer.      
 *   @param[in]     c_params        C-node parameters structure pointer.
 *   @param[out]    queue_index     Queue index pointer.
 *   @param[out]    a_node_index    A-node index pointer.
 *   @param[out]    b_node_index    B-node index pointer.
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
 *   @retval TM_CONF_Q_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_Q_PRIORITY_OOR.
 *   @retval TM_CONF_Q_QUANTUM_OOR.
 *   @retval TM_CONF_Q_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_A_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_A_PRIORITY_OOR.
 *   @retval TM_CONF_A_QUANTUM_OOR.
 *   @retval TM_CONF_A_DWRR_PRIO_OOR.
 *   @retval TM_CONF_A_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_B_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_B_PRIORITY_OOR.
 *   @retval TM_CONF_B_QUANTUM_OOR.
 *   @retval TM_CONF_B_DWRR_PRIO_OOR.
 *   @retval TM_CONF_B_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_C_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_C_PRIORITY_OOR.
 *   @retval TM_CONF_C_QUANTUM_OOR.
 *   @retval TM_CONF_C_DWRR_PRIO_OOR.
 *   @retval TM_CONF_C_WRED_PROF_REF_OOR.
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if queue download to HW fails.
 *   @retval TM_HW_A_NODE_CONFIG_FAIL if A-node download to HW fails.
 *   @retval TM_HW_B_NODE_CONFIG_FAIL if B-node download to HW fails.
 *   @retval TM_HW_C_NODE_CONFIG_FAIL if C-node download to HW fails.
 */
int tm_create_queue_to_port (tm_handle hndl, uint8_t port_index,
                             struct tm_queue_params *q_params,
                             struct tm_a_node_params *a_params,
                             struct tm_b_node_params *b_params,
                             struct tm_c_node_params *c_params,
                             uint32_t *queue_index,
                             uint32_t *a_node_index,
                             uint32_t *b_node_index,
                             uint32_t *c_node_index);


/** Create transparent path from Queue to Port.
 *
 *  @note This API introduces 'transparent' concept to TM nodes
 *  structure that deals with Queues and Ports only. In this case no
 *  configuration is needed for A,B,C level nodes, this nodes
 *  are created automatically (one C-node, one B-node and one
 *  A-node) and they are 'transparent' from the system point of
 *  view. Transparent path can be created under symmetric port
 *  only. To delete Queue from the structure - use
 *  'tm_delete_node' API with level Q_LEVEL. To update queue
 *  parameters - use 'tm_update_queue' API. To delete Port from
 *  the structure use 'tm_delete_trans_port' API. Applying any
 *  other APIs on nodes underlying the port can cause unexpected
 *  behavior of the system.
 *
 *  @note field shaping_profile_ref in q_params may be set
 *  to TM_INF_PROFILE if no shaping required for a queue.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     port_index      Port index to connect queue.      
 *   @param[in]     q_params        Queue parameters structure pointer.        
 *   @param[out]    queue_index     Queue index pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if port is assymetric.
 *   @retval -ENOBUFS if no free queues under the Port can be added.
 *   @retval TM_CONF_PORT_IND_OOR if port_index is out of range.
 *   @retval TM_CONF_PORT_IND_NOT_EXIST if port_index is not in use.
 *
 *   @retval TM_CONF_Q_SHAPING_PROF_REF_OOR.
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is oor
 *   @retval TM_CONF_Q_QUANTUM_OOR.
 *   @retval TM_CONF_Q_WRED_PROF_REF_OOR.
 *
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if queue download to HW fails.
 *   @retval TM_HW_A_NODE_CONFIG_FAIL if A-node download to HW fails.
 *   @retval TM_HW_B_NODE_CONFIG_FAIL if B-node download to HW fails.
 *   @retval TM_HW_C_NODE_CONFIG_FAIL if C-node download to HW fails.
 */
int tm_create_trans_queue_to_port (tm_handle hndl, uint8_t port_index,
                                   struct tm_queue_params *q_params,
                                   uint32_t *queue_index);


/** Create path from Queue to C-node.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     c_node_index    C-node index to connect queue.      
 *   @param[in]     q_params        Queue parameters structure pointer.        
 *   @param[in]     a_params        A-node parameters structure pointer.      
 *   @param[in]     b_params        B-node parameters structure pointer.      
 *   @param[out]    queue_index     Queue index pointer.
 *   @param[out]    a_node_index    A-node index pointer.
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
 *   @retval TM_CONF_Q_SHAPING_PROF_REF_OOR.
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_CONF_Q_PRIORITY_OOR.
 *   @retval TM_CONF_Q_QUANTUM_OOR.
 *   @retval TM_CONF_Q_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_A_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_A_PRIORITY_OOR.
 *   @retval TM_CONF_A_QUANTUM_OOR.
 *   @retval TM_CONF_A_DWRR_PRIO_OOR.
 *   @retval TM_CONF_A_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_B_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_B_PRIORITY_OOR.
 *   @retval TM_CONF_B_QUANTUM_OOR.
 *   @retval TM_CONF_B_DWRR_PRIO_OOR.
 *   @retval TM_CONF_B_WRED_PROF_REF_OOR.
 *
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if queue download to HW fails.
 *   @retval TM_HW_A_NODE_CONFIG_FAIL if A-node download to HW fails.
 *   @retval TM_HW_B_NODE_CONFIG_FAIL if B-node download to HW fails.
 */
int tm_create_queue_to_c_node (tm_handle hndl, uint32_t c_node_index,
                               struct tm_queue_params *q_params,
                               struct tm_a_node_params *a_params,
                               struct tm_b_node_params *b_params,
                               uint32_t *queue_index,
                               uint32_t *a_node_index,
                               uint32_t *b_node_index);


/** Create path from Queue to B-node.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     b_node_index    B-node index to connect queue.      
 *   @param[in]     q_params        Queue parameters structure pointer.        
 *   @param[in]     a_params        A-node parameters structure pointer.      
 *   @param[out]    queue_index     Queue index pointer.
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
 *   @retval TM_CONF_Q_SHAPING_PROF_REF_OOR.
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_CONF_Q_PRIORITY_OOR.
 *   @retval TM_CONF_Q_QUANTUM_OOR.
 *   @retval TM_CONF_Q_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_A_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_A_PRIORITY_OOR.
 *   @retval TM_CONF_A_QUANTUM_OOR.
 *   @retval TM_CONF_A_DWRR_PRIO_OOR.
 *   @retval TM_CONF_A_WRED_PROF_REF_OOR.
 *
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if queue download to HW fails.
 *   @retval TM_HW_A_NODE_CONFIG_FAIL if A-node download to HW fails.
 */
int tm_create_queue_to_b_node (tm_handle hndl, uint32_t b_node_index,
                               struct tm_queue_params *q_params,
                               struct tm_a_node_params *a_params,
                               uint32_t *queue_index,
                               uint32_t *a_node_index);


/** Create path from Queue to A-node.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     a_node_index    A-node index to connect queue.      
 *   @param[in]     q_params        Queue parameters structure pointer.        
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
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_CONF_Q_PRIORITY_OOR.
 *   @retval TM_CONF_Q_QUANTUM_OOR.
 *   @retval TM_CONF_Q_WRED_PROF_REF_OOR.
 *
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if queue download to HW fails.
 */
int tm_create_queue_to_a_node (tm_handle hndl, uint32_t a_node_index,
                               struct tm_queue_params *q_params,
                               uint32_t *queue_index);

/** Init Queue belonging to to A-node.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     a_node_index    A-node index to connect queue.      
 *   @param[in]     q_params        Queue parameters structure pointer.        
 *   @param[in]     queue_index     index of Queue to init .
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
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_CONF_Q_PRIORITY_OOR.
 *   @retval TM_CONF_Q_QUANTUM_OOR.
 *   @retval TM_CONF_Q_WRED_PROF_REF_OOR.
 *
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if queue download to HW fails.
 */

int tm_init_selected_queue_of_a_node(tm_handle hndl, uint32_t parent_a_node_index,
                              struct tm_queue_params *q_params,
                              uint32_t queue_index);
/***************************************************************************
 * A-node Creation 
 ***************************************************************************/

/** Create path from A-node to Port.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     port_index      Port index to connect queue.      
 *   @param[in]     a_params        A-node parameters structure pointer.      
 *   @param[in]     b_params        B-node parameters structure pointer.      
 *   @param[in]     c_params        C-node parameters structure pointer.
 *   @param[out]    a_node_index    A-node index pointer.
 *   @param[out]    b_node_index    B-node index pointer.
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
 *   @retval TM_CONF_A_SHAPING_PROF_REF_OOR.
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_CONF_A_PRIORITY_OOR.
 *   @retval TM_CONF_A_QUANTUM_OOR.
 *   @retval TM_CONF_A_DWRR_PRIO_OOR.
 *   @retval TM_CONF_A_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_B_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_B_PRIORITY_OOR.
 *   @retval TM_CONF_B_QUANTUM_OOR.
 *   @retval TM_CONF_B_DWRR_PRIO_OOR.
 *   @retval TM_CONF_B_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_C_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_C_PRIORITY_OOR.
 *   @retval TM_CONF_C_QUANTUM_OOR.
 *   @retval TM_CONF_C_DWRR_PRIO_OOR.
 *   @retval TM_CONF_C_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_INVALID_NUM_OF_QUEUES.
 *
 *   @retval TM_HW_A_NODE_CONFIG_FAIL if A-node download to HW fails.
 *   @retval TM_HW_B_NODE_CONFIG_FAIL if B-node download to HW fails.
 *   @retval TM_HW_C_NODE_CONFIG_FAIL if C-node download to HW fails.
 */
int tm_create_a_node_to_port (tm_handle hndl, uint8_t port_index,
                             struct tm_a_node_params *a_params,
                             struct tm_b_node_params *b_params,
                             struct tm_c_node_params *c_params,
                             uint32_t *a_node_index,
                             uint32_t *b_node_index,
                             uint32_t *c_node_index);


/** Create path from A-node to C-node.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     c_node_index    C-node index to connect queue.      
 *   @param[in]     a_params        A-node parameters structure pointer.      
 *   @param[in]     b_params        B-node parameters structure pointer.      
 *   @param[out]    a_node_index    A-node index pointer.
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
 *   @retval TM_CONF_A_SHAPING_PROF_REF_OOR.
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_CONF_A_PRIORITY_OOR.
 *   @retval TM_CONF_A_QUANTUM_OOR.
 *   @retval TM_CONF_A_DWRR_PRIO_OOR.
 *   @retval TM_CONF_A_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_B_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_B_PRIORITY_OOR.
 *   @retval TM_CONF_B_QUANTUM_OOR.
 *   @retval TM_CONF_B_DWRR_PRIO_OOR.
 *   @retval TM_CONF_B_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_INVALID_NUM_OF_QUEUES.
 *
 *   @retval TM_HW_A_NODE_CONFIG_FAIL if A-node download to HW fails.
 *   @retval TM_HW_B_NODE_CONFIG_FAIL if B-node download to HW fails.
 */
int tm_create_a_node_to_c_node (tm_handle hndl, uint32_t c_node_index,
                                struct tm_a_node_params *a_params,
                                struct tm_b_node_params *b_params,
                                uint32_t *a_node_index,
                                uint32_t *b_node_index);


/** Create path from A-node to B-node.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     b_node_index    B-node index to connect queue.      
 *   @param[in]     a_params        A-node parameters structure pointer.      
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
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
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
int tm_create_a_node_to_b_node (tm_handle hndl, uint32_t b_node_index,
                                struct tm_a_node_params *a_params,
                                uint32_t *a_node_index);


/** Create path from A-node to B-node with predefined  queue range.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *  @note   Not applicable for creation of  symmetric port tree.
 *
 *   @param[in]     hndl					TM lib handle.
 *   @param[in]     b_node_index			B-node index to connect queue.      
 *   @param[in]     a_params				A-node parameters structure pointer.
 *   @param[in]     first_queue_in_range    index of first queue in range.
 *   @param[in]     queue_range_size        size of allocated queues range.
 *
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
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
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
int tm_create_a_node_with_queue_range_to_b_node(tm_handle hndl, uint32_t parent_b_node_index,
												struct tm_a_node_params *a_params,
												uint32_t first_queue_in_range, uint32_t queue_range_size,
												uint32_t *a_node_index_ptr);


/***************************************************************************
 * B-node Creation 
 ***************************************************************************/

/** Create path from B-node to Port.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     port_index      Port index to connect queue.      
 *   @param[in]     b_params        B-node parameters structure pointer.      
 *   @param[in]     c_params        C-node parameters structure pointer.
 *   @param[out]    b_node_index    B-node index pointer.
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
 *   @retval TM_CONF_B_SHAPING_PROF_REF_OOR.
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_CONF_B_PRIORITY_OOR.
 *   @retval TM_CONF_B_QUANTUM_OOR.
 *   @retval TM_CONF_B_DWRR_PRIO_OOR.
 *   @retval TM_CONF_B_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_C_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_C_PRIORITY_OOR.
 *   @retval TM_CONF_C_QUANTUM_OOR.
 *   @retval TM_CONF_C_DWRR_PRIO_OOR.
 *   @retval TM_CONF_C_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_INVALID_NUM_OF_A_NODES.
 *
 *   @retval TM_HW_B_NODE_CONFIG_FAIL if B-node download to HW fails.
 *   @retval TM_HW_C_NODE_CONFIG_FAIL if C-node download to HW fails.
 */
int tm_create_b_node_to_port (tm_handle hndl, uint8_t port_index,
                             struct tm_b_node_params *b_params,
                             struct tm_c_node_params *c_params,
                             uint32_t *b_node_index,
                             uint32_t *c_node_index);


/** Create path from B-node to C-node.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     c_node_index    C-node index to connect queue.      
 *   @param[in]     b_params        B-node parameters structure pointer.      
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
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
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
int tm_create_b_node_to_c_node (tm_handle hndl, uint32_t c_node_index,
                                struct tm_b_node_params *b_params,
                                uint32_t *b_node_index);


/***************************************************************************
 * C-node Creation 
 ***************************************************************************/

/** Create path from C-node to Port.
 *
 *  @note field shaping_profile_ref in parameters may get
 *  TM_INF_PROFILE value when there is no shaping to the queue/node.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     port_index      Port index to connect queue.      
 *   @param[in]     c_params        C-node parameters structure pointer.
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
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
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
int tm_create_c_node_to_port (tm_handle hndl, uint8_t port_index,
                             struct tm_c_node_params *c_params,
                             uint32_t *c_node_index);

/**
 * Reallocate all A nodes under a B node so that more A nodes can be
 * allocated.
 */
int tm_reallocate_a_nodes(tm_handle hndl,
                          uint32_t b_node_ind,
                          uint32_t num_of_children,
                          uint32_t *a_node_ind);

#endif 	 /* TM_NODES_CREATE_H */

