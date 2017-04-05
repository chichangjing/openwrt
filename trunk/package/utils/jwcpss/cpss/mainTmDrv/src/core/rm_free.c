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
 * @brief Resource Manager free functions implementation.
 *
 * @file rm_free.c
 *
 * $Revision: 2.0 $
 */

#include "rm_internal_types.h"
#include "rm_free.h"

/** 
 */
int rm_free_shaping_profile(rmctl_t hndl, uint32_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	return release_resource(&(ctl->rm_profiles[RM_SHAPING_PRF]), entry_ind);
}
/** 
 */
int rm_free_wred_queue_curve(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	return release_resource(&(ctl->rm_profiles[RM_WRED_Q_CURVE]), entry_ind);
}
/** 
 */
int rm_free_wred_a_node_curve(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	return release_resource(&(ctl->rm_profiles[RM_WRED_A_CURVE]), entry_ind);
}
/** 
 */
int rm_free_wred_b_node_curve(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	return release_resource(&(ctl->rm_profiles[RM_WRED_B_CURVE]), entry_ind);
}
/** 
 */
int rm_free_wred_c_node_curve(rmctl_t hndl, uint8_t cos, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	return release_resource(&(ctl->rm_profiles[RM_WRED_P_CURVE_COS_0+cos]), entry_ind);
}
/** 
 */
int rm_free_wred_port_curve(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	return release_resource(&(ctl->rm_profiles[RM_WRED_P_CURVE]), entry_ind);
}
/** 
 */
int rm_free_wred_port_curve_cos(rmctl_t hndl, uint8_t cos, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	return release_resource(&(ctl->rm_profiles[RM_WRED_P_CURVE_COS_0+cos]), entry_ind);
}
/** 
 */
int rm_free_queue_drop_profile(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	return release_resource(&(ctl->rm_profiles[RM_Q_DROP_PRF]), entry_ind);
}
/** 
 */
int rm_free_a_node_drop_profile(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	return release_resource(&(ctl->rm_profiles[RM_A_DROP_PRF]), entry_ind);
}
/** 
 */
int rm_free_b_node_drop_profile(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	return release_resource(&(ctl->rm_profiles[RM_B_DROP_PRF]), entry_ind);
}
/** 
 */
int rm_free_c_node_drop_profile(rmctl_t hndl, uint8_t cos, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	return release_resource(&(ctl->rm_profiles[RM_C_DROP_PRF_COS_0+cos]), entry_ind);
}
/** 
 */
int rm_free_port_drop_profile(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	return release_resource(&(ctl->rm_profiles[RM_P_DROP_PRF]), entry_ind);
}
/** 
 */
int rm_free_port_drop_profile_cos(rmctl_t hndl, uint8_t cos, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	return release_resource(&(ctl->rm_profiles[RM_P_DROP_PRF_COS_0+cos]), entry_ind);
}

int rm_free_queue_aging_profile(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	return release_resource_range(&(ctl->rm_profiles[RM_Q_DROP_PRF]), entry_ind, AGING_RESOURCE_SLOT_SIZE);
}

