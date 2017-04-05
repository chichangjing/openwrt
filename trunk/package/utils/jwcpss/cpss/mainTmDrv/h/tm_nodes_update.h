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
 * @brief TM nodes update interface.
 *
 * @file tm_nodes_update.h
 *
 * $Revision: 2.0 $
 */

#ifndef TM_NODES_UPDATE_H
#define TM_NODES_UPDATE_H

#include "tm_defs.h"

/***************************************************************************
 * Parameters Update 
 ***************************************************************************/

/** Update node eligibility function (pointer).
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     level           The TM level the node is at.
 *   @param[in]     index           Index of node to be updated.
 *   @param[in]     elig_func       Index of eligibility function.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -@EINVAL if hndl is NULL.
 *   @retval -@EBADF if hndl is invalid.
 *   @retval -@EADDRNOTAVAIL no such level
 */
int tm_update_elig_fun(tm_handle hndl, uint8_t level, uint32_t index, uint8_t elig_func);

/** Update queue parameters.
 *
 *   @brief when error occurs, the entry is considered inconsistent.
 * 
 *   @note wred_profile_ref parameter will be updated in any case,
 *   set it's value to be the same as in DB if you don't want to change it.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     queue_index     Queue index to be updated.
 *   @param[in]     q_params        Queue parameters structure pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if queue_index is out of range.
 *   @retval -ENODATA if queue_index is not in use.
 *
 *   @retval TM_CONF_Q_SHAPING_PROF_REF_OOR.
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_CONF_Q_PRIORITY_OOR.
 *   @retval TM_CONF_Q_QUANTUM_OOR.
 *   @retval TM_CONF_Q_WRED_PROF_REF_OOR.
 *
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is oor
 *   @retval TM_HW_ELIG_PRIO_FUNC_FAILED if queue eligible function
 *   pointer download to HW failed .
 *
 *   @retval TM_HW_QUEUE_CONFIG_FAIL if download to HW fails..
 */
int tm_update_queue(tm_handle hndl, uint32_t queue_index,
                    struct tm_queue_params *q_params);


/** Update A-node parameters.
 *
 *   @brief when error occurs, the entry is considered inconsistent.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     a_node_index    A-node index to be updated.
 *   @param[in]     a_params        A-node parameters structure pointer.
 *
 *   @note 'num_of_children' cann't be updated.
 *   @note wred_profile_ref parameter will be updated in any case,
 *   set it's value to be the same as in DB if you don't want to change it.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if a_node_index is out of range.
 *   @retval -ENODATA if a_node_index is not in use.
 *
 *   @retval TM_CONF_A_SHAPING_PROF_REF_OOR.
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_CONF_A_PRIORITY_OOR.
 *   @retval TM_CONF_A_QUANTUM_OOR.
 *   @retval TM_CONF_A_DWRR_PRIO_OOR.
 *   @retval TM_CONF_A_WRED_PROF_REF_OOR.
 *
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is oor
 *   @retval TM_HW_A_NODE_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_ELIG_PRIO_FUNC_FAILED if A node eligible function
 *   pointer download to HW failed .
 */
int tm_update_a_node(tm_handle hndl, uint32_t a_node_index,
                     struct tm_a_node_params *a_params);


/** Update B-node parameters.
 *
 *   @brief when error occurs, the entry is considered inconsistent.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     b_node_index    B-node index to be updated.
 *   @param[in]     b_params        B-node parameters structure pointer.
 *
 *   @note 'num_of_children' cann't be updated.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if b_node_index is out of range.
 *   @retval -ENODATA if b_node_index is not in use.
 *
 *   @retval TM_CONF_B_SHAPING_PROF_REF_OOR.
 *   @retval TM_CONF_B_PRIORITY_OOR.
 *   @retval TM_CONF_B_QUANTUM_OOR.
 *   @retval TM_CONF_B_DWRR_PRIO_OOR.
 *   @retvak TM_CONF_B_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is oor
 *
 *   @retval TM_HW_B_NODE_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_ELIG_PRIO_FUNC_FAILED if B node eligible function
 *   pointer download to HW failed .
 */
int tm_update_b_node(tm_handle hndl, uint32_t b_node_index,
                     struct tm_b_node_params *b_params);


/** Update C-node parameters.
 *
 *   @brief when error occurs, the entry is considered inconsistent.
 *   @note  wred_cos can only be updated together with wred_profile_ref.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     c_node_index    C-node index to be updated.
 *   @param[in]     c_params        C-node parameters structure pointer.
 *
 *   @note 'num_of_children' cann't be updated.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if c_node_index is out of range.
 *   @retval -ENODATA if c_node_index is not in use.
 *
 *   @retval TM_CONF_C_SHAPING_PROF_REF_OOR.
 * 	 @retval TM_WRONG_SHP_PROFILE_LEVEL.
 *   @retval TM_CONF_C_PRIORITY_OOR.
 *   @retval TM_CONF_C_QUANTUM_OOR.
 *   @retval TM_CONF_C_DWRR_PRIO_OOR.
 *   @retval TM_CONF_C_WRED_PROF_REF_OOR.
 *   @retval TM_CONF_C_WRED_COS_OOR.
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is oor
 *
 *   @retval TM_HW_C_NODE_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_ELIG_PRIO_FUNC_FAILED if C node eligible function
 *   pointer download to HW failed .
 */
int tm_update_c_node(tm_handle hndl, uint32_t c_node_index,
                     struct tm_c_node_params *c_params);


