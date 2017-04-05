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
 * @brief TM nodes control interface.
 *
 * @file tm_nodes_ctl.h
 *
 * $Revision: 2.0 $
 */

#ifndef TM_NODES_CTL_H
#define TM_NODES_CTL_H

#include "tm_defs.h"


/**  Install queue by a given index.
 *
 *   @brief Makes a queue capable of getting traffic.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     queue_index     Queue index.             
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if queue_index is out of range.
 *   @retval -ENODATA if queue_index is not in use.
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if queue installation fails.
 */
int tm_install_queue(tm_handle hndl, uint32_t queue_index);


/**  Uninstall queue by a given index.
 *
 *   @brief Makes a queue incapable of getting traffic.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     queue_index     Queue index.             
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if queue_index is out of range.
 *   @retval -ENODATA if queue_index is not in use.
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if queue uninstallation fails.
 */
int tm_uninstall_queue(tm_handle hndl, uint32_t queue_index);

/**  Retrieve install status of queue by a given index.
 *
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     queue_index     Queue index.
 *   @param[out]    status          (pointer to)  install status of the Queue.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if queue_index is out of range.
 *   @retval -ENODATA if queue_index is not in use.
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if queue uninstallation fails.
 */

int tm_queue_install_status(tm_handle hndl, uint32_t queue_index, uint8_t * status);


/**  Flush queue by a given index.
 *  
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     index           Queue index.
 *   @param[in]     block_en        Blocking/Non-blocking API.
 *  
 *   @note All packets dequeued from this queue are dropped.
 *         If block_en=TM_ENABLE, wait till flush queue has
 *         completed.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if index/block_en is out of range.
 *   @retval -ENODATA if index is not in use.
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if queue flush fails.
 */
int tm_flush_queue(tm_handle hndl, uint32_t index, uint8_t block_en);


/**  Get Flush queue status by a given index.
 *  
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     index           Queue index.
 *   @param[out]    status          Pointer to Flush Status.
 *  
 *   @note status = TM_ENABLE - flush is active (packets are
 *         dropped on dequeue). status = TM_DISABLE - flush off
 *         / completed.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if index/block_en is out of range.
 *   @retval -ENODATA if index is not in use.
 *   @retval TM_HW_READ_QUEUE_STATUS_FAIL if queue flush read
 *           fails.
 */
int tm_flush_queue_status(tm_handle hndl, uint32_t index, uint8_t *status);


/**  Flush port (and all its queues) by a given index.
 *  
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     index           Port index.
 *   @param[in]     block_en        Blocking/Non-blocking API.
 *  
 *   @note All packets dequeued from this port (and all its
 *         queues) are dropped. If block_en=TM_ENABLE, wait till
 *         flush port has completed.
 *  
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if index/block_en is out of range.
 *   @retval -ENODATA if index is not in use.
 *   @retval TM_HW_PORT_CONFIG_FAIL if port flush fails.
 */
int tm_flush_port(tm_handle hndl, uint32_t index, uint8_t block_en);


/**  Get Flush port status by a given index.
 *  
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     index           Port index.
 *   @param[out]    status          Pointer to Flush Status.
 *  
 *   @note status = TM_ENABLE - flush is active (packets are
 *         dropped on dequeue). status = TM_DISABLE - flush off
 *         / completed.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if index/block_en is out of range.
 *   @retval -ENODATA if index is not in use.
 *   @retval TM_HW_READ_PORT_STATUS_FAIL if port flush read
 *           fails.
 */
int tm_flush_port_status(tm_handle hndl, uint32_t index, uint8_t *status);


/**  Delete node from scheduling hierarchy.
 *
 *   @param[in]     hndl        TM lib handle.
 *   @param[in]     level       Scheduling level: Queue/A/B/C-node/Port.
 *   @param[in]     node_id     Node index.                 
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -ERANGE if level is out of range.
 *   @retval -EFAULT if node index is out of range.
 *   @retval -ENODATA if node is not in use (free).
 *   @retval -EBUSY if any of children is still in use.
 *
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_A_NODE_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_B_NODE_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_C_NODE_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_PORT_CONFIG_FAIL if download to HW fails.
 */
int tm_delete_node(tm_handle hndl, enum tm_level level, uint32_t node_id);


/**  Delete port and all its subtree from scheduling hierarchy.
 *
 *   @param[in]     hndl        TM lib handle.
 *   @param[in]     index       Port index.                 
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if index is out of range.
 *
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_A_NODE_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_B_NODE_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_C_NODE_CONFIG_FAIL if download to HW fails.
 */
int tm_delete_trans_port(tm_handle hndl, uint8_t index);


#ifdef LOGICAL_LAYER
/*
* low level reshuffling functions are not necessary 
*/
#else

	/***************************************************************************
	 * Reshuffling 
	 ***************************************************************************/

	/** Read next tree index/range change after reshuffling.
	 *
	 *   @param[in]     hndl            TM lib handle.
	 *   @param[out]    change          Change structure pointer.
	 *
	 *   @return an integer return code.
	 *   @retval zero on success.
	 *   @retval -@EINVAL if hndl is NULL.
	 *   @retval -@EBADF if hndl is invalid.
	 *   @retval -@ENOBUFS if list is empty.
	 */
	int tm_nodes_read_next_change(tm_handle hndl, struct tm_tree_change *change);


	/** Empty list of reshuffling changes.
	 *
	 *   @param[in]     hndl            TM lib handle.
	 *
	 *   @return an integer return code.
	 *   @retval zero on success.
	 *   @retval -@EINVAL if hndl is NULL.
	 *   @retval -@EBADF if hndl is invalid.
	 */
	int tm_clean_list(tm_handle hndl);
#endif

#endif   /* TM_NODES_CTL_H */

