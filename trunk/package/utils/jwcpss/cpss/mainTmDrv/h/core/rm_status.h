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
 * @brief rm_status interface.
 *
 * @file rm_status.h
 *
 * $Revision: 2.0 $
 */

#ifndef RM_STATUS_H
#define RM_STATUS_H

#include "rm_interface.h"
	

/** Get Shaping Profile status.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Profile index.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 */
int rm_shaping_profile_status(rmctl_t hndl, uint32_t entry_ind);


/** Get WRED Queue Curve status.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Curve index.
 *   @param[out]	status		Curve status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 */
int rm_wred_queue_curve_status(rmctl_t hndl, uint8_t entry_ind);


/** Get WRED A-node Curve status.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Curve index.
 *   @param[out]	status		Curve status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 */
int rm_wred_a_node_curve_status(rmctl_t hndl, uint8_t entry_ind);


/** Get WRED B-node Curve status.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Curve index.
 *   @param[out]	status		Curve status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 */
int rm_wred_b_node_curve_status(rmctl_t hndl, uint8_t entry_ind);


/** Get WRED C-node Curve status.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos	        CoS of RED Curve.
 *   @param[in]		entry_ind	Curve index.
 *   @param[out]	status		Curve status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
*/
int rm_wred_c_node_curve_status_cos(rmctl_t hndl, uint8_t cos, uint8_t entry_ind);


/** Get WRED Port Curve status.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Curve index.
 *   @param[out]	status		Curve status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 */
int rm_wred_port_curve_status(rmctl_t hndl, uint8_t entry_ind);


/** Get WRED Port Curve status per Cos.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos	        CoS of RED Curve.
 *   @param[in]		entry_ind	Curve index.
 *   @param[out]	status		Curve status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 */
int rm_wred_port_curve_status_cos(rmctl_t hndl, uint8_t cos, uint8_t entry_ind);


/** Get Queue Drop Profile status.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Profile index.
 *   @param[out]	status		Profile status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 */
int rm_queue_drop_profile_status(rmctl_t hndl, uint16_t entry_ind);


/** Get A-node Drop Profile status.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Profile index.
 *   @param[out]	status		Profile status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 */
int rm_a_node_drop_profile_status(rmctl_t hndl, uint16_t entry_ind);


/** Get B-node Drop Profile status.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Profile index.
 *   @param[out]	status		Profile status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
*/
int rm_b_node_drop_profile_status(rmctl_t hndl, uint16_t entry_ind);


/** Get C-node Drop Profile status.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos	        CoS of RED Curve.
 *   @param[in]		entry_ind	Profile index.
 *   @param[out]	status		Profile status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 */
int rm_c_node_drop_profile_status_cos(rmctl_t hndl, uint8_t cos, uint16_t entry_ind);


/** Get Port Drop Profile status.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Profile index.
 *   @param[out]	status		Profile status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 */
int rm_port_drop_profile_status(rmctl_t hndl, uint16_t entry_ind);



/** Get Port Drop Profile status per Cos.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos	        CoS of RED Curve.
 *   @param[in]		entry_ind	Profile index.
 *   @param[out]	status		Profile status.
 *
 *   @return an integer .
 *   @retval  0   if resource free. 
 * 	 @retval  1   if resource is occupied.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 */
int rm_port_drop_profile_status_cos(rmctl_t hndl, uint8_t cos, uint16_t entry_ind);



/** validate that Queue profile is Aging Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero if entry_ind points to aging profile. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval 1	 entry is not aging profile entry.
 */

int rm_validate_queue_aging_profile(rmctl_t hndl, uint16_t entry_ind);




/* useful macros for testing  resources */

#define TEST_RESOURCE_EXISTS(function, not_in_range_error, is_free_error) \
	switch(function)\
	{\
		case 1: rc=0;break;\
		case 0: rc=is_free_error;break;\
		default:rc=not_in_range_error;break;\
	}
#define TEST_RESOURCE_FREE(function, not_in_range_error, is_used_error) \
	switch(function)\
	{\
		case 1: rc=is_used_error;break;\
		case 0: rc=0;break;\
		default:rc=not_in_range_error;break;\
	}



#endif   /* RM_STATUS_H */