/** Update Port Shaping parameters.
 *
 *   @brief when error occurs, the entry is considered inconsistent.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     port_id         Port index to be updated.
 *   @param[in]     cir_bw          Port CIR bandwidth.
 *   @param[in]     eir_bw          Port EIR bandwidth.
 *   @param[in]     cbs             Port CIR busrt size.
 *   @param[in]     ebs             Port EIR burst size.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if port_index is out of range.
 *   @retval -ENODATA if port_index is not in use.
 *  
 *   @retval TM_CONF_PORT_SPEED_OOR.
 *   @retval TM_CONF_PORT_BW_OUT_OF_SPEED.
 *   @retval TM_CONF_PORT_BS_OOR.
 *   @retval TM_BW_OUT_OF_RANGE  if  failed to configure desired b/w
 *   @retval * TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE	desired cbs or ebs value is too small for desired cir/eir
 *   @retval TM_BW_UNDERFLOW - if cir & eir are too different to configure together
 *
 *   @retval TM_HW_PORT_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_ELIG_PRIO_FUNC_FAILED if port eligible function
 *   pointer download to HW failed .
 */
int tm_update_port_shaping(tm_handle hndl, uint8_t port_id,
                           uint32_t cir_bw,
                           uint32_t eir_bw,
                           uint32_t cbs,
                           uint32_t ebs);


/** Update Port Scheduling parameters.
 *
 *   @brief when error occurs, the entry is considered inconsistent.
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     port_id         Port index to be updated.
 *   @param[in]     elig_prio_func  Eligible Priority Function pointer.
 *   @param[in]     quantum         Port quantum 8 cells array.
 *   @param[in]     dwrr_priority   Port dwrr priority pointer for C-level.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if port_index is out of range.
 *   @retval -ENODATA if port_index is not in use.
 *
 *   @retval TM_CONF_PORT_PRIORITY_OOR.
 *   @retval TM_CONF_PORT_QUANTUM_OOR.
 *   @retval TM_CONF_PORT_DWRR_PRIO_OOR.
 *   @retval TM_CONF_ELIG_PRIO_FUNC_ID_OOR if eligible function id is oor
 *
 *   @retval TM_HW_ELIG_PRIO_FUNC_FAILED if port eligible function
 *   pointer download to HW failed .
 *   @retval TM_HW_PORT_CONFIG_FAIL if download to HW fails.
 */
int tm_update_port_scheduling(tm_handle hndl, uint8_t port_id,
                              uint8_t elig_prio_func_ptr,
                              uint16_t *quantum,
                              uint8_t *dwrr_priority);


/** Update Port Drop parameters.
 *
 *   @brief when error occurs, the entry is considered inconsistent.
 *
 *   @param[in]     hndl                TM lib handle.
 *   @param[in]     port_index          Port index to be updated.
 *   @param[in]     wred_profile_ref    Port Drop Profile reference.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if port_index is out of range.
 *   @retval -ENODATA if port_index is not in use.
 *
 *   @retval TM_CONF_P_WRED_PROF_REF_OOR.
 *
 *   @retval TM_HW_PORT_CONFIG_FAIL if download to HW fails.
 */
int tm_update_port_drop(tm_handle hndl,
                        uint8_t port_index,
                        uint8_t wred_profile_ref);

/** Update Port Drop parameters per Cos.
 *
 *   @brief when error occurs, the entry is considered inconsistent.
 *
 *   @param[in]     hndl                TM lib handle.
 *   @param[in]     port_index          Port index to be updated.
 *   @param[in]     params              Port Drop parameters structure per Cos.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if port_index is out of range.
 *   @retval -ENODATA if port_index is not in use.
 *
 *   @retval TM_CONF_P_WRED_PROF_REF_OOR.
 *   @retval TM_HW_PORT_CONFIG_FAIL if download to HW fails.
 */
int tm_update_port_drop_cos(tm_handle hndl,
                            uint8_t index,
                            struct tm_port_drop_per_cos *params);




/** Validate Port Shaping configuration
 *
 *
 *   @param[in]     hndl            TM lib handle.
 *   @param[in]     port_type       Port B/W capacity(according to port type)
 *   @param[in]     cir_bw          Port CIR bandwidth.
 *   @param[in]     eir_bw          Port EIR bandwidth.
 *   @param[in/out] pcbs            (pointer to) Port CIR busrt size.
 *   @param[in/out] pebs            (pointer to) Port EIR burst size.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if port_index is out of range.
 *   @retval -ENODATA if port_index is not in use.
 *  
 *   @retval TM_CONF_PORT_SPEED_OOR.
 *   @retval TM_CONF_PORT_BW_OUT_OF_SPEED.
 *   @retval TM_CONF_PORT_BS_OOR.
 *   @retval TM_BW_OUT_OF_RANGE  if  failed to configure desired b/w
 *   @retval * TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE	desired cbs or ebs value is too small for desired cir/eir
 *		in this case   values in  pcbs_kb/pebs_kb are updated to minimum possible value.
 *   @retval TM_BW_UNDERFLOW - if cir & eir are too different to configure together
 *
 *   @retval TM_HW_PORT_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_ELIG_PRIO_FUNC_FAILED if port eligible function
 *   pointer download to HW failed .
 */
int tm_verify_port_shaping_configuration(tm_handle hndl,
                                         enum tm_port_bw port_type,
                                         uint32_t cir_bw,
                                         uint32_t eir_bw,
                                         uint32_t * pcbs,
                                         uint32_t * pebs);


#endif   /* TM_NODES_UPDATE_H */

