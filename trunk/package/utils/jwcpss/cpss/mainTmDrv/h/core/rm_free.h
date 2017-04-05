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
 * @brief rm_free interface.
 *
 * @file rm_free.h
 *
 * $Revision: 2.0 $
 */

#ifndef RM_FREE_H
#define RM_FREE_H

#include "rm_interface.h"


/** Free Shaping Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_shaping_profile(rmctl_t hndl, uint32_t entry_ind);


/** Free WRED Queue Curve entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_wred_queue_curve(rmctl_t hndl, uint16_t entry_ind);


/** Free WRED A-node Curve entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_wred_a_node_curve(rmctl_t hndl, uint16_t entry_ind);


/** Free WRED B-node Curve entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_wred_b_node_curve(rmctl_t hndl, uint16_t entry_ind);


/** Free WRED C-node Curve entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos 		CoS of RED Curve.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind or cos is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_wred_c_node_curve(rmctl_t hndl, uint8_t cos, uint16_t entry_ind);


/** Free WRED Port Curve entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_wred_port_curve(rmctl_t hndl, uint16_t entry_ind);

/** Free WRED Port Curve entry per Cos.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos		    CoS of RED Curve.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind or cos is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_wred_port_curve_cos(rmctl_t hndl, uint8_t cos, uint16_t entry_ind);



/** Free Queue Drop Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_queue_drop_profile(rmctl_t hndl, uint16_t entry_ind);


/** Free A-node Drop Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_a_node_drop_profile(rmctl_t hndl, uint16_t entry_ind);


/** Free B-node Drop Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_b_node_drop_profile(rmctl_t hndl, uint16_t entry_ind);


/** Free C-node Drop Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos 		CoS of RED Curve.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_c_node_drop_profile(rmctl_t hndl, uint8_t cos, uint16_t entry_ind);


/** Free Port Drop Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_port_drop_profile(rmctl_t hndl, uint16_t entry_ind);


/** Free Port Drop Profile entry per Cos.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos		    CoS of RED Curve.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind or cos is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */
int rm_free_port_drop_profile_cos(rmctl_t hndl, uint8_t cos, uint16_t entry_ind);

/** Free Queue Aging Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		entry_ind	Entry index.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if entry_ind is out of range.
 *   @retval -ENOMSG if entry_ind is already free.
 */

int rm_free_queue_aging_profile(rmctl_t hndl, uint16_t entry_ind);


#endif   /* RM_FREE_H */
