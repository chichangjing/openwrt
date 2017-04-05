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
 * @brief TM Scheduler interface.
 *
 * @file tm_sched.h
 *
 * $Revision: 2.0 $
 */

#ifndef   	TM_SCHED_H
#define   	TM_SCHED_H

#include "tm_defs.h"

/** Configure TM general registers. 
 * @param[in]	hndl	           TM lib handle
 * @param[in]	port_ext_bp        En/Dis port external BP	
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL if hndl is NULL
 * @retval -EBADF if hndl is an invalid handle
 * @retval -EFAULT if one of the parameters is out of range
 * @retval  TM_HW_GEN_CONFIG_FAILED if download to HW fails
*/
int tm_sched_general_config(tm_handle hndl, uint8_t port_ext_bp);


/** Configure Periodic Scheme.
 *
 * @param[in,out]   hndl         	    TM lib handle
 * @param[in]       lvl_params          Pointer to per level
 *                                      parameters array
 * 
 * @note 1  The API may be invoked once in a system lifetime
 *       2  To indicate the shaping is disabled for a level the
 *          lvl_params.per_state field must be set to TM_DISABLE
 *       3  lvl_params is handled as 2 elements array including also a
 *          port level scheduling configuration
 * 
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL if hndl is NULL
 * @retval -EBADF if hndl is an invalid handle
 * @retval -EADDRNOTAVAIL if per_state is invalid
 * @retval -EPERM if periodic update process has already been started
 * @retval TM_CONF_PER_RATE_L_K_N_NOT_FOUND if Periodic Rate
 * Shaping L, K, N parameters matching the shaped bank update rate
 * can not be found   
 * @retval TM_HW_CONF_PER_SCHEME_FAILED if download to HW fails
*/
int tm_configure_periodic_scheme(tm_handle hndl,
                                 struct tm_level_periodic_params *lvl_params);



/** Enable/Disable periodic update  for a given level: Q, A, B, C or Port 
 * as a result of disabling periodic update for the layer  traffic through 
 * nodes with shaping-enabled eligible functions will be stopped.
 * @param[in,out]   hndl	TM lib handle
 * @param[in] 		level   Level to enable/disable shaping for
 * @param[in] 		status  Enable/Disable shaping
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL if hndl is NULL
 * @retval -EBADF if  tm_hndl is an invalid handle
 * @retval -EFAULT if level is invalid 
 * @retval TM_HW_PERIODIC_UPDATE_STATUS_SET_FAILED if download to HW fails 
*/

int tm_periodic_update_status_set(tm_handle hndl, enum tm_level level, uint8_t status);


/** Read periodc update status for all TM levels
 * @param[in,out]   hndl	TM lib handle
 * @param[in]       status  Pointer to per level parameters array
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL if hndl is NULL
 * @retval -EBADF if  tm_hndl is an invalid handle
*/

int tm_periodic_update_status_get(tm_handle hndl, struct tm_level_periodic_params  * status);



/** Set the number of DWRR bytes per busrt limit for all ports.
 * @param [in]      hndl	TM lib handle
 * @param [in]      bytes   Number of bytes per burst limit
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL if hndl is NULL
 * @retval -EBADF  if  hndl is an invalid handle
 * @retval -EFAULT if number of bytes is out of range
 * @retval TM_HW_PORT_SET_DWRR_BYTES_BURST_FAILED if download to HW fails.
*/
int tm_port_level_set_dwrr_bytes_per_burst_limit(tm_handle hndl, uint8_t bytes);


/** Return the MSU (Maximal Selected Unit) in bytes which is a
 * function of the MTU which in turn can differ in parts of the
 * system.
 *
 * @param [in]      hndl    TM lib handle
 * @param [in]      mtu     System or other MTU
 *
 * @return MSU (positive number in bytes) on success
 *
 * @retval -EINVAL  If hndl is NULL
 * @retval -EBADF   If hndl is an invalid handle
 */
int tm_sched_max_selected_unit(tm_handle hndl, unsigned mtu);


/** Return the Port MDU (Port Maximal Data Unit) in bytes which is a
 * function of the MTU which may be different on different ports.
 *
 * @param [in]      hndl    TM lib handle
 * @param [in]      mtu     System or other MTU
 *
 * @return MSU (positive number in bytes) on success
 *
 * @retval -EINVAL  If hndl is NULL
 * @retval -EBADF   If hndl is an invalid handle
 */
int tm_sched_port_max_selected_unit(tm_handle hndl, unsigned mtu);




/** Return the quantum values for ports. 
 *  function returns min quantum value, max quantum value and unit for quantum value in bytes
 *
 * @param [in]      hndl		TM lib handle
 * @param [out]     * quantums  pointer to structure , whish will be filled by quantum values 
 *
 * @return 0		on success
 *
 * @retval -EINVAL  If hndl is NULL
 * @retval -EBADF   If hndl is an invalid handle
 */
int tm_sched_get_port_quantum_limits(tm_handle hndl, struct tm_quantum_limits * port_quantum_limits);


/** Return the quantum values for scheduler nodes (& queues). 
 *  function returns min quantum value, max quantum value and unit for quantum value in bytes
 *
 * @param [in]      hndl		TM lib handle
 * @param [out]     * quantums  pointer to structure , whish will be filled by quantum values 
 *
 * @return 0		on success
 *
 * @retval -EINVAL  If hndl is NULL
 * @retval -EBADF   If hndl is an invalid handle
 */
int tm_sched_get_node_quantum_limits(tm_handle hndl, struct tm_quantum_limits * node_quantum_limits);

#endif   /* TM_SCHED_H */

