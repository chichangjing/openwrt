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
 * @brief TM nodes status reading interface.
 *
 * @file tm_nodes_status.h
 *
 * $Revision: 2.0 $
 */

#ifndef TM_NODES_STATUS_H
#define TM_NODES_STATUS_H

#include "tm_defs.h"


/**  Read Port status.
 *
 *   @param[in]     hndl        TM lib handle.
 *   @param[in]     port_id     Port index.
 *   @param[out]    status      Port status structure pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if port_id is out of range.
 *   @retval -ENODATA if port_id is not in use.
 *   @retval TM_HW_READ_PORT_STATUS_FAIL if read from HW fails.
 */
int tm_read_port_status(tm_handle hndl, uint8_t port_id,
                        struct tm_port_status *status);


/**  Read C-node status.
 *
 *   @param[in]     hndl        TM lib handle.
 *   @param[in]     c_node_ind  C-node index.
 *   @param[out]    status      Node status structure pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if c_node_ind is out of range.
 *   @retval -ENODATA if c_node_ind is not in use.
 *   @retval TM_HW_READ_C_NODE_STATUS_FAIL if read from HW fails.
 */
int tm_read_c_node_status(tm_handle hndl, uint32_t c_node_ind,
                          struct tm_node_status *status);


/**  Read B-node status.
 *
 *   @param[in]     hndl        TM lib handle.
 *   @param[in]     b_node_ind  B-node ind.
 *   @param[out]    status      Node status structure pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if b_node_ind is out of range.
 *   @retval -ENODATA if b_node_ind is not in use.
 *   @retval TM_HW_READ_B_NODE_STATUS_FAIL if read from HW fails.
 */
int tm_read_b_node_status(tm_handle hndl, uint32_t b_node_ind,
                          struct tm_node_status *status);


/**  Read A-node status.
 *
 *   @param[in]     hndl        TM lib handle.
 *   @param[in]     a_node_ind  A-node ind.
 *   @param[out]    status      Node status structure pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if a_node_ind is out of range.
 *   @retval -ENODATA if a_node_ind is not in use.
 *   @retval TM_HW_READ_A_NODE_STATUS_FAIL if read from HW fails.
 */
int tm_read_a_node_status(tm_handle hndl, uint32_t a_node_ind,
                          struct tm_node_status *status);


/**  Read Queue status.
 *
 *   @param[in]     hndl        TM lib handle.
 *   @param[in]     queue_ind   Queue index.
 *   @param[out]    status      Node status structure pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if queue_ind is out of range.
 *   @retval -ENODATA if queue_ind is not in use.
 *   @retval TM_HW_READ_Q_STATUS_FAIL if read from HW fails.
 */
int tm_read_queue_status(tm_handle hndl, uint32_t queue_ind,
                         struct tm_node_status *status);


/**  Read Queue drop length.
 *
 *   @param[in]     hndl                TM lib handle.
 *   @param[in]     lvl                 TM level.
 *   @param[in]     index               Port/node/queue index.
 *   @param[out]    inst_queue_length   Pointer to current Queue length.
 *   @param[out]    av_queue_length     Pointer to average Queue length.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -ERANGE if level is out of range.
 *   @retval -EFAULT if index is out of range.
 *   @retval -ENODATA if index is not in use.
 *   @retval TM_HW_GET_QUEUE_LENGTH_FAIL if read from HW fails.
 */
int tm_drop_get_queue_length(tm_handle hndl,
                             enum tm_level lvl,
                             uint32_t index,
                             uint32_t *inst_queue_length,
                             uint32_t *av_queue_length);


#endif   /* TM_NODES_STATUS_H */

