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
 * @brief rm_ctl interface.
 *
 * @file rm_ctl.h
 *
 * $Revision: 2.0 $
 */

#ifndef RM_CTL_H
#define RM_CTL_H

#include "rm_interface.h"

/** Create RM handle.
 * 
 *   @param[in]     hlog			         generic log handle.	
 *   @param[in]		total_ports		         Total num Ports. 
 *   @param[in]		total_c_nodes		     Total num C-nodes.
 *   @param[in]		total_b_nodes		     Total num B-nodes.	
 *   @param[in]		total_a_nodes		     Total num A-nodes.
 *   @param[in]		total_queues		     Total num Queues.
 *   @param[out]    hndl			    	 RM handle pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -ENOMEM when out of memory space.
 */
rmctl_t *rm_open(	uint16_t total_ports,
					uint16_t total_c_nodes,
					uint16_t total_b_nodes,
					uint16_t total_a_nodes,
					uint32_t total_queues);
		
/** Close RM handle.
 * 
 *   @param[in]		hndl		Resource Manager handle.	
 *
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle
 */
int rm_close(rmctl_t hndl);




#endif   /* RM_CTL_H */
