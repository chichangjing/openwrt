#ifndef	_RM_ALLOC_H_
#define	_RM_ALLOC_H_

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
 * @brief rm_alloc interface.
 *
 * @file rm_alloc.h
 *
 * $Revision: 2.0 $
 */


#include "rm_interface.h"
/* for definition of tm_level */
#include "tm_defs.h" 


/** Find free Shaping Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_shaping_profile(rmctl_t hndl);


/** Find free WRED Queue Curve entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_wred_queue_curve(rmctl_t hndl);


/** Find free WRED A-node Curve entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_wred_a_node_curve(rmctl_t hndl);


/** Find free WRED B-node Curve entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_wred_b_node_curve(rmctl_t hndl);


/** Find free WRED C-node Curve entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos		    CoS of RED Curve.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if cos is out of range.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_wred_c_node_curve(rmctl_t hndl, uint8_t cos);


/** Find free WRED Port Curve entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_wred_port_curve(rmctl_t hndl);


/* BC2 only */
/** Find free WRED Port Curve entry per Cos.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos		    CoS of RED Curve.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if cos is out of range.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_wred_port_curve_cos(rmctl_t hndl, uint8_t cos);


/** Find free Queue Drop Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_queue_drop_profile(rmctl_t hndl);


/** Find free A-node Drop Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_a_node_drop_profile(rmctl_t hndl);


/** Find free B-node Drop Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_b_node_drop_profile(rmctl_t hndl);


/** Find free C-node Drop Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos		    CoS of RED Curve.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if cos is out of range.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_c_node_drop_profile(rmctl_t hndl, uint8_t cos);


/** Find free Port Drop Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_port_drop_profile(rmctl_t hndl);


/** Find free Port Drop Profile entry per Cos.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		cos		    CoS of RED Curve.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -EFAULT if cos is out of range.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_port_drop_profile_cos(rmctl_t hndl, uint8_t cos);

/** get next allocated Drop Profile resource.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *   @param[in]		level		TM Level.
 *   @param[in]		cos		    CoS of RED Curve.
 *   @param[in]		index		resource index.
 *
 *   @return an integer positive index of next allocated
 *           resource, otherwise -1;
 */
int rm_get_next_allocated_drop_resource(rmctl_t hndl, enum tm_level level, uint8_t cos, int index);

/** Find free Queue Aging Profile entry.
 * 
 *   @param[in]		hndl		Resource Manager handle.
 *
 *   @return an integer positive index of found free entry.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOBUFS when no free entry.
 */
int rm_find_free_queue_aging_profile(rmctl_t hndl);


#endif   /* RM_ALLOC_H */
